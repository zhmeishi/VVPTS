/*
 * greedy.h
 *
 *  Created on: Dec, 2018
 *      Author: Mei
 */

#ifndef EXPERIMENT_H_
#define EXPERIMENT_H_

#include "vpts.h"
#include "sanity.h"
#include "heuristic.h"
#include "error.h"
#include "Const.h"
#include "input.h"
#include "split.h"
#include "greedy.h"
#include "merge.h"
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <sys/resource.h>

using namespace std;

extern emp_stat_t* emp_stat_v;

class Experiment
{
  public:
    Input* input;
    string config;
    Point *input_tra;
    int length;
    double error;
    bool state;
    Point* re;
    int num;
    int alg_opt;
    traInfo_t* traInfo;

    Experiment(string cf)
    {
        config = cf;
        input = new Input(config);
        input_tra = input->tra_list_v;
        length = input->config->pos_n;
        util::traRefresh(input_tra,length);
        traInfo = util::traInfo(input_tra,length);
        num = input->config->pos_n-1;
        error = input->config->eps;
        state = true;
        alg_opt = input->config->alg_opt;
        emp_stat_v = new emp_stat();
        emp_stat_v->memory_max = 0;
        emp_stat_v->memory_v = 0;
        /*s*/
        emp_stat_v->memory_v += sizeof( Point) * length;
        if( emp_stat_v->memory_v > emp_stat_v->memory_max)
            emp_stat_v->memory_max = emp_stat_v->memory_v;
        /*s*/
    }

    ~Experiment()
    {
        if (input != NULL){
            if(state){
                write_result();
            }
            if(traInfo != NULL){
                delete traInfo;
                traInfo = NULL;
            }
            input->~Input();
            input=NULL;
            if(re != NULL){
                delete [] re;
                re = NULL;
            }
            /*s*/
            emp_stat_v->memory_v -= sizeof( Point) * length;
            /*s*/
            if(emp_stat_v != NULL){
                delete [] emp_stat_v;
                emp_stat_v = NULL;
            }
        }
    }

    void run()
    {
        Const::receive_error(error);
        //printf("j: %d, error: %d \n", j, error);
        printf( "Simplifying ...\n");
        rusage before_simp_t;
	    util::get_cur_time( &before_simp_t);
        if( alg_opt == 0)
        {
            printf( "Sanity \n");
            Sanity s = Sanity(input_tra, length, error);
            num = s.call_algorithm();
            re = util::getResult(input_tra,length, true);
            s.~Sanity();
        }
        else if( alg_opt == 1)
        {
            printf( "VVPTS \n");
            VPTS v = VPTS(input_tra, length, error);
            num = v.call_algorithm();
            re = util::getResult(input_tra,length, false);
            v.~VPTS();
        }
        else if( alg_opt == 2)
        {
            printf( "Heuristic \n");
            Heuristic h = Heuristic(input_tra, length, error);
            num = h.call_algorithm();
            re = util::getResult(input_tra,length, true);
        }
        else if( alg_opt == 3)
        {
            printf( "Split \n");
            Split sp = Split(input_tra, length, error);
            num = sp.call_algorithm();
            re = util::getResult(input_tra,length, true);
        }
        else if( alg_opt == 4)
        {
            printf( "Merge \n");
            Merge m = Merge(input_tra, length, error);
            num = m.call_algorithm();
            re = util::getResult(input_tra,length,false);
        }
        else if( alg_opt == 5)
        {
            printf( "Greedy \n");
            Greedy g = Greedy(input_tra, length, error);
            num = g.call_algorithm();
            re = util::getResult(input_tra,length,true);
        }
        else
        {
            cout << "Invaild Algorithm indicator";
            state = false;
            return;
        }
	    rusage after_simp_t;
	
	    util::get_cur_time( &after_simp_t);
	    util::collect_time( &before_simp_t, &after_simp_t, &emp_stat_v->r_time);
        printf( "Finished!\n");

        return;
    }

    void write_result(){
        //cmp_ratio.
	    emp_stat_v->cmp_ratio = ( double)(num+1) / length;
        if(input->config->check!=0){
            VelocityError ve = VelocityError();
            DistanceError diste = DistanceError();
            DirectionError direce = DirectionError();
            CEDError cede = CEDError();
            SEDError sede = SEDError();
            emp_stat_v->ve = ve.check_error(input_tra, re, num+1);
            emp_stat_v->diste = diste.check_error(input_tra, re, num+1);
            emp_stat_v->direce = direce.check_error(input_tra, re, num+1)*180/Const::PI;
            emp_stat_v->cede = cede.check_error(input_tra, re, num+1);
            emp_stat_v->sede = sede.check_error(input_tra, re, num+1);
        }
        FILE* s_fp;
        if( ( s_fp = fopen( input->config->s_name, "w")) == NULL)
        {
            fprintf( stderr, "Cannot open the stat.txt\n");
            exit( 0);
        }
        //Simplification results.
        fprintf( s_fp, "%i\n%i\n%lf\n\n", length, num+1, emp_stat_v->cmp_ratio);
        //Statistics.
        fprintf( s_fp, "%lf\n%lf\n\n", emp_stat_v->r_time, emp_stat_v->memory_max / (1024 * 1024));
        if(input->config->check!=0){
            fprintf( s_fp, "%lf\n%lf\n%lf\n%lf\n%lf\n", emp_stat_v->ve, emp_stat_v->diste, 
            emp_stat_v->direce, emp_stat_v->cede, emp_stat_v->sede);
        }
        fclose( s_fp);
    }
};

#endif /* EXPERIMENT_H_ */
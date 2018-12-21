/*
 * input.h
 *
 *  Created on: Dec, 2018
 *      Author: Mei
 */

#ifndef INPUT
#define INPUT
#include <fstream>
#include <string>
#include <string.h>
#include <iostream>
#include <vector>
#include <ctime>
#include "Const.h"
#include <math.h>
using namespace std;
//template <class T>


/*
    int tm_sec;   // second [0,59]
    int tm_min;   // minute [0,59]
    int tm_hour;  // hour   [0,23]
    int tm_mday;  // day    [1,31]
    int tm_mon;   // month  [0,11]
    int tm_year;  // start from 1900
*/


class Input{
public:
    string filename;
    bool state;
    emp_config_t* config;
    Point * tra_list_v;
    double scale;
    double t_max;
    double v_max;
    double d_max;
  
    Input(string fn="config.txt"){
        scale = Const::ZOOM;
        filename = fn;
        config = emp_read_config();
        tra_list_v = new Point[config->pos_n+1];
        if( config->dataset_tag == 1)
        {
            config->line_ignore_n = max(6,config->line_ignore_n);
            emp_read_data_v1( config);
        }
        else
        {
            //config->line_ignore_n = 0;
            emp_read_data_v2( config);
        }
    }
   
    ~Input(){
        if (tra_list_v != NULL){delete [] tra_list_v;tra_list_v=NULL;}
        if (config != NULL){delete config;config=NULL;}
    }
    emp_config_t* emp_read_config( )
    {                        
        emp_config_t* emp_config_v = new emp_config_t;
        FILE* c_fp;

        if( ( c_fp = fopen( filename.c_str(), "r")) == NULL)
        {
            fprintf( stderr, "Cannot open the config.txt\n");
            exit( 0);
        }    

        fscanf( c_fp, "%s%i", &emp_config_v->f_name, &emp_config_v->pos_n);
        fscanf( c_fp, "%lf%i", &emp_config_v->eps, &emp_config_v->alg_opt);
        fscanf( c_fp, "%i", &emp_config_v->dataset_tag);
        fscanf( c_fp, "%i", &emp_config_v->line_ignore_n);
        fscanf( c_fp, "%i", &emp_config_v->check);
        fscanf( c_fp, "%s", &emp_config_v->s_name);
        //cout<< emp_config_v->f_name;

        return emp_config_v;
    }
  
    /*
    *    Specific to the Geolife data.
    */
    void emp_read_data_v1( emp_config_t* emp_config_v)
    {
        int i, j;
        char tmp[ MAX_LINE_LENG];
        char* tok;
        double pre_x = 0.0;
        double pre_y = 0.0;
        double prev_t = 0.0;


        FILE* i_fp;

        if( ( i_fp = fopen( emp_config_v->f_name, "r")) == NULL)
        {
            fprintf( stderr, "Cannot open the data file.\n");
            exit( 0);
        }

        //Filter the useless lines if necessary.
        for( i=0; i<emp_config_v->line_ignore_n; i++)
        {
            fgets( tmp, MAX_LINE_LENG, i_fp);
        }
        
        struct tm  time_init;
        time_t t1;
        time_init.tm_year = 2000 - 1900;
        time_init.tm_mon = 0;
        time_init.tm_mday = 1;
        time_init.tm_hour = 0;
        time_init.tm_min = 0;
        time_init.tm_sec = 0;
        t1 = mktime(&time_init);

        //Read the positions.
        for( i=0; i<emp_config_v->pos_n; i++)
        {
            //Read the line.
            fgets( tmp, MAX_LINE_LENG, i_fp);
            struct tm  time_p;

            //Parse the line to get the x and y coordinates.
            if( !(tok = strtok( tmp, ",")))
            {
                fprintf( stderr, "Data format inconsistency.\n");
                exit( 0);
            }

            double x = atof( tok)*scale;

            if( !(tok = strtok( NULL, ",")))
            {
                fprintf( stderr, "Data format inconsistency.\n");
                exit( 0);
            }
            double y = atof( tok)*scale;
            if( fabs( x - pre_x) <= Const::EPSILION &&
                fabs( y - pre_y) <= Const::EPSILION)
                x += 100*Const::EPSILION;


            for( j=0; j<3; j++)
            {
                if( !(tok = strtok( NULL, ",")))
                {
                    fprintf( stderr, "Data format inconsistency.\n");
                    exit( 0);
                }      
            }

            //year.
            if( !(tok = strtok( NULL, "-")))
            {
                fprintf( stderr, "Data format inconsistency.\n");
                exit( 0);
            }

            time_p.tm_year = atoi( tok) - 1900;

            //month.
            if( !(tok = strtok( NULL, "-")))
            {
                fprintf( stderr, "Data format inconsistency.\n");
                exit( 0);
            }

            time_p.tm_mon = atoi( tok)-1;

            //day.
            if( !(tok = strtok( NULL, ",")))
            {
                fprintf( stderr, "Data format inconsistency.\n");
                exit( 0);
            }

            time_p.tm_mday = atoi( tok);

            //hour.
            if( !(tok = strtok( NULL, ":")))
            {
                fprintf( stderr, "Data format inconsistency.\n");
                exit( 0);
            }

            time_p.tm_hour = atoi( tok);

            //minute.   
            if( !(tok = strtok( NULL, ":")))
            {
                fprintf( stderr, "Data format inconsistency.\n");
                exit( 0);
            }

            time_p.tm_min = atoi( tok);

            //second.
            if( !(tok = strtok( NULL, "")))
            {
                fprintf( stderr, "Data format inconsistency.\n");
                exit( 0);
            }
        
            time_p.tm_sec = atoi( tok);
            time_t t2;
            t2 = mktime(&time_p);

            double t = difftime ( t2, t1 );
            //cout<<t2<<" "<<t1<<endl;
                        
            if( t - prev_t < -0.1 ){
                fprintf( stderr, "Time decrease. The trajectory is invalid. \n");
                exit( 0);
            }
            
            //cout<<x - pre_x<<","<<y - pre_y<<","<<t<<"\n";
            if( fabs( t - prev_t) <= Const::EPSILION )
            t += 0.01;
            prev_t = t;
            pre_x = x;
            pre_y = y;
            tra_list_v[i+1]=Point(x,y,t,i+1);
        }        
    }

    /*
    *    Specific to the T-Drive data.
    */
    void emp_read_data_v2( emp_config_t* emp_config_v)
    {
        int i;
        char tmp[ MAX_LINE_LENG];
        char* tok;
        double pre_x = 0.0;
        double pre_y = 0.0;
        double prev_t = 0.0;

        FILE* i_fp;

        if( ( i_fp = fopen( emp_config_v->f_name, "r")) == NULL)
        {
            fprintf( stderr, "Cannot open the data file.\n");
            exit( 0);
        }

        //Filter the useless lines if necessary.
        for( i=0; i<emp_config_v->line_ignore_n; i++)
        {
            fgets( tmp, MAX_LINE_LENG, i_fp);
        }

        struct tm  time_init;
        time_t t1;
        time_init.tm_year = 2000 - 1900;
        time_init.tm_mon = 0;
        time_init.tm_mday = 1;
        time_init.tm_hour = 0;
        time_init.tm_min = 0;
        time_init.tm_sec = 0;
        t1 = mktime(&time_init);

        //Read the positions.
        for( i=0; i<emp_config_v->pos_n; i++)
        {
            //Read the line.
            fgets( tmp, MAX_LINE_LENG, i_fp);
            struct tm  time_p;

            //Parse the line to get the x and y coordinates.
            //useless.
            if( !(tok = strtok( tmp, ",")))
            {
                fprintf( stderr, "Data format inconsistency.\n");
                exit( 0);
            }
            //year.
            if( !(tok = strtok( NULL, "-")))
            {
                fprintf( stderr, "Data format inconsistency.\n");
                exit( 0);
            }

            time_p.tm_year = atoi( tok) - 1900;

            //month.
            if( !(tok = strtok( NULL, "-")))
            {
                fprintf( stderr, "Data format inconsistency.\n");
                exit( 0);
            }
            time_p.tm_mon = atoi( tok)-1;

            //day.
            if( !(tok = strtok( NULL, " ")))
            {
                fprintf( stderr, "Data format inconsistency.\n");
                exit( 0);
            }

            time_p.tm_mday = atoi( tok);

            //hour.
            if( !(tok = strtok( NULL, ":")))
            {
                fprintf( stderr, "Data format inconsistency.\n");
                exit( 0);
            }

            time_p.tm_hour = atoi( tok);

            //min.
            if( !(tok = strtok( NULL, ":")))
            {
                fprintf( stderr, "Data format inconsistency.\n");
                exit( 0);
            }

            time_p.tm_min = atoi( tok);

            //sec
            if( !(tok = strtok( NULL, ",")))
            {
                fprintf( stderr, "Data format inconsistency.\n");
                exit( 0);
            }

            time_p.tm_sec = atoi( tok);
            time_t t2;
            t2 = mktime(&time_p);

            double t = difftime ( t2, t1 );

            //x coordinate.
            if( !(tok = strtok( NULL, ",")))
            {
                fprintf( stderr, "Data format inconsistency.\n");
                exit( 0);
            }

            double x = atof( tok)*scale;

            //y coordinate.
            if( !(tok = strtok( NULL, "")))
            {
                fprintf( stderr, "Data format inconsistency.\n");
                exit( 0);
            }

            double y = atof( tok)*scale;
            if( fabs( x - pre_x) <= Const::EPSILION &&
            fabs( y - pre_y) <= Const::EPSILION)
            x += 10000*Const::EPSILION;
            
            if( t - prev_t < -0.1 ){
                fprintf( stderr, "Time decrease. The trajectory is invalid.\n");
                exit( 0);
            }

            if( fabs( t - prev_t) <= Const::EPSILION )
            t += 0.5;
            prev_t = t;

            //cout<<x - pre_x<<","<<y - pre_y<<","<<t<<"\n";
            pre_x = x;
            pre_y = y;

            tra_list_v[i+1]=Point(x,y,t,i+1);
        }    
    }

};

#endif

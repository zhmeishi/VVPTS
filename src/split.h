/*
 * split.h
 *
 *  Created on: Dec, 2018
 *      Author: Mei
 */

#ifndef SPLIT_H_
#define SPLIT_H_

#include <cstdlib>
#include "Const.h"
#include <algorithm> 
using namespace std;
extern emp_stat_t* emp_stat_v;
// x_coo and y_coo are coordinates of the vector

class Split
{
  public:
    Point *tra; //input trajectory
    int length;
    double err;
    double max_vx;
    double min_vx;
    double max_vy;
    double min_vy; 
    int i;
    int j;

    Split(Point *input_tra, int tra_length, double error)
    {
        tra = input_tra;
        length = tra_length;
        err = error;
    }

    // the main algorithm
    int call_algorithm()
    {
        int result;
	    result = Split_sub( 1, length, err);
        tra[1].parent = -1;
        return result;   
    }
    int Split_sub(int seg_sta, int seg_end, double eps)
    {
        int k, split_seg, i, j;
        double error_max, v_diff;

        i = seg_sta;
        j = seg_end;

        if( i >= j)
            return 0;
        if( i == j-1){
            // tra[j].parent = i;
            return 1;
        }

        //Compute the \epsilon(\overline{p_ip_j}).
        error_max = -1;
        split_seg = i;
        for( k=i; k<j; k++)
        {
            //error_v = check_error( R_array_v->array[ k], &R_node_v);
            v_diff = util::calc_v_diff( tra[i],tra[j],tra[k],tra[k+1]);

            if( v_diff > error_max)
            {
                error_max = v_diff;
                split_seg = k;
            }
        }

        if( error_max <= eps){
            tra[j].parent = i;
            return 1;
        }
 
        return Split_sub(i, split_seg, eps) + Split_sub(split_seg + 1, j, eps) + 1;
    }
};

#endif /* SPLIT_H_ */

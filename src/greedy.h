/*
 * greedy.h
 *
 *  Created on: Dec, 2018
 *      Author: Mei
 */

#ifndef GREEDY_H_
#define GREEDY_H_

#include <cstdlib>
#include "Const.h"
#include <algorithm> 
using namespace std;

// x_coo and y_coo are coordinates of the vector

class Greedy
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

    Greedy(Point *input_tra, int tra_length, double error)
    {
        tra = input_tra;
        length = tra_length;
        err = error;
    }

    // the main algorithm
    int call_algorithm()
    {
        int result = 0;
        double v_diff;

        int start = 1;
        int end = 3;

        while(end <= length){
            for(int k = start; k < end; k++){
                v_diff = util::calc_v_diff( tra[start],tra[end],tra[k],tra[k+1]);
                if(v_diff>err){
                    tra[end-1].parent = start;
                    start = end-1;
                    result++;
                    break;
                }
            }
            if(end == length){
                result++;
                tra[end].parent = start;
            }
            end = end + 1;
        }

        return result;   
    }
};

#endif /* GREEDY_H_ */

/*
 * heuristic.h
 *
 *  Created on: Dec, 2018
 *      Author: Bruce
 */

#ifndef HEURISTIC_H_
#define HEURISTIC_H_

#include <cstdlib>
#include "Const.h"
using namespace std;

// x_coo and y_coo are coordinates of the vector

class Heuristic
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

    Heuristic(Point *input_tra, int tra_length, double error)
    {
        tra = input_tra;
        length = tra_length;
        err = error;
        i = 1;
        j = 2;
        max_vx = (tra[2].x_coo - tra[1].x_coo) / (tra[2].time - tra[1].time);
        min_vx = max_vx;
        max_vy = (tra[2].y_coo - tra[1].y_coo) / (tra[2].time - tra[1].time);
        min_vy = max_vy;
        tra[1].visited = 0;
    }

    // the main algorithm
    int call_algorithm()
    {
        while(true){
            if (j==length){
                tra[j].parent = i;
                tra[j].visited = tra[i].visited + 1;
                return tra[j].visited;
            }

            double vx = (tra[j+1].x_coo - tra[j].x_coo) / (tra[j+1].time - tra[j].time);
            double vy = (tra[j+1].y_coo - tra[j].y_coo) / (tra[j+1].time - tra[j].time);
            max_vx = Const::max(max_vx, vx);
            min_vx = Const::min(min_vx, vx);
            max_vy = Const::max(max_vy, vy);
            min_vy = Const::min(min_vy, vy);

            if (Const::L2norm(max_vx - min_vx, max_vy - min_vy) < err){
                j += 1;
            }
            else{
                tra[j].parent = i;
                tra[j].visited = tra[i].visited + 1; 
                i = j;
                j += 1;
                max_vx = vx;
                min_vx = vx;
                max_vy = vy;
                min_vy = vy;
            }
        }   
    }
};

#endif /* HEURISTIC_H_ */

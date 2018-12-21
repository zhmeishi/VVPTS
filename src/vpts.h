/*
 * vpts.h
 *
 *  Created on: Dec, 2018
 *      Author: Bruce
 */

#ifndef VPTS_H_
#define VPTS_H_

#include <cstdlib>
#include <math.h>
#include <list>
#include <vector>
#include "Const.h"
#include "BinarySearch.h"
#include "DLL.h"
using namespace std;
extern emp_stat_t* emp_stat_v;

// x_coo and y_coo are coordinates of the vector

class VPTS
{
  public:
    Point *tra; //input trajectory
    int length;
    double err;
    Arc *arc_table;
    bool state;

    VPTS(Point *input_tra, int tra_length, double error)
    {
        tra = input_tra;
        length = tra_length;
        err = error;
        arc_table = new Arc[length];
        state = true;

        // initialize the information of last two points
        tra[length].visited = 0;
        tra[length-1].visited = 1;
        tra[length-1].parent = length;
        /*s*/
        emp_stat_v->memory_v += sizeof( Arc) * length;
        if( emp_stat_v->memory_v > emp_stat_v->memory_max)
            emp_stat_v->memory_max = emp_stat_v->memory_v;
        /*s*/
    }
    ~VPTS()
    {
        if (arc_table != NULL){

            delete[] arc_table;
            arc_table = NULL;
            /*s*/
            emp_stat_v->memory_v -= sizeof( Arc) * length;
            /*s*/
        }
 
    }

    // the main algorithm
    int call_algorithm()
    {
        int n = length;
        // traverse for point i
        for (int i = n-2; i >= 1; i--){

            // update arc_table
            // arc_table[i] is saved for reference
            arc_table[i] = circle_intersection(i+1, i);
            arc_table[i+1] = circle_intersection(i, i+1);
            for (int j = i+2; j <= n-1; j++){
                if (!arc_table[j].not_empty){continue;}
                Arc iANDj = circle_intersection(i, j);
                // printf("\n\n\n\n\n\n\n\n%2f, %f, %d, %d\n\n\n\n\n\n\n\n\n\n", iANDj.theta1, iANDj.theta2, i, j);
                arc_table[j] = Arc::arc_intersection(arc_table[j], iANDj);
            }

            // initialize point i
            tra[i].visited = tra[i+1].visited+1;
            tra[i].parent = i+1;

            // initialize point R and index j
            Point R;
            int j = i+1;
            // initialize dll
            DLL dll = DLL(arc_table, length, i, tra, err); 

            while (j < length-1 && !dll.isIEmpty()){
                j++;
                dll.updateNode(j);
            }

            R = dll.get_R();
            dll.~DLL();

            // initialize index j
            j = i+1;
            // initiailize dll and bst
            DLL dll2 = DLL(arc_table, length, i, tra, err);
            BinarySearchTree bst = BinarySearchTree(i, length, err, &dll2, &R, arc_table);

            while (j < length && !dll2.isIEmpty()){
                Point p1 = tra[i];
                Point p2 = tra[j + 1];
                double t1 = p1.time;
                double t2 = p2.time;
                double dt = t2 - t1;
                double v_x = (p2.x_coo - p1.x_coo) / dt;
                double v_y = (p2.y_coo - p1.y_coo) / dt;
                bool interior = bst.check(v_x, v_y, Const::angle(v_x - R.x_coo, v_y - R.y_coo));
                if(interior && tra[j+1].visited+1 < tra[i].visited){
                    tra[i].visited = tra[j+1].visited+1;
                    tra[i].parent = j+1;
                }

                // update ddl and bst
                j++;
                if (j == length){
                    break;
                }
                dll2.updateNode(j);
                bool result = bst.update(j);
                if(result == false){
                    //cout<<"bst update false"<<endl;
                    break;
                }
            }            
        }
        // for (int i = 1; i <= n; i++){
        //     printf("parent %d\n", tra[i].parent);
        // }
        return tra[1].visited;
    }

    // compute the intersection of two circles and returns the arc of disk j
    Arc circle_intersection(int i, int j)
    {

        Point p1 = tra[i];
        Point p2 = tra[i + 1];
        Point p3 = tra[j];
        Point p4 = tra[j + 1];
        double t1 = p1.time;
        double t2 = p2.time;
        double t3 = p3.time;
        double t4 = p4.time;
        double dt1 = t2 - t1;
        double dt2 = t4 - t3;
        double v_x1 = (p2.x_coo - p1.x_coo) / dt1;
        double v_y1 = (p2.y_coo - p1.y_coo) / dt1;
        double v_x2 = (p4.x_coo - p3.x_coo) / dt2;
        double v_y2 = (p4.y_coo - p3.y_coo) / dt2;
        Arc result = Arc();
        result.r = err;
        result.disk_num = j;
        result.center_x = v_x2;
        result.center_y = v_y2;

        double c1_c2 = Const::L2norm(v_x2 - v_x1, v_y2 - v_y1);
        // two circles overlap
        if (Const::areSame(c1_c2, 0.0)){
            result.not_empty = true;
            result.disk1_num = i;
            result.disk2_num = i;
            result.point1_x = result.center_x + result.r;
            result.point1_y = result.center_y;
            result.point2_x = result.point1_x;
            result.point2_y = result.point1_y;
            result.theta1 = 0;
            result.theta2 = 360;
            result.mid_x = result.point1_x;
            result.mid_y = result.point1_y;  
        }
        // two circles separate
        else if (c1_c2 > 2.0 * result.r)
        {
            result.not_empty = false;
        }
        // two circles are tangent
        else if (Const::areSame(c1_c2, 2.0*result.r)){
            result.not_empty = true;
            result.disk1_num = i;
            result.disk2_num = i;
            result.point1_x = (v_x1 + v_x2) / 2;
            result.point1_y = (v_y1 + v_y2) / 2;
            result.point2_x = result.point1_x;
            result.point2_y = result.point1_y;
            result.theta1 = Const::angle(result.point1_x - v_x2, result.point1_y - v_y2);
            result.theta2 = result.theta1;
            result.mid_x = result.point1_x;
            result.mid_y = result.point1_y;
        }
        // two circles intersect
        else if (c1_c2 < 2.0*result.r && c1_c2 > 0){
            result.not_empty = true;
            result.disk1_num = i;
            result.disk2_num = i;

            double mid_x = (v_x2 - v_x1) * 0.5;
            double mid_y = (v_y2 - v_y1) * 0.5;
            double norm_x = 0.0;
            double norm_y = 0.0;
            if (Const::areSame(mid_x, 0))
            {
                norm_x = 1.0;
                norm_y = 0.0;
            }
            else if (Const::areSame(mid_y, 0))
            {
                norm_x = 0.0;
                norm_y = 1.0;
            }
            else
            {
                norm_x = -mid_y / mid_x;
                norm_y = 1.0;
            }
            double norm = Const::L2norm(norm_x, norm_y);

            norm_x = norm_x / norm * sqrt(result.r * result.r - c1_c2 * 0.5 * c1_c2 * 0.5);
            norm_y = norm_y / norm * sqrt(result.r * result.r - c1_c2 * 0.5 * c1_c2 * 0.5);

            result.point1_x = (v_x1 + v_x2) * 0.5 + norm_x;
            result.point1_y = (v_y1 + v_y2) * 0.5 + norm_y;
            result.point2_x = (v_x1 + v_x2) * 0.5 - norm_x;
            result.point2_y = (v_y1 + v_y2) * 0.5 - norm_y;
            result.theta1 = Const::angle(result.point1_x - v_x2, result.point1_y - v_y2);
            result.theta2 = Const::angle(result.point2_x - v_x2, result.point2_y - v_y2);

            // first sort
            if (result.theta1 > result.theta2)
            {
                swap(result.point1_x, result.point2_x);
                swap(result.point1_y, result.point2_y);
                swap(result.theta1, result.theta2);
            }
            // then check whether difference is larger than 180
            if (result.theta2 - result.theta1 > 180)
            {
                swap(result.point1_x, result.point2_x);
                swap(result.point1_y, result.point2_y);
                swap(result.theta1, result.theta2);
            }

            double theta_mid = (result.theta1 + result.theta2)/2;
            if (result.theta1 > result.theta2){
                theta_mid += 180;
            }
            result.mid_x = result.center_x + result.r * cos(theta_mid*Const::PI/180.0);
            result.mid_y = result.center_y + result.r * sin(theta_mid*Const::PI/180.0);

        }
        else{
            printf("Something goes wrong!\n");
        }
        return result;
    }
};

#endif /* VPTS_H_ */

/*
 * vpts.h
 *
 *  Created on: Oct 1, 2017
 *      Author: Bruce
 */

#ifndef VANILLA_VPTS_H_
#define VANILLA_VPTS_H_

#include <cstdlib>
#include <math.h>
#include <list>
#include <vector>
#include "Const.h"
#include "BinarySearch.h"
#include "DLL.h"
using namespace std;

// x_coo and y_coo are coordinates of the vector

class VanillaVPTS
{
  public:
    Point *tra; //input trajectory
    int length;
    double err;
    Arc **arc_table;
    Edge **edge_table;
    Point *R_table;
    list<int> *steps;

    VanillaVPTS(Point *input_tra, int tra_length, double error)
    {
        tra = input_tra;
        length = tra_length;
        err = error;
        R_table = new Point[length]

        // Step1: initialize edge_table
        for (int i = 1; i < length; i++)
        {
            edge_table[i] = new Edge[length+1];
            edge_table[i][i+1].in_graph = true;
            for(int j = i+1; j <= length; j++)
            {
                double dt = tra[j].time - tra[i].time;

                edge_table[i][j].v_x = (tra[j].x_coo - tra[i].x_coo) / dt;
                edge_table[i][j].v_y = (tra[j].y_coo - tra[i].y_coo) / dt;
            }
        }


        // Step2: initialize arc_table
        arc_table = new Arc *[length];
        for (int j = 0; j < length; j++)
        {
            arc_table[j] = new Arc[length];
        }

        // calculate arc_table[1][0], only need center
        arc_table[1][0] =  circle_intersection(2, 1);

        // then assign value to arc_table
        for (int j = 2; j <= (length - 1); j++)
        {
            // for each row, first find the right most arc
            int i = j - 1;
            arc_table[j][i] = circle_intersection(i, j);
            //printf("()()()()()()()()()()()()()\n");

            // compute arc ii for later use
            arc_table[i][i] = circle_intersection(i + 1, i);

            // if the right most arc is empty
            if (arc_table[j][i].not_empty == false){continue;}

            // then find the rest of arcs
            for (int i = j - 2; i >= 1; i--)
            {
                Arc iANDj = circle_intersection(i, j);
                arc_table[j][i] = Arc::arc_intersection(arc_table[j][i + 1], iANDj);
                if (arc_table[j][i].not_empty == false){break;}
            }
        }


        // initialize H0 and unvisited
        steps = new list<int>[length];
        steps[0].push_back(1);
        tra[1].visited = 0;
    }
    ~VanillaVPTS()
    {
        // delete edge_table
        for (int i = 1; i < length; i++)
        {
            if (edge_table[i] != NULL){delete[] edge_table[i];edge_table[i]= NULL;}
        }
        if(edge_table != NULL){
            delete[] edge_table;
            edge_table = NULL;
        }

        // delete arc_table
        for (int j = 0; j < length; j++)
        {
            // for (int i = 0; i < length; i++)
            // {
            //     if (arc_table[j][i].not_empty)
            //     {
            //         //delete arc_table[j][i];
            //     }
            // }
            if (arc_table[j] != NULL)
            {
                delete[] arc_table[j];
            }
        }
        if (arc_table != NULL)
        {
            delete[] arc_table;
        }
    }

    // the main algorithm
    int call_algorithm()
    {
        //Step3: find R points
        for (int i = 1; i <= n-2; i++)
        {                   
            int j = i + 1;

            //initialize R_table
            DLL dll = DLL(arc_table, length, i, tra, err);            
                                            
            while (j < length - 1 && !dll.isIEmpty())
            {
                j = j + 1;
                dll.updateNode(j);
            }

            R = dll.get_R();
            R.time = 1;

            // step4 of graph construction
            if (tra[i + 1].visited == -1)
            {
                /*if (i + 1 == length)
                {
                    state = false;
                    tra[i + 1].parent = i;
                    tra[i + 1].visited = tra[i].visited + 1;
                    return l;
                }*/
                steps[l].push_back(i + 1);
                tra[i + 1].parent = i;
                tra[i + 1].visited = tra[i].visited + 1;
            }

            j = i + 1;
            
            //initiailize dll and lra
            DLL dll2 = DLL(arc_table, length, i, tra, err);
            BinarySearchTree bst = BinarySearchTree(i, length, err, &dll2, &R, arc_table);
            //bst.update(j);

            if (!dll2.isIEmpty() && tra[j + 1].visited == -1)
            {
                Point p1 = tra[i];
                Point p2 = tra[j + 1];
                double t1 = p1.time;
                double t2 = p2.time;
                double dt = t2 - t1;
                double v_x = (p2.x_coo - p1.x_coo) / dt;
                double v_y = (p2.y_coo - p1.y_coo) / dt;

                bool interior = bst.check(v_x, v_y, Const::angle(v_x - R.x_coo, v_y - R.y_coo));
                if (interior == true)
                {
                    ////printf("%d, %d++\n",i,j+1);
                    if (j + 1 == length)
                    {
                        state = false;
                        tra[j + 1].parent = i;
                        tra[j + 1].visited = tra[i].visited + 1;
                        return l;
                    }
                    steps[l].push_back(j + 1);
                    tra[j + 1].parent = i;
                    tra[j + 1].visited = tra[i].visited + 1;
                }
            }
                                             
            while (j < length - 1 && !dll2.isIEmpty())
            {
                j = j + 1;
                dll2.updateNode(j);
                bst.update(j);

                if (!dll2.isIEmpty() && tra[j + 1].visited == -1)
                {   
                    Point p1 = tra[i];
                    Point p2 = tra[j + 1];
                    double t1 = p1.time;
                    double t2 = p2.time;
                    double dt = t2 - t1;
                    double v_x = (p2.x_coo - p1.x_coo) / dt;
                    double v_y = (p2.y_coo - p1.y_coo) / dt;
                                    
                    bool interior = bst.check(v_x, v_y, Const::angle(v_x - R.x_coo, v_y - R.y_coo));
                                    
                    if (interior == true)
                    {////printf("%d, %d--\n",i,j+1);
                        if (j + 1 == length)
                        {
                            state = false;
                            tra[j + 1].parent = i;
                            tra[j + 1].visited = tra[i].visited + 1;
                            return l;
                        }
                        steps[l].push_back(j + 1);
                        tra[j + 1].parent = i;
                        tra[j + 1].visited = tra[i].visited + 1;
                    }
                }
            }
            if (model == 1){
                break;
            }    
        }
        l = l + 1;
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

        // two circles separate
        if (c1_c2 > 2.0 * result.r)
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
        // two circles overlap
        else{
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
        return result;
    }
};

#endif /* VPTS_H_ */

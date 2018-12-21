/*
 * sanity.h
 *
 *  Created on: Dec, 2018
 *      Author: Bruce
 */

#ifndef SANITY_H_
#define SANITY_H_

#include <cstdlib>
#include <math.h>
#include <list>
#include "Const.h"
using namespace std;
extern emp_stat_t* emp_stat_v;
// x_coo and y_coo are coordinates of the vector

class Sanity
{
  public:
    Point *tra; //input trajectory
    int length;
    double err;
    Edge **edge_table;
    list<int> *steps;
    bool state;

    Sanity(Point *input_tra, int tra_length, double error)
    {
        tra = input_tra;
        length = tra_length;
        err = error;
        edge_table = new Edge*[length+1];
        state = true;

        // first initialize edge_table
        // printf("initialize edge_table\n");
        for (int i = 1; i < length; i++)
        {
            edge_table[i] = new Edge[length+1];
            for(int j = i+1; j <= length; j++)
            {
                double dt = tra[j].time - tra[i].time;

                edge_table[i][j].v_x = (tra[j].x_coo - tra[i].x_coo) / dt;
                edge_table[i][j].v_y = (tra[j].y_coo - tra[i].y_coo) / dt;
            }
        }

        // printf("check whether each edge is in the graph\n");
        // then check whether edge is in the graph
        for (int i = 1; i < length; i++)
        {
            edge_table[i][i+1].in_graph = true;
            for(int j = i+2; j <= length; j++)
            {
                bool in_graph = true;
                for(int k = i; k <= j-1; k++)
                {
                    double local_error = Const::L2norm(edge_table[k][k+1].v_x-edge_table[i][j].v_x, edge_table[k][k+1].v_y-edge_table[i][j].v_y);
                    if (local_error > err){
                        in_graph = false;
                        break;
                    }
                }
                edge_table[i][j].in_graph = in_graph;
            }
        }

        // initialize H0 and unvisited
        steps = new list<int>[length];
        steps[0].push_back(1);
        tra[1].visited = 0;

        /*s*/
        emp_stat_v->memory_v += sizeof( Edge*) * length +
                                sizeof( Edge) * length * length+
                                sizeof( int) * length;
        if( emp_stat_v->memory_v > emp_stat_v->memory_max)
            emp_stat_v->memory_max = emp_stat_v->memory_v;
        /*s*/
    }
    ~Sanity()
    {

        // delete edge_table
        if(edge_table != NULL){
            for (int i = 1; i < length; i++)
            {
                if (edge_table[i] != NULL){delete[] edge_table[i];edge_table[i]= NULL;}
            }
        }
        if(edge_table != NULL){
            delete[] edge_table;
            edge_table = NULL;
            
            /*s*/
            emp_stat_v->memory_v -= sizeof( Edge*) * length +
                                    sizeof( Edge) * length * length+
                                    sizeof( int) * length;
            /*s*/
        }
        if (steps != NULL){steps->~list<int>();steps=NULL;}
    }

    // the main algorithm
    int call_algorithm()
    {
        int l = 1;
        while (state)
        {       
            for (list<int>::iterator it = steps[l - 1].begin(); it != steps[l - 1].end(); ++it)
            {
                int i = *it;
                int j = i + 1;

                while(j <= length){

                    if (j==length && edge_table[i][j].in_graph ){
                        state = false;
                        tra[j].parent = i;
                        tra[j].visited = tra[i].visited + 1;
                        return l;  
                    }

                    if(tra[j].visited == -1 && edge_table[i][j].in_graph){
                        steps[l].push_back(j);
                        tra[j].parent = i;
                        tra[j].visited = tra[i].visited + 1;
                    }
                    j++;
                }
            }
            l = l + 1;
        }
        
    }
    
};

#endif /* SANITY_H_ */

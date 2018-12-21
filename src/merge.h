/*
 * merge.h
 *
 *  Created on: Dec, 2018
 *      Author: Mei
 */

#ifndef MERGE_H_
#define MERGE_H_

#include <cstdlib>
#include "Const.h"
#include <algorithm> 
#include <limits>
#include <iostream>
using namespace std;
extern emp_stat_t* emp_stat_v;
// x_coo and y_coo are coordinates of the vector

typedef struct group_node
{
	int					sta_seg_id;
	int					end_seg_id;
	struct group_node*	next;
	struct group_node*	pre;

}	group_node_t;

typedef struct group_list
{
	int				size;
	group_node_t*	head;
	group_node_t*	rear;

}	group_list_t;

class Merge
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

    Merge(Point *input_tra, int tra_length, double error)
    {
        tra = input_tra;
        length = tra_length;
        err = error;
    }

    // the main algorithm
    int call_algorithm()
    {
        int i, sta, end;
        double error_min, error_v, v_diff;

        group_node_t* merge_loc, *group_node_v1, *group_node_v2, *group_node_v;
        group_list_t* group_list_v;

        if( length == 0)
            return 0;
        
        //Initialize the group_list_t struct.
        group_list_v = group_list_alloc( );

        for( i=1; i<=length; i++)
        {
            append_group_list_entry( group_list_v, i, i+1);
        }
        
        while( true)
        {
            //Finding the merge location.
            error_min = std::numeric_limits<double>::max();
            merge_loc = NULL;

            group_node_v1 = group_list_v->head->next;
            while( group_node_v1->next != NULL)
            {
                group_node_v2 = group_node_v1->next;

                //Compute the error of the merged group.
                sta = group_node_v1->sta_seg_id;
                end = group_node_v2->end_seg_id;
                error_v = 0;
                for( int k=sta; k<end; k++)
                {
                    //error_v = check_error( R_array_v->array[ k], &R_node_v);
                    v_diff = util::calc_v_diff( tra[sta],tra[end],tra[k],tra[k+1]);

                    if( v_diff > error_v)
                    {
                        error_v = v_diff;
                    }
                }

                if( error_v <= err && error_v < error_min)
                {
                    error_min = error_v;
                    merge_loc = group_node_v1;
                }

                group_node_v1 = group_node_v2;
            }

            if( merge_loc == NULL)
                break;

            //Perform the merge operation.
            group_node_v = merge_loc->next;

            merge_loc->end_seg_id = group_node_v->end_seg_id;
            merge_loc->next = group_node_v->next;
            group_list_v->size --;

            if( group_list_v->rear == group_node_v)
                group_list_v->rear = merge_loc;

            free( group_node_v);
            /*s*/
            emp_stat_v->memory_v -= sizeof( group_node_t);
            /*s*/
        }
        
        
        group_node_v = group_list_v->head->next;
        int index = 1;
        int step = -1;
        //cout<<endl<<"----"<<endl;
        while( group_node_v != NULL)
        {
            tra[index].parent = group_node_v->sta_seg_id;
            index = group_node_v->sta_seg_id;
            //cout<< index<<" ";
            step++;
            group_node_v = group_node_v->next;
        }
        if(index<length){
            tra[index].parent = length;
            //cout<<length;
            step++;
        }
        //cout<<endl;
        
        //Release the resource.
	    group_list_release( group_list_v);
        

        return step;
    }


    group_list_t* group_list_alloc( )
    {
        group_list_t* group_list_v = new group_list_t();
        group_list_v->head = new group_node_t();
        group_list_v->rear = group_list_v->head;
        /*s*/
        emp_stat_v->memory_v += sizeof( group_list_t) + sizeof( group_node_t);
        if( emp_stat_v->memory_v > emp_stat_v->memory_max)
            emp_stat_v->memory_max = emp_stat_v->memory_v;
        /*s*/
        return group_list_v;
    }
    
    void append_group_list_entry( group_list_t* group_list_v, int sta_seg_id, int end_seg_id)
    {
        group_node_t* group_node_v;

        group_node_v =  new group_node_t(); 
        group_node_v->sta_seg_id = sta_seg_id;
        group_node_v->end_seg_id = end_seg_id;

        group_list_v->rear->next = group_node_v;
        group_node_v->pre = group_list_v->rear;
        group_list_v->rear = group_node_v;
        group_list_v->size ++;
        /*s*/
        emp_stat_v->memory_v += sizeof( group_node_t);
        if( emp_stat_v->memory_v > emp_stat_v->memory_max)
            emp_stat_v->memory_max = emp_stat_v->memory_v;
        /*s*/
        return;
    }

    void group_list_release( group_list_t* group_list_v)
    {
        group_node_t* group_node_v1, *group_node_v2;

        group_node_v1 = group_list_v->head;
        while( group_node_v1 != NULL)
        {
            group_node_v2 = group_node_v1->next;
            
            free( group_node_v1);
            /*s*/
            emp_stat_v->memory_v -= sizeof( group_node_t);
            /*s*/
            group_node_v1 = group_node_v2;
        }

        free( group_list_v);
        /*s*/
        emp_stat_v->memory_v -= sizeof( group_list_t);
        /*s*/
        return;
    }


};

#endif /* MERGE_H_ */

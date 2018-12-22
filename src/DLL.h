/*
 * DLL.h
 *
 *  Created on: Dec, 2018
 *      Author: Bruce
 */

#ifndef DLLIST
#define DLLIST

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "Const.h"

using namespace std;
extern emp_stat_t* emp_stat_v;


/*
 * Node Declaration
 */
struct Node
{
    Arc* info=NULL;
    Node* next=NULL;
    Node* prev=NULL;
};

/*
 Class Declaration
 */
class DLL 
{
    public:
        Node** ref_array; // reference array
        Node* head;
        //node* insert_node;
        Node* delete_head;
        Arc* arc_table;
        Point R;
        Point* tra;
        bool update;
        bool isPoint;
        bool empty;
        int n; //length
        int row_index;
        double err;
        DLL(){};
        DLL(Arc* table, int length, int i, Point* trajectory, double error)
        {
            ref_array = NULL;
            head = NULL;
            //insert_node = NULL;
            delete_head = NULL;
            arc_table = table;
            tra = trajectory;
            update = false;
            isPoint = false;
            empty = true;
            n = length;
            row_index = i;
            err = error;
            initialize();
        }
        ~DLL(){
            
            if (head != NULL){
                clear(head);
                head = NULL;
            }
            if (delete_head != NULL){
                clear(delete_head);
                delete_head = NULL;
            }
            if (ref_array != NULL){
                delete[] ref_array;
                /*s*/
                emp_stat_v->memory_v -= sizeof( Node*) * (n-row_index+1);
                /*s*/
                ref_array = NULL;
            }
            // //printf("DLL is destructed\n\n");
        }

        void initialize(){
            //printf("\nstart initializing circular doubly linked list and reference array\n");
            int i = row_index;
            // if the first arc is empty, no dll and lra will be constructed 
            if(arc_table[i+1].not_empty == false){
                //printf("arc_table [%d] is empty, no doubly linked list and reference array will be constructed\n", i+1);
                isPoint = false;
                empty = true;
                update = false;
                return;
            }

            Arc* arc1 = new Arc;
            *arc1 = arc_table[i];
            Arc* arc2 = new Arc;
            *arc2 = arc_table[i+1];
            /*s*/
            emp_stat_v->memory_v += sizeof( Arc) * 2;
            if( emp_stat_v->memory_v > emp_stat_v->memory_max)
                emp_stat_v->memory_max = emp_stat_v->memory_v;
            /*s*/
            
            if(Const::areSameAngle(arc2->theta1, arc2->theta2)){
                // a point
                //printf("arc_table[%d] is a point, no doubly linked list and reference array will be constructed\n", i+1);
                isPoint = true;
                empty = true;
                update = false;
                R.x_coo = arc2->point1_x;
                R.y_coo = arc2->point1_y;
                delete arc1;
                delete arc2;
                /*s*/
                emp_stat_v->memory_v -= sizeof( Arc) * 2;
                /*s*/
                return;
            }
            else{
                isPoint = false;
                empty = false;
                update = true;
            }

            // construct an array with (n-i) elements
            ref_array = new Node*[n-i+1];
            /*s*/
            emp_stat_v->memory_v += sizeof( Node*) * (n-row_index+1);
            if( emp_stat_v->memory_v > emp_stat_v->memory_max)
                emp_stat_v->memory_max = emp_stat_v->memory_v;
            /*s*/
            
            //printf("construct a reference array with %d elements\n", n-i);
            for(int j=0; j<(n-i+1);j++){
                ref_array[j] = NULL;
            }

            if(Const::areSameAngle(arc2->theta1,0) && Const::areSameAngle(arc2->theta2,360)){
                // a disk
                //printf("arc_table[%d] is a disk, initialize a circular doubly linked list with one node\n", i+1);
                head = new Node;
                /*s*/
                emp_stat_v->memory_v += sizeof( Node);
                if( emp_stat_v->memory_v > emp_stat_v->memory_max)
                    emp_stat_v->memory_max = emp_stat_v->memory_v;
                /*s*/
                head->info = arc1;
                head->prev = head;
                head->next = head;
                ref_array[1] = head;                
                delete arc2;
                /*s*/
                emp_stat_v->memory_v -= sizeof( Arc);
                /*s*/
            }
            else{
                //printf("arc_table[%d] is a normal arc, initialize a circular doubly linked list with two nodes\n", i+1);
                head = new Node;
                head->info = arc1;
                Node* second_node = new Node;
                /*s*/
                emp_stat_v->memory_v += sizeof( Node)*2;
                if( emp_stat_v->memory_v > emp_stat_v->memory_max)
                    emp_stat_v->memory_max = emp_stat_v->memory_v;
                /*s*/
                second_node->info = arc2;
                head->prev = second_node;
                head->next = second_node;
                second_node->prev = head;
                second_node->next = head;
                ref_array[1] = head;
                ref_array[2] = second_node;
                //printf("===Head===\n");
                //print_head();
            }
            //printf("finish initializing circular doubly linked list and reference array\n\n");
        }



        void updateNode(int j){
            int i = row_index;
            //printf("updating I[%d][%d]\n", i, j);

            bool delete_head_execution = clear(delete_head);
            if(delete_head_execution){
                //printf("delete_head is not NULL, clear delete_head\n");
            }
            else{
                //printf("delete_head is NULL, no need to clear delete_head\n");
            }

            update = false;

            // check whether I[i][j-1] is a point
            if(isPoint){
                //printf("I[%d][%d] is a point, check whether point R is in disk %d\n", i, j-1, j);
                bool inside = point_in_circle(&R,j);
                if(inside){
                    //printf("R is in disk %d\n", j);
                    isPoint = true;
                    update = false;
                }
                else{
                    //printf("R is not in disk %d\n", j);
                    isPoint = false;
                    update = false;
                    empty = true;
                }
                return;
            }

            // check whether the current linked list of I[i][j-1] is empty
            if(empty){
                // the current linked list is empty and there will be no update to the linked list and reference array
                //printf("the current linked list is empty, there will be no update to the linked list and reference array\n");
                update = false;
                return;
            }

            //printf("loading arc_table[%d]\n", j);
            Arc* arc_j_i = new Arc;
            *arc_j_i  = arc_table[j];
            /*s*/
            emp_stat_v->memory_v += sizeof( Arc);
            if( emp_stat_v->memory_v > emp_stat_v->memory_max)
                emp_stat_v->memory_max = emp_stat_v->memory_v;
            /*s*/

            // check whether arc_table[j] x is empty
            if(arc_j_i->not_empty == false){
                //printf("arc_table[%d] is empty, there are two cases:\n", j);
                Point end_point;
                end_point.x_coo = head->info->point1_x;
                end_point.y_coo = head->info->point1_y;
                bool inside = point_in_circle(&end_point, j);// need check
                                
                if(inside){
                    //printf("case1: the end point is inside disk %d, there will be no update to the linked list and reference array\n", j);
                    update = false;
                }
                else{
                    //printf("case2: the end point is outside disk %d, I[%d][%d] is empty, delete linked list and reference array\n", j, i, j);
                    //printf("save point R before deleting head\n");
                    R.x_coo = (head->info->point1_x + head->info->point2_x)/2;
                    R.y_coo = (head->info->point1_y + head->info->point2_y)/2;

                    // delete the linked list                    
                    delete_head = head;
                    head = NULL;

                    empty = true;
                    update = true;
                }
                delete arc_j_i;
                /*s*/
                emp_stat_v->memory_v -= sizeof( Arc);
                /*s*/
                return;
            }

            // check whether arc_table[j] is [0, 360]
            if(Const::areSameAngle(arc_j_i->theta1,0) && Const::areSameAngle(arc_j_i->theta2,360)){
                // a disk
                //printf("arc_table[%d] is a disk, there will be no update to the linked list and reference array\n", j);
                update = false;

                delete arc_j_i;
                /*s*/
                emp_stat_v->memory_v -= sizeof( Arc);
                /*s*/
                return;
            }

            // check whether arc_table[j][i].theta1 = arc_table[j][i].theta2
            if(Const::areSameAngle(arc_j_i->theta1, arc_j_i->theta2)){
                //printf("arc_table[%d] is a point, there are two cases:\n", j);

                Point end_point_1;
                end_point_1.x_coo = head->info->point1_x;
                end_point_1.y_coo = head->info->point1_y;
                Point end_point_2;
                end_point_2.x_coo = head->info->point2_x;
                end_point_2.y_coo = head->info->point2_y;
                Point mid_point;
                mid_point.x_coo = (end_point_1.x_coo+end_point_2.x_coo)/2;
                mid_point.y_coo = (end_point_1.y_coo+end_point_2.y_coo)/2;
                bool inside =point_in_circle(&mid_point, j);// need check

                if(inside){
                    //printf("case1: the mid point is inside disk %d, there will be no update to the linked list and reference array\n", j);
                    update = false;
                }
                else{
                    // a point;
                    //printf("case2: the mid point is outside disk %d, I[%d][%d] becomes a point\n", j, i, j);
                    //printf("save point R before deleting head\n");
                    R.x_coo = arc_j_i->point1_x;
                    R.y_coo = arc_j_i->point1_y;
                    
                    // delete the linked list
                    delete_head = head;
                    head = NULL;
                    isPoint = true;
                    empty = true;
                    update = true;
                }
                delete arc_j_i;
                /*s*/
                emp_stat_v->memory_v -= sizeof( Arc);
                /*s*/
                return;
            }

            // general case
            //printf("arc_table[%d] is a normal arc\n", j);

            Node* insert_node = new Node;
            /*s*/
            emp_stat_v->memory_v += sizeof( Node);
            if( emp_stat_v->memory_v > emp_stat_v->memory_max)
                emp_stat_v->memory_max = emp_stat_v->memory_v;
            /*s*/
            //printf("===Head===\n");
            //print_head();
            insert_node->info = arc_j_i;
            insert_node->prev = insert_node;
            insert_node->next = insert_node;
            ref_array[j-i+1] = insert_node;

            Point p1;
            p1.x_coo = arc_j_i->point1_x;
            p1.y_coo = arc_j_i->point1_y;

            Point p2;
            p2.x_coo = arc_j_i->point2_x;
            p2.y_coo = arc_j_i->point2_y;

            int k1 = arc_j_i->disk1_num;
            int k2 = arc_j_i->disk2_num;

            Node* node_k1 = ref_array[k1-i+1];
            Node* node_k2 = ref_array[k2-i+1];

            if (node_k1 == NULL || node_k2 == NULL){
                // //printf("in order to prevent segmentation fault, we skip this case\n");
                delete arc_j_i;
                insert_node->info = NULL;
                delete insert_node;
                ref_array[j-i+1] = NULL;
                update = false;
                delete_head = NULL;
                /*s*/
                emp_stat_v->memory_v -= sizeof( Arc) + sizeof( Node);
                /*s*/
                return;
            }
            //printf("%d, %d\n", k1, k2);
            //printf("%f, %f\n", arc_j_i->theta1, arc_j_i->theta2);
            //printf("(%f, %f)\n", p1.x_coo, p1.y_coo);
            //printf("(%f, %f)\n", p2.x_coo, p2.y_coo);
            // cout << (node_k1 == NULL)<<"\n";
            // cout << (node_k2 == NULL)<<"\n";

            // we need to consider different situations for delete_head
            // if (node_k1->next == node_k2->prev){
            //     if (i==1 && j==3){
            //         ////printf("hahahahaha\n");
            //     ////printf("%f, %f, %d\n", p1.x_coo, node_k1->next->info->point1_x, Const::areSame(p1.x_coo, node_k1->next->info->point1_x));
            //     ////printf("%d\n", Const::areSame(p1.y_coo, node_k1->next->info->point1_y));
            //     ////printf("%f, %f, %d\n", p2.x_coo, node_k1->next->info->point2_x, Const::areSame(p2.x_coo, node_k1->next->info->point2_x));
            //     ////printf("%d\n", Const::areSame(p2.y_coo, node_k1->next->info->point2_y));
            //     ////printf("%d\n", Const::areSame(arc_j_i->center_x, node_k1->next->info->center_x));
            //     ////printf("%d\n", Const::areSame(arc_j_i->center_y, node_k1->next->info->center_y));
            //         exit(0);
            //     }
            // }
            if (node_k1->next == node_k2->prev &&\
                Const::areSame(p1.x_coo, node_k1->next->info->point1_x) &&\
                Const::areSame(p1.y_coo, node_k1->next->info->point1_y) &&\
                Const::areSame(p2.x_coo, node_k1->next->info->point2_x) &&\
                Const::areSame(p2.y_coo, node_k1->next->info->point2_y) &&
                Const::areSame(arc_j_i->center_x, node_k1->next->info->center_x) &&\
                Const::areSame(arc_j_i->center_y, node_k1->next->info->center_y)
                ){
                delete arc_j_i;
                insert_node->info = NULL;
                delete insert_node;
                ref_array[j-i+1] = NULL;
                update = false;
                delete_head = NULL;
                /*s*/
                emp_stat_v->memory_v -= sizeof( Arc) + sizeof( Node);
                /*s*/
                //printf("two circles coincide, nothing needs to do\n");
                // if (i==1 && j==3){exit(0);}
                return;
            }

            else if (node_k1->next == node_k2) {
                //printf("arc_table[%d] has two intersection points with two adjacent arcs\n", j);
                delete_head = NULL;
                // circle coincide
                if ((Const::areSame(p1.x_coo, node_k1->info->point1_x) &&\
                     Const::areSame(p1.y_coo, node_k1->info->point1_y) &&\
                     Const::areSame(p2.x_coo, node_k2->info->point1_x) &&\
                     Const::areSame(p2.y_coo, node_k2->info->point1_y) &&\
                     Const::areSame(arc_j_i->center_x, node_k1->info->center_x) &&\
                     Const::areSame(arc_j_i->center_y, node_k1->info->center_y)) ||\
                    (Const::areSame(p1.x_coo, node_k1->info->point2_x) &&\
                     Const::areSame(p1.y_coo, node_k1->info->point2_y) &&\
                     Const::areSame(p2.x_coo, node_k2->info->point2_x) &&\
                     Const::areSame(p2.y_coo, node_k2->info->point2_y) &&\
                     Const::areSame(arc_j_i->center_x, node_k2->info->center_x) &&\
                     Const::areSame(arc_j_i->center_x, node_k2->info->center_x))){
                    delete arc_j_i;
                    insert_node->info = NULL;
                    delete insert_node;
                    ref_array[j-i+1] = NULL;
                    update = false;
                    delete_head = NULL;
                    /*s*/
                    emp_stat_v->memory_v -= sizeof( Arc) + sizeof( Node);
                    /*s*/
                    //printf("two circles coincide, nothing needs to do\n");
                    //printf("\n\n\nbut this case is soooooooooo strange!!!\n\n\n");
                    // exit(0);
                    return;
                }
            }
            else{
                //printf("arc_table[%d] has two intersection points with two normal arcs\n", j);
                delete_head = node_k1->next;
                node_k1->next->prev = node_k2->prev;                
                node_k2->prev->next = node_k1->next;
            }
                
            if(delete_head != NULL){
                //printf("updating reference array\n");
                //printf("===Delete Head===\n");
                print_delete_head();
                for(Node* ptr = delete_head;;){
                    int k = ptr->info->disk_num;
                    ref_array[k-i+1] = NULL;
                    ptr = ptr->next;
                    if (ptr == delete_head){
                        break;
                    }
                }
            }

            head = insert_node;

            node_k1->info->point2_x = p1.x_coo;
            node_k1->info->point2_y = p1.y_coo;
            node_k1->info->theta2 = Const::angle(node_k1->info->point2_x-node_k1->info->center_x, node_k1->info->point2_y-node_k1->info->center_y);
            node_k1->info->disk2_num = j;
            node_k1->next = insert_node;
            insert_node->prev = node_k1;

            node_k2->info->point1_x = p2.x_coo;
            node_k2->info->point1_y = p2.y_coo;
            node_k2->info->theta1 = Const::angle(node_k2->info->point1_x-node_k2->info->center_x, node_k2->info->point1_y-node_k2->info->center_y);
            node_k2->info->disk1_num = j;
            node_k2->prev = insert_node;
            insert_node->next = node_k2;

            update = true;
            //printf("===Head===\n");
            //print_head();
            return;
        }

        void print_head(){
            if(head != NULL){
                for(Node* ptr = head;;){
                    int k = ptr->info->disk_num;
                    double theta1 = ptr->info->theta1;
                    double theta2 = ptr->info->theta2;
                    double point1_x = ptr->info->point1_x;
                    double point1_y = ptr->info->point1_y;
                    double point2_x = ptr->info->point2_x;
                    double point2_y = ptr->info->point2_y;
                    double center_x = ptr->info->center_x;
                    double center_y = ptr->info->center_y;
                    //printf("Disk: %d, (%f, %f), (%f, %f), (%f, %f), [%f, %f]\n", k, point1_x, point1_y, point2_x, point2_y, center_x, center_y, theta1, theta2);
                    ptr = ptr->next;
                    if (ptr == head){
                        break;
                    }
                }
            }
        }

        void print_delete_head(){
            if(delete_head != NULL){
                for(Node* ptr = delete_head;;){
                    int k = ptr->info->disk_num;
                    double theta1 = ptr->info->theta1;
                    double theta2 = ptr->info->theta2;
                    double point1_x = ptr->info->point1_x;
                    double point1_y = ptr->info->point1_y;
                    double point2_x = ptr->info->point2_x;
                    double point2_y = ptr->info->point2_y;
                    double center_x = ptr->info->center_x;
                    double center_y = ptr->info->center_y;
                    //printf("Disk: %d, (%f, %f), (%f, %f), (%f, %f), [%f, %f]\n", k, point1_x, point1_y, point2_x, point2_y, center_x, center_y, theta1, theta2);
                    ptr = ptr->next;
                    if (ptr == delete_head){
                        break;
                    }
                }
            }
        }

        bool clear(Node*& head_ptr){
            if(head_ptr != NULL){
                Node* ptr = head_ptr;
                ptr-> prev -> next = NULL;
                for(; ptr != NULL;){
                    delete ptr->info;
                    ptr->info = NULL;
                    Node* temp = ptr;
                    ptr = ptr->next;
                    delete temp;
                    /*s*/
                    emp_stat_v->memory_v -= sizeof( Arc) + sizeof( Node);
                    /*s*/
                    temp = NULL;
                }
                head_ptr = NULL;
                return true;
            }
            else{
                return false;
            }
        }
        // j is the disk number
        bool point_in_circle(Point* R, int j){
            Point p1 = tra[j];
            Point p2 = tra[j+1];
            double t1 = p1.time;
            double t2 = p2.time;
            double dt = t2-t1;
            double v_x = (p2.x_coo-p1.x_coo)/dt;
            double v_y = (p2.y_coo-p1.y_coo)/dt;
            bool inside = (((R->x_coo-v_x)*(R->x_coo-v_x)+(R->y_coo-v_y)*(R->y_coo-v_y)) <= err*err);
            if(inside){
                return true;
            }
            else{
                return false;
            }
        }

        bool isEmpty(){
            return empty;
        }


        bool get_update(){
            return update;
        }

        bool get_isPoint(){
            return isPoint;
        }

        bool isIEmpty(){
            return (empty && (!isPoint));
        }

        void* get_delete_head(){
            return delete_head;
        }

        Point get_R(){
            if (! empty){
                R.x_coo = (head->info->point1_x + head->info->point2_x)/2;
                R.y_coo = (head->info->point1_y + head->info->point2_y)/2;
            }
            return R;
        }

};


#endif /* DLLIST_H_ */

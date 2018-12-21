/*
 * BinarySearch.h
 *
 *  Created on: Dec, 2018
 *      Author: Mei
 */


#ifndef _BINARYSEARCH_HPP_
#define _BINARYSEARCH_HPP_


#include "DLL.h"
#include "AVLtree.h"
#include "Const.h"
#include <cstdlib>

extern emp_stat_t* emp_stat_v;

class BinarySearchTree 
{
public:
    BinarySearchTree(){};
    BinarySearchTree(int i, int n, double e, DLL* dll, Point* r, Arc* a_t);
    ~BinarySearchTree();
    bool update_node(int disk, double al,int model);
    bool delete_node(int j);
    bool insert(interval data, int ref);
    bool lookup(interval data, double & x, double & y);
    bool update(int j);
    bool check(double x, double y,double angle);
    bool clear();
private:
    AVLtree<interval>* tree;  
    AVLnode<interval>** ref_array;
    DLL* dllist;
    Point* R;
    Arc* arc_table;
    int index;
    int point_num;
    bool isPoint;
    double err;
};


BinarySearchTree::~BinarySearchTree()
{
    clear();    
}

BinarySearchTree::BinarySearchTree(int i, int n, double e, DLL* dll, Point* r, Arc* a_t):
index(i),point_num(n), err(e), dllist(dll), R(r), arc_table(a_t){
    isPoint = false;
    ref_array = NULL;
    tree = NULL;
    if(dllist->get_isPoint()){
        isPoint=true;
        return;
    }
    if(dllist->isEmpty()){
        return;
    }
    //cout<<"R:"<<R->x_coo<<", "<<R->y_coo<<"=======\n";
    ref_array= new AVLnode<interval>* [2*(point_num-index)];
    tree = new AVLtree<interval>;

    /*s*/
    emp_stat_v->memory_v += sizeof( AVLnode<interval>*) * 2*(point_num-index)+sizeof( AVLnode<interval>);
    if( emp_stat_v->memory_v > emp_stat_v->memory_max)
        emp_stat_v->memory_max = emp_stat_v->memory_v;
    /*s*/

    for(int i=0;i<2*(point_num-index);i++) ref_array[i]=NULL;
    if(Const::areSameAngle(arc_table[index+1].theta1,0) && Const::areSameAngle(arc_table[index+1].theta2,360)){

        arc_table[index+1].compute_angle(R);
        interval data1(0,arc_table[index+1].alpha1,arc_table[index+1].center_x,arc_table[index+1].center_y);
        interval data2(arc_table[index+1].alpha1,360,arc_table[index+1].center_x,arc_table[index+1].center_y);
        insert(data1,0);
        insert(data2,1);  
        return;
    }
    if(Const::areSameAngle(arc_table[index+1].theta1,arc_table[index+1].theta2)){
          isPoint=true;
          return;
    }
    arc_table[index+1].compute_angle(R);
    arc_table[index].compute_angle(R);
    
    //computer interval belongs to which disk
    double dx_i = arc_table[index].compute_distance(R);
    double dx_i_add_one = arc_table[index+1].compute_distance(R);

    double angle1 = Const::min(arc_table[index+1].alpha1,arc_table[index+1].alpha2);
    double angle2 = Const::max(arc_table[index+1].alpha1,arc_table[index+1].alpha2);
    if(Const::areSame(dx_i,dx_i_add_one)){
        if(arc_table[index].alphaMid<arc_table[index+1].alphaMid){
            interval data1(0,180,arc_table[index].center_x,arc_table[index].center_y);
            insert(data1,0);
            interval data2(180,360,arc_table[index+1].center_x,arc_table[index+1].center_y);
            insert(data2,2);
        }
        else{
            interval data1(0,180,arc_table[index+1].center_x,arc_table[index+1].center_y);
            insert(data1,2);
            interval data2(180,360,arc_table[index].center_x,arc_table[index].center_y);
            insert(data2,0);
        }
    }
    else if(dx_i<dx_i_add_one){
        interval data1(0,angle1,arc_table[index].center_x,arc_table[index].center_y);
        insert(data1,0);
        interval data2(angle1,angle2,arc_table[index+1].center_x,arc_table[index+1].center_y);
        insert(data2,2);
        interval data3(angle2,360,arc_table[index].center_x,arc_table[index].center_y);
        insert(data3,1);
    }
    else{
        interval data1(0,angle1,arc_table[index+1].center_x,arc_table[index+1].center_y);
        insert(data1,2);
        interval data2(angle1,angle2,arc_table[index].center_x,arc_table[index].center_y);
        insert(data2,0);
        interval data3(angle2,360,arc_table[index+1].center_x,arc_table[index+1].center_y);
        insert(data3,3);
    } 
}

bool BinarySearchTree::insert(interval data, int ref)
{
    bool result = tree->insert(data);
    //cout<<"++++++++++"<<data;
    //tree->printBalance();
    if (result){
        ref_array[ref] = tree->insert_ptr;
        return true;
    }
    else{
        ref_array[ref] = NULL;
        return false;
    }
}

bool BinarySearchTree::check(double x, double y,double angle)
{     
    //cout<<"R:"<<R->x_coo<<", "<<R->y_coo<<"\n";
    //cout<<angle<<"\n";
    if (((R->x_coo-x)*(R->x_coo-x)+(R->y_coo-y)*(R->y_coo-y))>4*err*err)
    {
        return false;
    }
    if(isPoint){
        if(Const::areSame(R->x_coo,x)&&Const::areSame(R->y_coo,y)) {return true;}
        return false;
    }
    double c_x;
    double c_y;
    interval p(angle,angle);
    lookup(p,c_x,c_y);
    //tree->printBalance();
    //cout<<"P:"<<x<<", "<<y<<"\n";
    //cout<<"C:"<<c_x<<", "<<c_y<<"\n";
    //cout<<"r:"<<err<<"\n";
    if (((c_x-x)*(c_x-x)+(c_y-y)*(c_y-y))>err*err)
    {
        return false;
    }
    return true;
}

bool BinarySearchTree::lookup(interval data, double & x, double & y)
{
    if (tree->root == NULL)
    {
        return false;
    }
    AVLnode<interval> *current = tree->root;
    AVLnode<interval> *parent;
    while (current != NULL)
    {
        if (current->key == data)
        {    
            interval temp =current->key;
            x=temp.get_x(); 
            y=temp.get_y(); 
            return true;
        }

        if(data < current->key)
        {
            current = current->left;
        }else{
            current = current->right;
        }
    }

    return false;
}


bool BinarySearchTree::update(int j)
{
    if (dllist->get_isPoint())
    {
        isPoint = true;
        clear();
        return true;
    }
    if (dllist->isEmpty())
    {
        clear();
        return true;
    }
    if (!dllist->update)
    {
        return true;
    }
    bool result = true;

    Node* it = dllist->delete_head;
    int disk1=arc_table[j].disk1_num;
    int disk2=arc_table[j].disk2_num;
    if(it!=NULL){
        int disk_num = it->info->disk_num;
    
        delete_node(2*(disk_num-index));
        delete_node(2*(disk_num-index)+1);
        it=it->next;
        while(it!=dllist->delete_head){
            int disk_num = it->info->disk_num;
            delete_node(2*(disk_num-index));
            delete_node(2*(disk_num-index)+1);
            it=it->next;
        }
    }
    arc_table[j].compute_angle(R);
    //cout<<"disk1 "<<disk1<<"\n";
    //cout<<"disk2 "<<disk2<<"\n";
    //cout<<"alpha1 "<<arc_table[j].alpha1<<"\n";
    //cout<<"alphaMid "<<arc_table[j].alphaMid<<"\n";
    //cout<<"alpha2 "<<arc_table[j].alpha2<<"\n";
                

    if(arc_table[j].alpha1 < arc_table[j].alpha2){
        result = result && update_node(disk1, arc_table[j].alpha1,1);
        result = result && update_node(disk2, arc_table[j].alpha2,2);
        interval data1(arc_table[j].alpha1,arc_table[j].alpha2,arc_table[j].center_x,arc_table[j].center_y);
        result = result && insert(data1,2*(j-index));
    }
    else{
        result = result && update_node(disk1, arc_table[j].alpha1,1);
        result = result && update_node(disk2, arc_table[j].alpha2,2);
        interval data1(arc_table[j].alpha1,360,arc_table[j].center_x,arc_table[j].center_y);
        result = result && insert(data1,2*(j-index)+1);
        interval data2(0,arc_table[j].alpha2,arc_table[j].center_x,arc_table[j].center_y);
        result = result && insert(data2,2*(j-index));
    }  
    return result;  
}

bool BinarySearchTree::delete_node(int ref)
{
    if(ref_array[ref]==NULL){
      return true;
    }
    interval data = ref_array[ref]->key;
    //cout<<"~~~~~~~~~~~"<<data;

    tree->deleteKeyByReference(ref_array[ref]);
    //tree->printBalance();
        
    ref_array[ref]=NULL;
    return true;
}

bool BinarySearchTree::clear()
{
    if(ref_array!=NULL){
        delete[] ref_array;
        ref_array = NULL;
        /*s*/
        emp_stat_v->memory_v -= sizeof( AVLnode<interval>*) * 2*(point_num-index);
        /*s*/
    }
    if(tree!=NULL){
        tree->~AVLtree();
        tree = NULL;
    }
    return true;
}

bool BinarySearchTree::update_node(int disk, double al,int model)
{  
    if(model==1){
        if(ref_array[2*(disk-index)+1] == NULL){
            if(ref_array[2*(disk-index)]==NULL){
                return false;
            }
            ref_array[2*(disk-index)]->key.theta_2=al;
            //cout<<">>>>>>>>1 "<<ref_array[2*(disk-index)]->key;
        }
        else
        {   if(ref_array[2*(disk-index)+1]==NULL){
                return false;
            }
            if(ref_array[2*(disk-index)+1]->key.theta_1 <= al+Const::EPSILION_ANGLE){ 
                ref_array[2*(disk-index)+1]->key.theta_2=al;
                delete_node(2*(disk-index));
                ref_array[2*(disk-index)] = ref_array[2*(disk-index)+1];
                ref_array[2*(disk-index)+1] = NULL;
                //cout<<">>>>>>>>2 "<<ref_array[2*(disk-index)]->key;
            }
            else{
                if(ref_array[2*(disk-index)]==NULL){
                    return false;
                }
                ref_array[2*(disk-index)]->key.theta_2=al;
                //cout<<">>>>>>>>3 "<<ref_array[2*(disk-index)]->key;
            }
        }
    }
    else{
        if(ref_array[2*(disk-index)+1] == NULL){
            if(ref_array[2*(disk-index)]==NULL){
                return false;
            }
            ref_array[2*(disk-index)]->key.theta_1=al;
            //cout<<">>>>>>>>4 "<<ref_array[2*(disk-index)]->key;
        }
        else
        {   if(ref_array[2*(disk-index)]==NULL){
                return false;
            }
            if( al<=ref_array[2*(disk-index)]->key.theta_2+Const::EPSILION_ANGLE ){ 
                ref_array[2*(disk-index)]->key.theta_1=al;
                delete_node(2*(disk-index)+1);
                //cout<<">>>>>>>>5 "<<ref_array[2*(disk-index)]->key;
            }
            else{
                if(ref_array[2*(disk-index)+1]==NULL){
                    return false;
                }
                ref_array[2*(disk-index)+1]->key.theta_1=al;
                //cout<<">>>>>>>>6 "<<ref_array[2*(disk-index)+1]->key;
            }
        }
         
    } 
    //tree->printBalance();
    return true;
}

#endif //_BINARYSEARCH_HPP_

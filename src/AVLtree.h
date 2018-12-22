/*
 * AVLtree.h
 *
 *  Created on: Dec, 2018
 *      Author: Mei
 */


#ifndef _AVL_HPP_
#define _AVL_HPP_

#include <algorithm>
#include <iostream>
#include "Const.h"
extern emp_stat_t* emp_stat_v;
/* AVL node */
template <class T>
class AVLnode {
public:
    T key;
    int balance;
    AVLnode<T> *left, *right, *parent;
 
    AVLnode(T k, AVLnode *p) : key(k), balance(0), parent(p),
                        left(NULL), right(NULL) {}
 
    ~AVLnode() {
        if(left!=NULL){
            left->~AVLnode();
            delete left;
            /*s*/
            emp_stat_v->memory_v -= sizeof( AVLnode<T>);
            /*s*/
            left = NULL;
        }
        if(right!=NULL){
            right->~AVLnode();
            delete right;
            /*s*/
            emp_stat_v->memory_v -= sizeof( AVLnode<T>);
            /*s*/
            right = NULL;
        }
    }
};
 
/* AVL tree */
template <class T>
class AVLtree {
public:
    AVLnode<T> *insert_ptr;
    AVLtree(void);
    ~AVLtree();
    bool insert(T key);
    void deleteKey(const T key);
    void deleteKeyByReference(AVLnode<T>* ptr);
    void printBalance();
 

    AVLnode<T> *root;
 private:
    AVLnode<T>* rotateLeft          ( AVLnode<T> *a );
    AVLnode<T>* rotateRight         ( AVLnode<T> *a );
    AVLnode<T>* rotateLeftThenRight ( AVLnode<T> *n );
    AVLnode<T>* rotateRightThenLeft ( AVLnode<T> *n );
    AVLnode<T>* findMin             ( AVLnode<T> *n );
    void rebalance                  ( AVLnode<T> *n );
    int height                      ( AVLnode<T> *n );
    void setBalance                 ( AVLnode<T> *n );
    void printBalance               ( AVLnode<T> *n, int & i );
    void clearNode                  ( AVLnode<T> *n );
};
 
/* AVL class definition */
template <class T>
AVLtree<T>::~AVLtree() {
    if(root!=NULL){
        root->~AVLnode();
        delete root;
        /*s*/
        emp_stat_v->memory_v -= sizeof( AVLnode<T>);
        /*s*/
        root = NULL;
    }
}

template <class T>
void AVLtree<T>::rebalance(AVLnode<T> *n) {
    setBalance(n);
 
    if (n->balance == -2) {
        if (height(n->left->left) >= height(n->left->right))
            n = rotateRight(n);
        else
            n = rotateLeftThenRight(n);
    }
    else if (n->balance == 2) {
        if (height(n->right->right) >= height(n->right->left))
            n = rotateLeft(n);
        else
            n = rotateRightThenLeft(n);
    }
 
    if (n->parent != NULL) {
        rebalance(n->parent);
    }
    else {
        root = n;
    }
}
 
template <class T>
AVLnode<T>* AVLtree<T>::rotateLeft(AVLnode<T> *a) {
    AVLnode<T> *b = a->right;
    b->parent = a->parent;
    a->right = b->left;
 
    if (a->right != NULL)
        a->right->parent = a;
 
    b->left = a;
    a->parent = b;
 
    if (b->parent != NULL) {
        if (b->parent->right == a) {
            b->parent->right = b;
        }
        else {
            b->parent->left = b;
        }
    }
 
    setBalance(a);
    setBalance(b);
    return b;
}
 
template <class T>
AVLnode<T>* AVLtree<T>::rotateRight(AVLnode<T> *a) {
    AVLnode<T> *b = a->left;
    b->parent = a->parent;
    a->left = b->right;
 
    if (a->left != NULL)
        a->left->parent = a;
 
    b->right = a;
    a->parent = b;
 
    if (b->parent != NULL) {
        if (b->parent->right == a) {
            b->parent->right = b;
        }
        else {
            b->parent->left = b;
        }
    }
 
    setBalance(a);
    setBalance(b);
    return b;
}
 
template <class T>
AVLnode<T>* AVLtree<T>::rotateLeftThenRight(AVLnode<T> *n) {
    n->left = rotateLeft(n->left);
    return rotateRight(n);
}
 
template <class T>
AVLnode<T>* AVLtree<T>::rotateRightThenLeft(AVLnode<T> *n) {
    n->right = rotateRight(n->right);
    return rotateLeft(n);
}
 
template <class T>
int AVLtree<T>::height(AVLnode<T> *n) {
    if (n == NULL)
        return -1;
    return 1 + std::max(height(n->left), height(n->right));
}
 
template <class T>
void AVLtree<T>::setBalance(AVLnode<T> *n) {
    n->balance = height(n->right) - height(n->left);
}
 
template <class T>
void AVLtree<T>::printBalance(AVLnode<T> *n,int & i) {
    if (n != NULL && i<10) {
        printBalance(n->left,i);
        i++;
        std::cout << n->key ;
        printBalance(n->right,i);
        i++;
    }
}
 
template <class T>
AVLtree<T>::AVLtree(void) : root(NULL),insert_ptr(NULL) {}
 
template <class T>
bool AVLtree<T>::insert(T key) {
    if (root == NULL) {
        root = new AVLnode<T>(key, NULL);
        /*s*/
        emp_stat_v->memory_v += sizeof( AVLnode<T>);
        if( emp_stat_v->memory_v > emp_stat_v->memory_max)
            emp_stat_v->memory_max = emp_stat_v->memory_v;
        /*s*/
        insert_ptr = root;
    }
    else {
        AVLnode<T>
            *n = root,
            *parent;
 
        while (true) {
            if (!( n->key <= key || n->key >= key)){
                //printf("oops! Insert False happens\n");
                // cout<<key;
                return false;
            }
            parent = n;
 
            bool goLeft = n->key > key;
            n = goLeft ? n->left : n->right;
 
            if (n == NULL) {
                if (goLeft) {
                    parent->left = new AVLnode<T>(key, parent);
                    insert_ptr = parent->left;
                    /*s*/
                    emp_stat_v->memory_v += sizeof( AVLnode<T>);
                    if( emp_stat_v->memory_v > emp_stat_v->memory_max)
                        emp_stat_v->memory_max = emp_stat_v->memory_v;
                    /*s*/
                }
                else {
                    parent->right = new AVLnode<T>(key, parent);
                    insert_ptr = parent->right;
                    /*s*/
                    emp_stat_v->memory_v += sizeof( AVLnode<T>);
                    if( emp_stat_v->memory_v > emp_stat_v->memory_max)
                        emp_stat_v->memory_max = emp_stat_v->memory_v;
                    /*s*/
                }
 
                //rebalance(parent);
                break;
            }
        }
    }
 
    return true;
}
 
template <class T>
void AVLtree<T>::deleteKey(const T delKey) {
    if (root == NULL)
        return;
 
    AVLnode<T>
        *n       = root,
        *parent  = root,
        *delNode = NULL,
        *child   = root;
 
    while (child != NULL) {
        parent = n;
        n = child;
        child = delKey >= n->key ? n->right : n->left;
        if (delKey == n->key)
            delNode = n;
    }
 
    if (delNode != NULL) {
        //delNode->key = n->key;
 
        child = n->left != NULL ? n->left : n->right;
 
        if (root->key == delKey) {
            root = child;
        }
        else {
            if (parent->left == n) {
                parent->left = child;
            }
            else {
                parent->right = child;
            }
 
            rebalance(parent);
        }
        delete delNode;
        /*s*/
        emp_stat_v->memory_v -= sizeof( AVLnode<T>);
        /*s*/
    }
}

template <class T>
AVLnode<T>* AVLtree<T>::findMin(AVLnode<T>* n){
    if(n == NULL)
        return NULL;
    else if(n->left == NULL)
        return n;
    else
        return findMin(n->left);
}

template <class T>
void AVLtree<T>::deleteKeyByReference(AVLnode<T>* ptr){
    if(ptr == NULL)
        return ;
    AVLnode<T>* temp;
    //cout<<root->key;
    if(ptr->left && ptr->right)
    {  //cout<<">>>>>>>>> 1\n";
        temp = findMin(ptr->right);
        //cout<<temp->key;
        if(ptr->parent!= NULL){
            //cout<<">>>>>>>>> 2\n";
            if((ptr->parent->left!=NULL)&&(ptr->parent->left == ptr)){
                ptr->parent->left = temp;
                //cout<<">>>>>>>>> 3\n";
            }
            else{
                //cout<<">>>>>>>>> 4\n";
                ptr->parent->right = temp;
            }
        }
        else{
            //cout<<">>>>>>>>> 5\n";
            root = temp;
        } 
        if (temp->parent != NULL){
            //cout<<temp->parent->key;
        }
        if (temp->parent == ptr){
            //cout<<">>>>>>>>> 6\n";
            temp-> parent = ptr->parent;
            temp->left = ptr->left;
            ptr->left->parent = temp;
            //delete ptr;
            /*s*/
            emp_stat_v->memory_v -= sizeof( AVLnode<T>);
            /*s*/
            ptr = NULL;
            return;
        }
        if (temp->right != NULL){
            //cout<<">>>>>>>>> 7\n";
            temp-> right -> parent = temp->parent;
        }
        temp-> parent -> left = temp->right;

        temp->right = ptr->right;
        temp->left = ptr->left;
        temp-> parent = ptr->parent;
        ptr->right->parent = temp;
        ptr->left->parent = temp;
        //cout<<">>>>>>>>> 8\n";
    }
    else
    {//cout<<">>>>>>>>> 9\n";
        if(ptr->left != NULL){
            temp = ptr->left;
            //cout<<">>>>>>>>> 10\n";
        }
        else if(ptr->right != NULL){
            temp = ptr->right;
            //cout<<">>>>>>>>> 11\n";
        }
        else{
            temp = NULL;
            //cout<<">>>>>>>>> 12\n";
        }
        if(ptr->parent != NULL){
            //cout<<">>>>>>>>> 13\n";
            if((ptr->parent->left!=NULL)&&(ptr->parent->left == ptr)){
                ptr->parent->left = temp;
                //cout<<">>>>>>>>> 14\n";
            }
            else{
                ptr->parent->right = temp;
                //cout<<">>>>>>>>> 15\n";
            }
        }
        else{
            //cout<<">>>>>>>>> 16\n";
            root = temp;
        }
        if (temp!=NULL){
            //cout<<">>>>>>>>> 17\n";
            temp-> parent = ptr->parent;
        }
    }
    //delete ptr;
    /*s*/
    emp_stat_v->memory_v -= sizeof( AVLnode<T>);
    /*s*/
    ptr = NULL;
    return;
}
 
template <class T>
void AVLtree<T>::printBalance() {
    int i = 0;
    std::cout << std::endl;
    printBalance(root,i);
    std::cout << std::endl;
}
/*
int main(void)
{
    AVLtree<int> t;
 
    std::////cout << "Inserting integer values 1 to 10" << std::endl;
    for (int i = 1; i <= 10; ++i)
        t.insert(i);
 
    std::////cout << "Printing balance: ";
    t.printBalance();
}
*/
#endif

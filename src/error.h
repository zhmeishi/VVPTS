/*
 * error.h
 *
 *  Created on: Dec, 2018
 *      Author: Bruce
 */

#ifndef METRIC_H_
#define METRIC_H_
#include "Const.h"

class VelocityError{
    public:
        VelocityError(){}

        double check_error(Point*& tra, Point*& tra_result, int result_length){
            double max_error = -1.0;
            for (int i = 1; i < result_length; i++){
                Point* a = &tra_result[i];
                Point* b = &tra_result[i+1];
                int a_index = a->index;
                int b_index = b->index;
                double x1 = (b->x_coo - a->x_coo) / (b->time - a->time);
                double y1 = (b->y_coo - a->y_coo) / (b->time - a->time);
                for (int j = a_index; j < b_index; j++){
                    Point* c = &tra[j];
                    Point* d = &tra[j+1];
                    double x2 = (d->x_coo - c->x_coo) / (d->time - c->time);
                    double y2 = (d->y_coo - c->y_coo) / (d->time - c->time);
                    double local_error = Const::L2norm(x1-x2, y1-y2);
                    if (local_error > max_error){
                        max_error = local_error;
                    }
                }
            }
            return max_error;
        }
};

class DistanceError{
    public:
        DistanceError(){}

        double check_error(Point*& tra, Point*& tra_result, int result_length){
            double max_error = -1.0;
            for (int i = 1; i < result_length; i++){
                Point* a = &tra_result[i];
                Point* b = &tra_result[i+1];
                int a_index = a->index;
                int b_index = b->index;
                double dist_ab = Const::L2norm(b->x_coo - a->x_coo, b->y_coo - a->y_coo);
                if (Const::areSame(dist_ab, 0)){
                    continue;
                }
                double dist_sum = 0;
                for (int j = a_index; j < b_index; j++){
                    Point* c = &tra[j];
                    Point* d = &tra[j+1];
                    dist_sum += Const::L2norm(d->x_coo - c->x_coo, d->y_coo - c->y_coo);
                }
                double local_error = dist_sum / dist_ab;
                if (local_error > max_error){
                        max_error = local_error;
                    }
            }
            return max_error;
        }
};

class DirectionError{
    public:
        DirectionError(){}

        double check_error(Point*& tra, Point*& tra_result, int result_length){
            double max_error = -1.0;
            for (int i = 1; i < result_length; i++){
                Point* a = &tra_result[i];
                Point* b = &tra_result[i+1];
                int a_index = a->index;
                int b_index = b->index;
                double x1 = b->x_coo - a->x_coo;
                double y1 = b->y_coo - a->y_coo;
                double theta1 = Const::angle(x1, y1) * Const::PI / 180;
                for (int j = a_index; j < b_index; j++){
                    Point* c = &tra[j];
                    Point* d = &tra[j+1];
                    double x2 = d->x_coo - c->x_coo;
                    double y2 = d->y_coo - c->y_coo;
                    double theta2 = Const::angle(x2, y2) * Const::PI / 180;
                    double diff1 = fabs(theta1-theta2);
                    double diff2 = 2 * Const::PI - diff1;                    
                    double local_error = Const::min(diff1, diff2);
                    if (local_error > max_error){
                        max_error = local_error;
                    }
                }
            }
            return max_error;
        }
};


class CEDError{
    public:
        CEDError(){}

        double check_error(Point*& tra, Point*& tra_result, int result_length){
            double max_error = -1.0;
            for (int i = 1; i < result_length; i++){
                Point* a = &tra_result[i];
                Point* b = &tra_result[i+1];
                int a_index = a->index;
                int b_index = b->index;
                double x1 = b->x_coo - a->x_coo;
                double y1 = b->y_coo - a->y_coo;
                double theta1 = Const::angle(x1, y1) * Const::PI / 180;
                for (int j = a_index; j < b_index; j++){
                    Point* c = &tra[j];
                    double x2 = c->x_coo - a->x_coo;
                    double y2 = c->y_coo - a->y_coo;
                    double theta2 = Const::angle(x2, y2) * Const::PI / 180;
                    double diff = fabs(theta1-theta2);
                    double local_error = Const::L2norm(x2, y2) * fabs(sin(diff));
                    if (local_error > max_error){
                        max_error = local_error;
                    }
                }
            }
            return max_error;
        }
};


class SEDError{
    public:
        SEDError(){}

        double check_error(Point*& tra, Point*& tra_result, int result_length){
            double max_error = -1.0;
            for (int i = 1; i < result_length; i++){
                Point* a = &tra_result[i];
                Point* b = &tra_result[i+1];
                int a_index = a->index;
                int b_index = b->index;
                double vx = (b->x_coo - a->x_coo) / (b->time - a->time);
                double vy = (b->y_coo - a->y_coo) / (b->time - a->time);
                for (int j = a_index; j < b_index; j++){
                    Point* c = &tra[j];
                    double dt = c->time - a->time;
                    double x = a->x_coo + vx * dt;
                    double y = a->y_coo + vy * dt;
                    double local_error = Const::L2norm(x - c->x_coo, y - c->y_coo);
                    if (local_error > max_error){
                        max_error = local_error;
                    }
                }
            }
            return max_error;
        }
};

#endif /* METRIC_H_ */
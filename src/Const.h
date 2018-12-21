/*
 * Const.h
 *
 *  Created on: Dec, 2018
 *      Author: Mei
 */


#ifndef _CONST_H_
#define _CONST_H_

#include <iostream>
#include <vector>
#include <math.h>
#include <sys/time.h>
#include <sys/resource.h>
using namespace std;

#define        MAX_LINE_LENG        1024
#define        MAX_FILENAME_LENG    256

struct traInfo_t
{
    double v_max;
    double d_max;
    double t_max;
};

typedef struct emp_stat
{
	double		r_time;
	double		memory_v;
	double		memory_max;

	double		cmp_ratio;		//compression ratio;
	double		error;

	//for error measurements.
	double		ve;
	double		diste;
	double		direce;
	double		cede;
	double		sede;

}	emp_stat_t;

struct emp_config_t
{
    //data specific.
    char    f_name[ MAX_FILENAME_LENG];
    char    s_name[ MAX_FILENAME_LENG]; 
    int     pos_n;
    int     line_ignore_n;

    //problem specific.
    double   eps;            //for the Min-Size problem
    int     W;              //for the Min-Error problem
    double   W_percent;    

    int     alg_opt;        //algorithm specific.
    int     dataset_tag;    //appr_bound tag  
    int     check;          //ErrorCheck  
};

class Const 
{

public:
    static constexpr double PI = 3.14159265;
    static constexpr double EPSILION = 0.00002;
    // static constexpr double EPSILION = 0.0000000001;
    static constexpr double EPSILION_ANGLE = 0.00005;
    // static constexpr double EPSILION_CIRCLE = 0.000001;
    //static constexpr double EPSILION = 0.00000000001;
    //static constexpr double EPSILION_ANGLE = 0.000001;
    static constexpr double ZOOM = 10000;
    static double error;
    // static constexpr double ZOOM = 1000000;
    static void receive_error(double err){
        error =err;
    }
    static bool areSame(double a, double b){
        //if ((a-b)<EPSILION*(error/0.000001)*(error/0.000001) && (a-b)>-EPSILION*(error/0.000001)*(error/0.000001)){return true;}
        //if ((a-b)<EPSILION*(error/0.1) && (a-b)>-EPSILION*(error/0.1)){return true;}
        if ((a-b)<EPSILION && (a-b)>-EPSILION){return true;}
        else{return false;}
    }

    static bool areSameAngle(double a, double b){
        if ((a-b)<EPSILION_ANGLE && (a-b)>-EPSILION_ANGLE){return true;}
        else{return false;}
    }

    // static bool areSame(double a, double b){
    //     if ((a-b)<EPSILION_CIRCLE && (a-b)>-EPSILION_CIRCLE){return true;}
    //     else{return false;}
    // }

    static double min(double a, double b){
        if(a<b){return a;}
        else{return b;}
    }

    static double max(double a, double b){
        if(a>b){return a;}
        else{return b;}
    }

    static double angle(double x_coo, double y_coo){
        double result = atan2 (y_coo,x_coo) * 180 / PI;
        if(result<0){result+=360;}
        return result;
    }

    static double L2norm(double a, double b){
        return sqrt((a*a + b*b));
    }

    static void swap(double& a, double& b){
        double temp = a;
        a = b;
        b = temp;
    }
};
double Const::error = 0.1;
class interval{
public:
    double theta_1;
    double theta_2; 
    double c_x;
    double c_y;
    interval(double t_1,double t_2, double x, double y){theta_1=t_1; theta_2=t_2; c_x=x; c_y=y;}
    interval(double t_1,double t_2){theta_1=t_1; theta_2=t_2;}
    interval(){theta_1=-1; theta_2=-1;}
    double get_x(){return c_x;}
    double get_y(){return c_y;}
    bool operator==(const interval &p) const
    {
        if ((theta_1-p.theta_1)*(theta_2-p.theta_2)<=Const::EPSILION_ANGLE){
            return true;
        }
        else{
            return false;
        }
    }
    bool operator < (const interval &p) const
    {
        //return theta_1<p.theta_1+Const::EPSILION_ANGLE && theta_2<p.theta_2+Const::EPSILION_ANGLE;
        return theta_2<p.theta_1+Const::EPSILION_ANGLE; //&& theta_2<p.theta_2+Const::EPSILION_ANGLE;
    }  
    bool operator > (const interval &p) const
    {
        //return theta_1+Const::EPSILION_ANGLE>p.theta_1 && theta_2+Const::EPSILION_ANGLE>p.theta_2;
        return theta_1+Const::EPSILION_ANGLE>p.theta_2;// && theta_2+Const::EPSILION_ANGLE>p.theta_2;
    }
    bool operator <= (const interval &p) const
    {
        return theta_2<=p.theta_1+Const::EPSILION_ANGLE;
    }
    bool operator>= (const interval &p) const
    {
        return theta_1+Const::EPSILION_ANGLE>=p.theta_2;
    }
    friend ostream& operator<<(ostream& os, const interval& in);  
};  
  
ostream& operator<<(ostream& os, const interval& in)  
{  
    os << "theta:[" <<in.theta_1  << ","<<in.theta_2 << "] xy:[" << in.c_x<< ","<< in.c_y<<"]\n";  
    return os;  
}  


class Point{
    public:
        double x_coo;
        double y_coo;
        double time;
        int visited;
        int parent;
        int index;
        Point(){
            parent = -1;
            visited = -1;
            time = -1;
            index = 0;
        }
        Point(double x, double y, double t,int i = 0):x_coo(x),y_coo(y),time(t),index(i){
            parent = -1;
            visited = -1;
        }
        friend ostream& operator<<(ostream& os, const Point& p);  
};  
  
ostream& operator<<(ostream& os, const Point& p)  
{  
    os << "xy:[" << p.x_coo<< ","<< p.x_coo<<"] time:"<<p.time<<'\n';  
    return os;  
} 

class Edge{
    public:
        double v_x;
        double v_y;
        bool in_graph;

        Edge(){
            v_x = -1;
            v_y = -1;
            in_graph = false;
        }
};

class Arc
{
    public:
        bool not_empty;
        int disk_num;
        int disk1_num;
        int disk2_num;
        double center_x;
        double center_y;
        double mid_x;
        double mid_y;
        double point1_x;
        double point1_y;
        double point2_x;
        double point2_y;
        double theta1;
        double theta2;
        double alpha1;
        double alpha2;
        double alphaMid;
        double r;
        
        Arc(){
            not_empty = false;
            disk_num = -1;
            disk1_num = -1;
            disk2_num = -1;
            center_x = 0;
            center_y = 0;
            mid_x = -1;
            mid_y = -1;
            point1_x = 0;
            point1_y = 0;
            point2_x = 0;
            point2_y = 0;
            theta1 = -1;
            theta2 = -1;
            alpha1 = -1;
            alpha2 = -1;
            r = -1;
            
        }

        bool compute_angle(Point* R){
            alpha1 = Const::angle(point1_x - R->x_coo, point1_y - R->y_coo);
            alpha2 = Const::angle(point2_x - R->x_coo, point2_y - R->y_coo);
            if (Const::areSameAngle(alpha2,0))alpha2 = 360;
            alphaMid = Const::angle(mid_x - R->x_coo, mid_y - R->y_coo);
            //alpha1 = Const::min(result1,result2);
            //alpha2 = Const::max(result1,result2);

        }

        double compute_distance(Point* R){
            double y = R->y_coo;
            double dy = y - center_y;
            double dx = sqrt(r*r-dy*dy);
            return dx + center_x;
        }
        // compute the intersection of two arcs within the same circle
        static Arc arc_intersection(Arc &a, Arc &b)
        {
            Arc result = Arc();
            result.r = a.r;
            result.disk_num = a.disk_num;
            result.center_x = a.center_x;
            result.center_y = a.center_y;

            double a_theta1 = a.theta1;
            double a_theta2 = a.theta2;
            double b_theta1 = b.theta1;
            double b_theta2 = b.theta2;

            double theta1 = -1.0;
            double theta2 = -1.0;
            
            // if(Const::areSame(a_theta1, 0) && Const::areSame(a_theta2, 360)){
            //      result.not_empty = false;
            //      result.theta1 = -1.0;
            //      result.theta2 = -1.0;
            //      return result;
            // }

            // if(Const::areSameAngle(b_theta1, 0) && Const::areSameAngle(b_theta2, 360)){
            //     printf("\n\n\n\n\n\noverlapping happens\n\n\n\n\n\n\n\n\n\n\n\n");
            //     exit(0);
            //     // if (Const::areSameAngle(a_theta1, 0) && Const::areSameAngle(a_theta2, 360))
            //     return a;
            // }

            // two zero crossings
            if (a_theta1 > a_theta2 && b_theta1 > b_theta2)
            {
                theta1 = max(a_theta1, b_theta1);
                theta2 = min(a_theta2, b_theta2);
            }
            // zero crossing for a
            else if (a_theta1 > a_theta2)
            {
                if(Const::areSameAngle(b_theta1, 0) && Const::areSameAngle(b_theta2, 360)){
                    theta1 = a_theta1;
                    theta2 = a_theta2;
                }
                else if (b_theta2 >= a_theta1)
                {
                    theta1 = max(a_theta1, b_theta1);
                    theta2 = b_theta2;
                }
                else if (b_theta1 <= a_theta2)
                {
                    theta1 = b_theta1;
                    theta2 = min(a_theta2, b_theta2);
                }
                else
                {
                    theta1 = -1.0;
                    theta2 = -1.0;
                }
            }
            // zero crossing for b
            else if (b_theta1 > b_theta2)
            {
                if(Const::areSameAngle(a_theta1, 0) && Const::areSameAngle(a_theta2, 360)){
                    theta1 = b_theta1;
                    theta2 = b_theta2;
                }
                else if (a_theta2 >= b_theta1)
                {
                    theta1 = max(b_theta1, a_theta1);
                    theta2 = a_theta2;
                }
                else if (a_theta1 <= b_theta2)
                {
                    theta1 = a_theta1;
                    theta2 = min(b_theta2, a_theta2);
                }
                else
                {
                    theta1 = -1.0;
                    theta2 = -1.0;
                }
            }
            // no zero crossing
            else
            {
                // disjoint
                if ((a_theta1 > b_theta2) || (a_theta2 < b_theta1))
                {
                    theta1 = -1.0;
                    theta2 = -1.0;
                }
                // overlapping
                else
                {
                    theta1 = max(a_theta1, b_theta1);
                    theta2 = min(a_theta2, b_theta2);
                }
            }

            if (theta1<-0.1)
            {
                result.not_empty = false;
            }
            else
            {
                result.theta1 = theta1;
                result.not_empty = true;
                if (Const::areSameAngle(theta1, a_theta1))
                {
                    result.disk1_num = a.disk1_num;
                    result.point1_x = a.point1_x;
                    result.point1_y = a.point1_y;
                }
                else
                {
                    result.disk1_num = b.disk1_num;
                    result.point1_x = b.point1_x;
                    result.point1_y = b.point1_y;
                }
                result.theta2 = theta2;
                if (Const::areSameAngle(theta2, a_theta2))
                {
                    result.disk2_num = a.disk2_num;
                    result.point2_x = a.point2_x;
                    result.point2_y = a.point2_y;
                }
                else
                {
                    result.disk2_num = b.disk2_num;
                    result.point2_x = b.point2_x;
                    result.point2_y = b.point2_y;
                }
                    // printf("%d, %d\n", result.disk1_num, result.disk2_num);
                    // printf("%d, %d, %d\n", a_theta1, b_theta1, Const::areSameAngle(a_theta1, b_theta1));
                    // printf("hahahahaha\n");
                    // exit(0);
                if(Const::areSameAngle(a_theta1, b_theta1)){
                    result.disk1_num = Const::min(a.disk1_num, b.disk1_num);
                    // printf("hahahahaha\n");
                    // exit(0);
                }

                if(Const::areSameAngle(a_theta2, b_theta2)){
                    result.disk2_num = Const::min(a.disk2_num, b.disk2_num);
                }

                /*
                if (Const::areSame(a_theta1, b_theta1)&&Const::areSame(a_theta2, b_theta2)){
                    result.disk1_num = Const::max(a.disk1_num, b.disk1_num);
                    result.disk2_num = Const::max(a.disk2_num, b.disk2_num);
                }*/

                double theta_mid = (theta1 + theta2)/2;
                if (theta1 > theta2){
                    theta_mid += 180;
                }

                result.mid_x = result.center_x + result.r * cos(theta_mid*Const::PI/180.0);
                result.mid_y = result.center_y + result.r * sin(theta_mid*Const::PI/180.0);
            }
            return result;
        }
        friend ostream& operator<<(ostream& os, const Arc& ar);  
};  
  
ostream& operator<<(ostream& os, const Arc& ar)  
{  
    os  << "disk:" << ar.disk_num <<", "
        << "disk1:" << ar.disk1_num <<", "
        << "disk2:" << ar.disk2_num <<",\n"
        << "cxy:[" << ar.center_x <<","<<ar.center_y<<"] "
        << "1xy:[" << ar.point1_x <<","<<ar.point1_y<<"] "
        << "2xy:[" << ar.point2_x <<","<<ar.point2_y<<"]\n"
        << "theta:[" << ar.theta1 <<","<<ar.theta2<<"] "
        << "alpha:[" << ar.alpha1 <<","<<ar.alpha2<<"]\n"
        << "r:" << ar.r <<"\n";   
    return os;  
}

class util{
public:
    // get the final simplified trajectory
    static Point* getResult(Point* tra, int length, bool reverse)
    {
        if (reverse){
            vector<int> result;
            result.push_back(length);
            int it = 0;
            // int a;
            // cin>>a;
            while (result.back() != 1)
            {
                it++;
                // cout<< tra[result.back()].parent<<" ";
                result.push_back(tra[result.back()].parent);
                if (it>length+1){
                    //printf("You code has error");
                    return NULL;
                }
            }
            Point* tra_result = new Point[result.size() + 1];
            for (int k = result.size(); k >= 1; k--)
            {
                tra_result[result.size() - k + 1] = tra[result[k - 1]];
            }
            for (int k = 1; k<=result.size(); k ++)
            {
                //cout<<tra_result[k].index<<" ";
            }
            result.clear();
            return tra_result;
        }
        else{
            vector<int> result;
            result.push_back(1);
            int it = 0;
            while (result.back() != length)
            {
                it++;
                result.push_back(tra[result.back()].parent);
                if (it>length+1){
                    //printf("You code has error");
                    return NULL;
                }
            }
            Point* tra_result = new Point[result.size() + 1];
            for (int k = 1; k <=result.size(); k++)
            {
                tra_result[k] = tra[result[k - 1]];
            }
            for (int k = 1; k<=result.size(); k ++)
            {
                //cout<<tra_result[k].index<<" ";
            }
            result.clear();
            return tra_result;
        }

        
    }

    static void traRefresh(Point* tra, int length)
    {
        for (int k = 1; k <= length; k++)
        {
            tra[k].parent = k-1;
            tra[k].visited = -1;
        }
        tra[1].parent = -1;
    }

    static traInfo_t* traInfo(Point* tra, int length)
    {
        traInfo_t* result = new traInfo_t();
        result->d_max = 0;
        result->t_max = 0;
        result->v_max = 0;
        for (int k = 1; k < length; k++)
        {   
            double d = Const::L2norm( tra[k+1].x_coo-tra[k].x_coo, tra[k+1].y_coo-tra[k].y_coo);
            double t = tra[k+1].time-tra[k].time;
            result->d_max = max(result->d_max, d);
            result->t_max = max(result->t_max, t);
            result->v_max = max(result->v_max, d/t);
        }
        return result;
    }

    static bool traEqual(Point* tra1, Point* tra2, int num1, int num2)
    {
        if (num1!=num2)
        {
            return false;
        }
        for (int k = 1; k <= num1; k++)
        {
            if(tra1[k].index != tra2[k].index){
                return false;
            }
        }
        return true;
    }
    static double calc_v_diff(Point A,Point B,Point C,Point D)
    {
            double v1_x = (B.x_coo-A.x_coo)/(B.time-A.time);
            double v1_y = (B.y_coo-A.y_coo)/(B.time-A.time);
            double v2_x = (D.x_coo-C.x_coo)/(D.time-C.time);
            double v2_y = (D.y_coo-C.y_coo)/(D.time-C.time);
            return Const::L2norm(v2_x-v1_x,v2_y-v1_y);
    }

    static void get_cur_time( rusage* cur_t)
    {
        if( getrusage( RUSAGE_SELF, cur_t) != 0)
        {
            fprintf( stderr, "The invokation of getrusage fails.\n");
            exit( 0);
        }
    }
    static void collect_time( struct rusage* sta_t, struct rusage* end_t, double* user_t)
    {
        (*user_t) = ((double)(end_t->ru_utime.tv_sec - sta_t->ru_utime.tv_sec)) + 
            ((double)(end_t->ru_utime.tv_usec - sta_t->ru_utime.tv_usec)) * 1e-6;
    }
};
#endif 

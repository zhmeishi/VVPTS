#include <iostream>
#include "Experiment.h"
#include "Const.h"


using namespace std;

emp_stat_t* emp_stat_v;

int main(int argv, char **args)
{   
    string config;
    if(argv == 1){
        config = "config.txt";
    }
    else{
        config = args[1];
    }
    Experiment exp(config);
    exp.run();
    exp.~Experiment();
    exit(0);
    return 0;
}

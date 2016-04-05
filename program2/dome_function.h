#ifndef DOME_FUNCTION_H
#define DOME_FUNCTION_H
#include <math.h>
#include "function.h"

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

//step function where first half is scale and second is top half of circle
class DomeFunction: public Function
{
    public:
        double compute(double scale, double value){return (value > .5) ? sqrt(pow(scale, 2) - pow((value - .5) * 2, 2)) : scale;}
};

#endif

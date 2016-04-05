#ifndef CIR_FUNCTION_H
#define CIR_FUNCTION_H
#include <math.h>
#include "function.h"

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

//equation for a circle, only use from -scale to scale
class CircleFunction: public Function
{
    public:
        double compute(double scale, double value){return sqrt(pow(scale, 2) - pow(value, 2));}
};

#endif

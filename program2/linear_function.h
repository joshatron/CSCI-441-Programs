#ifndef LIN_FUNCTION_H
#define LIN_FUNCTION_H
#include <math.h>
#include "function.h"

//linear function with a slope of scale
class LinearFunction: public Function
{
    public:
        double compute(double scale, double value){return value * scale;}
};

#endif

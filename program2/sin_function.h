#ifndef SIN_FUNCTION_H
#define SIN_FUNCTION_H
#include <math.h>
#include "function.h"

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

class SinFunction: public Function
{
    public:
        double compute(double scale, double value){return sin(value * 2 * M_PI) * scale + 1;}
};

#endif

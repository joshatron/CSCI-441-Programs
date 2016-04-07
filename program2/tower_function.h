#ifndef TOWER_FUNCTION_H
#define TOWER_FUNCTION_H
#include <math.h>
#include "function.h"

//step function that is scale width with a cone on top
class TowerFunction: public Function
{
    public:
        double compute(double scale, double value){return (value > .8) ? ((1 - value) * 7 * scale) : scale;}
};

#endif

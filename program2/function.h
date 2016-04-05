#ifndef FUNCTION_H
#define FUNCTION_H

//inherit from this to make a new function
//this one just returns 1, regardless of scale or value given
class Function
{
    public:
        virtual double compute(double scale, double value){return 1;}
};

#endif

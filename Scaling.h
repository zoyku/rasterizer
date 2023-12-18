#ifndef __SCALING_H__
#define __SCALING_H__

#include "Matrix4.h"
#include <iostream>
#include <iomanip>
#include <sstream>

class Scaling
{
public:
    int scalingId;
    double sx, sy, sz;
    Matrix4 matrix;


    Scaling();
    Scaling(int scalingId, double sx, double sy, double sz);
    friend std::ostream &operator<<(std::ostream &os, const Scaling &s);
    Matrix4 calculateMatrix();
};

#endif
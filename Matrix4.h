#ifndef __MATRIX4_H__
#define __MATRIX4_H__

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

class Matrix4
{
public:
    double values[4][4];

    Matrix4();
    Matrix4(double values[4][4]);
    Matrix4(const Matrix4 &other);
    friend std::ostream &operator<<(std::ostream &os, const Matrix4 &m);
};

#endif
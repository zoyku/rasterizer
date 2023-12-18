#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include <iostream>
#include <iomanip>
#include <sstream>
class Triangle
{
public:
    int vertexIds[3];

    Triangle();
    Triangle(int vid1, int vid2, int vid3);
    Triangle(const Triangle &other);
    friend std::ostream &operator<<(std::ostream &os, const Triangle &t);
};

#endif
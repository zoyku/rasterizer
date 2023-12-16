#include <iomanip>
#include "Scaling.h"

Scaling::Scaling() {
    this->scalingId = -1;
    this->sx = 0;
    this->sy = 0;
    this->sz = 0;
    this->matrix = Matrix4();
}

Scaling::Scaling(int scalingId, double sx, double sy, double sz)
{
    this->scalingId = scalingId;
    this->sx = sx;
    this->sy = sy;
    this->sz = sz;
    this->matrix = this->calculateMatrix();
}

std::ostream &operator<<(std::ostream &os, const Scaling &s)
{
    os << std::fixed << std::setprecision(3) << "Scaling " << s.scalingId << " => [" << s.sx << ", " << s.sy << ", " << s.sz << "]";

    return os;
}

Matrix4 Scaling::calculateMatrix()
{
    double matrix[4][4] = {
        {this->sx, 0.0, 0.0, 0.0},
        {0.0, this->sy, 0.0, 0.0},
        {0.0, 0.0, this->sz, 0.0},
        {0.0, 0.0, 0.0, 1.0}
    };
    return Matrix4(matrix);
}

#include <iomanip>
#include "Rotation.h"
#include <cmath>

Rotation::Rotation() {
    this->rotationId = -1;
    this->angle = 0;
    this->ux = 0;
    this->uy = 0;
    this->uz = 0;
    this->matrix = Matrix4();
}

Rotation::Rotation(int rotationId, double angle, double x, double y, double z)
{
    this->rotationId = rotationId;
    this->angle = angle;
    this->ux = x;
    this->uy = y;
    this->uz = z;
    this->matrix = this->calculateMatrix();
}

std::ostream &operator<<(std::ostream &os, const Rotation &r)
{
    os << std::fixed << std::setprecision(3) << "Rotation " << r.rotationId << " => [angle=" << r.angle << ", " << r.ux << ", " << r.uy << ", " << r.uz << "]";
    return os;
}



Matrix4 Rotation::calculateMatrix()
{
    /*
        get an orthonormal basis from ux,uy,uz
    */
    Vec3 b = Vec3(this->ux, this->uy, this->uz);

    Vec3 u,w,v;

    findOrtonormalBasis(b, u, w, v);

    double matrix[4][4] = {
        {u.x, u.y, u.z, 0.0},
        {v.x, v.y, v.z, 0.0},
        {w.x, w.y, w.z, 0.0},
        {0.0, 0.0, 0.0, 1.0}
    };

    Matrix4 result = Matrix4(matrix);

    double angle = this->angle * M_PI / 180.0;

    double rotation[4][4] = {
        {1.0, 0.0, 0.0, 0.0},
        {0.0, cos(angle), -sin(angle), 0.0},
        {0.0, sin(angle), cos(angle), 0.0},
        {0.0, 0.0, 0.0, 1.0}
    };
    result = multiplyMatrixWithMatrix(Matrix4(rotation), result);


    double transpose[4][4] = {
        {u.x, v.x, w.x, 0.0},
        {u.y, v.y, w.y, 0.0},
        {u.z, v.z, w.z, 0.0},
        {0.0, 0.0, 0.0, 1.0}
    };
    result = multiplyMatrixWithMatrix(Matrix4(transpose), result);




    /*
    method taken as-is from the slides
    */



    return result;
}
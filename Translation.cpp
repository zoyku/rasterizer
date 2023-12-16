#include <iomanip>
#include "Translation.h"
#include "Matrix4.h"

Translation::Translation()
{
    this->translationId = -1;
    this->tx = 0.0;
    this->ty = 0.0;
    this->tz = 0.0;
    this->matrix = Matrix4();
}

Translation::Translation(int translationId, double tx, double ty, double tz)
{
    this->translationId = translationId;
    this->tx = tx;
    this->ty = ty;
    this->tz = tz;
    this->matrix = this->calculateMatrix();
}

std::ostream &operator<<(std::ostream &os, const Translation &t)
{
    os << std::fixed << std::setprecision(3) << "Translation " << t.translationId << " => [" << t.tx << ", " << t.ty << ", " << t.tz << "]";
    return os;
}

Matrix4 Translation::calculateMatrix()
{
    double matrix[4][4] = {
        {1.0, 0.0, 0.0, this->tx},
        {0.0, 1.0, 0.0, this->ty},
        {0.0, 0.0, 1.0, this->tz},
        {0.0, 0.0, 0.0, 1.0}
    };
    return Matrix4(matrix);
}
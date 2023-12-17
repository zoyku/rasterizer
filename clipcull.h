#include "Helpers.h"
#include "Color.h"

int backfaceCull(Vec4, Vec4, Vec4);
int visible(double, double, double &, double &);
int liangClip(Vec4 &, Vec4 &, Color &, Color &);
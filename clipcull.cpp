#include "clipcull.h"

int backfaceCull(Vec4 v0, Vec4 v1, Vec4 v2) {
    Vec3 a = Vec3(v0.x, v0.y, v0.z);
    Vec3 b = Vec3(v1.x, v1.y, v1.z);
    Vec3 c = Vec3(v2.x, v2.y, v2.z);
    Vec3 ab = subtractVec3(b, a);
    Vec3 ac = subtractVec3(c, a);
    Vec3 n = normalizeVec3(crossProductVec3(ab, ac));
    return dotProductVec3(n, a);
} 

int visible(double den, double num, double &tE, double &tL) {

    double t;
    if (den > 0) {
        t = num / den;
        if (t > tL) {
            return 0;
        }
        if (t > tE) {
            tE = t;
        }
    } else if (den < 0) {
        t = num / den;
        if (t < tE) {
            return 0;
        }
        if (t < tL) {
            tL = t;
        }
    } else if (num > 0) {
        return 0;
    }
    return 1;
}

int liangClip(Vec4 &v0, Vec4 &v1, Color &c0, Color &c1) {
    double tE = 0;
    double tL = 1;
    int isVisible = 0;
    double dX = v1.x - v0.x;
    double dY = v1.y - v0.y;
    double dZ = v1.z - v0.z;
    double xMin = -1;
    double yMin = -1;
    double zMin = -1;
    double xMax = 1;
    double yMax = 1;
    double zMax = 1;

    Color dC = Color(c1.r - c0.r, c1.g - c0.g, c1.b - c0.b);

    if (visible(dX, xMin-v0.x, tE, tL)) {
        if (visible(-dX, v0.x-xMax, tE, tL)) {
            if (visible(dY, yMin-v0.y, tE, tL)) {
                if (visible(-dY, v0.y-yMax, tE, tL)) {
                    if (visible(dZ, zMin-v0.z, tE, tL)) {
                        if (visible(-dZ, v0.z-zMax, tE, tL)) {
                            isVisible = 1;
                            if (tL < 1) {
                                v1.x = v0.x + (dX * tL);
                                v1.y = v0.y + (dY * tL);
                                v1.z = v0.z + (dZ * tL);
                                c1.r = c0.r + (tL * dC.r);
                                c1.g = c0.g + (tL * dC.g);
                                c1.b = c0.b + (tL * dC.b);
                            }
                            if (tE > 0) {
                                v0.x = v0.x + (dX * tE);
                                v0.y = v0.y + (dY * tE);
                                v0.z = v0.z + (dZ * tE);
                                c0.r = c0.r + (tE * dC.r);
                                c0.g = c0.g + (tE * dC.g);
                                c0.b = c0.b + (tE * dC.b);
                            }
                        }
                    }
                }
            }
        }
    }

    return isVisible;


}

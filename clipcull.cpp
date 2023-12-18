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

/*int visible(double den, double num, double &tE, double &tL) {

    /* 
        from slides
    
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
    double tE = 0.0;
    double tL = 1.0;
    int isVisible = 0;
    double dX = v1.x - v0.x;
    double dY = v1.y - v0.y;
    double dZ = v1.z - v0.z;
    double xMin, yMin, zMin = -1;
    double xMax, yMax, zMax = 1;

    /*
        Taken from slides, with color interpolation added
    
    Color dC = Color(c1.r - c0.r, c1.g - c0.g, c1.b - c0.b);

    if (visible(dX, xMin - v0.x, tE, tL)) {
        if (visible(-dX, v0.x - xMax, tE, tL)) {
            if (visible(dY, yMin - v0.y, tE, tL)) {
                if (visible(-dY, v0.y - yMax, tE, tL)) {
                    if (visible(dZ, zMin - v0.z, tE, tL)) {
                        if (visible(-dZ, v0.z - zMax, tE, tL)) {
                            isVisible = 1;
                            if (tL < 1) {
                                v1.x = v0.x + tL * dX;
                                v1.y = v0.y + tL * dY;
                                v1.z = v0.z + tL * dZ;
                                c1.r = c0.r + tL * dC.r;
                                c1.g = c0.g + tL * dC.g;
                                c1.b = c0.b + tL * dC.b;
                            }
                            if (tE > 0) {
                                v0.x = v0.x + tE * dX;
                                v0.y = v0.y + tE * dY;
                                v0.z = v0.z + tE * dZ;
                                c0.r = c0.r + tE * dC.r;
                                c0.g = c0.g + tE * dC.g;
                                c0.b = c0.b + tE * dC.b;
                            }
                        }
                    }
                }
            }
        }
    }
    return isVisible;


}*/




int visible(double den, double num, double & t_E, double & t_L) {
    /* Helper function for checking visibility of the lines - updates t_E and t_L as needed */
    double t = num / den;
    if (den > 0) {
        /* Potentially Entering */
        if (t > t_L)
            return 0;
        else if (t > t_E)
            t_E = t;
    }
    else if (den < 0) {
        /* Potentially Leaving */
        if (t < t_E)
            return 0;
        else if (t < t_L)
            t_L = t;
    }
    else if (num > 0) {
        /* Line is parallel to the edge and outside of the bounds */
        return 0;
    }
    return 1;
}


int liangClip(Vec4 &v0, Vec4 &v1, Color &c0, Color &c1) {
    /* Clips given line with Liang-Barsky Algorithm in 3D
     * as a result v0 and v1 gets updated as needed */
    int isVisible = 0;;
    double t_E = 0, t_L = 1;
    double dx = v1.x - v0.x, dy = v1.y - v0.y, dz = v1.z - v0.z;
    Color dc = Color(c1.r - c0.r, c1.g - c0.g, c1.b - c0.b);
    double x_min = -1, y_min = -1, z_min = -1;

    double x_max = 1, y_max = 1, z_max = 1;
    if (visible(dx, x_min-v0.x, t_E, t_L) && visible(-dx, v0.x-x_max, t_E, t_L)
        && visible(dy, y_min-v0.y, t_E, t_L) && visible(-dy, v0.y-y_max, t_E, t_L)
        && visible(dz, z_min-v0.z, t_E, t_L) && visible(-dz, v0.z-z_max, t_E, t_L)) {
        isVisible = 1;
        /* Check if at least some part of the line is clipped */
        if (t_L < 1) {
            v1.x = v0.x + (dx * t_L);
            v1.y = v0.y + (dy * t_L);
            v1.z = v0.z + (dz * t_L);
            c1 = Color(c0.r + (dc.r * t_L), c0.g + (dc.g * t_L), c0.b + (dc.b * t_L));
        }
        if (t_E > 0) {
            v0.x = v0.x + (dx * t_E);
            v0.y = v0.y + (dy * t_E);
            v0.z = v0.z + (dz * t_E);
            c0 = Color(c0.r + (dc.r * t_E), c0.g + (dc.g * t_E), c0.b + (dc.b * t_E));
        }
    }


    return isVisible;
}
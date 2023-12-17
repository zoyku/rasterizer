#define SWAP(a, b) \
    do { \
        auto temp = (a); \
        (a) = (b); \
        (b) = temp; \
    } while (0)

#define ROUND(a) ((int) (a + 0.5))

#include "rasterize.h"
#include <vector>
using namespace std;

void lineRasterization(Vec4 first, Vec4 second, Color c0, Color c1, vector<vector<Color>> & image) {
    int d, increment = 1;
    double m = ABS((second.y - first.y) / (second.x - first.x));
    Color c,dc;
    
    if(m <= 1) {
        if (first.x > second.x) {
            SWAP(first, second);
            SWAP(c0, c1);
        }
        if (first.y > second.y) {
            increment = -1;
        }
        d = first.y - second.y + 0.5 * increment * (second.x - first.x);
        c = c0;
        dc = Color((c1.r - c0.r) / (second.x - first.x), (c1.g - c0.g) / (second.x - first.x), (c1.b - c0.b) / (second.x - first.x));
        int y = first.y;
        for (int x = first.x; x <= second.x; x++) {
            image[x][y] = c;
            if (d > 0) {
                y += increment;
                d += (second.x - first.x) * increment;
            }
            d += second.y - first.y;
            c = Color(c.r + dc.r, c.g + dc.g, c.b + dc.b);
        }
    }
    else {
        if (first.y > second.y) {
            SWAP(first, second);
            SWAP(c0, c1);
        }
        if (first.x > second.x) {
            increment = -1;
        }
        d = first.x - second.x + 0.5 * increment * (second.y - first.y);
        c = c0;
        dc = Color((c1.r - c0.r) / (second.y - first.y), (c1.g - c0.g) / (second.y - first.y), (c1.b - c0.b) / (second.y - first.y));
        int x = first.x;
        for (int y = first.y; y <= second.y; y++) {
            image[x][y] = c;
            if (d > 0) {
                x += increment;
                d += (second.y - first.y) * increment;
            }
            d += second.x - first.x;
            c = Color(c.r + dc.r, c.g + dc.g, c.b + dc.b);
        }
    }

}

void triangleRasterization(vector<vector<Color>> &image, vector<vector<double>> &depth, Vec3 v0, Vec3 v1, Vec3 v2, Color c0, Color c1, Color c2) {
    /*
        Barycentric coordinate method from slides
    */
    double xMin = min(v0.x, min(v1.x, v2.x));
    double xMax = max(v0.x, max(v1.x, v2.x));
    double yMin = min(v0.y, min(v1.y, v2.y));
    double yMax = max(v0.y, max(v1.y, v2.y));
    

    


}
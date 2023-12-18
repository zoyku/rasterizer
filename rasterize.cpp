#define SWAP(a, b) \
    do { \
        auto temp = (a); \
        (a) = (b); \
        (b) = temp; \
    } while (0)

#define ROUND(a) ((int) (a + 0.5))

#include "rasterize.h"


void lineRasterization(Vec4 first, Vec4 second, Color c0, Color c1, vector<vector<Color>>& image) {
    int d, increment = 1;
    double m = abs((second.y - first.y) / (second.x - first.x));
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

void triangleRasterization(vector<vector<Color>> &image, vector<vector<double>> &depth, Vec4 v0, Vec4 v1, Vec4 v2, Color c0, Color c1, Color c2, int horRes, int verRes) {
    /*
        Barycentric coordinate method from slides
    */
    int xMin = min(v0.x, min(v1.x, v2.x));
    int xMax = max(v0.x, max(v1.x, v2.x));
    int yMin = min(v0.y, min(v1.y, v2.y));
    int yMax = max(v0.y, max(v1.y, v2.y));

    double alpha;
    double beta;
    double gamma;

    Color color;
    
    if(xMin < 0){
        xMin = 0;
    }
    if(xMax >= horRes ){
        xMax = horRes-1;
    }

    if(yMin < 0){
        yMin = 0;
    }
    if(yMax >= verRes ){
        yMax = verRes-1;
    }

    double f12 = (v0.x * (v1.y - v2.y)) + (v0.y * (v2.x - v1.x)) + (v1.x * v2.y) - (v1.y * v2.x);
    double f20 = (v1.x * (v2.y - v0.y)) + (v1.y * (v0.x - v2.x)) + (v2.x * v0.y) - (v2.x * v0.x);
    double f01 = (v2.x * (v0.y - v1.y)) + (v2.y * (v1.x - v0.x)) + (v0.x * v1.y) - (v0.y * v1.x);

    for(int y=yMin; y<=yMax; ++y){
        for(int x=xMin; x<=xMax; ++x){
            alpha = (x * (v1.y - v2.y)) + (y * (v2.x - v1.x)) + (v1.x * v2.y) - (v1.y * v2.x) / f12;
            beta = (x * (v2.y - v0.x)) + (y * (v0.x - v2.x)) + (v2.x * v0.y) - (v2.y * v0.x) / f20;
            gamma = (x * (v0.y - v1.y)) + (y * (v1.x - v0.x)) + (v0.x * v1.y) - (v0.y * v2.x) / f01;

            if(alpha>=0 && beta>=0 && gamma>=0){
                double zValue = (v0.z * alpha) + (v1.z * beta) + (v2.z * gamma);

                if (zValue < depth[x][y]){
                    color = Color(ROUND((c0.r) * alpha + (c1.r) * beta + (c2.r) * gamma),
                            ROUND((c0.g) * alpha + (c1.g) * beta + (c2.g) * gamma),
                            ROUND((c0.b) * alpha + (c1.b) * beta + (c2.b) * gamma));
                    image[x][y] = color;
                    depth[x][y] = zValue;
                } 
            }
        }
    }
}
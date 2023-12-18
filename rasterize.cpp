#define SWAP(a, b) \
    do { \
        auto temp = (a); \
        (a) = (b); \
        (b) = temp; \
    } while (0)

#define ROUND(a) ((int) (a + 0.5))

#include "rasterize.h"


double f_(double x, double y, double x_n, double y_n, double x_m, double y_m){
    return (x * (y_n - y_m)) + (y * (x_m - x_n)) + (x_n * y_m) - (y_n * x_m);
}

void lineRasterization(Vec4 first, Vec4 second, Color c0, Color c1, vector<vector<Color>>& image) {
    int d, increment = 1;
    double m = abs((second.y - first.y) / (second.x - first.x));
    Color c,dc;
    
    if(m <= 1) {
        if (second.x < first.x) {
            SWAP(first, second);
            SWAP(c0, c1);
        }
        if (second.y < first.y) {
            increment = -1;
        }
        d = (first.y - second.y) + (0.5 * increment * (second.x - first.x));
        c = c0;
        dc = Color((c1.r - c0.r) / (second.x - first.x), (c1.g - c0.g) / (second.x - first.x), (c1.b - c0.b) / (second.x - first.x));
        int y = first.y;
        for (int x = first.x; x <= second.x; x++) {
            image[x][y] = Color(ROUND(c.r), ROUND(c.g), ROUND(c.b));
            if (d * increment < 0) { // choose NE
                y += increment;
                d += (first.y - second.y) + (increment * (second.x - first.x));
            }
            else // choose E
                d += (first.y - second.y);
            c = Color(c.r + dc.r, c.g + dc.g, c.b + dc.b);
        }
    }
    else {
        if (second.y < first.y) {
            SWAP(first, second);
            SWAP(c0, c1);
        }
        if (second.x < first.x) {
            increment = -1;
        }
        d = (first.x - second.x) + (0.5 * increment * (second.y - first.y));
        c = c0;
        dc = Color((c1.r - c0.r) / (second.y - first.y), (c1.g - c0.g) / (second.y - first.y), (c1.b - c0.b) / (second.y - first.y));
        int x = first.x;
        for (int y = first.y; y <= second.y; y++) {
            image[x][y] = Color(ROUND(c.r), ROUND(c.g), ROUND(c.b));
            if (d * increment > 0) {
                x += increment;
                d += (second.x - first.x) + (increment * (first.y - second.y));
            }
            else
                d += (second.x - first.x);
            c = Color(c.r + dc.r, c.g + dc.g, c.b + dc.b);
        }
    }

}

void triangleRasterization(vector<vector<Color>> &image, vector<vector<double>> &depth, Vec4 v0, Vec4 v1, Vec4 v2, Color c0, Color c1, Color c2, int nx, int ny) {

    int x_min = min(min(v0.x, v1.x), v2.x) >= 0 ? min(min(v0.x, v1.x), v2.x) : 0;
    x_min = x_min <= nx-1 ? x_min: nx-1;
    int y_min = min(min(v0.y, v1.y), v2.y) >= 0 ? min(min(v0.y, v1.y), v2.y) : 0;
    y_min = y_min <= ny-1 ? y_min: ny-1;

    int x_max = max(max(v0.x, v1.x), v2.x) < 0 ? 0 : max(max(v0.x, v1.x), v2.x) > nx-1 ? nx-1 : max(max(v0.x, v1.x), v2.x);
    int y_max = max(max(v0.y, v1.y), v2.y) < 0 ? 0 : max(max(v0.y, v1.y), v2.y) > ny-1 ? ny-1 : max(max(v0.y, v1.y), v2.y);

    double alpha,beta,gamma;
    Color c;

    for(int y=y_min; y<=y_max; ++y){
        for(int x=x_min; x<=x_max; ++x){
            alpha = f_(x,y, v1.x, v1.y, v2.x, v2.y) / f_(v0.x,v0.y, v1.x,v1.y, v2.x,v2.y); // L12
            beta = f_(x,y, v2.x, v2.y, v0.x, v0.y) / f_(v1.x,v1.y, v2.x,v2.y, v0.x,v0.y); // L20
            gamma = f_(x,y, v0.x, v0.y, v1.x, v1.y) / f_(v2.x,v2.y, v0.x,v0.y, v1.x,v1.y); // L01

            if(alpha>=0 && beta>=0 && gamma>=0){
                double zValue = (v0.z * alpha) + (v1.z * beta) + (v2.z * gamma);

                if (zValue < depth[x][y]){
                    depth[x][y] = zValue;
                    Color color = Color(ROUND((c0.r) * alpha + (c1.r) * beta + (c2.r) * gamma),
                            ROUND((c0.g) * alpha + (c1.g) * beta + (c2.g) * gamma),
                            ROUND((c0.b) * alpha + (c1.b) * beta + (c2.b) * gamma));
                    image[x][y] = color;
                }
            }
        }
    }
}

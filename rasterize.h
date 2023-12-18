#include "Helpers.h"
#include "Color.h"
#include <vector>
#include <cmath>
using namespace std;

void lineRasterization(Vec4 first, Vec4 second, Color c0, Color c1, vector<vector<Color>>& image);
void triangleRasterization(vector<vector<Color>> &image, vector<vector<double>> &depth, Vec4 v0, Vec4 v1, Vec4 v2, Color c0, Color c1, Color c2, int horRes, int verRes);
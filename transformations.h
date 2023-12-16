#include "Helpers.h"
#include "Matrix4.h"
#include "Translation.h"
#include "Scaling.h"
#include "Rotation.h"
#include "Mesh.h"
#include "Camera.h"
#include <vector>

Matrix4 calculateModelingTransformation(Mesh* mesh, std::vector<Translation*> translations, std::vector<Scaling*> scalings, std::vector<Rotation*> rotations);

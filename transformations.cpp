#include "transformations.h"

Matrix4 calculateModelingTransformation(Mesh* mesh, std::vector<Translation*> translations, std::vector<Scaling*> scalings, std::vector<Rotation*> rotations)
{
    Matrix4 modelingTransformation = getIdentityMatrix();
    for (int i = 0; i < mesh->numberOfTransformations; i++)
    {
        if (mesh->transformationTypes[i] == 't')
        {
            modelingTransformation = multiplyMatrixWithMatrix(modelingTransformation,translations[mesh->transformationIds[i]]->matrix);
        }
        else if (mesh->transformationTypes[i] == 's')
        {
            modelingTransformation = multiplyMatrixWithMatrix(modelingTransformation,scalings[mesh->transformationIds[i]]->matrix);
        }
        else if (mesh->transformationTypes[i] == 'r')
        {
            modelingTransformation = multiplyMatrixWithMatrix(modelingTransformation,rotations[mesh->transformationIds[i]]->matrix);
        }
    }
    return modelingTransformation;
}

Matrix4 calculateCameraTransformation(Camera* camera)
{
    double matrix[4][4] = {
        {camera->u.x, camera->u.y, camera->u.z, -1.0 * dotProductVec3(camera->u, camera->position)},
        {camera->v.x, camera->v.y, camera->v.z, -1.0 * dotProductVec3(camera->v, camera->position)},
        {camera->w.x, camera->w.y, camera->w.z, -1.0 * dotProductVec3(camera->w, camera->position)},
        {0.0, 0.0, 0.0, 1.0}
    };

    return Matrix4(matrix);
}

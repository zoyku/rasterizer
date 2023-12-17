#include "transformations.h"

/*
    Most of these calculations are taken straight from the slides
*/

Matrix4 calculateModelingTransformation(Mesh* mesh, std::vector<Translation*> translations, std::vector<Scaling*> scalings, std::vector<Rotation*> rotations){
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

Matrix4 calculateCameraTransformation(Camera* camera){
    double matrix[4][4] = {
        {camera->u.x, camera->u.y, camera->u.z, -1.0 * dotProductVec3(camera->u, camera->position)},
        {camera->v.x, camera->v.y, camera->v.z, -1.0 * dotProductVec3(camera->v, camera->position)},
        {camera->w.x, camera->w.y, camera->w.z, -1.0 * dotProductVec3(camera->w, camera->position)},
        {0.0, 0.0, 0.0, 1.0}
    };

    return Matrix4(matrix);
}

Matrix4 calculateOrthographicTransformation(Camera* camera){
    double matrix[4][4] = {
        {2.0 / (camera->right - camera->left), 0.0, 0.0, -1.0 * (camera->right + camera->left) / (camera->right - camera->left)},
        {0.0, 2.0 / (camera->top - camera->bottom), 0.0, -1.0 * (camera->top + camera->bottom) / (camera->top - camera->bottom)},
        {0.0, 0.0, 2.0 / (camera->near - camera->far), -1.0 * (camera->near + camera->far) / (camera->near - camera->far)},
        {0.0, 0.0, 0.0, 1.0}
    };

    return Matrix4(matrix);

}

Matrix4 calculatePerspectiveTransformation(Camera* camera) {

    /*
    calculate p2o, then multiply to get Mper
    */

   Matrix4 ortho = calculateOrthographicTransformation(camera);

   double p2o[4][4] = {
       {camera->near, 0.0, 0.0, 0.0},
       {0.0, camera->near, 0.0, 0.0},
       {0.0, 0.0, camera->near + camera->far, camera->near * camera->far},
       {0.0, 0.0, -1.0, 0.0}
   };

    Matrix4 perspectiveToOrtho = Matrix4(p2o);

    Matrix4 result = multiplyMatrixWithMatrix(ortho, perspectiveToOrtho);
    
    return result; 
    
}

Matrix4 calculateViewportTransformation(Camera* camera){
    double matrix[4][4] = {
        {camera->horRes / 2.0, 0.0, 0.0, (camera->horRes - 1.0) / 2.0},
        {0.0, camera->verRes / 2.0, 0.0, (camera->verRes - 1.0) / 2.0},
        {0.0, 0.0, 0.5, 0.5},
        {0.0, 0.0, 0.0, 1.0}
    };
    
    return Matrix4(matrix);
}

Matrix4 calculateViewingTransformation(Camera* camera){
    Matrix4 cameraTransformation = calculateCameraTransformation(camera);
    Matrix4 orthographicTransformation = calculateOrthographicTransformation(camera);
    Matrix4 viewportTransformation = calculateViewportTransformation(camera);
    if (camera->projectionType == 1)
        return multiplyMatrixWithMatrix(viewportTransformation, multiplyMatrixWithMatrix(orthographicTransformation, cameraTransformation));

    Matrix4 perspectiveTransformation = calculatePerspectiveTransformation(camera);
    return multiplyMatrixWithMatrix(viewportTransformation, multiplyMatrixWithMatrix(perspectiveTransformation, cameraTransformation));
}
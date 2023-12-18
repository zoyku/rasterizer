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
            Translation* translation = translations[mesh->transformationIds[i]-1];
            Matrix4 m = translation->calculateMatrix();
            modelingTransformation = multiplyMatrixWithMatrix(m, modelingTransformation);
        }
        else if (mesh->transformationTypes[i] == 's')
        {
            Scaling* scaling = scalings[mesh->transformationIds[i]-1];
            Matrix4 m = scaling->calculateMatrix();
            modelingTransformation = multiplyMatrixWithMatrix(m, modelingTransformation);
        }
        else if (mesh->transformationTypes[i] == 'r')
        {
            Rotation* rotation = rotations[mesh->transformationIds[i]-1];
            Matrix4 m = rotation->calculateMatrix();
            modelingTransformation = multiplyMatrixWithMatrix(m, modelingTransformation);
        }
    }
    return modelingTransformation;
}

Matrix4 calculateCameraTransformation(Camera* camera){

    // The last column written in the slide is basically the negative dot product of the basis vector and the camera position

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
        {(2.0 / (camera->right - camera->left)), 0.0, 0.0, -1.0 * ((camera->right + camera->left) / (camera->right - camera->left))},
        {0.0, (2.0 / (camera->top - camera->bottom)), 0.0, -1.0 * ((camera->top + camera->bottom) / (camera->top - camera->bottom))},
        {0.0, 0.0, (- (2.0 / (camera->far - camera->near))), -1.0 * ((camera->far + camera->near) / (camera->far - camera->near))},
        {0.0, 0.0, 0.0, 1.0}
    };

    return Matrix4(matrix);

}

Matrix4 calculatePerspectiveTransformation(Camera* camera) {

    // Calculate p2o, then multiply to get Mper: the result matrix is directly from slides

    double perspective[4][4] = {{(2*camera->near) / (camera->right - camera->left), 0, (camera->right + camera->left) / (camera->right - camera->left), 0},
                            {0, (2*camera->near) / (camera->top - camera->bottom), (camera->top + camera->bottom) / (camera->top - camera->bottom), 0},
                            {0, 0, -((camera->far + camera->near) / (camera->far - camera->near)), -((2*camera->far*camera->near) / (camera->far - camera->near))},
                            {0, 0, -1, 0}};
    return Matrix4(perspective);
    
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
    if (camera->projectionType == 0)
        return calculateOrthographicTransformation(camera);

    else 
        return calculatePerspectiveTransformation(camera);
}
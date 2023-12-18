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
     double translate[4][4] = {{1, 0, 0, -(camera->position.x)},
                        {0, 1, 0, -(camera->position.y)},
                        {0, 0, 1, -(camera->position.z)},
                        {0, 0, 0, 1}};
    double rotate[4][4] = {{camera->u.x, camera->u.y, camera->u.z, 0},
                      {camera->v.x, camera->v.y, camera->v.z, 0},
                      {camera->w.x, camera->w.y, camera->w.z, 0},
                      {0, 0, 0, 1}};
    return multiplyMatrixWithMatrix(rotate, translate);
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
    calculate p2o, then multiply to get Mper, write the result matrix directly
    */

   Matrix4 ortho = calculateOrthographicTransformation(camera);

   double p2o[4][4] = {
       {camera->near, 0.0, 0.0, 0.0},
       {0.0, camera->near, 0.0, 0.0},
       {0.0, 0.0, camera->near + camera->far, camera->near * camera->far},
       {0.0, 0.0, -1.0, 0.0}
   };

    Matrix4 perspectiveToOrtho = Matrix4(p2o);

    Matrix4 result = multiplyMatrixWithMatrix(perspectiveToOrtho, ortho);
    
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
    if (camera->projectionType == 0)
        return calculateOrthographicTransformation(camera);

    else 
        return calculatePerspectiveTransformation(camera);
}
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <cstring>
#include <string>
#include <vector>
#include <cmath>

#include "tinyxml2.h"
#include "Triangle.h"
#include "Helpers.h"
#include "Scene.h"
#include "clipcull.h"
#include "transformations.h"
#include "rasterize.h"


using namespace tinyxml2;
using namespace std;

/*
	Parses XML file
*/
Scene::Scene(const char *xmlPath)
{
	const char *str;
	XMLDocument xmlDoc;
	XMLElement *xmlElement;

	xmlDoc.LoadFile(xmlPath);

	XMLNode *rootNode = xmlDoc.FirstChild();

	// read background color
	xmlElement = rootNode->FirstChildElement("BackgroundColor");
	str = xmlElement->GetText();
	sscanf(str, "%lf %lf %lf", &backgroundColor.r, &backgroundColor.g, &backgroundColor.b);

	// read culling
	xmlElement = rootNode->FirstChildElement("Culling");
	if (xmlElement != NULL)
	{
		str = xmlElement->GetText();

		if (strcmp(str, "enabled") == 0)
		{
			this->cullingEnabled = true;
		}
		else
		{
			this->cullingEnabled = false;
		}
	}

	// read cameras
	xmlElement = rootNode->FirstChildElement("Cameras");
	XMLElement *camElement = xmlElement->FirstChildElement("Camera");
	XMLElement *camFieldElement;
	while (camElement != NULL)
	{
		Camera *camera = new Camera();

		camElement->QueryIntAttribute("id", &camera->cameraId);

		// read projection type
		str = camElement->Attribute("type");

		if (strcmp(str, "orthographic") == 0)
		{
			camera->projectionType = ORTOGRAPHIC_PROJECTION;
		}
		else
		{
			camera->projectionType = PERSPECTIVE_PROJECTION;
		}

		camFieldElement = camElement->FirstChildElement("Position");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->position.x, &camera->position.y, &camera->position.z);

		camFieldElement = camElement->FirstChildElement("Gaze");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->gaze.x, &camera->gaze.y, &camera->gaze.z);

		camFieldElement = camElement->FirstChildElement("Up");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->v.x, &camera->v.y, &camera->v.z);

		camera->gaze = normalizeVec3(camera->gaze);
		camera->u = crossProductVec3(camera->gaze, camera->v);
		camera->u = normalizeVec3(camera->u);

		camera->w = inverseVec3(camera->gaze);
		camera->v = crossProductVec3(camera->u, camera->gaze);
		camera->v = normalizeVec3(camera->v);

		camFieldElement = camElement->FirstChildElement("ImagePlane");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf %lf %lf %lf %d %d",
			   &camera->left, &camera->right, &camera->bottom, &camera->top,
			   &camera->near, &camera->far, &camera->horRes, &camera->verRes);

		camFieldElement = camElement->FirstChildElement("OutputName");
		str = camFieldElement->GetText();
		camera->outputFilename = string(str);

		this->cameras.push_back(camera);

		camElement = camElement->NextSiblingElement("Camera");
	}

	// read vertices
	xmlElement = rootNode->FirstChildElement("Vertices");
	XMLElement *vertexElement = xmlElement->FirstChildElement("Vertex");
	int vertexId = 1;

	while (vertexElement != NULL)
	{
		Vec3 *vertex = new Vec3();
		Color *color = new Color();

		vertex->colorId = vertexId;

		str = vertexElement->Attribute("position");
		sscanf(str, "%lf %lf %lf", &vertex->x, &vertex->y, &vertex->z);

		str = vertexElement->Attribute("color");
		sscanf(str, "%lf %lf %lf", &color->r, &color->g, &color->b);

		this->vertices.push_back(vertex);
		this->colorsOfVertices.push_back(color);

		vertexElement = vertexElement->NextSiblingElement("Vertex");

		vertexId++;
	}

	// read translations
	xmlElement = rootNode->FirstChildElement("Translations");
	XMLElement *translationElement = xmlElement->FirstChildElement("Translation");
	while (translationElement != NULL)
	{
		Translation *translation = new Translation();

		translationElement->QueryIntAttribute("id", &translation->translationId);

		str = translationElement->Attribute("value");
		sscanf(str, "%lf %lf %lf", &translation->tx, &translation->ty, &translation->tz);

		this->translations.push_back(translation);

		translationElement = translationElement->NextSiblingElement("Translation");
	}

	// read scalings
	xmlElement = rootNode->FirstChildElement("Scalings");
	XMLElement *scalingElement = xmlElement->FirstChildElement("Scaling");
	while (scalingElement != NULL)
	{
		Scaling *scaling = new Scaling();

		scalingElement->QueryIntAttribute("id", &scaling->scalingId);
		str = scalingElement->Attribute("value");
		sscanf(str, "%lf %lf %lf", &scaling->sx, &scaling->sy, &scaling->sz);

		this->scalings.push_back(scaling);

		scalingElement = scalingElement->NextSiblingElement("Scaling");
	}

	// read rotations
	xmlElement = rootNode->FirstChildElement("Rotations");
	XMLElement *rotationElement = xmlElement->FirstChildElement("Rotation");
	while (rotationElement != NULL)
	{
		Rotation *rotation = new Rotation();

		rotationElement->QueryIntAttribute("id", &rotation->rotationId);
		str = rotationElement->Attribute("value");
		sscanf(str, "%lf %lf %lf %lf", &rotation->angle, &rotation->ux, &rotation->uy, &rotation->uz);

		this->rotations.push_back(rotation);

		rotationElement = rotationElement->NextSiblingElement("Rotation");
	}

	// read meshes
	xmlElement = rootNode->FirstChildElement("Meshes");

	XMLElement *meshElement = xmlElement->FirstChildElement("Mesh");
	while (meshElement != NULL)
	{
		Mesh *mesh = new Mesh();

		meshElement->QueryIntAttribute("id", &mesh->meshId);

		// read projection type
		str = meshElement->Attribute("type");

		if (strcmp(str, "wireframe") == 0)
		{
			mesh->type = WIREFRAME_MESH;
		}
		else
		{
			mesh->type = SOLID_MESH;
		}

		// read mesh transformations
		XMLElement *meshTransformationsElement = meshElement->FirstChildElement("Transformations");
		XMLElement *meshTransformationElement = meshTransformationsElement->FirstChildElement("Transformation");

		while (meshTransformationElement != NULL)
		{
			char transformationType;
			int transformationId;

			str = meshTransformationElement->GetText();
			sscanf(str, "%c %d", &transformationType, &transformationId);

			mesh->transformationTypes.push_back(transformationType);
			mesh->transformationIds.push_back(transformationId);

			meshTransformationElement = meshTransformationElement->NextSiblingElement("Transformation");
		}

		mesh->numberOfTransformations = mesh->transformationIds.size();

		// read mesh faces
		char *row;
		char *cloneStr;
		int v1, v2, v3;
		XMLElement *meshFacesElement = meshElement->FirstChildElement("Faces");
		str = meshFacesElement->GetText();
		cloneStr = strdup(str);

		row = strtok(cloneStr, "\n");
		while (row != NULL)
		{
			int result = sscanf(row, "%d %d %d", &v1, &v2, &v3);

			if (result != EOF)
			{
				mesh->triangles.push_back(Triangle(v1, v2, v3));
			}
			row = strtok(NULL, "\n");
		}
		mesh->numberOfTriangles = mesh->triangles.size();
		this->meshes.push_back(mesh);

		meshElement = meshElement->NextSiblingElement("Mesh");
	}
}

void Scene::assignColorToPixel(int i, int j, Color c)
{
	this->image[i][j].r = c.r;
	this->image[i][j].g = c.g;
	this->image[i][j].b = c.b;
}

/*
	Initializes image with background color
*/
void Scene::initializeImage(Camera *camera)
{
	if (this->image.empty())
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			vector<Color> rowOfColors;
			vector<double> rowOfDepths;

			for (int j = 0; j < camera->verRes; j++)
			{
				rowOfColors.push_back(this->backgroundColor);
				rowOfDepths.push_back(1.01);
			}

			this->image.push_back(rowOfColors);
			this->depth.push_back(rowOfDepths);
		}
	}
	else
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			for (int j = 0; j < camera->verRes; j++)
			{
				assignColorToPixel(i, j, this->backgroundColor);
				this->depth[i][j] = 1.01;
				this->depth[i][j] = 1.01;
				this->depth[i][j] = 1.01;
			}
		}
	}
}

/*
	If given value is less than 0, converts value to 0.
	If given value is more than 255, converts value to 255.
	Otherwise returns value itself.
*/
int Scene::makeBetweenZeroAnd255(double value)
{
	if (value >= 255.0)
		return 255;
	if (value <= 0.0)
		return 0;
	return (int)(value);
}

/*
	Writes contents of image (Color**) into a PPM file.
*/
void Scene::writeImageToPPMFile(Camera *camera)
{
	ofstream fout;

	fout.open(camera->outputFilename.c_str());

	fout << "P3" << endl;
	fout << "# " << camera->outputFilename << endl;
	fout << camera->horRes << " " << camera->verRes << endl;
	fout << "255" << endl;

	for (int j = camera->verRes - 1; j >= 0; j--)
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			fout << makeBetweenZeroAnd255(this->image[i][j].r) << " "
				 << makeBetweenZeroAnd255(this->image[i][j].g) << " "
				 << makeBetweenZeroAnd255(this->image[i][j].b) << " ";
		}
		fout << endl;
	}
	fout.close();
}

/*
	Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
*/
void Scene::convertPPMToPNG(string ppmFileName)
{
	string command;

	// TODO: Change implementation if necessary.
	command = "./magick convert " + ppmFileName + " " + ppmFileName + ".png";
	system(command.c_str());
}

/*
	Transformations, clipping, culling, rasterization are done here.
*/
void Scene::forwardRenderingPipeline(Camera *camera)
{
	Matrix4 cameraMatrix = calculateCameraTransformation(camera);
	Matrix4 viewingTransformation = calculateViewingTransformation(camera);
	Matrix4 viewportTransformation = calculateViewportTransformation(camera);


	for (int i = 0; i < this->meshes.size(); i++) {
		Matrix4 modelTransformation = calculateModelingTransformation(this->meshes[i], this->translations, this->scalings, this->rotations);
		Matrix4 modelCameraMatrix = multiplyMatrixWithMatrix(cameraMatrix, modelTransformation);
		Matrix4 resultMatrix = multiplyMatrixWithMatrix(viewingTransformation, modelCameraMatrix);
		Mesh *mesh = this->meshes[i];
		for (int j = 0; j < mesh->triangles.size(); j++){
			Triangle triangle = mesh->triangles[j];
			int v0_id = triangle.vertexIds[0]-1;
			int v1_id = triangle.vertexIds[1]-1;
			int v2_id = triangle.vertexIds[2]-1;
			Vec4 v0 = Vec4(this->vertices[v0_id]->x, this->vertices[v0_id]->y, this->vertices[v0_id]->z, 1.0, this->vertices[v0_id]->colorId);
			Vec4 v1 = Vec4(this->vertices[v1_id]->x, this->vertices[v1_id]->y, this->vertices[v1_id]->z, 1.0, this->vertices[v1_id]->colorId);
			Vec4 v2 = Vec4(this->vertices[v2_id]->x, this->vertices[v2_id]->y, this->vertices[v2_id]->z, 1.0, this->vertices[v2_id]->colorId);
			v0 = multiplyMatrixWithVec4(resultMatrix, v0);
			v1 = multiplyMatrixWithVec4(resultMatrix, v1);
			v2 = multiplyMatrixWithVec4(resultMatrix, v2);

			if (this->cullingEnabled && backfaceCull(v0, v1, v2) < 0) {
				continue;
			}

			v0 = Vec4(v0.x/v0.t, v0.y/v0.t, v0.z/v0.t, 1.0, v0.colorId);
			v1 = Vec4(v1.x/v1.t, v1.y/v1.t, v1.z/v1.t, 1.0, v1.colorId);
			v2 = Vec4(v2.x/v2.t, v2.y/v2.t, v2.z/v2.t, 1.0, v2.colorId);
			Vec4 v0_0 = v0;
			Vec4 v1_0 = v1;
			Vec4 v1_1 = v1;
			Vec4 v2_1 = v2;
			Vec4 v2_2 = v2;
			Vec4 v0_2 = v0;

			if (mesh->type == 0) {
				int line0 = liangClip(v0_0, v1_0, *(this->colorsOfVertices[v0.colorId-1]), *(this->colorsOfVertices[v1.colorId-1]));
				int line1 = liangClip(v1_1, v2_1, *(this->colorsOfVertices[v1.colorId-1]), *(this->colorsOfVertices[v2.colorId-1]));
				int line2 = liangClip(v2_2, v0_2, *(this->colorsOfVertices[v2.colorId-1]), *(this->colorsOfVertices[v0.colorId-1]));

				if (line0) {
					v0 = multiplyMatrixWithVec4(viewportTransformation, v0_0);
					v1 = multiplyMatrixWithVec4(viewportTransformation, v1_0);
					lineRasterization(v0, v1, *this->colorsOfVertices[v0.colorId-1], *this->colorsOfVertices[v1.colorId-1], this->image);
				}
				if (line1) {
					v1 = multiplyMatrixWithVec4(viewportTransformation, v1_1);
					v2 = multiplyMatrixWithVec4(viewportTransformation, v2_1);
					lineRasterization(v1, v2, *this->colorsOfVertices[v1.colorId-1], *this->colorsOfVertices[v2.colorId-1], this->image);
				}
				if (line2) {
					v2 = multiplyMatrixWithVec4(viewportTransformation, v2_2);
					v0 = multiplyMatrixWithVec4(viewportTransformation, v0_2);
					lineRasterization(v2, v0, *this->colorsOfVertices[v2.colorId-1], *this->colorsOfVertices[v0.colorId-1], this->image);
				}
			}
			else {
				v0 = multiplyMatrixWithVec4(viewportTransformation, v0);
				v1 = multiplyMatrixWithVec4(viewportTransformation, v1);
				v2 = multiplyMatrixWithVec4(viewportTransformation, v2);
				triangleRasterization(this->image, this->depth, v0, v1, v2, *this->colorsOfVertices[v0.colorId-1], *this->colorsOfVertices[v1.colorId-1], *this->colorsOfVertices[v2.colorId-1], camera->horRes, camera->verRes);

			}
		}
	}

	// TODO: Implement this function
}

#ifndef MODEL_H
#define MODEL_H

#include <Windows.h>
#include <GL\GL.h>
#include <GL\freeglut.h>
#include <glm.hpp>
#include "Importer.h"
#include <assimp\scene.h>
#include <assimp\cimport.h>
#include <IL\il.h>
#include <map>
#include <vector>
#include <iterator>

struct Mesh{
	glm::vec3* vertices;
	glm::vec3* normals;
	int** faceVertexIndices;

	int numFaces;
	int mNumVertices;
	bool hasTexture;
	int materialIndex;

	std::string name;
};

class Model{
private:
	/* Variables */
	Assimp::Importer importer;
	const aiScene* model;
	int numMeshes;
	Mesh** meshes;
	GLuint displayListID;
	bool yUp;
	float modelScale;
	float time;
	std::map<std::string, aiNodeAnim*> nodeAnimationMap;

	aiMatrix3x3 MODEL_ROT_MAT;

	/* Temp Veriables for getVertices and getNormals */
	std::map<std::string, Mesh> meshesMap;
	bool gettingVertsAndNormals;

	/* Functions */
	aiNode* findNodeRecurse(aiNode*, aiString);
	void recursiveRender(const struct aiNode*);
	void initNodes(const aiScene*, aiNode*);
	void initTextures(const aiScene*);
	void initAnimations(const aiScene*);
	std::map<std::string, GLuint> textureIdMap;	
public:
	Model();
	~Model();

	bool loadModel(const char*);
	void initModel(const aiScene*);
	void setLighting();
	void initDisplayList();
	void drawModel();
	void drawFromDisplayList();
	void setYUp(bool y){ yUp = y; };
	void setModelScale(float s){ modelScale = s; };
	void animate(float);

	std::map<std::string, Mesh> getMeshes();
};

#endif
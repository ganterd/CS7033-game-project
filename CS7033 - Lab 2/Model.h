#pragma once

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
#include <pthread.h>
#include "Logger.h"

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

	/* Thread specific variables */

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
	void* initDisplayListThread();

	void drawModel();
	void drawFromDisplayList();
	void setYUp(bool y){ yUp = y; };
	void setModelScale(float s){ modelScale = s; };
	void animate(float);

	std::map<std::string, Mesh> getMeshes();


};

struct _loadModelThreadArgs{
	const char* path;
	Model* m;
};

static void* _loadModelThread(void* threadArgs){
	_loadModelThreadArgs* argv = (_loadModelThreadArgs*) threadArgs;
	Model* model = argv->m;
	model->loadModel(argv->path);
	pthread_exit(NULL);
	return NULL;
}

static pthread_mutex_t _loadTextureMutex = PTHREAD_MUTEX_INITIALIZER;
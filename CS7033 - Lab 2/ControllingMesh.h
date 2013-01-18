#ifndef CONTROLLING_MESH_H
#define CONTROLLING_MESH_H

#include <iostream>
#include <glm.hpp>
#include <gtx/rotate_vector.hpp>

#include "Model.h"
#include "Util.h"

class ControllingMesh{
private:
	Model* meshModel;	
	bool* somethingInMesh;
	int numMeshes;
	Mesh* meshes;
public:
	ControllingMesh();
	ControllingMesh(std::string);

	void clear();
	void loadFromModel(Model* model);
	void drawMeshes();
	void drawMeshes(glm::vec3, glm::vec3);
	std::string inMesh(float, float, float);
	std::string inMesh(glm::vec3);
	std::string inMesh(glm::vec3, glm::vec3, glm::vec3);
	std::string intersectsMesh(glm::vec3, glm::vec3, glm::vec3, glm::vec3);
	int mNumMeshes(){ return numMeshes; };
	glm::vec3 meshMedian(int);
};

#endif
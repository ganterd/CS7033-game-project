#include "ControllingMesh.h"

ControllingMesh::ControllingMesh(){
	numMeshes = 0;
	meshes = new Mesh[numMeshes];
	somethingInMesh = new bool[numMeshes];
}

ControllingMesh::ControllingMesh(std::string filePath){
	meshModel = new Model();
	meshModel->loadModel(filePath.c_str());
	loadFromModel(meshModel);
}

void ControllingMesh::loadFromModel(Model* model){
	std::map<std::string, Mesh> meshesMap(model->getMeshes());

	numMeshes = meshesMap.size();
	meshes = new Mesh[meshesMap.size()];
	somethingInMesh = new bool[numMeshes];
	std::map<std::string, Mesh>::iterator iter;
	std::string strToReturn; //This is no longer on the heap

	int i = 0;
	for (iter = meshesMap.begin(); iter != meshesMap.end(); ++iter, ++i) {
		meshes[i] = iter->second;
		meshes[i].name = iter->first;
		somethingInMesh[i] = false;
	}
}

void ControllingMesh::drawMeshes(){
	drawMeshes(glm::vec3(0,0,0), glm::vec3(0,0,0));
}

void ControllingMesh::drawMeshes(glm::vec3 offset, glm::vec3 rotations){
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for(int i = 0; i < numMeshes; i++){
		Mesh m = meshes[i];
		if(somethingInMesh[i])
			glColor4f(0.8f, 0.8f, 1.0f, 0.5f);
		else
			glColor4f(1, 0.8f, 0.8f, 0.5f);
		somethingInMesh[i] = false;

		glBegin(GL_TRIANGLES);
		for(int f = 0; f < m.numFaces; f++){
			int* v = m.faceVertexIndices[f];

			glm::vec3 v1 = m.vertices[m.faceVertexIndices[f][0]];
			v1 = glm::rotateX(v1, rotations[0]);
			v1 = glm::rotateY(v1, rotations[1]);
			v1 = glm::rotateZ(v1, rotations[2]);
			v1 += offset;

			glm::vec3 v2 = m.vertices[m.faceVertexIndices[f][1]];
			v2 = glm::rotateX(v2, rotations[0]);
			v2 = glm::rotateY(v2, rotations[1]);
			v2 = glm::rotateZ(v2, rotations[2]);
			v2 += offset;

			glm::vec3 v3 = m.vertices[m.faceVertexIndices[f][2]];
			v3 = glm::rotateX(v3, rotations[0]);
			v3 = glm::rotateY(v3, rotations[1]);
			v3 = glm::rotateZ(v3, rotations[2]);
			v3 += offset;

			glVertex3f(v1.x, v1.y, v1.z);
			glVertex3f(v2.x, v2.y, v2.z);
			glVertex3f(v3.x, v3.y, v3.z);
		}
		glEnd();
	}
	
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

std::string ControllingMesh::inMesh(float x, float y, float z){
	return inMesh(glm::vec3(x, y, z));
}

std::string ControllingMesh::inMesh(glm::vec3 p){
	return inMesh(p, glm::vec3(0,0,0), glm::vec3(0,0,0));
}

std::string ControllingMesh::inMesh(glm::vec3 p, glm::vec3 offset, glm::vec3 rotations){
	for(int i = 0; i < numMeshes; i++){
		Mesh m = meshes[i];
		bool inMesh = true;
		for(int f = 0; f < m.numFaces && inMesh; f++){
			glm::vec3 v1 = m.vertices[m.faceVertexIndices[f][0]];
			v1 = glm::rotateX(v1, rotations[0]);
			v1 = glm::rotateY(v1, rotations[1]);
			v1 = glm::rotateZ(v1, rotations[2]);
			v1 += offset;

			glm::vec3 toPoint = glm::normalize(p - v1);

			glm::vec3 v2 = m.vertices[m.faceVertexIndices[f][1]];
			v2 = glm::rotateX(v2, rotations[0]);
			v2 = glm::rotateY(v2, rotations[1]);
			v2 = glm::rotateZ(v2, rotations[2]);
			v2 += offset;

			glm::vec3 v3 = m.vertices[m.faceVertexIndices[f][2]];
			v3 = glm::rotateX(v3, rotations[0]);
			v3 = glm::rotateY(v3, rotations[1]);
			v3 = glm::rotateZ(v3, rotations[2]);
			v3 += offset;

			glm::vec3 faceNormal = glm::cross(v2 - v1, v3 - v1);

			if(glm::dot(faceNormal, toPoint) > 0.0f)
				inMesh = false;
		}
		if(inMesh){
			somethingInMesh[i] = true;
			return m.name;
		}
	}
	return "";
}

std::string ControllingMesh::intersectsMesh(glm::vec3 p, glm::vec3 dir, glm::vec3 offset, glm::vec3 rotations){
	for(int i = 0; i < numMeshes; i++){
		Mesh m = meshes[i];
		for(int f = 0; f < m.numFaces; f++){
			glm::vec3 v1 = m.vertices[m.faceVertexIndices[f][0]];
			v1 = glm::rotateX(v1, rotations[0]);
			v1 = glm::rotateY(v1, rotations[1]);
			v1 = glm::rotateZ(v1, rotations[2]);
			v1 += offset;

			glm::vec3 toPoint = glm::normalize(p - v1);

			glm::vec3 v2 = m.vertices[m.faceVertexIndices[f][1]];
			v2 = glm::rotateX(v2, rotations[0]);
			v2 = glm::rotateY(v2, rotations[1]);
			v2 = glm::rotateZ(v2, rotations[2]);
			v2 += offset;

			glm::vec3 v3 = m.vertices[m.faceVertexIndices[f][2]];
			v3 = glm::rotateX(v3, rotations[0]);
			v3 = glm::rotateY(v3, rotations[1]);
			v3 = glm::rotateZ(v3, rotations[2]);
			v3 += offset;

			glm::vec3 faceNormal = glm::cross(v2 - v1, v3 - v1);

			if(Util::lineTriangleIntersection(p, dir, v1, v2, v3)){
				somethingInMesh[i] = true;
				return m.name;
			}
		}
	}
	return "";
}

glm::vec3 ControllingMesh::meshMedian(int meshNo){
	glm::vec3 result(0,0,0);
	if(meshNo > numMeshes || meshNo < 0)
		return result;
	Mesh m = meshes[meshNo];
	result = m.vertices[0];

	for(int i = 1; i < m.mNumVertices; i++){
		result +=  (m.vertices[i] - result) * glm::vec3(0.5f, 0.5f, 0.5f);
	}
	return result;
}
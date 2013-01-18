#ifndef LEVEL_H
#define LEVEL_H

#include "Model.h"
#include "ControllingMesh.h"
#include <string>

#define LEVELS_FOLDER "resources/levels/"
#define LEVEL_BOUNDARY 0
#define LEVEL_GIVEWEAPON_SLINGSHOT 1

class Level{
private:
	std::string levelName;
	Model* levelModel;
	Model* skyBox;
	ControllingMesh* controlMesh;
	ControllingMesh* floorMesh;
	ControllingMesh* boundaryMesh;
	ControllingMesh* spawnPoints;
	std::map<unsigned int, bool> controlEventEncountered;

public:
	Level(std::string levelName);
	void draw();
	unsigned int controllerInteraction(glm::vec3);
	unsigned int controllerInteraction(float, float, float);
	bool encounteredEvent(unsigned int);
	bool outOfBounds(glm::vec3);
	void setEventEncoutered(unsigned int);
	glm::vec3 chooseRandomSpawn();
	glm::vec3 chooseSpawnBehind(glm::vec3, glm::vec3);
};

#endif
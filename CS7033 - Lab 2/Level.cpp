#include "Level.h"

Level::Level(std::string levelName){
	std::string levelModelName = LEVELS_FOLDER;
	std::string levelSkyBoxName = LEVELS_FOLDER;
	std::string levelControlMeshName = LEVELS_FOLDER;
	std::string levelFloorMeshName = LEVELS_FOLDER;
	std::string levelBoundaryMeshName = LEVELS_FOLDER;
	std::string levelSpawnPointsName = LEVELS_FOLDER;

	levelModelName.append(levelName).append("_MODEL.dae");
	levelSkyBoxName.append(levelName).append("_SKY_BOX.dae");
	levelControlMeshName.append(levelName).append("_CONTROL_MESH.dae");
	levelFloorMeshName.append(levelName).append("_FLOOR.dae");
	levelBoundaryMeshName.append(levelName).append("_BOUNDARIES.dae");
	levelSpawnPointsName.append(levelName).append("_SPAWNS.dae");

	levelModel = new Model();
	if(!levelModel->loadModel(levelModelName.c_str())){
		levelModel = NULL;
		return;
	}

	levelModel->initDisplayList();

	skyBox = new Model();
	skyBox->loadModel(levelSkyBoxName.c_str());
	skyBox->initDisplayList();

	boundaryMesh = new ControllingMesh(levelBoundaryMeshName);
	controlMesh = new ControllingMesh(levelControlMeshName);
	spawnPoints = new ControllingMesh(levelSpawnPointsName);
}

void Level::draw(){
	if(!levelModel)
		return;

	/* Draw the skybox first */
	glPushMatrix();
		if(skyBox)
			skyBox->drawFromDisplayList();
	glPopMatrix();

	/* Set the lighting from the level */
	levelModel->setLighting();

	glPushMatrix();
		levelModel->drawFromDisplayList();
	glPopMatrix();
	//boundaryMesh->drawMeshes();
}

bool Level::outOfBounds(glm::vec3 position){
	std::string intersectedMesh = boundaryMesh->inMesh(position);
	return !intersectedMesh.empty();
}

unsigned int Level::controllerInteraction(float x, float y, float z){
	return controllerInteraction(glm::vec3(x, y, z));
}

bool Level::encounteredEvent(unsigned int controlEventID){
	if(controlEventEncountered.find(controlEventID) != controlEventEncountered.end())
		return true;
	return false;
}

void Level::setEventEncoutered(unsigned int controlEventID){
	controlEventEncountered[controlEventID] = true;
}

unsigned int Level::controllerInteraction(glm::vec3 p){
	if(strcmp(controlMesh->inMesh(p).c_str(), "CMESH_WEAPON_SLING-material") == 0)
		if(!encounteredEvent(LEVEL_GIVEWEAPON_SLINGSHOT)){
			setEventEncoutered(LEVEL_GIVEWEAPON_SLINGSHOT);
			return LEVEL_GIVEWEAPON_SLINGSHOT;
		}
	return 0;
}

glm::vec3 Level::chooseRandomSpawn(){
	if(spawnPoints->mNumMeshes() == 0)
		return glm::vec3(0,0,0);

	int randomNum = (rand() / RAND_MAX) / spawnPoints->mNumMeshes();
	return spawnPoints->meshMedian(randomNum);
}

glm::vec3 Level::chooseSpawnBehind(glm::vec3 pPoint, glm::vec3 dir){
	if(spawnPoints->mNumMeshes() == 0)
		return glm::vec3(0,0,0);

	std::map<int, glm::vec3> possibleSpawns;
	int j = 0;
	for(int i = 0; i < spawnPoints->mNumMeshes(); i++){
		glm::vec3 sPoint = spawnPoints->meshMedian(i);
		glm::vec3 toPoint = sPoint - pPoint;
		if(glm::dot(dir, sPoint) < 0 )
			possibleSpawns[j++] = sPoint;
	}

	srand(glutGet(GLUT_ELAPSED_TIME));
	if(possibleSpawns.size() == 0)
		return chooseRandomSpawn();

	float t = (rand() / (float)RAND_MAX) * (float)possibleSpawns.size();
	int randomNum = t;

	return possibleSpawns[randomNum];
}
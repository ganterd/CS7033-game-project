#include "ZombieController.h"

ZombieController::ZombieController(){
	zombieCount = 0;

	model = new Model();
	model->loadModel(std::string(MODEL_FOLDER).append(ZOMBIE_MODEL_FILE).c_str());
	boundingBox = new ControllingMesh(std::string(MODEL_FOLDER).append(ZOMBIE_HITBOX_FILE).c_str());
}

std::vector<glm::vec3> ZombieController::getZombiePositions(){
	std::vector<glm::vec3> positions;
	for(unsigned int i = 0; i < zombies.size(); i++){
		if(zombies[i].isAlive())
			positions.push_back(zombies[i].getPos());
	}
	return positions;
}

int ZombieController::collideAndKill(glm::vec3 p, glm::vec3 v){
	int hits = 0;
	for(unsigned int i = 0; i < zombies.size(); i++){
		if(zombies[i].checkCollision(p, v)){
			zombies[i].setAlive(false);
			zombies[i].setAnimation(ZOMBIE_DEATH_ANIMATION);
			hits++;
		}
	}

	return hits;
}

void ZombieController::allFollow(glm::vec3 p){
	target = p;
}

void ZombieController::animate(float gameTime){
	this->gameTime = gameTime;
	for(unsigned int i = 0; i < zombies.size(); i++){
		if(follow){
			zombies[i].follow(target[0], target[1], target[2]);
			if(zombies[i].isAlive() && !zombies[i].isPlayingAnimation())
					zombies[i].setAnimation(ZOMBIE_WALK_ANIMATION);
		}
	}
}

void ZombieController::draw(){
	for(unsigned int i = 0; i < zombies.size(); i++){
		zombies[i].animate(gameTime);
		zombies[i].draw();
	}
}

void ZombieController::addZombieAt(glm::vec3 p){
	Zombie z(p[0], 0, p[2]);
	z.setModel(model);
	z.setBoundingBox(boundingBox);
	if(zombies.size() + 1 > MAX_ZOMBIES)
		zombies.erase(zombies.begin());
	zombies.push_back(z);
}

void ZombieController::addZombieBehindPoint(Level* l, glm::vec3 pPoint, glm::vec3 pDir){
	glm::vec3 spawnPoint = l->chooseSpawnBehind(pPoint, pDir);
	this->addZombieAt(spawnPoint);
}
#ifndef ZOMBIE_CONTROLLER_H
#define ZOMBIE_CONTROLLER_H

#include <iostream>
#include "Zombie.h"
#include "Level.h"
#include <vector>

#define MAX_ZOMBIES 6

class ZombieController{
private:
	std::vector<Zombie> zombies;
	float gameTime;
	bool follow;
	glm::vec3 target;
	int zombieCount;
	Model* model;
	ControllingMesh* boundingBox;

public:
	ZombieController();

	void addZombie();
	void addZombieAt(glm::vec3);
	void addZombieRandomSpawn(Level*);
	void addZombieBehindPoint(Level*, glm::vec3, glm::vec3);

	std::vector<glm::vec3> getZombiePositions();

	int collideAndKill(glm::vec3, glm::vec3);
	void allFollow(glm::vec3);

	bool full(){ return zombies.size() + 1 >= MAX_ZOMBIES; };
	void setGameTime(float t){ gameTime = t; };	
	void draw();
	void animate(float);
};

#endif
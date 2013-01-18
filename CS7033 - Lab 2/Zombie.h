#ifndef ZOMBIE_H
#define ZOMBIE_H

#include <iostream>
#include <glm.hpp>
#include <deque>
#include "Model.h"
#include "ControllingMesh.h"
#include "Util.h"
#include "ModelTimings.h"

#define ZOMBIE_MODEL_FILE "boxman_MODEL.dae"
#define ZOMBIE_HITBOX_FILE "boxman_HITBOXES.dae"
#define ZOMBIE_MAX_ROTATION_SPEED 2.0f

class Zombie{
private:
	glm::vec3 position;
	glm::vec3 rotation;

	float maxWalkSpeed;

	Model* model;
	ControllingMesh* boundingBox;

	std::deque<unsigned int> queuedAnimations;

	void init();

	unsigned int currentWeapon;
	unsigned int currentAnimation;
	float animationStartTime;
	float currentAnimationTime;
	float targetAnimationTime;
	float lastGameTime;
	bool startAnimation;
	bool loopAnimation;
	bool alive;
public:
	Zombie();
	Zombie(float, float, float);
	void setModel(Model* m){ model = m; };
	void setBoundingBox(ControllingMesh* cM){ boundingBox = cM; };

	~Zombie();

	void setPos(float, float, float);
	void setRot(float, float, float);
	void follow(float, float, float);

	void setAnimation(unsigned int);
	void queueAnimation(unsigned int);
	bool isPlayingAnimation();

	void animate(float);

	void draw();
	bool checkCollision(glm::vec3, glm::vec3);
	bool isAlive(){ return alive; };
	void setAlive(bool b){ alive = b; };
	glm::vec3 getPos();
	glm::vec3 getRotations();
};

#endif
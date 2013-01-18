#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <glm.hpp>
#include "Model.h"
#include "Util.h"

class Projectile{
private:
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 gravity;
	float distanceTravelled;
	Model* model;

public:
	Projectile();
	Projectile(glm::vec3, glm::vec3);
	Projectile(glm::vec3, glm::vec3, glm::vec3);
	void init();

	void setModel(Model* m);
	void setPosition(glm::vec3);
	void setVelocity(glm::vec3);
	void setVelocity(glm::vec3, float);
	void setGravity(glm::vec3);
	glm::vec3 getPosition();
	glm::vec3 getVelocity();
	bool hasExpired();

	void update();
	void draw();
};

#endif
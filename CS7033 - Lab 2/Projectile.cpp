#include "Projectile.h"

Projectile::Projectile(){
	init();
}

Projectile::Projectile(glm::vec3 p, glm::vec3 v){
	init();
	setPosition(p);
	setVelocity(v);
}

Projectile::Projectile(glm::vec3 p, glm::vec3 v, glm::vec3 g){
	init();
	setPosition(p);
	setVelocity(v);
	setGravity(g);
}

void Projectile::init(){
	position = glm::vec3(0, 0, 0);
	velocity = glm::vec3(0, 0, 0);
	gravity = glm::vec3(0, PROJECTILE_GRAVITY, 0); /* -9.8 / 60FPS */
	distanceTravelled = 0;
}

void Projectile::setModel(Model* m){
	model = m;
}

void Projectile::setPosition(glm::vec3 p){
	position = p;
}

void Projectile::setVelocity(glm::vec3 v){
	velocity = v;
}

void Projectile::setVelocity(glm::vec3 orientation, float speed){
	velocity = glm::vec3(
		glm::cos(glm::radians(orientation[1])) * speed,
		glm::sin(glm::radians(orientation[0])) * speed,
		glm::sin(glm::radians(orientation[1])) * speed
	);
}

void Projectile::setGravity(glm::vec3 g){
	gravity = g;
}

glm::vec3 Projectile::getPosition(){
	return position;
}

glm::vec3 Projectile::getVelocity(){
	return velocity;
}

bool Projectile::hasExpired(){
	return distanceTravelled >= PROJECTILE_MAX_DISTANCE;
}

void Projectile::update(){
	if(hasExpired())
		return;
	velocity += gravity;
	position += velocity;
	distanceTravelled += glm::distance(glm::vec3(0,0,0), velocity);
}

void Projectile::draw(){
	if(hasExpired())
		return;
	glPushMatrix();
	glTranslatef(position[0], position[1], position[2]);
	model->drawModel();
	glPopMatrix();
}
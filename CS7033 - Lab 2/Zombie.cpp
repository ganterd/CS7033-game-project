#include "Zombie.h"

Zombie::Zombie(){
	init();
	setPos(0, 0, 0);
}

Zombie::Zombie(float x, float y, float z){
	init();
	setPos(x, y, z);
}

Zombie::~Zombie(){
}

void Zombie::init(){
	loopAnimation = false;

	//boundingBox = new ControllingMesh(std::string(MODEL_FOLDER).append(ZOMBIE_HITBOX_FILE).c_str());

	maxWalkSpeed = 0.07f;
	rotation = glm::vec3(0, 90, 0);
}

void Zombie::setPos(float x, float y, float z){
	position[0] = x;
	position[1] = y;
	position[2] = z;
}

glm::vec3 Zombie::getPos(){
	return position;
}

glm::vec3 Zombie::getRotations(){
	return rotation;
}

void Zombie::follow(float x, float y, float z){
	if(!alive)
		return;

	glm::vec2 target(x, z);
	glm::vec2 origin(position[0], position[2]);

	float d = glm::distance(target, origin);

	float angleToTarget = glm::degrees(glm::asin((x - position[0]) / d));
	if(z - position[2] < 0)
		angleToTarget += 2 * (90 - angleToTarget);
	if(angleToTarget < 0)
		angleToTarget += 360;

	if(angleToTarget - rotation[1] + 360 > 0)
		rotation[1] += ZOMBIE_MAX_ROTATION_SPEED;
	else
		rotation[1] -= ZOMBIE_MAX_ROTATION_SPEED;

	position[0] += glm::sin(glm::radians(rotation[1])) * maxWalkSpeed;
	position[2] += glm::cos(glm::radians(rotation[1])) * maxWalkSpeed;
}

bool Zombie::checkCollision(glm::vec3 p, glm::vec3 d){
	if(!alive)
		return false;
	std::string intersectedMesh = boundingBox->inMesh(p, position, rotation);
	if(!intersectedMesh.empty())
		return true;
	intersectedMesh = boundingBox->intersectsMesh(p, d, position, rotation);
	return !intersectedMesh.empty();
}

bool Zombie::isPlayingAnimation(){
	return currentAnimationTime < targetAnimationTime;
}

void Zombie::queueAnimation(unsigned int animation){
	if(!isPlayingAnimation())
		setAnimation(animation);
	else
		queuedAnimations.push_back(animation);
}

void Zombie::setAnimation(unsigned int animation){
	currentAnimation = animation;

	if(animation == ZOMBIE_WALK_ANIMATION){
		currentAnimationTime = ZOMBIE_WALK_ANIMATION_START;
		targetAnimationTime = ZOMBIE_WALK_ANIMATION_END;
		startAnimation = true;
		loopAnimation = false;
		return;
	}

	if(animation == ZOMBIE_DEATH_ANIMATION){
		currentAnimationTime = ZOMBIE_DEATH_ANIMATION_START;
		targetAnimationTime = ZOMBIE_DEATH_ANIMATION_END;
		startAnimation = true;
		loopAnimation = false;
		queueAnimation(ZOMBIE_IS_DEAD_ANIMATION);
		return;
	}

	if(animation == ZOMBIE_IS_DEAD_ANIMATION){
		currentAnimationTime = ZOMBIE_IS_DEAD_ANIMATION_START;
		targetAnimationTime = ZOMBIE_IS_DEAD_ANIMATION_END;
		loopAnimation = true;
		return;
	}
}

void Zombie::animate(float gameTime){
	if(startAnimation){
		startAnimation = false;
		animationStartTime = gameTime;
		lastGameTime = gameTime;
	}

	if(currentAnimationTime >= targetAnimationTime){
		if(loopAnimation){
			setAnimation(currentAnimation);
			animate(gameTime);
			return;
		}
		if(queuedAnimations.size() > 0){
			setAnimation(queuedAnimations.at(0));
			queuedAnimations.pop_front();
		}
		return;
	}

	currentAnimationTime += gameTime - lastGameTime;
	lastGameTime = gameTime;

	model->animate(currentAnimationTime);
}

void Zombie::draw(){
	glPushMatrix();
		glTranslatef(position[0], position[1], position[2]);
		glRotatef(rotation[1], 0,1,0);
		model->drawModel();
	glPopMatrix();
}
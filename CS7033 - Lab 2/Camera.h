#ifndef CAMERA_H
#define CAMERA_H

/* Global Libraries */
#include <iostream>
#include <sstream>
#include <windows.h>
#include <deque>
#include "Util.h"
#include "ModelTimings.h"
#include "ControllingMesh.h"

/* Local Libraries */
#include "gl/glu.h"
#include "glm.hpp"
#include "GL\freeglut.h"
#include "Model.h"

#define CAM_ARMS_DEFAULT "arms_DEFAULT.dae"
#define CAM_ARMS_SLINGSHOT "arms_SLINGSHOT.dae"
#define CAM_WEAPON_ARMS	0
#define CAM_WEAPN_SLINGSHOT 1
#define INDEX_SCREEN_WIDTH 0
#define INDEX_SCREEN_HEIGHT 1
#define INDEX_SCREEN_RATIO 2
#define INDEX_CAM_X 0
#define INDEX_CAM_Y 1
#define INDEX_CAM_Z 2
#define CAM_YAW 1
#define CAM_PITCH 0
#define CAM_ROLL 2
#define INDEX_CAM_FORWARD_ACCEL 0
#define INDEX_CAM_BACKWARD_ACCEL 1
#define INDEX_CAM_STRAFE_ACCEL 2
#define CAM_FADE_SPEED 0.01f

static std::deque<std::string> debugText;

class Camera{
private:
	Model* arms;
	ControllingMesh* boundingBox;

	glm::vec3 position;
	glm::vec3 nPosition;
	glm::vec3 accelerations;
	glm::vec3 velocity;
	glm::vec3 orientation;
	glm::vec3 orientationSpeed;
	glm::vec3 screenAttr;

	float currentFOV;
	float targetFOV;

	float currentFade;
	float targetFade;
	
	bool drawDebug;
	bool flashlightOn;

	unsigned int currentWeapon;
	int playerHealth;
	float lastDamageTime;
	float animationStartTime;
	float currentAnimationTime;
	float targetAnimationTime;
	float lastGameTime;
	bool startAnimation;
public:
	Camera();
	void look();
	void animate(float);
	unsigned int action();
	void drawOverlay();
	void fadeToBlack();
	void fadeFromBlack();
	void fadeToggle();
	void setFade(float);
	void setWeapon(unsigned int);
	unsigned int getWeapon();
	bool isFading();
	void update();
	void setScreenSize(int, int);
	void warpPointerToCenter();
	void mouseReposition(int x, int y);
	void offsetCamera(float x, float y, float z);
	void changeFOV(float);
	glm::vec3 moveForward();
	glm::vec3 moveBackwards();
	glm::vec3 strafeRight();
	glm::vec3 strafeLeft();
	glm::vec3 targetPosition();
	void confirmMove();
	void showDebug(bool t){ drawDebug = t; };
	void toggleDebug(){ drawDebug = !drawDebug; };
	void toggleFlashlight(){ flashlightOn = !flashlightOn; };
	void flashlight();
	glm::vec3 getPos(){ return position; };
	glm::vec3 getOrientation(){ return orientation; };
	glm::vec3 getOrientationAsNormal();
	static void debug(std::string);
	int getHealth(){ return playerHealth; };
	void damage(int);
	bool checkCollision(glm::vec3);
};

#endif
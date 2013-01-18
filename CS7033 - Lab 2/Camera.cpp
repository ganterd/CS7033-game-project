#include  "Camera.h"

Camera::Camera(){
	std::cout << "Initialising camera..." << std::endl;
	accelerations[INDEX_CAM_FORWARD_ACCEL] = 0.2f;
	accelerations[INDEX_CAM_BACKWARD_ACCEL] = -0.1f;
	accelerations[INDEX_CAM_STRAFE_ACCEL] = 0.05f;

	orientation[CAM_YAW] = 90;
	orientation[CAM_PITCH] = 0;
	orientation[CAM_ROLL] = 10;

	orientationSpeed[CAM_YAW] = 0.1f;
	orientationSpeed[CAM_PITCH] = 0.1f;
	orientationSpeed[CAM_ROLL] = 0.01f;

	drawDebug = false;
	flashlightOn = false;
	currentFade = 0;

	currentFOV = 40;
	targetFOV = 45;

	arms = new Model();
	arms->loadModel(MODEL_FOLDER.append(CAM_ARMS_DEFAULT).c_str());
	currentWeapon = CAM_WEAPON_ARMS;
	bool startAnimation = false;
	currentAnimationTime = 0;
	targetAnimationTime = 0;

	boundingBox = new ControllingMesh(MODEL_FOLDER.append("player_HITBOXES.dae").c_str());
	playerHealth = 100;
}

void Camera::changeFOV(float target){
	this->targetFOV = target;
}

void Camera::fadeToBlack(){
	targetFade = 1.0f;
}

void Camera::fadeFromBlack(){
	targetFade = 0.0f;
}

void Camera::setFade(float f){
	currentFade = f;
}

void Camera::fadeToggle(){
	if(targetFade == 0)
		targetFade = 1;
	else
		targetFade = 0;
}

bool Camera::isFading(){
	return currentFade != targetFade;
}

void Camera::setWeapon(unsigned int weaponID){
	switch(weaponID){
	case CAM_WEAPN_SLINGSHOT:
		arms->loadModel(MODEL_FOLDER.append(CAM_ARMS_SLINGSHOT).c_str());
		currentWeapon = CAM_WEAPN_SLINGSHOT;
		break;
	default:
		arms->loadModel(MODEL_FOLDER.append(CAM_ARMS_DEFAULT).c_str());
		break;
	}
}

void Camera::damage(int damage){
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	if(time - lastDamageTime < 1)
		return;
	playerHealth -= damage;
	lastDamageTime = time;
}

unsigned int Camera::action(){
	/* Check if we are currently in an action */
	if(currentAnimationTime < targetAnimationTime)
		return ACTION_NONE;
	if(currentWeapon == CAM_WEAPN_SLINGSHOT){
		currentAnimationTime = ARMS_SLINGSHOT_FIRE_START;
		targetAnimationTime = ARMS_SLINGSHOT_FIRE_END;
		startAnimation = true;
		return ACTION_FIRE_PROJECTILE_SLOW;
	}

	return ACTION_NONE;
}

void Camera::look(){
	glm::vec3 look;

	look[0] = glm::cos(glm::radians(orientation[CAM_YAW])) * 0.2f + position[0];
	look[1] = glm::sin(glm::radians(orientation[CAM_PITCH])) * 0.2f + position[1];
	look[2] = glm::sin(glm::radians(orientation[CAM_YAW])) * 0.2f + position[2];

	gluLookAt(
		position[INDEX_CAM_X],position[INDEX_CAM_Y],position[INDEX_CAM_Z],
		look[INDEX_CAM_X],look[INDEX_CAM_Y],look[INDEX_CAM_Z],
		0,1,0
	);

	if(!drawDebug)
		return;

	std::ostringstream positionDebug;
	std::ostringstream orientationDebug;
	std::ostringstream fovDebug;
	
	positionDebug << "[CAM] Position [" << position[0] << ", " << position[1] << ", " << position[2] << "]";
	orientationDebug << "[CAM] Orientation [" << orientation[CAM_YAW] << ", " << orientation[CAM_PITCH] << ", " << orientation[CAM_ROLL] << "]";
	fovDebug << "[CAM] FOV = " << currentFOV;
	
	debugText.push_back(positionDebug.str());
	debugText.push_back(orientationDebug.str());
	debugText.push_back(fovDebug.str());
}

void Camera::animate(float gameTime){
	if(startAnimation){
		startAnimation = false;
		animationStartTime = gameTime;
		lastGameTime = gameTime;
	}

	if(currentAnimationTime >= targetAnimationTime){
		return;
	}

	currentAnimationTime += gameTime - lastGameTime;
	lastGameTime = gameTime;

	arms->animate(currentAnimationTime);
}

void Camera::drawOverlay(){
	glPushMatrix();
		glTranslatef(position[INDEX_CAM_X], position[INDEX_CAM_Y] - 0.2, position[INDEX_CAM_Z]);
		glRotatef(-orientation[CAM_YAW] + 90, 0,1,0);
		glRotatef(-orientation[CAM_PITCH], 1,0,0);
		arms->drawModel();
	glPopMatrix();

	if(currentFade > 1 || currentFade < 0){
		currentFade = targetFade;
	}else{
		if(targetFade > currentFade){
			currentFade += CAM_FADE_SPEED;
		}else{
			currentFade -= CAM_FADE_SPEED;
		}
	}

	/* Switch Draw Mode */
	glPushMatrix();			// 1
		glDisable(GL_LIGHTING);
		glEnable(GL_PROJECTION);
		glPushMatrix(); // 2
			glLoadIdentity();	
			glMatrixMode(GL_MODELVIEW);
			glOrtho(0, screenAttr[INDEX_SCREEN_WIDTH], screenAttr[INDEX_SCREEN_HEIGHT], 0, -1, 1);
			
			glPushMatrix(); // 3
				glLoadIdentity();
	
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glColor4f(0, 0, 0, currentFade);
				glBegin(GL_QUADS);
					glVertex2f(0, 0);
					glVertex2f(screenAttr[INDEX_SCREEN_WIDTH], 0);
					glVertex2f(screenAttr[INDEX_SCREEN_WIDTH], screenAttr[INDEX_SCREEN_WIDTH]);
					glVertex2f(0, screenAttr[INDEX_SCREEN_WIDTH]);
				glEnd();
				glDisable(GL_BLEND);

				if(drawDebug){
					glColor3f( 255, 255, 255 );

					for(unsigned int w = 0; w < debugText.size(); w++){
						glRasterPos2i(10, 20 + (15 * w));
						const char* str = debugText[w].c_str();
						int len, i;
						len = (int)strlen(str);
						for (i = 0; i < len; i++) {
							glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[i]);		
						}
		
					}
					debugText.clear();
				}
	
			glPopMatrix(); // 3
		glPopMatrix(); // 2
		glEnable(GL_LIGHTING);
	glPopMatrix(); // 1

	std::ostringstream fadeDebug;
	fadeDebug << "[CAM] Fade = " << currentFade;
	debugText.push_back(fadeDebug.str());
}

void Camera::update(){
	if(currentFOV != targetFOV){
		currentFOV += (targetFOV - currentFOV) / 2;
		if(glm::abs(targetFOV - currentFOV) < 1)
			currentFOV = targetFOV;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective((int)currentFOV, screenAttr[INDEX_SCREEN_RATIO], .1, 1000);
	}
	this->flashlight();
}

void Camera::debug(std::string s){
	debugText.push_back(s);
}

void Camera::setScreenSize(int width, int height){
	screenAttr[INDEX_SCREEN_WIDTH] = (float)width;
	screenAttr[INDEX_SCREEN_HEIGHT] = (float)height;
	screenAttr[INDEX_SCREEN_RATIO] = (float)width / (float)height;
	gluPerspective((int)currentFOV, screenAttr[INDEX_SCREEN_RATIO], .1, 1000);
}

void Camera::warpPointerToCenter(){
	int centerX = (int)screenAttr[INDEX_SCREEN_WIDTH] / 2;
	int centerY = (int)screenAttr[INDEX_SCREEN_HEIGHT] / 2;
	glutWarpPointer( centerX, centerY );
}

void Camera::mouseReposition(int x, int y){
	int centerX = (int)screenAttr[INDEX_SCREEN_WIDTH] / 2;
	int centerY = (int)screenAttr[INDEX_SCREEN_HEIGHT] / 2;

	float deltaX = (float)x - centerX;
	float deltaY = (float)y - centerY;

	if (deltaX == 0 && deltaY == 0)
		return;

	orientation[CAM_YAW] = orientation[CAM_YAW] + (deltaX * orientationSpeed[CAM_YAW]);
	orientation[CAM_PITCH] = orientation[CAM_PITCH] - (deltaY * orientationSpeed[CAM_PITCH]);

	if(orientation[CAM_YAW] >= 360) orientation[CAM_YAW] -= 360;
	if(orientation[CAM_YAW] < 0) orientation[CAM_YAW] += 360;
	if(orientation[CAM_PITCH] >= 360) orientation[CAM_PITCH] -= 360;
	if(orientation[CAM_PITCH] < 0) orientation[CAM_PITCH] += 360;	

	glutWarpPointer( centerX, centerY );
}

glm::vec3 Camera::moveForward(){
	nPosition[INDEX_CAM_X] = position[INDEX_CAM_X] + glm::cos(glm::radians(orientation[CAM_YAW])) * accelerations[INDEX_CAM_FORWARD_ACCEL];
	nPosition[INDEX_CAM_Z] = position[INDEX_CAM_Z] + glm::sin(glm::radians(orientation[CAM_YAW])) * accelerations[INDEX_CAM_FORWARD_ACCEL];
	return nPosition;
}

glm::vec3 Camera::moveBackwards(){
	nPosition[INDEX_CAM_X] = position[INDEX_CAM_X] + glm::cos(glm::radians(orientation[CAM_YAW])) * accelerations[INDEX_CAM_BACKWARD_ACCEL];
	nPosition[INDEX_CAM_Z] = position[INDEX_CAM_Z] + glm::sin(glm::radians(orientation[CAM_YAW])) * accelerations[INDEX_CAM_BACKWARD_ACCEL];
	return nPosition;
}

glm::vec3 Camera::strafeLeft(){
	nPosition[INDEX_CAM_X] = position[INDEX_CAM_X] + glm::cos(glm::radians(orientation[CAM_YAW] - 90)) * accelerations[INDEX_CAM_STRAFE_ACCEL];
	nPosition[INDEX_CAM_Z] = position[INDEX_CAM_Z] + glm::sin(glm::radians(orientation[CAM_YAW] - 90)) * accelerations[INDEX_CAM_STRAFE_ACCEL];
	return nPosition;
}

glm::vec3 Camera::strafeRight(){
	nPosition[INDEX_CAM_X] = position[INDEX_CAM_X] + glm::cos(glm::radians(orientation[CAM_YAW] + 90)) * accelerations[INDEX_CAM_STRAFE_ACCEL];
	nPosition[INDEX_CAM_Z] = position[INDEX_CAM_Z] + glm::sin(glm::radians(orientation[CAM_YAW] + 90)) * accelerations[INDEX_CAM_STRAFE_ACCEL];
	return nPosition;
}

glm::vec3 Camera::targetPosition(){
	return nPosition;
}

void Camera::confirmMove(){
	position = nPosition;
}

void Camera::offsetCamera(float x, float y, float z){
	position[INDEX_CAM_X] += x;
	position[INDEX_CAM_Y] += y;
	position[INDEX_CAM_Z] += z;
	nPosition = position;
}

glm::vec3 Camera::getOrientationAsNormal(){
	glm::vec3 look;

	look[0] = glm::cos(glm::radians(orientation[CAM_YAW])) * 1.0f;
	look[1] = glm::sin(glm::radians(orientation[CAM_PITCH])) * 1.0f;
	look[2] = glm::sin(glm::radians(orientation[CAM_YAW])) * 1.0f;
	return look;
}

void Camera::flashlight(){
	std::ostringstream flashLightOnDebug;

	if(!flashlightOn){
		flashLightOnDebug << "[Camera] Flashlight OFF";
		Camera::debug(flashLightOnDebug.str());
		glDisable(GL_LIGHT5);
		return;
	}

	flashLightOnDebug << "[Camera] Flashlight ON";
	Camera::debug(flashLightOnDebug.str());

	glPushMatrix();

	float fPos[] = {position[INDEX_CAM_X], position[INDEX_CAM_Y], position[INDEX_CAM_Z], 1};
	glm::vec3 dir = this->getOrientationAsNormal();
	float fDiffuse[] = {0.5f,0.5f,0.5f};
	float fSpecular[] = {0.1f, 0.1f, 0.1f};
	float fSpotDir[] = {dir[0],dir[1],dir[2]};
	float fSpotExp[] = {10};
	float fSpotCut[] = {15};

	glEnable(GL_LIGHT5);
	glLightfv(GL_LIGHT5, GL_POSITION, fPos);
	glLightfv(GL_LIGHT5, GL_DIFFUSE, fDiffuse);
	glLightfv(GL_LIGHT5, GL_SPECULAR, fSpecular);
	glLightfv(GL_LIGHT5, GL_SPOT_DIRECTION, fSpotDir);
	glLightfv(GL_LIGHT5, GL_SPOT_EXPONENT, fSpotExp);
	glLightfv(GL_LIGHT5, GL_SPOT_CUTOFF, fSpotCut);

	glPopMatrix();
}

bool Camera::checkCollision(glm::vec3 p){
	return glm::abs(glm::distance(position, p)) < 3;
}
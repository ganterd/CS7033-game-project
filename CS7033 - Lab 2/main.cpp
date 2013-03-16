#include <windows.h>	// for timeGetTime()
#include "Shader.h"
#include "GL/glew.h"
#include "gl/glu.h"		// for gluPerspective & gluLookAt
#include "Camera.h"
#include  "glfont.h"
#include "Importer.h"
#include "Model.h"
#include "ControllingMesh.h"
#include "Level.h"
#include "Zombie.h"
#include "ZombieController.h"
#include "Projectile.h"
#include <math.h>
#include <string.h>
#include "textureTGA.h"
#include "assimp/Importer.hpp"      // C++ importer interface
#include "DebugInfoScreen.h"
#include <mmsystem.h>	// ditto
#include <iostream>		// I/O
#include "GL/glut.h"	// GLUT
#include "Logger.h"

#include <fstream>
#include <pthread.h>

void setupScene();
void updateScene();
void renderScene();
void exitScene();
void keypress(unsigned char key, int x, int y);
void setViewport(int width, int height);

Shader*		shader;
Camera*		camera;
Zombie*		zombie;
Level*		level;
Model*		bouncingBall;
Model*		projectileModel;
ZombieController*	zombieController;
std::vector<Projectile> projectiles;
int score;

GLFONT font;
bool		wireframe=false;
bool		pressedKeyMap[1024];
bool		pressingShift = false;
bool		gameEnd;
int         windowId;
int			FPSframeCount;
int			FPSpreviousTime;
int			FPS;
DWORD		lastTickCount;
float		GAME_TIME;

void calculateFPS(){
    FPSframeCount++;

    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    int timeInterval = currentTime - FPSpreviousTime;

    if(timeInterval > 1000){
        FPS = FPSframeCount / (timeInterval / 1000);

        FPSpreviousTime = currentTime;
        FPSframeCount = 0;
    }
}

void renderScene(){
    /* Clear and set-up GL states */
	glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(1, 1, 1);
	glEnable(GL_LIGHTING);

	//shader->bind();

	/* Set view position & direction */
	camera->look();
	

	///* Draw Models */
	glPushMatrix();
		glTranslatef(0, 0, 10);
		bouncingBall->drawModel();
	glPopMatrix();

	glPushMatrix();
		level->draw();
	glPopMatrix();

	zombieController->draw();

	for(unsigned int i = 0; i < projectiles.size(); i++){
		projectiles[i].draw();
	}

	/* Collect debug information */
	std::ostringstream fpsDebug;
	std::ostringstream runTimeDebug;
	calculateFPS();
	fpsDebug << FPS << " FPS";
	runTimeDebug << "Game Time: " << GAME_TIME;

	camera->drawOverlay();

	/* Print debug and fade screen */
	std::vector<std::string> functionRunTimes = Logger::runTimesVector();
	for(int i = 0; i < functionRunTimes.size(); i++)
		DebugInfoScreen::debug(functionRunTimes[i]);
	Logger::resetRunTimes();
	DebugInfoScreen::debug(fpsDebug.str(), 255, 255, 0);
	DebugInfoScreen::debug(runTimeDebug.str());
	DebugInfoScreen::showDebugInfo();


	/* Swap buffers */
    glutSwapBuffers();
}



void keyboardUpdate(){
	/* Player Controls */
	if(pressedKeyMap['w'])
		if(!level->outOfBounds(camera->moveForward()))
			camera->confirmMove();
	if(pressedKeyMap['s'])
		if(!level->outOfBounds(camera->moveBackwards()))
			camera->confirmMove();
	if(pressedKeyMap['a'])
		if(!level->outOfBounds(camera->strafeLeft()))
			camera->confirmMove();
	if(pressedKeyMap['d'])
		if(!level->outOfBounds(camera->strafeRight()))
			camera->confirmMove();
}

float lastSpawnTime;
float zombieSpawnTime;
void updateScene(){
	GAME_TIME = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

	zombieController->allFollow(camera->getPos());

	std::vector<glm::vec3> zombiePositions = zombieController->getZombiePositions();
	for(unsigned int i = 0; i < zombiePositions.size(); i++){
		if(camera->checkCollision(zombiePositions[i])){
			std::cout << "Player Hit!";
			camera->damage(10);
		}
	}

	if(camera->getHealth() <= 0)
		gameEnd = true;

	bouncingBall->animate(GAME_TIME);
	camera->animate(GAME_TIME);
	zombieController->animate(GAME_TIME);

	/* Cap frame rate at 60FPS */
	while(timeGetTime()-lastTickCount<16);
	lastTickCount=timeGetTime();

	keyboardUpdate();
	camera->update();

	if(level->controllerInteraction(camera->getPos()) == LEVEL_GIVEWEAPON_SLINGSHOT){
		camera->setWeapon(CAM_WEAPN_SLINGSHOT);
		zombieController->addZombieAt(glm::vec3(0,0,0));
		lastSpawnTime = GAME_TIME;
	}

	// Draw the next frame
    glutPostRedisplay();
}



void KeyboardPress(unsigned char key, int x, int y){
    if(key==27){
		exitScene();
	}

	if(key == 'x' || key == 'X'){
		wireframe=!wireframe;
		if(wireframe){
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if(key == 'l' || key == 'L'){
		DebugInfoScreen::log("Toggle flashlight");
		camera->toggleFlashlight();
	}

	if(key == 'f' || key == 'F')
		glutFullScreenToggle();

	if(key == '~'){
		camera->toggleDebug();
	}

	if(key == 'k' || key == 'K')
		camera->fadeToggle();

	if(key < 1024)
		pressedKeyMap[key] = true;
}

void KeyboardRelease(unsigned char key, int x, int y){
	if(key < 1024)
		pressedKeyMap[key] = false;
}

void setupScene(){
	lastSpawnTime = -1;
	zombieSpawnTime = 3;
	gameEnd = false;
	std::cout<<"Initializing scene..."<<std::endl;
	
	glShadeModel(GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	camera->offsetCamera(0.0f, 1.6f, 0.0f);
	camera->setFade(1);
	camera->fadeFromBlack();

	shader = new Shader("Shader2.vert", "Shader2.frag");
	zombieController = new ZombieController();
}

void initModels(){
	level = new Level("level_one");
	
	bouncingBall = new Model();
	bouncingBall->loadModel(std::string(MODEL_FOLDER).append("ball.dae").c_str());

	projectileModel = new Model();
	projectileModel->loadModel(MODEL_FOLDER.append(PROJECTILE_MODEL_FILE).c_str());
}

void exitScene(){
    std::cout<<"Exiting scene..."<<std::endl;
    glutDestroyWindow(windowId);
    exit(0);
}

void setViewport(int width, int height) {
	std::cout << "Reshaping window: [" << width << ", " << height << "]" << std::endl;
    /* Work out window ratio, avoid divide-by-zero */
    if(height==0)height=1;
	float ratio = float(width)/float(height);

	/* Reset projection matrix */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	/* Fill screen with viewport */
	glViewport(0, 0, width, height);

	/* Set a 45 degree perspective */
	//gluPerspective(10, ratio, .1, 1000);
	
	camera->setScreenSize(width, height);
	camera->changeFOV(45);
	camera->warpPointerToCenter();
	DebugInfoScreen::setScreenSize(width, height);
}

void PressKey(int key, int x, int y){
	if(key==27){
		exitScene();
	}

	if(key < 1024)
		pressedKeyMap[key] = true;
}

void ReleaseKey(int key, int x, int y){
	if(key < 1024)
		pressedKeyMap[key] = false;
}

void mouseMotion( int x, int y ) {
	camera->mouseReposition(x, y);
}

void mousePress(int button, int state, int x, int y){
	if(button == GLUT_RIGHT_BUTTON){
		if(state == GLUT_DOWN)
			camera->changeFOV(10);
		else
			camera->changeFOV(45);
	}

	if(button == GLUT_LEFT_BUTTON){
		if(state == GLUT_DOWN){
			if(camera->action() == ACTION_FIRE_PROJECTILE_SLOW){
				Projectile p;
				p.setPosition(camera->getPos());
				p.setVelocity(camera->getOrientation(), PROJECTILE_SPEED_SLOW);
				p.setModel(projectileModel);
				projectiles.push_back(p);
			}
		}
	}
}

int main(int argc, char *argv[]){
    // Initialise OpenGL
    glutInit(&argc, argv);
	Logger::initLogger();
	unsigned int t = Logger::enterFunction("Class", "Func");
	

    // Set window position, size & create window
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(640, 480);

	/* Set up the camera */
	camera = new Camera();
	camera->setScreenSize(640, 480);
	
	windowId = glutCreateWindow("Game");
	
    
    // Set GLUT callback functions
    glutReshapeFunc(setViewport);
    glutDisplayFunc(renderScene);
    glutIdleFunc(updateScene);
	glutIgnoreKeyRepeat(1);
    glutKeyboardFunc(KeyboardPress);
	glutKeyboardUpFunc(KeyboardRelease);
	glutPassiveMotionFunc(mouseMotion);
	glutMotionFunc(mouseMotion);
	glutSpecialFunc(PressKey);
	glutSpecialUpFunc(ReleaseKey);
	glutMouseFunc(mousePress);
	glutSetCursor(GLUT_CURSOR_NONE);

	glFontCreate(&font, "resources/quartz_FONT.glf", 1);

	glewInit();
	initModels();
    // Setup OpenGL state & scene resources (models, textures etc)
    setupScene();

	Logger::exitFunction(t);
	Logger::printRunTimes();

    // Show window & start update loop
	//glutFullScreen();
	camera->warpPointerToCenter();

    glutMainLoop(); 

    return 0;
}
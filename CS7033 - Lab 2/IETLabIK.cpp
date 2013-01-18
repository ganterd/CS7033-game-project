#include <windows.h>	// for timeGetTime()
#include <mmsystem.h>	// ditto
#include <iostream>		// I/O
#include <GL/freeglut.h>
#include <glm.hpp>		// Mathematic library

#define _USE_MATH_DEFINES //To include the constante M_PI
#include <math.h>

void setupScene();
void updateScene();
void renderScene();
void exitScene();
void keyPressed(unsigned char key, int x, int y);
void keySpecialPressed(unsigned char key, int x, int y);
void setViewport(int width, int height);
void setOrthogonalView();
void disableOrthogonalView();
void drawTarget();
void computeInverseKinematicsAngles();

bool		wireframe=false;
int         windowId;
DWORD		lastTickCount;
GLfloat		windowHeight(1024), windowWidth(1280);

GLfloat white_light[] = {1.0, 1.0, 1.0, 1.0};
GLfloat left_light_position[] = {1,0,-1, 1.0}; 
GLfloat right_light_position[] = {-1,0,-1, 1.0};

GLfloat grey_ambient[] = {0.01745, 0.01745, 0.01745};
GLfloat grey_diffuse[] = {0.161424, 0.161424, 0.161424};
GLfloat grey_specular[] = {0.727811, 0.727811, 0.727811};
GLfloat grey_shininess = 76.8;

GLfloat BoneA_Length = 1.0;
GLfloat BoneB_Length = 0.7;
GLfloat BoneA_Angle(0), BoneB_Angle(0);

glm::vec2 target2D_position(BoneA_Length+BoneB_Length, 0);
GLfloat deltaPos = 0.01;
bool invertedSolution = false;
 
void renderScene(){
        
    // Clear framebuffer & depth buffer
	glClearColor(0.5,0.8,0.7,0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//Enable lighting
 	glEnable(GL_LIGHTING);

	//Set the material properties
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, grey_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, grey_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, grey_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, grey_shininess);   

	// Reset Modelview matrix      	
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// Set view position & direction (Camera at (-5,0,0) looking on the positive X-axis). World is Y-up.
	gluLookAt(-5,0,0, 1,0,0, 0,1,0);

	//To setup the creation of quadric objects
	GLUquadric* nQ;
	nQ=gluNewQuadric();

    glPushMatrix(); //remember the current state of the modelview matrix

	// Rotate from the angle between the first bone and the Z axis
	glRotatef(BoneA_Angle,1,0,0);

	//Draw the first cone at the origin
	gluCylinder(nQ, 0.1, 0.01, BoneA_Length, 30, 5) ;
	gluSphere(nQ, 0.1, 30, 5);
	
	// Translate by the bone length
	glTranslatef(0,0.0,BoneA_Length);

	glPushMatrix(); //remember the current state of the modelview matrix

	// Rotate from the angle between the first and second bones
	glRotatef(BoneB_Angle,1,0,0);

	//Draw the second cone at the origin, pointing up
	gluCylinder(nQ, 0.1, 0.01, BoneB_Length, 30, 5) ;
	gluSphere(nQ, 0.1, 30, 5);

	glPopMatrix(); //restore the state of the modelview matrix

	glPopMatrix(); //restore the state of the modelview matrix

	drawTarget();

	glDisable(GL_LIGHTING);
	
    // Swap double buffer for flicker-free animation
    glutSwapBuffers();
        
}

void drawTarget()
{
	// Draw the targer
	glPushMatrix(); //remember the current state of the modelview matrix
	
	glTranslatef(0,target2D_position[1],target2D_position[0]);

	glColor3ub(255, 0, 0);
	glLineWidth(4.0);
	glBegin(GL_LINES);
	glVertex3f(0.0, -0.1, 0);
	glVertex3f(0.0,  0.1, 0);
	glVertex3f(0.0, 0, -0.1);
	glVertex3f(0.0, 0,  0.1);
	glEnd();

	glPopMatrix(); //restore the state of the modelview matrix
}

void computeInverseKinematicsAngles()
{
	BoneA_Angle = 0.0;
	BoneB_Angle = 0.0;
	
	// TODO: compute bone angles depending on the target position target2D_position
	float Pa = BoneA_Length;
	float PaSq = BoneA_Length * BoneA_Length;

	float Pb = BoneB_Length;
	float PbSq = BoneB_Length * BoneB_Length;

	float d = glm::sqrt((target2D_position[0] * target2D_position[0]) + (target2D_position[1] * target2D_position[1]));
	
	float max = BoneA_Length + BoneB_Length;
	float min = BoneA_Length - BoneB_Length;

	std::cout << std::endl << "[d, min, max]: " << d << ", " << min << ", " << max << "]" << std::endl;

	float theta = glm::degrees(glm::acos(target2D_position[0] / d));
	if(target2D_position[1] > 0)
		theta *= -1;

	/* Limit the apparant range of d so we don't get undefined errors */
	if(d > BoneA_Length + BoneB_Length)
		d = BoneA_Length + BoneB_Length;
	else if(d < BoneA_Length - BoneB_Length)
		d = BoneA_Length - BoneB_Length;
	float dSq = d * d;

	/* Angle for first length */
	float alpha = glm::degrees(glm::acos((dSq + PaSq - PbSq) / (2 * d * Pa)));

	/* Part of the equation for angle for second length*/
	/* Split to avoid NaN errors when betaPart is < -1 or > 1 rads */
	float betaPart = (PaSq + PbSq - dSq) / (2 * Pa * Pb);
	if(betaPart > 1)
		betaPart = 1;
	else if(betaPart < -1)
		betaPart = -1;
	float beta = glm::degrees(glm::acos(betaPart));

	std::cout << "[theta, alpha, beta]: [" << theta << ", " << alpha << ", " << beta << "]" << std::endl;


	/* Convert the angles in degrees as mathematical operations are usually conducted in radians */
	if(invertedSolution){
		alpha *= -1;
		beta *= -1;
	}
	BoneA_Angle = alpha + theta;
	BoneB_Angle = beta - 180;
}

void updateScene(){
	
	// Wait until at least 16ms passed since start of last frame
	// Effectively caps framerate at ~60fps
	while(timeGetTime()-lastTickCount<16);
	lastTickCount=timeGetTime();

	computeInverseKinematicsAngles();
    
    // Do any other updates here
	
	// Draw the next frame
    glutPostRedisplay();

}

void keyPressed(unsigned char key, int x, int y){
	
	switch (key)
	{
	case 27:
		// Test if user pressed ESCAPE (ascii 27)
		// If so, exit the program
		exitScene();
		break;

	case 'w':
	case 'W':
		wireframe=!wireframe;
		glPolygonMode(GL_FRONT_AND_BACK, (wireframe ? GL_LINE : GL_FILL));
		break;

	case 'i':
	case 'I':
		invertedSolution=!invertedSolution;
		break;

	case 'z':
	case 'Z':
		BoneA_Length -= deltaPos;
		break;

	case 'a':
	case 'A':
		BoneA_Length += deltaPos;
		break;

	case 'x':
	case 'X':
		BoneB_Length -= deltaPos;
		break;

	case 's':
	case 'S':
		BoneB_Length += deltaPos;
		break;

	// Other possible keypresses go here
	/*case 'a':
		...
		break;*/

	default:
		break;
	}
}

void keySpecialPressed (int key, int x, int y) {  
	switch (key)
	{
	case GLUT_KEY_LEFT:
		target2D_position[0] -= deltaPos;
		break;
	case GLUT_KEY_RIGHT:
		target2D_position[0] += deltaPos;
		break;
	case GLUT_KEY_DOWN:
		target2D_position[1] -= deltaPos;
		break;
	case GLUT_KEY_UP:
		target2D_position[1] += deltaPos;
		break;

	default:
		break;
	}
}

void setupScene(){

	std::cout<<"Initializing scene..."<<std::endl;
    
	//Set up Lighting Stuff
	glLightfv(GL_LIGHT0, GL_POSITION, left_light_position);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
	glLightfv(GL_LIGHT1, GL_POSITION, right_light_position);
	glLightfv(GL_LIGHT1, GL_SPECULAR, white_light);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, white_light);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);        
}

void exitScene(){

    std::cout<<"Exiting scene..."<<std::endl;

    // Close window
    glutDestroyWindow(windowId);

    // Free any allocated memory

    // Exit program
    exit(0);
}

void setViewport(int width, int height) {

    // Work out window ratio, avoid divide-by-zero
    if(height==0)height=1;
	float ratio = float(width)/float(height);

	// Reset projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Fill screen with viewport
	glViewport(0, 0, width, height);

	// Set a 45 degree perspective
	gluPerspective(45, ratio, .1, 1000);

}

int main(int argc, char *argv[]){
        
    // Initialise OpenGL
    glutInit(&argc, argv); 

    // Set window position, size & create window
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(50,50);
    glutInitWindowSize(windowWidth,windowHeight);
	windowId = glutCreateWindow("IET Lab: 2D Inverse Kinematics");
    
    // Set GLUT callback functions
    glutReshapeFunc(setViewport);
    glutDisplayFunc(renderScene);
    glutIdleFunc(updateScene);
    glutKeyboardFunc(keyPressed);
	glutSpecialFunc(keySpecialPressed); // Tell GLUT to use the method "keySpecialPressed" for special key presses  

    // Setup OpenGL state & scene resources (models, textures etc)
    setupScene();

    // Show window & start update loop
    glutMainLoop();    

    return 0;
    
}
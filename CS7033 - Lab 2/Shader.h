#ifndef SHADER_H
#define SHADER_H

#include <iostream>
#include <fstream>
#include <ostream>
#include <sstream>

/* Includes for OpenGL */
#include <Windows.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <GL\GL.h>

class Shader{
public:
	Shader(const char*, const char*);
	void bind();
	void unbind();
private:
	unsigned int uiShaderID;
	unsigned int uiShaderVertP;
	unsigned int uiShaderFragP;

	void init(const char*, const char*);
	const char* readFile(const char*);
};

#endif
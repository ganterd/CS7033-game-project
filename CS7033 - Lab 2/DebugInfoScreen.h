#pragma once

#include <iostream>
#include <sstream>
#include <deque>
#include <GL\glut.h>
#include <GL\GLU.h>

struct DebugText{
	std::string text;
	float color[3];
};

struct DebugLogText{
	std::string text;
	float color[3];
	float timeOnScreen;
};

static std::deque<DebugText> debugText;
static std::deque<DebugLogText> debugLogText;

class DebugInfoScreen{
private:
	static int screenWidth;
	static int screenHeight;
	static int textFadeTime;
	static int overlayPadding;

	/* Draw Methods */
	static void drawBackground(int,int,int);
	static void drawDebugText();
	static void drawLogText();
public:
	static void setScreenSize(int,int);

	/* Add or remove debug information */
	static void debug(std::string);
	static void debug(std::string, float, float, float);
	static void debug(std::string, float[]);
	static void debug(DebugText);
	static void clearDebug();

	/* Single debug lines */
	static void log(std::string);
	static void log(std::string, float, float, float);
	static void log(std::string, float[]);
	static void log(DebugLogText);

	static void showDebugInfo();
};
#include "DebugInfoScreen.h"

int DebugInfoScreen::screenWidth(640);
int DebugInfoScreen::screenHeight(480);
int DebugInfoScreen::textFadeTime(50);
int DebugInfoScreen::overlayPadding(5);

void DebugInfoScreen::setScreenSize(int width, int height){
	screenWidth = width;
	screenHeight = height;
};

void DebugInfoScreen::drawBackground(int x, int y, int lines){
	glColor4f(0.0f,0.0f,0.0f,0.5f);
	//glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int padding = overlayPadding;
	int left = x;
	int right = screenWidth - padding;
	int top = y;
	int bottom = top + padding + (15 * lines);
	int z = 0;
	glRasterPos3i(0, 0, 0);
	glBegin(GL_QUADS);
		glVertex3i(left, top, z);
		glVertex3i(right, top, z);
		glVertex3i(right, bottom, z);
		glVertex3i(left, bottom, z);
	glEnd();

	//glDisable(GL_BLEND);
}

void DebugInfoScreen::showDebugInfo(){
	/* Switch Draw Mode */
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, screenWidth, screenHeight, 0);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadIdentity();
			glEnable(GL_BLEND);

			int x = 10;
			int y = 20;

			DebugInfoScreen::drawBackground(x - overlayPadding, y, int(debugText.size()));

			for(unsigned int w = 0; w < debugText.size(); w++){
				float* c = debugText[w].color;
				glColor4f(c[0], c[1], c[2], 1.0f);
				y += 15;
				glRasterPos3i(10, y, 1);
				const char* str = debugText[w].text.c_str();
				int len, i;
				len = (int)strlen(str);
				for (i = 0; i < len; i++) {
					glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[i]);		
				}
			}
			debugText.clear();

			y += 20;

			if(!debugLogText.empty()){
				DebugInfoScreen::drawBackground(x - overlayPadding, y, int(debugLogText.size()));

				std::deque<DebugLogText>::iterator logTextIter = debugLogText.begin();
				while(logTextIter != debugLogText.end()){
					float* c = logTextIter->color;
					glColor4f(c[0], c[1], c[2], float(logTextIter->timeOnScreen) / float(textFadeTime));
					y += 15;
					glRasterPos3i(10, y, 1);
					const char* str = logTextIter->text.c_str();
					int len, i;
					len = (int)strlen(str);
					for (i = 0; i < len; i++) {
						glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[i]);		
					}

					if(--(logTextIter->timeOnScreen) < 0){
						logTextIter = debugLogText.erase(logTextIter);
					}else{
						logTextIter++;
					}
				}
			}
				
			
			glDisable(GL_BLEND);
		glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glEnable(GL_LIGHTING);
}

void DebugInfoScreen::debug(std::string s){
	float color[3] = {1.0f, 1.0f, 1.0f};
	DebugInfoScreen::debug(s, color);
}

void DebugInfoScreen::debug(std::string s, float r, float g, float b){
	DebugText d;
	d.text = s;
	d.color[0] = r;
	d.color[1] = g;
	d.color[2] = b;
	DebugInfoScreen::debug(d);
}

void DebugInfoScreen::debug(std::string s, float color[3]){
	DebugText d;
	d.text = s;
	d.color[0] = color[0];
	d.color[1] = color[1];
	d.color[2] = color[2];
	DebugInfoScreen::debug(d);
}

void DebugInfoScreen::debug(DebugText d){
	debugText.push_back(d);
}

void DebugInfoScreen::clearDebug(){
	debugText.clear();
}

void DebugInfoScreen::log(std::string s){
	DebugLogText d;
	d.text = s;
	d.timeOnScreen = textFadeTime;
	d.color[0] = 1.0f;
	d.color[1] = 1.0f;
	d.color[2] = 1.0f;
	DebugInfoScreen::log(d);
}

void DebugInfoScreen::log(std::string s, float color[3]){
	DebugLogText d;
	d.text = s;
	d.timeOnScreen = textFadeTime;
	d.color[0] = color[0];
	d.color[1] = color[1];
	d.color[2] = color[2];
	DebugInfoScreen::log(d);
}

void DebugInfoScreen::log(std::string s, float r, float g, float b){
	DebugLogText d;
	d.text = s;
	d.timeOnScreen = textFadeTime;
	d.color[0] = r;
	d.color[1] = g;
	d.color[2] = b;
	DebugInfoScreen::log(d);
}

void DebugInfoScreen::log(DebugLogText d){
	debugLogText.push_back(d);
}
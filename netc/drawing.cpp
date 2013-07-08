#include "GL/glew.h"
#include "duel_canvas.h"
#include "image_mgr.h"

namespace ygopro
{

	void wxDuelCanvas::drawScene() {
		static bool first_time = true;
		if(first_time) {
			imageMgr.loadTextures();
			first_time = false;
		}
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60, 1, .1, 100);
		gluLookAt(5, 5, 5, 0, 0, 0, 0, 0, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glEnable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
		GLfloat ambient[] = {1.0, 1.0, 1.0, 1.0};
		GLfloat r[] = {1.0, 0.0, 0.0, 1.0};
		GLfloat g[] = {0.0, 1.0, 0.0, 1.0};
		GLfloat b[] = {0.0, 0.0, 1.0, 1.0};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
 		glBindTexture(GL_TEXTURE_2D, imageMgr.card_texture);
  		glBegin(GL_QUADS);
  			glNormal3f(0.0f, 1.0f, 0.0f);glTexCoord2f(imageMgr.sleeve->lx, imageMgr.sleeve->ly);glVertex3f(0.0f, 0.0f, 0.0f);
  			glNormal3f(0.0f, 1.0f, 0.0f);glTexCoord2f(imageMgr.sleeve->lx, imageMgr.sleeve->ry);glVertex3f(0.0f, -1.0f, 0.0f);
 			glNormal3f(0.0f, 1.0f, 0.0f);glTexCoord2f(imageMgr.sleeve->rx, imageMgr.sleeve->ry);glVertex3f(1.0f, -1.0f, 0.0f);
 			glNormal3f(0.0f, 1.0f, 0.0f);glTexCoord2f(imageMgr.sleeve->rx, imageMgr.sleeve->lx);glVertex3f(1.0f, 0.0f, 0.0f);
  		glEnd();
		glFlush();
	}

}

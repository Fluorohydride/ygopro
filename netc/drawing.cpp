#include "GL/glew.h"
#include "duel_canvas.h"
#include "image_mgr.h"

namespace ygopro
{

	void wxDuelCanvas::drawScene() {
		static bool first_time = true;
		if(first_time) {
			imageMgr.InitTextures();
			first_time = false;
		}
		// background
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
 		glMatrixMode(GL_PROJECTION);
 		glLoadIdentity();
 		glMatrixMode(GL_MODELVIEW);
 		glLoadIdentity();
 		glDisable(GL_DEPTH_TEST);
 		glDisable(GL_LIGHTING);
		glDisable(GL_BLEND);
 		glEnable(GL_TEXTURE_2D);
 		glBindTexture(GL_TEXTURE_2D, imageMgr.textureid_bg);
 		glBegin(GL_QUADS);
		{
 			glTexCoord2f(imageMgr.background.lx, imageMgr.background.ly);glVertex2f(-1.0f, 1.0f);
 			glTexCoord2f(imageMgr.background.rx, imageMgr.background.ly);glVertex2f(1.0f, 1.0f);
 			glTexCoord2f(imageMgr.background.rx, imageMgr.background.ry);glVertex2f(1.0f, -1.0f);
 			glTexCoord2f(imageMgr.background.lx, imageMgr.background.ry);glVertex2f(-1.0f, -1.0f);
		}
 		glEnd();
		// field
		glShadeModel(GL_SMOOTH);
		glEnable(GL_LIGHTING);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45, 1.5f, 0.1f, 100);
		gluLookAt(3.95f, 8.0f, 7.8f, 3.95f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		GLfloat ambient[] = {1.0, 1.0, 1.0, 1.0};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
		glBindTexture(GL_TEXTURE_2D, imageMgr.textureid_all);
		glBegin(GL_QUADS);
		{
			glNormal3f(0.0f, 0.0f, 1.0f);glTexCoord2f(imageMgr.texture_field.lx, imageMgr.texture_field.ly);glVertex3f(0.0f, -4.0f, 0.0f);
			glNormal3f(0.0f, 0.0f, 1.0f);glTexCoord2f(imageMgr.texture_field.rx, imageMgr.texture_field.ly);glVertex3f(8.0f, -4.0f, 0.0f);
			glNormal3f(0.0f, 0.0f, 1.0f);glTexCoord2f(imageMgr.texture_field.rx, imageMgr.texture_field.ry);glVertex3f(8.0f, 4.0f, 0.0f);
			glNormal3f(0.0f, 0.0f, 1.0f);glTexCoord2f(imageMgr.texture_field.lx, imageMgr.texture_field.ry);glVertex3f(0.0f, 4.0f, 0.0f);
		}
		glEnd();
		glFlush();
	}

}

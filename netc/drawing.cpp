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

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glShadeModel(GL_SMOOTH);

		//draw background
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
			glTexCoord2f(imageMgr.background.lx, imageMgr.background.ry);glVertex2f(-1.0f, -1.0f);
 			glTexCoord2f(imageMgr.background.rx, imageMgr.background.ry);glVertex2f(1.0f, -1.0f);
 			glTexCoord2f(imageMgr.background.rx, imageMgr.background.ly);glVertex2f(1.0f, 1.0f);
		}
 		glEnd();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45, 1.5f, 0.1f, 100);
		gluLookAt(3.95f, 8.0f, 7.8f, 3.95f, 0.8f, 0.0f, 0.0f, 0.0f, 1.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//draw field
		glBindTexture(GL_TEXTURE_2D, imageMgr.textureid_all);
		glBegin(GL_QUADS);
		{
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glTexCoord2f(imageMgr.texture_field.lx, imageMgr.texture_field.ly);glVertex3f(0.0f, -4.0f, 0.0f);
			glTexCoord2f(imageMgr.texture_field.lx, imageMgr.texture_field.ry);glVertex3f(0.0f, 4.0f, 0.0f);
			glTexCoord2f(imageMgr.texture_field.rx, imageMgr.texture_field.ry);glVertex3f(8.0f, 4.0f, 0.0f);
			glTexCoord2f(imageMgr.texture_field.rx, imageMgr.texture_field.ly);glVertex3f(8.0f, -4.0f, 0.0f);
		}
		glEnd();

		// other 2d stuff
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glDisable(GL_DEPTH_TEST);
		float avawidth = 300.0f / glwidth;
		float avaheight = 400.0f / glheight;

		glBegin(GL_QUADS);
		{
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glTexCoord2f(imageMgr.texture_attack.lx, imageMgr.texture_attack.ly);glVertex2f(-1.0f, -1.0f + avaheight);
			glTexCoord2f(imageMgr.texture_attack.rx, imageMgr.texture_attack.ly);glVertex2f(-1.0f + avawidth, -1.0f + avaheight);
			glTexCoord2f(imageMgr.texture_attack.rx, imageMgr.texture_attack.ry);glVertex2f(-1.0f + avawidth, -1.0f);
			glTexCoord2f(imageMgr.texture_attack.lx, imageMgr.texture_attack.ry);glVertex2f(-1.0f, -1.0f);
			glColor4f(0.5f, 0.5f, 0.5f, 0.8f);
			glTexCoord2f(imageMgr.texture_lpbar.lx, imageMgr.texture_lpbar.ly);glVertex2f(-0.70f, -0.8f);
			glTexCoord2f(imageMgr.texture_lpbar.rx, imageMgr.texture_lpbar.ly);glVertex2f(0.35f, -0.8f);
			glTexCoord2f(imageMgr.texture_lpbar.rx, imageMgr.texture_lpbar.ry);glVertex2f(0.35f, -1.0f);
			glTexCoord2f(imageMgr.texture_lpbar.lx, imageMgr.texture_lpbar.ry);glVertex2f(-0.70f, -1.0f);
			glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
			glTexCoord2f(imageMgr.texture_lpbar.lx, imageMgr.texture_lpbar.ly);glVertex2f(-0.70f, -0.8f);
			glTexCoord2f(imageMgr.texture_lpbar.lx + (imageMgr.texture_lpbar.rx - imageMgr.texture_lpbar.lx) * 0.8095238f, imageMgr.texture_lpbar.ly);glVertex2f(0.15f, -0.8f);
			glTexCoord2f(imageMgr.texture_lpbar.lx + (imageMgr.texture_lpbar.rx - imageMgr.texture_lpbar.lx) * 0.8095238f, imageMgr.texture_lpbar.ry);glVertex2f(0.15f, -1.0f);
			glTexCoord2f(imageMgr.texture_lpbar.lx, imageMgr.texture_lpbar.ry);glVertex2f(-0.70f, -1.0f);
			glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
			glTexCoord2f(imageMgr.texture_lpframe.lx, imageMgr.texture_lpframe.ly);glVertex2f(-0.70f, -0.8f);
			glTexCoord2f(imageMgr.texture_lpframe.rx, imageMgr.texture_lpframe.ly);glVertex2f(0.35f, -0.8f);
			glTexCoord2f(imageMgr.texture_lpframe.rx, imageMgr.texture_lpframe.ry);glVertex2f(0.35f, -1.0f);
			glTexCoord2f(imageMgr.texture_lpframe.lx, imageMgr.texture_lpframe.ry);glVertex2f(-0.70f, -1.0f);
		}
		glEnd();

		glFlush();
	}

}

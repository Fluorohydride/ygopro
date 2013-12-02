#include "duel_canvas.h"
#include "image_mgr.h"

#ifdef __WXMAC__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

namespace ygopro
{

	void wxDuelCanvas::drawScene() {

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
 		glBindTexture(GL_TEXTURE_2D, t_duelbg->tex());
 		glBegin(GL_QUADS);
		{
 			glTexCoord2f(t_duelbg->lx, t_duelbg->ly);glVertex2f(-1.0f, 1.0f);
			glTexCoord2f(t_duelbg->lx, t_duelbg->ry);glVertex2f(-1.0f, -1.0f);
 			glTexCoord2f(t_duelbg->rx, t_duelbg->ry);glVertex2f(1.0f, -1.0f);
 			glTexCoord2f(t_duelbg->rx, t_duelbg->ly);glVertex2f(1.0f, 1.0f);
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
		glBindTexture(GL_TEXTURE_2D, t_field->tex());
		glBegin(GL_QUADS);
		{
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glTexCoord2f(t_field->lx, t_field->ly);glVertex3f(0.0f, -4.0f, 0.0f);
			glTexCoord2f(t_field->lx, t_field->ry);glVertex3f(0.0f, 4.0f, 0.0f);
			glTexCoord2f(t_field->rx, t_field->ry);glVertex3f(8.0f, 4.0f, 0.0f);
			glTexCoord2f(t_field->rx, t_field->ly);glVertex3f(8.0f, -4.0f, 0.0f);
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

        glBindTexture(GL_TEXTURE_2D, t_field->tex());
		glBegin(GL_QUADS);
		{
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glTexCoord2f(t_avatar1->lx, t_avatar1->ly);glVertex2f(-1.0f, -1.0f + avaheight);
			glTexCoord2f(t_avatar1->rx, t_avatar1->ly);glVertex2f(-1.0f + avawidth, -1.0f + avaheight);
			glTexCoord2f(t_avatar1->rx, t_avatar1->ry);glVertex2f(-1.0f + avawidth, -1.0f);
			glTexCoord2f(t_avatar1->lx, t_avatar1->ry);glVertex2f(-1.0f, -1.0f);
		}
		glEnd();
        
        glBindTexture(GL_TEXTURE_2D, 0);
        
		glFlush();
	}

}

#include "GL/glew.h"
#include "duel_canvas.h"

namespace ygopro
{

	void wxDuelCanvas::drawScene() {
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION); 
		glLoadIdentity(); 
		gluPerspective(60, 1, .1, 100); 
		gluLookAt(9, 9, 9, 0, 0, 0, 0, 1, 0); 
		glMatrixMode(GL_MODELVIEW); 
		glLoadIdentity(); 
		
	}

}

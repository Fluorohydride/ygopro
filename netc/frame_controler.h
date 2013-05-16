#ifndef FRAME_CONTROLER_H
#define FRAME_CONTROLER_H

#include <GL/glfw3.h>

namespace ygopro
{

	class FrameControler {
	private:
		double prev_time;
		double frame_time;
		double left_time;
		double fps_time;
		int fps;
		std::string fpsstr;

	public:

		void FpsInitialise() {
			prev_time = glfwGetTime();
			fps_time = 0.0;
			fps = 0;
		}

		void FpsSet(unsigned int fps) {
			frame_time = 1.0 / fps;
		}

		double FpsNextFrame() {
			double current_time = glfwGetTime();
			double interval = current_time - prev_time;
			if(interval + left_time < frame_time) {
#ifdef _WIN32
				Sleep((unsigned int)(frame_time * 1000));
#else
#endif
				current_time = glfwGetTime();
				interval = current_time - prev_time;
			} else {
				left_time = interval + left_time - frame_time;
			}
			prev_time = current_time;
			fps++;
			if(current_time - fps_time >= 1.0) {
				fps_time = current_time;
				char buf[16];
				sprintf(buf, "FPS: %d", fps);
				fpsstr = buf;
			}
			return interval;
		}
	};

}

#endif

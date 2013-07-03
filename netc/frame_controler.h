#ifndef _FRAME_CONTROLER_H_
#define _FRAME_CONTROLER_H_

#include <time.h>

namespace ygopro
{

	class FrameControler {
	private:
		clock_t prev_time;
		clock_t frame_time;
		clock_t left_time;
		clock_t fps_time;
		int fps;
		std::string fpsstr;

	public:

		void FpsInitialise() {
			prev_time = clock();
			fps_time = 0;
			frame_time = 16;
			fps = 0;
		}

		void FpsSet(unsigned int fps) {
			frame_time = 1000 / fps;
		}

		double FpsNextFrame() {
			clock_t current_time = clock();
			clock_t interval = current_time - prev_time;
			if(interval + left_time < frame_time) {
#ifdef _WIN32
				Sleep(frame_time);
#else
#endif
				current_time = clock();
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

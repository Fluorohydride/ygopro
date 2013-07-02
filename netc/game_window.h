#ifndef _GAME_H_
#define _GAME_H_

#include "../common/common.h"
#include "GL/glfw3.h"
#include "MyGUI.h"
#include "MyGUI_OpenGLPlatform.h"
#include "frame_controler.h"
#include "config_mgr.h"
#include "custom_texture_mgr.h"
#include <vector>
#include <queue>

#include "duel_message.h"

namespace ygopro
{

	class GameWindow : public DuelMessage, public FrameControler, public ConfigMgr, public CustomTextureMgr {
	public:
		GameWindow();
		~GameWindow();
		void Initialise(int sx, int sy);
		void ShowWindow(int x, int y);
		void Close();

		virtual void LoadTexture(const std::string& name);
		virtual void DestroyTexture(const std::string& name);
		virtual void HandleMessage(unsigned int msg, unsigned int size, void* data);

		static void mousePosFunc(GLFWwindow* win, double x, double y);
		static void mouseButtonFunc(GLFWwindow* win, int button, int state, int modkey);
		static void mouseWheelFunc(GLFWwindow* win, double x, double y);

	private:
		bool exiting;
		MyGUI::Gui* mGUI;
		MyGUI::OpenGLPlatform* mPlatform;
		GLFWwindow* glWindow;
	};

	extern GameWindow mainGame;

}

#endif //_GAME_H_

#include "game_window.h"
#include "image_loader.h"
#include "panels.h"

Random globalRandom;

namespace ygopro
{

	GameWindow mainGame;

	GameWindow::GameWindow(): exiting(false), mGUI(nullptr), mPlatform(nullptr), ConfigMgr("config.conf") {
		
	}

	GameWindow::~GameWindow() {
		mGUI->shutdown();
		mPlatform->shutdown();
		delete mGUI;
		delete mPlatform;
	}

	void GameWindow::Initialise(int x, int y) {
		glfwInit();
// #ifdef _WIN32
// 		int cx = GetSystemMetrics(SM_CXSCREEN);
// 		int cy = GetSystemMetrics(SM_CYSCREEN);
// 		glfwWindowHint(GLFW_POSITION_X, (cx - x) / 2);
// 		glfwWindowHint(GLFW_POSITION_Y, (cy - y) / 2);
// #endif
		glfwWindowHint(GLFW_VISIBLE, 0);
		glWindow = glfwCreateWindow(x, y, GLFW_WINDOWED, "YGOPRO", 0);
		glfwMakeContextCurrent(glWindow);
		// callbacks
		glfwSetCursorPosCallback(glWindow, mousePosFunc);
		glfwSetMouseButtonCallback(glWindow, mouseButtonFunc);
		glfwSetScrollCallback(glWindow, mouseWheelFunc);

		mPlatform = new MyGUI::OpenGLPlatform();
		mPlatform->initialise(&imageLoader);
		mPlatform->getDataManagerPtr()->addResourceLocation("./skin", false);
		mGUI = new MyGUI::Gui();
		mGUI->initialise("MyGUI_Core.xml");
		mPlatform->getRenderManagerPtr()->setViewSize(x, y);
		MyGUI::PointerManager::getInstancePtr()->setVisible(false);
		FpsSet(60);
	}

	void GameWindow::MainLoop(int x, int y) {
		glfwShowWindow(glWindow);
		FpsInitialise();
		while(!exiting && !glfwGetWindowParam(glWindow, GLFW_CLOSE_REQUESTED)) {
			FpsNextFrame();
			CheckMessage();
			glfwPollEvents();
			glClear(GL_COLOR_BUFFER_BIT);
			mPlatform->getRenderManagerPtr()->drawOneFrame();
			glfwSwapBuffers(glWindow);
		}
		glfwHideWindow(glWindow);

//		glfwTerminate();
	}

	void GameWindow::Close() {
		exiting = true;
	}

	void GameWindow::LoadTexture(const std::string& name) {
		auto iter = custom_textures.find(name);
		if(iter == custom_textures.end()) {
			CustomTextureInfo cti;
			cti.ptexture = mPlatform->getRenderManagerPtr()->createTexture(name);
			cti.ptexture->loadFromFile(name);
			cti.ref_count = 1;
			custom_textures[name] = cti;
		} else {
			iter->second.ref_count++;
		}
	}

	void GameWindow::DestroyTexture(const std::string& name) {
		auto iter = custom_textures.find(name);
		if(iter != custom_textures.end()) {
			iter->second.ref_count--;
			if(iter->second.ref_count == 0) {
				mPlatform->getRenderManagerPtr()->destroyTexture(iter->second.ptexture);
				custom_textures.erase(iter);
			}
		}
	}

	void GameWindow::HandleMessage(unsigned int msg, unsigned int size, void* data) {

	}

	void GameWindow::mousePosFunc(GLFWwindow win, int x, int y) {
		MyGUI::InputManager::getInstancePtr()->injectMouseMove(x, y, 0);
	}

	void GameWindow::mouseButtonFunc(GLFWwindow win, int button, int state) {
		int xpos, ypos;
		glfwGetCursorPos(win, &xpos, &ypos);
		if(state == GLFW_PRESS)
			MyGUI::InputManager::getInstancePtr()->injectMousePress(xpos, ypos, (MyGUI::MouseButton::Enum)button);
		else
			MyGUI::InputManager::getInstancePtr()->injectMouseRelease(xpos, ypos, (MyGUI::MouseButton::Enum)button);
	}

	void GameWindow::mouseWheelFunc(GLFWwindow win, double button, double state) {

	}

}

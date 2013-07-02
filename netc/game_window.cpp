#include "game_window.h"
#include "image_loader.h"
#include "panels.h"

Random globalRandom;

namespace ygopro
{

	GameWindow mainGame;

	GameWindow::GameWindow(): exiting(false), mGUI(nullptr), mPlatform(nullptr), ConfigMgr("config.conf") {
		glfwInit();
	}

	GameWindow::~GameWindow() {
		if(mGUI) {
			mGUI->shutdown();
			delete mGUI;
		}
		if(mPlatform) {
			mPlatform->shutdown();
			delete mPlatform;
		}
		glfwTerminate();
	}

	void GameWindow::Initialise(int sx, int sy) {
		if(mPlatform) {
			mGUI->shutdown();
			delete mGUI;
		}
		if(mGUI) {
			mPlatform->shutdown();
			delete mPlatform;
		}
		if(glWindow)
			glfwDestroyWindow(glWindow);
		glfwWindowHint(GLFW_VISIBLE, 0);
		glfwWindowHint(GLFW_RESIZABLE, 0);
		glWindow = glfwCreateWindow(sx, sy, "YGOPRO", 0, 0);
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
		MyGUI::PointerManager::getInstancePtr()->setVisible(false);
		mPlatform->getRenderManagerPtr()->setViewSize(sx, sy);
		FpsSet(60);
	}

	void GameWindow::ShowWindow(int x, int y) {
		glfwSetWindowPos(glWindow, x, y);
		glfwShowWindow(glWindow);
		FpsInitialise();
		(new PanelSysMsg(PANEL_SYSMSG_DEFAULT))->fadeShow();
		while(!exiting && !glfwWindowShouldClose(glWindow)) {
			FpsNextFrame();
			CheckMessage();
			glfwPollEvents();
			glClear(GL_COLOR_BUFFER_BIT);
			mPlatform->getRenderManagerPtr()->drawOneFrame();
			glfwSwapBuffers(glWindow);
		}
		glfwHideWindow(glWindow);
	}

	void GameWindow::Close() {
		exiting = true;
	}

	void GameWindow::LoadTexture(const std::string& name) {
		auto iter = custom_textures.find(name);
		if(iter == custom_textures.end()) {
			CustomTextureInfo& cti = custom_textures[name];
			cti.ptexture = mPlatform->getRenderManagerPtr()->createTexture(name);
			cti.ptexture->loadFromFile(name);
			cti.ref_count = 1;
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

	void GameWindow::mousePosFunc(GLFWwindow* win, double x, double y) {
		MyGUI::InputManager::getInstancePtr()->injectMouseMove((int)x, (int)y, 0);
	}

	void GameWindow::mouseButtonFunc(GLFWwindow* win, int button, int state, int modkey) {
		double xpos, ypos;
		glfwGetCursorPos(win, &xpos, &ypos);
		if(state == GLFW_PRESS)
			MyGUI::InputManager::getInstancePtr()->injectMousePress((int)xpos, (int)ypos, (MyGUI::MouseButton::Enum)button);
		else
			MyGUI::InputManager::getInstancePtr()->injectMouseRelease((int)xpos, (int)ypos, (MyGUI::MouseButton::Enum)button);
	}

	void GameWindow::mouseWheelFunc(GLFWwindow* win, double button, double state) {

	}

}

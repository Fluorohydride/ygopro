#include <iostream>
#include <chrono>
#include <ctime>

#include "scene_mgr.h"
#include "build_scene.h"
#include "image_mgr.h"
#include "card_data.h"
#include "deck_data.h"
#include "sungui.h"

using namespace ygopro;

static float xrate = 0.0f;
static float yrate = 0.0f;

int main(int argc, char* argv[]) {
    if(!glfwInit())
        return 0;
    if(!commonCfg.LoadConfig(L"common.xml"))
        return 0;
    int width = commonCfg[L"window_width"];
    int height = commonCfg[L"window_height"];
    GLFWwindow* window = glfwCreateWindow(width, height, "Ygopro", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(window);
	glewExperimental = true;
    glewInit();

    imageMgr.InitTextures(commonCfg[L"image_file"]);
    if(!stringCfg.LoadConfig(commonCfg[L"string_conf"]))
        return 0;
    if(dataMgr.LoadDatas(commonCfg[L"database_file"]))
        return 0;
    if(!imageMgr.LoadImageConfig(commonCfg[L"textures_conf"]))
        return 0;
    limitRegulationMgr.LoadLimitRegulation(commonCfg[L"limit_regulation"], stringCfg[L"eui_list_default"]);
    stringCfg.ForEach([](const std::wstring& name, ValueStruct& value) {
        if(name.find(L"setname_") == 0 ) {
            dataMgr.RegisterSetCode(static_cast<unsigned int>(value), name.substr(8));
        }
    });
    
    int bwidth, bheight;
    glfwGetFramebufferSize(window, &bwidth, &bheight);
    xrate = (float)bwidth / width;
    yrate = (float)bheight / height;
    
    sgui::SGGUIRoot::GetSingleton().LoadConfigs();
    sgui::SGGUIRoot::GetSingleton().SetSceneSize({bwidth, bheight});
    
    sceneMgr.Init();
    sceneMgr.SetSceneSize({bwidth, bheight});
    sceneMgr.InitDraw();
    sceneMgr.SetFrameRate((int)commonCfg[L"frame_rate"]);
    auto sc = std::make_shared<BuildScene>();
    sceneMgr.SetScene(std::static_pointer_cast<Scene>(sc));
    sc->LoadDeckFromFile(L"./deck/807.ydk");

    glfwSetKeyCallback(window, [](GLFWwindow* wnd, int key, int scan, int action, int mods) {
        if(action == GLFW_PRESS) {
            if(key == GLFW_KEY_R && (mods & GLFW_MOD_ALT)) {
//                auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
//                auto tm = std::localtime(&t);
//                char buf[256];
//                sprintf(buf, "./screenshot/%d%02d%02d-%ld.png", tm->tm_year + 1900, tm->tm_mon, tm->tm_mday, t);
//                window.capture().saveToFile(buf);
            }
            if(!sgui::SGGUIRoot::GetSingleton().InjectKeyDownEvent({key, mods}))
                sceneMgr.KeyDown({key, mods});
        } else if(action == GLFW_RELEASE) {
            if(!sgui::SGGUIRoot::GetSingleton().InjectKeyUpEvent({key, mods}))
                sceneMgr.KeyUp({key, mods});
        }
    });
    glfwSetCharCallback(window, [](GLFWwindow* wnd, unsigned int unichar) {
        sgui::SGGUIRoot::GetSingleton().InjectCharEvent({unichar});
    });
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* wnd, int width, int height) {
        int x, y;
        glfwGetWindowSize(wnd, &x, &y);
        xrate = (float)width / x;
        yrate = (float)height / y;
        sceneMgr.SetSceneSize(v2i{width, height});
        sgui::SGGUIRoot::GetSingleton().SetSceneSize(v2i{width, height});
    });
    glfwSetCursorEnterCallback(window, [](GLFWwindow* wnd, int enter) {
        if(enter == GL_TRUE)
            sgui::SGGUIRoot::GetSingleton().InjectMouseEnterEvent();
        else
            sgui::SGGUIRoot::GetSingleton().InjectMouseLeaveEvent();
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow* wnd, double xpos, double ypos) {
        sceneMgr.SetMousePosition({(int)(xpos * xrate), (int)(ypos * yrate)});
        if(!sgui::SGGUIRoot::GetSingleton().InjectMouseMoveEvent({(int)(xpos * xrate), (int)(ypos * yrate)}))
            sceneMgr.MouseMove({(int)(xpos * xrate), (int)(ypos * yrate)});
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow* wnd, int button, int action, int mods) {
        double xpos, ypos;
        glfwGetCursorPos(wnd, &xpos, &ypos);
        xpos *= xrate;
        ypos *= yrate;
        sceneMgr.SetMousePosition({(int)xpos, (int)ypos});
        if(action == GLFW_PRESS) {
            if(!sgui::SGGUIRoot::GetSingleton().InjectMouseButtonDownEvent({button, mods, (int)xpos, (int)ypos}))
                sceneMgr.MouseButtonDown({button, mods, (int)xpos, (int)ypos});
        } else {
            if(!sgui::SGGUIRoot::GetSingleton().InjectMouseButtonUpEvent({button, mods, (int)xpos, (int)ypos}))
                sceneMgr.MouseButtonUp({button, mods, (int)xpos, (int)ypos});
        }
    });
    glfwSetScrollCallback(window, [](GLFWwindow* wnd, double xoffset, double yoffset) {
        sgui::SGGUIRoot::GetSingleton().InjectMouseWheelEvent({(float)xoffset, (float)yoffset});
    });
    while (!glfwWindowShouldClose(window)) {
        sceneMgr.CheckFrameRate();
        sceneMgr.InitDraw();
        glfwPollEvents();
        sceneMgr.Update();
        sceneMgr.Draw();
        sgui::SGGUIRoot::GetSingleton().Draw();
        glfwSwapBuffers(window);
    }
    
    sceneMgr.Uninit();
    sgui::SGGUIRoot::GetSingleton().Unload();
    imageMgr.UninitTextures();
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

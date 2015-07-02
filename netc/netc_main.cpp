#include "utils/common.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "utils/textfile.h"
#include "utils/filesystem.h"
#include "utils/sgui.h"

#include "config.h"

#include "scene_mgr.h"
#include "image_mgr.h"
#include "card_data.h"
#include "deck_data.h"
#include "bs/build_input_handler.h"
#include "bs/build_scene_handler.h"
#include "bs/build_scene.h"
//#include "ds/duel_input_handler.h"
//#include "ds/duel_scene_handler.h"
//#include "ds/duel_network.h"
//#include "ds/duel_scene.h"

using namespace ygopro;

static float xrate = 0.0f;
static float yrate = 0.0f;
static bool need_draw = true;

jaweson::JsonRoot<> commonCfg;
jaweson::JsonRoot<> stringCfg;
jaweson::JsonRoot<> layoutCfg;
jaweson::JsonRoot<> textureCfg;

int32_t main(int32_t argc, char* argv[]) {
    if(!glfwInit())
        return 0;
    {
        TextFile jsonfile;
        jsonfile.Load("common.json");
        if(!commonCfg.parse(jsonfile.Data(), jsonfile.Length()))
            return 0;
        jsonfile.Load(FileSystem::UTF8ToLocalFilename(commonCfg["string_conf"].to_string()));
        if(!stringCfg.parse(jsonfile.Data(), jsonfile.Length()))
            return 0;
        jsonfile.Load(FileSystem::UTF8ToLocalFilename(commonCfg["layout_conf"].to_string()));
        if(!layoutCfg.parse(jsonfile.Data(), jsonfile.Length()))
            return 0;
        jsonfile.Load(FileSystem::UTF8ToLocalFilename(commonCfg["textures_conf"].to_string()));
        if(!textureCfg.parse(jsonfile.Data(), jsonfile.Length()))
            return 0;
    }
    int32_t width = (int32_t)commonCfg["window_width"].to_integer();
    int32_t height = (int32_t)commonCfg["window_height"].to_integer();;
	int32_t fsaa = (int32_t)commonCfg["fsaa"].to_integer();;
    int32_t vsync = (int32_t)commonCfg["vertical_sync"].to_integer();;
	if(fsaa)
		glfwWindowHint(GLFW_SAMPLES, fsaa);
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    auto mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    GLFWwindow* window = glfwCreateWindow(width, height, "Ygopro", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return 0;
    }
    glfwSetWindowPos(window, (mode->width - width) / 2, (mode->height - height) / 2);
    glfwShowWindow(window);
    glfwMakeContextCurrent(window);
	glewExperimental = true;
    glewInit();

    int32_t major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    std::cout << "GL Vendor    : " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "GL Renderer  : " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "GL Version (string)  : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GL Version (integer) : " << major << "." << minor << std::endl;
    std::cout << "GLSL Version : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    glGetError();
    
    int32_t bwidth, bheight;
    glfwGetFramebufferSize(window, &bwidth, &bheight);
    xrate = (float)bwidth / width;
    yrate = (float)bheight / height;
    
    ImageMgr::Get().InitTextures(FileSystem::UTF8ToLocalFilename(commonCfg["image_path"].to_string()));
    if(DataMgr::Get().LoadDatas(FileSystem::UTF8ToLocalFilename(commonCfg["database_file"].to_string()))
       || !ImageMgr::Get().LoadImageConfig()
       || !sgui::SGGUIRoot::GetSingleton().Init(FileSystem::UTF8ToLocalFilename(commonCfg["gui_conf"].to_string()), {bwidth, bheight}, true)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 0;
    }
    LimitRegulationMgr::Get().LoadLimitRegulation(FileSystem::UTF8ToLocalFilename(commonCfg["limit_regulation"].to_string()),
                                                  To<std::wstring>(stringCfg["eui_list_default"].to_string()));
    stringCfg["setname"].for_each([](const std::string& name, jaweson::JsonNode<>& node) {
        std::wstring setname = To<std::wstring>(name);
        DataMgr::Get().RegisterSetCode(To<uint32_t>(node.to_string()), setname);
    });
    
    SceneMgr::Get().Init();
    SceneMgr::Get().SetSceneSize({bwidth, bheight});
    SceneMgr::Get().SetFrameRate((int32_t)commonCfg["frame_rate"].to_integer());
    
    auto sc = std::make_shared<BuildScene>();
    auto ih = std::make_shared<BuildInputHandler>(sc);
    auto sh = std::make_shared<BuildSceneHandler>(sc);
    sc->SetInputHandler(ih);
    sc->SetSceneHandler(sh);
    SceneMgr::Get().SetScene(sc);
    
//    auto sc = std::make_shared<DuelScene>();
//    auto ih = std::make_shared<DuelInputHandler>(sc);
//    auto sh = std::make_shared<DuelSceneHandler>(sc);
//    auto ph = std::make_shared<DuelProtoTcp>();
//    sh->SetProtoHandler(ph);
//    sc->SetInputHandler(ih);
//    sc->SetSceneHandler(sh);
    SceneMgr::Get().SetScene(sc);
    
    glfwSetKeyCallback(window, [](GLFWwindow* wnd, int32_t key, int32_t scan, int32_t action, int32_t mods) {
        if(action == GLFW_PRESS) {
            if(key == GLFW_KEY_GRAVE_ACCENT && (mods & GLFW_MOD_ALT))
                SceneMgr::Get().ScreenShot();
            if(!sgui::SGGUIRoot::GetSingleton().InjectKeyDownEvent(key, mods))
                SceneMgr::Get().GetScene()->GetInputHandler()->KeyDown(key, mods);
        } else if(action == GLFW_RELEASE) {
            if(!sgui::SGGUIRoot::GetSingleton().InjectKeyUpEvent(key, mods))
                SceneMgr::Get().GetScene()->GetInputHandler()->KeyUp(key, mods);
        }
    });
    glfwSetCharCallback(window, [](GLFWwindow* wnd, uint32_t unichar) {
        sgui::SGGUIRoot::GetSingleton().InjectCharEvent({unichar});
    });
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* wnd, int32_t width, int32_t height) {
        int32_t x, y;
        glfwGetWindowSize(wnd, &x, &y);
        xrate = (float)width / x;
        yrate = (float)height / y;
        SceneMgr::Get().SetSceneSize({width, height});
        sgui::SGGUIRoot::GetSingleton().SetScreenSize({width, height});
    });
    glfwSetCursorEnterCallback(window, [](GLFWwindow* wnd, int32_t enter) {
        if(enter == GL_TRUE)
            sgui::SGGUIRoot::GetSingleton().InjectMouseEnterEvent();
        else
            sgui::SGGUIRoot::GetSingleton().InjectMouseLeaveEvent();
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow* wnd, double xpos, double ypos) {
        SceneMgr::Get().SetMousePosition({(int32_t)(xpos * xrate), (int32_t)(ypos * yrate)});
        if(!sgui::SGGUIRoot::GetSingleton().InjectMouseMoveEvent((int32_t)(xpos * xrate), (int32_t)(ypos * yrate)))
            SceneMgr::Get().GetScene()->GetInputHandler()->MouseMove((int32_t)(xpos * xrate), (int32_t)(ypos * yrate));
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow* wnd, int32_t button, int32_t action, int32_t mods) {
        double xpos, ypos;
        glfwGetCursorPos(wnd, &xpos, &ypos);
        xpos *= xrate;
        ypos *= yrate;
        SceneMgr::Get().SetMousePosition({(int32_t)xpos, (int32_t)ypos});
        if(action == GLFW_PRESS) {
            if(!sgui::SGGUIRoot::GetSingleton().InjectMouseButtonDownEvent(button, mods, (int32_t)xpos, (int32_t)ypos))
                SceneMgr::Get().GetScene()->GetInputHandler()->MouseButtonDown(button, mods, (int32_t)xpos, (int32_t)ypos);
        } else {
            if(!sgui::SGGUIRoot::GetSingleton().InjectMouseButtonUpEvent(button, mods, (int32_t)xpos, (int32_t)ypos))
                SceneMgr::Get().GetScene()->GetInputHandler()->MouseButtonUp(button, mods, (int32_t)xpos, (int32_t)ypos);
        }
    });
    glfwSetScrollCallback(window, [](GLFWwindow* wnd, double xoffset, double yoffset) {
        if(!sgui::SGGUIRoot::GetSingleton().InjectMouseWheelEvent((float)xoffset, (float)yoffset))
            SceneMgr::Get().GetScene()->GetInputHandler()->MouseWheel((float)xoffset, (float)yoffset);
    });
    glfwSetWindowIconifyCallback(window, [](GLFWwindow* wnd, int32_t iconified) {
        need_draw = (iconified == GL_FALSE);
        if(need_draw)
            SceneMgr::Get().SetFrameRate((int32_t)commonCfg["frame_rate"].to_integer());
        else
            SceneMgr::Get().SetFrameRate(10);
    });
    if(vsync)
        glfwSwapInterval(1);
    else
        glfwSwapInterval(0);
    while (!glfwWindowShouldClose(window)) {
        SceneMgr::Get().CheckFrameRate();
        auto& shader = base::Shader::GetDefaultShader();
        shader.Use();
        shader.SetParam1i("texID", 0);
        glfwPollEvents();
        if(!SceneMgr::Get().Update())
            break;
        shader.Unuse();
        if(need_draw) {
            SceneMgr::Get().Draw();
            sgui::SGGUIRoot::GetSingleton().Render();
        }
        glfwSwapBuffers(window);
    }
    
    SceneMgr::Get().Uninit();
    sgui::SGGUIRoot::GetSingleton().Uninit();
    ImageMgr::Get().UninitTextures();
    base::Shader::GetDefaultShader().Unload();
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

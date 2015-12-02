#include "utils/common.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "utils/textfile.h"
#include "utils/filesystem.h"

#include "sgui.h"
#include "config.h"

#include "scene_mgr.h"
#include "image_mgr.h"
#include "card_data.h"
#include "deck_data.h"
#include "bs/build_scene_handler.h"
#include "bs/build_scene.h"
#include "ds/duel_scene_handler.h"
//#include "ds/duel_network.h"
#include "ds/duel_scene.h"

using namespace ygopro;

static float xrate = 0.0f;
static float yrate = 0.0f;
static bool need_draw = true;

jaweson::JsonRoot<> commonCfg;
jaweson::JsonRoot<> stringCfg;
jaweson::JsonRoot<> layoutCfg;
jaweson::JsonRoot<> textureCfg;
jaweson::JsonRoot<> dialogCfg;

int32_t node2int(jaweson::JsonValue& node) {
    return node.is_number() ? node.to_value<int32_t>() : To<int32_t>(node.to_string());
};

int32_t main(int32_t argc, char* argv[]) {
    if(!glfwInit())
        return 0;
    {
        TextFile textfile;
        textfile.Load("common.json");
        if(!commonCfg.parse(textfile.Data(), textfile.Length()))
            return 0;
        for(int32_t i = 1; i < argc; ++i) {
            std::string argstr = FileSystem::LocalCharsetToUTF8(argv[i]);
            if(argstr.length() < 2)
                continue;
            if(argstr[0] != '-' || argstr[1] != '-')
                continue;
            auto epos = argstr.find('=');
            if(epos == argstr.npos)
                continue;
            commonCfg[argstr.substr(2, epos - 2)].set_value<jaweson::JsonString<>>(argstr.substr(epos + 1));
        }
        textfile.Load(FileSystem::UTF8ToLocalCharset(commonCfg["string_conf"].to_string()));
        if(!stringCfg.parse(textfile.Data(), textfile.Length()))
            return 0;
        textfile.Load(FileSystem::UTF8ToLocalCharset(commonCfg["layout_conf"].to_string()));
        if(!layoutCfg.parse(textfile.Data(), textfile.Length()))
            return 0;
        textfile.Load(FileSystem::UTF8ToLocalCharset(commonCfg["textures_conf"].to_string()));
        if(!textureCfg.parse(textfile.Data(), textfile.Length()))
            return 0;
        textfile.Load(FileSystem::UTF8ToLocalCharset(commonCfg["dialog_conf"].to_string()));
        if(!dialogCfg.parse(textfile.Data(), textfile.Length()))
            return 0;
        stringCfg["setname"].for_each([](const std::string& name, jaweson::JsonValue& node) {
            std::wstring setname = To<std::wstring>(node.to_string());
            DataMgr::Get().RegisterSetCode(To<uint32_t>(name), setname);
        });
    }
    int32_t width = node2int(commonCfg["window_width"]);
    int32_t height = node2int(commonCfg["window_height"]);
	int32_t fsaa = node2int(commonCfg["fsaa"]);
    int32_t vsync = node2int(commonCfg["vertical_sync"]);
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
    
    std::shared_ptr<base::Shader> _2dshader;
    std::shared_ptr<base::Shader> _3dshader;
    {
        TextFile textfile;
        textfile.Load(FileSystem::UTF8ToLocalCharset(commonCfg["2d_shader"].to_string()));
        _2dshader = std::make_shared<base::Shader>();
        _2dshader->LoadSource(textfile.Data());
        _2dshader->SetDefaultParamCallback([](base::Shader* shader) {
            shader->SetParam1i("texid", 0);
            shader->SetParam1f("alpha", 1.0);
        });
        textfile.Load(FileSystem::UTF8ToLocalCharset(commonCfg["3d_shader"].to_string()));
        _3dshader = std::make_shared<base::Shader>();
        _3dshader->LoadSource(textfile.Data());
        _3dshader->SetDefaultParamCallback([](base::Shader* shader) {
            shader->SetParam1i("texid", 0);
            shader->SetParamMat4("mvp", glm::value_ptr(glm::mat4(1.0f)));
        });
    }
    
    int32_t bwidth, bheight;
    glfwGetFramebufferSize(window, &bwidth, &bheight);
    xrate = (float)bwidth / width;
    yrate = (float)bheight / height;
    ImageMgr::Get().InitTextures(FileSystem::UTF8ToLocalCharset(commonCfg["image_path"].to_string()), _2dshader.get());
    if(DataMgr::Get().LoadDatas(FileSystem::UTF8ToLocalCharset(commonCfg["database_file"].to_string()))
       || !ImageMgr::Get().LoadImageConfig()
       || !sgui::SGGUIRoot::GetSingleton().Init(FileSystem::UTF8ToLocalCharset(commonCfg["gui_conf"].to_string()), {bwidth, bheight}, true)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 0;
    }
    ImageMgr::Get().LoadAvatar(0, FileSystem::UTF8ToLocalCharset(commonCfg["avatar1"].to_string()));
    ImageMgr::Get().LoadAvatar(1, FileSystem::UTF8ToLocalCharset(commonCfg["avatar2"].to_string()));
    ImageMgr::Get().LoadSleeve(0, FileSystem::UTF8ToLocalCharset(commonCfg["sleeve1"].to_string()));
    ImageMgr::Get().LoadSleeve(1, FileSystem::UTF8ToLocalCharset(commonCfg["sleeve2"].to_string()));
    LimitRegulationMgr::Get().LoadLimitRegulation(FileSystem::UTF8ToLocalCharset(commonCfg["limit_regulation"].to_string()),
                                                  To<std::wstring>(stringCfg["eui_list_default"].to_string()));
    
    glfwSetKeyCallback(window, [](GLFWwindow* wnd, int32_t key, int32_t scan, int32_t action, int32_t mods) {
        if(action == GLFW_PRESS) {
            if(key == GLFW_KEY_GRAVE_ACCENT && (mods & GLFW_MOD_ALT))
                SceneMgr::Get().ScreenShot();
            sgui::SGGUIRoot::GetSingleton().InjectKeyDownEvent(key, mods);
            SceneMgr::Get().GetScene()->GetSceneHandler()->KeyDown(key, mods);
        } else if(action == GLFW_RELEASE) {
            sgui::SGGUIRoot::GetSingleton().InjectKeyUpEvent(key, mods);
            SceneMgr::Get().GetScene()->GetSceneHandler()->KeyUp(key, mods);
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
            SceneMgr::Get().GetScene()->GetSceneHandler()->MouseMove((int32_t)(xpos * xrate), (int32_t)(ypos * yrate));
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow* wnd, int32_t button, int32_t action, int32_t mods) {
        double xpos, ypos;
        glfwGetCursorPos(wnd, &xpos, &ypos);
        xpos *= xrate;
        ypos *= yrate;
        SceneMgr::Get().SetMousePosition({(int32_t)xpos, (int32_t)ypos});
        if(action == GLFW_PRESS) {
            if(!sgui::SGGUIRoot::GetSingleton().InjectMouseButtonDownEvent(button, mods, (int32_t)xpos, (int32_t)ypos))
                SceneMgr::Get().GetScene()->GetSceneHandler()->MouseButtonDown(button, mods, (int32_t)xpos, (int32_t)ypos);
        } else {
            if(!sgui::SGGUIRoot::GetSingleton().InjectMouseButtonUpEvent(button, mods, (int32_t)xpos, (int32_t)ypos))
                SceneMgr::Get().GetScene()->GetSceneHandler()->MouseButtonUp(button, mods, (int32_t)xpos, (int32_t)ypos);
        }
    });
    glfwSetScrollCallback(window, [](GLFWwindow* wnd, double xoffset, double yoffset) {
        if(!sgui::SGGUIRoot::GetSingleton().InjectMouseWheelEvent((float)xoffset, (float)yoffset))
            SceneMgr::Get().GetScene()->GetSceneHandler()->MouseWheel((float)xoffset, (float)yoffset);
    });
    glfwSetWindowIconifyCallback(window, [](GLFWwindow* wnd, int32_t iconified) {
        need_draw = (iconified == GL_FALSE);
        if(need_draw)
            SceneMgr::Get().SetFrameRate(node2int(commonCfg["frame_rate"]));
        else
            SceneMgr::Get().SetFrameRate(10);
    });
    if(vsync)
        glfwSwapInterval(1);
    else
        glfwSwapInterval(0);
    
    sgui::SGGUIRoot::GetSingleton().SetShader(_2dshader.get());
    sgui::SGGUIRoot::GetSingleton().SetClipboardCallback([window]()->std::string {
        std::string str = glfwGetClipboardString(window);
        return std::move(str);
    }, [window](const std::string& str) {
        glfwSetClipboardString(window, str.c_str());
    });
    
    SceneMgr::Get().Init();
    SceneMgr::Get().SetSceneSize({bwidth, bheight});
    SceneMgr::Get().SetFrameRate(node2int(commonCfg["frame_rate"]));
    
//    auto sc = std::make_shared<BuildScene>(_2dshader.get());
    auto sc = std::make_shared<DuelScene>(_2dshader.get(), _3dshader.get());

    SceneMgr::Get().SetScene(sc);
    SceneMgr::Get().InitFrameControler();
    SceneMgr::Get().SetFrameRate(60);
    
    while (!glfwWindowShouldClose(window)) {
        SceneMgr::Get().CheckFrameRate();
        glfwPollEvents();
        if(!SceneMgr::Get().Update())
            break;
        if(need_draw && SceneMgr::Get().Draw())
            glfwSwapBuffers(window);
    }
    
    SceneMgr::Get().Uninit();
    sgui::SGGUIRoot::GetSingleton().Uninit();
    ImageMgr::Get().UninitTextures();
    _2dshader.reset();
    _3dshader.reset();
    glfwDestroyWindow(window);
    glfwTerminate();
    if(commonCfg["save_config_on_exit"].to_bool()) {
        std::ofstream conf_file("common.json");
        conf_file << commonCfg;
    }
    return 0;
}

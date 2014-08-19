#include "../common/common.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../buildin/stb_image_write.h"

#include "glbase.h"
#include "scene_mgr.h"

namespace ygopro
{

	CommonConfig commonCfg;
    CommonConfig stringCfg;
    
    void SceneMgr::Init() {
        start_time = std::chrono::system_clock::now().time_since_epoch().count();
    }
    
    void SceneMgr::Uninit() {
        if(current_scene != nullptr)
            current_scene.reset();
    }
    
    void SceneMgr::InitDraw() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        glActiveTexture(GL_TEXTURE0);
    }
    
    bool SceneMgr::Update() {
        if(current_scene != nullptr)
            return current_scene->Update();
        return true;
    }
    
    void SceneMgr::Draw() {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        if(current_scene != nullptr)
            current_scene->Draw();
    }
    
    double SceneMgr::GetGameTime() {
        unsigned long long now = std::chrono::system_clock::now().time_since_epoch().count();
        return (double)(now - start_time) * std::chrono::system_clock::period::num / std::chrono::system_clock::period::den;
    }
    
    void SceneMgr::SetFrameRate(double rate) {
        frame_interval = 1.0 / rate;
    }
    
    void SceneMgr::CheckFrameRate() {
        double now = GetGameTime();
        frame_check += frame_interval - (now - frame_time);
        if(frame_check >= 0.0)
            std::this_thread::sleep_for(std::chrono::microseconds((int)(frame_interval * 1000000)));
        frame_time = now;
    }
    
    void SceneMgr::MouseMove(sgui::MouseMoveEvent evt) {
        if(current_scene != nullptr)
            current_scene->MouseMove(evt);
    }
    
    void SceneMgr::MouseButtonDown(sgui::MouseButtonEvent evt) {
        if(current_scene != nullptr)
            current_scene->MouseButtonDown(evt);
    }
    
    void SceneMgr::MouseButtonUp(sgui::MouseButtonEvent evt) {
        if(current_scene != nullptr)
            current_scene->MouseButtonUp(evt);
    }
    
    void SceneMgr::KeyDown(sgui::KeyEvent evt) {
        if(current_scene != nullptr)
            current_scene->KeyDown(evt);
    }
    
    void SceneMgr::KeyUp(sgui::KeyEvent evt) {
        if(current_scene != nullptr)
            current_scene->KeyUp(evt);
    }
    
    void SceneMgr::SetSceneSize(v2i sz) {
        scene_size = sz;
        if(current_scene != nullptr)
            current_scene->SetSceneSize(sz);
    }
    
    void SceneMgr::SetScene(std::shared_ptr<Scene> sc) {
        if(current_scene == sc)
            return;
        sgui::SGGUIRoot::GetSingleton().ClearChild();
        current_scene = sc;
        if(current_scene != nullptr) {
            current_scene->SetSceneSize(scene_size);
            current_scene->Activate();
        }
    }
    
    void SceneMgr::ScreenShot() {
        if(current_scene == nullptr)
            return;
        auto clip = current_scene->GetScreenshotClip();
        unsigned char* image_buff = new unsigned char[scene_size.x * scene_size.y * 4];
        unsigned char* clip_buff = new unsigned char[clip.width * clip.height * 4];
        glReadPixels(0, 0, scene_size.x, scene_size.y, GL_RGBA, GL_UNSIGNED_BYTE, image_buff);
        for(int h = 0; h < clip.height; ++h) {
            int offset = scene_size.x * 4 * (scene_size.y - 1 - clip.top - h);
            memcpy(&clip_buff[clip.width * 4 * h], &image_buff[offset], clip.width * 4);
        }
        auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        auto tm = std::localtime(&t);
        char file[256];
        sprintf(file, "/%d%02d%02d-%ld.png", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, t);
        std::wstring wpath = commonCfg["screenshot_path"];
        std::string path = To<std::string>(wpath);
        path.append(file);
        stbi_write_png(path.c_str(), clip.width, clip.height, 4, clip_buff, 0);
        delete[] clip_buff;
        delete[] image_buff;
    }
}

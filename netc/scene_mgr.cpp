#include "utils/common.h"
#include "utils/filesystem.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "utils/stb_image_write.h"

#include "render_base.h"
#include "render_util.h"
#include "sgui.h"

#include "config.h"
#include "scene_mgr.h"

namespace ygopro
{

    void SceneMgr::Init() {
        InitActionTime(GetSysClock());
    }
    
    void SceneMgr::Uninit() {
        if(current_scene != nullptr) {
            current_scene->Terminate();
            current_scene.reset();
        }
    }
    
    bool SceneMgr::Update() {
        UpdateSysClock();
        UpdateActionTime(GetSysClock());
        if(current_scene != nullptr)
            return current_scene->Update();
        return true;
    }
    
    bool SceneMgr::Draw() {
        if(current_scene != nullptr)
            return current_scene->Draw();
        return false;
    }
    
    void SceneMgr::SetSceneSize(v2i sz) {
        scene_size = sz;
        if(current_scene != nullptr)
            current_scene->SetSceneSize(sz);
    }
    
    void SceneMgr::SetScene(std::shared_ptr<Scene> sc) {
        if(current_scene == sc)
            return;
        if(current_scene)
            current_scene->Terminate();
        ClearAllActions();
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
        uint8_t* image_buff = new uint8_t[scene_size.x * scene_size.y * 4];
        uint8_t* clip_buff = new uint8_t[clip.width * clip.height * 4];
        glReadPixels(0, 0, scene_size.x, scene_size.y, GL_RGBA, GL_UNSIGNED_BYTE, image_buff);
        for(int32_t h = 0; h < clip.height; ++h) {
            int32_t offset = scene_size.x * 4 * (scene_size.y - 1 - clip.top - h) + clip.left * 4;
            memcpy(&clip_buff[clip.width * 4 * h], &image_buff[offset], clip.width * 4);
        }
        std::string path = commonCfg["screenshot_path"].to_string();
        std::thread([path, clip, clip_buff, image_buff]() {
            auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            auto tm = std::localtime(&t);
            std::string file = To<std::string>("/%d%02d%02d-%ld.png", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, t);
            auto localfile = FileSystem::UTF8ToLocalFilename(path + file);
            stbi_write_png(localfile.c_str(), clip.width, clip.height, 4, clip_buff, 0);
            delete[] clip_buff;
            delete[] image_buff;
        }).detach();
    }
}

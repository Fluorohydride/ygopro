#include <array>
#include <chrono>
#include <thread>
#include <iostream>

#include <wx/filename.h>
#include <wx/clipbrd.h>
#include <wx/utils.h>

#include "glbase.h"
#include "sungui.h"
#include "scene_mgr.h"
#include "image_mgr.h"
#include "card_data.h"
#include "deck_data.h"
#include "build_scene.h"

namespace ygopro
{

    SceneMgr sceneMgr;
	Random globalRandom;
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
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_INDEX_ARRAY);
        glLoadIdentity();
    }
    
    void SceneMgr::Update() {
        if(current_scene != nullptr)
            current_scene->Update();
    }
    
    void SceneMgr::Draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        if(current_scene != nullptr)
            current_scene->Draw();
    }
    
    float SceneMgr::GetGameTime() {
        unsigned long long now = std::chrono::system_clock::now().time_since_epoch().count();
        return (float)(now - start_time) * std::chrono::system_clock::period::num / std::chrono::system_clock::period::den;
    }
    
    void SceneMgr::SetFrameRate(float rate) {
        frame_interval = 1.0f / rate;
    }
    
    void SceneMgr::CheckFrameRate() {
        float now = GetGameTime();
        frame_check += frame_interval - (now - frame_time);
        if(frame_check >= 0.0f)
            std::this_thread::sleep_for(std::chrono::microseconds((int)(frame_interval * 1000000)));
        frame_time = now;
    }
    
    void SceneMgr::MouseMove(sf::Event::MouseMoveEvent evt) {
        if(current_scene != nullptr)
            current_scene->MouseMove(evt);
    }
    
    void SceneMgr::MouseButtonDown(sf::Event::MouseButtonEvent evt) {
        if(current_scene != nullptr)
            current_scene->MouseButtonDown(evt);
    }
    
    void SceneMgr::MouseButtonUp(sf::Event::MouseButtonEvent evt) {
        if(current_scene != nullptr)
            current_scene->MouseButtonUp(evt);
    }
    
    void SceneMgr::KeyDown(sf::Event::KeyEvent evt) {
        if(current_scene != nullptr)
            current_scene->KeyDown(evt);
    }
    
    void SceneMgr::KeyUp(sf::Event::KeyEvent evt) {
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
    
}

#include "../common/common.h"

#include "duel_scene.h"

namespace ygopro
{
    
    DuelScene::DuelScene() {
        
    }
    
    DuelScene::~DuelScene() {

    }
    
    void DuelScene::Activate() {

    }
    
    bool DuelScene::Update() {
        std::shared_ptr<DuelCommand> cmd;
        while((cmd = duel_commands.PullCommand()) != nullptr)
            cmd->Handle();
        return true;
    }
    
    void DuelScene::Draw() {
        
    }
    
    void DuelScene::SetSceneSize(v2i sz) {
        scene_size = sz;
    }
    
    recti DuelScene::GetScreenshotClip() {
        return {0, 0, scene_size.x, scene_size.y};
    }
    
    void DuelScene::MouseMove(sgui::MouseMoveEvent evt) {
        
    }
    
    void DuelScene::MouseButtonDown(sgui::MouseButtonEvent evt) {
        
    }
    
    void DuelScene::MouseButtonUp(sgui::MouseButtonEvent evt) {
        
    }
    
    void DuelScene::KeyDown(sgui::KeyEvent evt) {
        
    }
    
    void DuelScene::KeyUp(sgui::KeyEvent evt) {
        
    }
    
}

#include "../common/common.h"

#include "duel_scene.h"

namespace ygopro
{
    
    DuelScene::DuelScene() {
        glGenBuffers(1, &index_buffer);
        glGenBuffers(1, &back_buffer);
        glGenBuffers(1, &field_buffer);
        glGenBuffers(1, &card_buffer);
        glGenBuffers(1, &misc_buffer);
        GLCheckError(__FILE__, __LINE__);
        glBindBuffer(GL_ARRAY_BUFFER, back_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * 4, nullptr, GL_DYNAMIC_DRAW);
        GLCheckError(__FILE__, __LINE__);
        glBindBuffer(GL_ARRAY_BUFFER, field_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * 256 * 16, nullptr, GL_DYNAMIC_DRAW);
        GLCheckError(__FILE__, __LINE__);
        glBindBuffer(GL_ARRAY_BUFFER, card_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * 33 * 4, nullptr, GL_DYNAMIC_DRAW);
        GLCheckError(__FILE__, __LINE__);
        glBindBuffer(GL_ARRAY_BUFFER, misc_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * 10 * 16, nullptr, GL_DYNAMIC_DRAW);
        GLCheckError(__FILE__, __LINE__);
        std::vector<unsigned short> index;
        index.resize(256 * 4 * 6);
        for(int i = 0; i < 256 * 4; ++i) {
            index[i * 6] = i * 4;
            index[i * 6 + 1] = i * 4 + 2;
            index[i * 6 + 2] = i * 4 + 1;
            index[i * 6 + 3] = i * 4 + 3;
            index[i * 6 + 4] = i * 4 + 3;
            index[i * 6 + 5] = i * 4 + 4;
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 256 * 4 * 6, &index[0], GL_STATIC_DRAW);
        GLCheckError(__FILE__, __LINE__);
    }
    
    DuelScene::~DuelScene() {
        glDeleteBuffers(1, &index_buffer);
        glDeleteBuffers(1, &back_buffer);
        glDeleteBuffers(1, &field_buffer);
        glDeleteBuffers(1, &card_buffer);
        glDeleteBuffers(1, &misc_buffer);
    }
    
    void DuelScene::Activate() {
        
    }
    
    bool DuelScene::Update() {
        PullEvent();
        double now = SceneMgr::Get().GetGameTime();
        while(now >= waiting_time) {
            if(current_cb != nullptr) {
                current_cb();
                current_cb = nullptr;
            }
            std::shared_ptr<DuelCommand> cmd = duel_commands.PullCommand();
            if(cmd == nullptr)
                break;
            cmd->Handle();
            waiting_time = now + cmd->wait_time;
            current_cb = cmd->cb;
        }
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

#include "../common/common.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gui_extra.h"
#include "duel_command.h"
#include "image_mgr.h"

#include "duel_scene.h"

static const char* vert_shader = "\
#version 330\n\
layout (location = 0) in vec2 v_position;\n\
layout (location = 1) in vec4 v_color;\n\
layout (location = 2) in vec4 v_hcolor;\n\
layout (location = 3) in vec2 v_texcoord;\n\
uniform mat4 mvp;\n\
out vec4 color;\n\
out vec4 hcolor;\n\
out vec2 texcoord;\n\
void main() {\n\
color = v_color;\n\
hcolor = v_hcolor;\n\
texcoord = v_texcoord;\n\
gl_Position = mvp * vec4(v_position, 0.0, 1.0);\n\
}\n\
";
static const char* frag_shader = "\
#version 330\n\
in vec4 color;\n\
in vec4 hcolor;\n\
in vec2 texcoord;\n\
layout (location = 0) out vec4 frag_color;\n\
uniform sampler2D texid;\n\
void main() {\n\
vec4 texcolor = texture(texid, texcoord);\n\
frag_color = texcolor * color;\n\
}\n\
";

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
        glBufferData(GL_ARRAY_BUFFER, sizeof(v3hct) * 4, nullptr, GL_DYNAMIC_DRAW);
        GLCheckError(__FILE__, __LINE__);
        glBindBuffer(GL_ARRAY_BUFFER, field_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(v3hct) * 36 * 4, nullptr, GL_DYNAMIC_DRAW);
        GLCheckError(__FILE__, __LINE__);
        glBindBuffer(GL_ARRAY_BUFFER, card_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(v3hct) * 512 * 16, nullptr, GL_DYNAMIC_DRAW);
        GLCheckError(__FILE__, __LINE__);
        glBindBuffer(GL_ARRAY_BUFFER, misc_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(v3hct) * 32 * 4, nullptr, GL_DYNAMIC_DRAW);
        GLCheckError(__FILE__, __LINE__);
        std::vector<unsigned short> index;
        index.resize(512 * 4 * 6);
        for(int i = 0; i < 512 * 4; ++i) {
            index[i * 6] = i * 4;
            index[i * 6 + 1] = i * 4 + 2;
            index[i * 6 + 2] = i * 4 + 1;
            index[i * 6 + 3] = i * 4 + 3;
            index[i * 6 + 4] = i * 4 + 3;
            index[i * 6 + 5] = i * 4 + 4;
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 512 * 4 * 6, &index[0], GL_STATIC_DRAW);
        glGenVertexArrays(1, &field_vao);
        glGenVertexArrays(1, &back_vao);
        glGenVertexArrays(1, &card_vao);
        glGenVertexArrays(1, &misc_vao);
        for(int i = 0; i < 4; ++i) {
            switch(i) {
                case 0: glBindVertexArray(field_vao); glBindBuffer(GL_ARRAY_BUFFER, field_buffer); break;
                case 1: glBindVertexArray(back_vao); glBindBuffer(GL_ARRAY_BUFFER, back_buffer); break;
                case 2: glBindVertexArray(card_vao); glBindBuffer(GL_ARRAY_BUFFER, card_buffer); break;
                case 3: glBindVertexArray(misc_vao); glBindBuffer(GL_ARRAY_BUFFER, misc_buffer); break;
                default: break;
            }
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(v3hct), 0);
            glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(v3hct), (const GLvoid*)v3hct::color_offset);
            glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(v3hct), (const GLvoid*)v3hct::hcolor_offset);
            glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(v3hct), (const GLvoid*)v3hct::tex_offset);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
            glBindVertexArray(0);
        }
        GLCheckError(__FILE__, __LINE__);
        duel_shader.LoadVertShader(vert_shader);
        duel_shader.LoadFragShader(frag_shader);
        duel_shader.Link();
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
        glViewport(0, 0, scene_size.x, scene_size.y);
        duel_shader.Use();
        duel_shader.SetParam1i("texid", 0);
        // background
        ImageMgr::Get().GetRawBGTexture()->Bind();
        duel_shader.SetParamMat4("mvp", glm::value_ptr(glm::mat4(1.0f)));
        glBindVertexArray(back_vao);
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0);
        GLCheckError(__FILE__, __LINE__);
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, -2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(45.0f, 1.0f * scene_size.x / scene_size.y, 0.1f, 10.0f);
        glm::mat4 mvp = projection * view;
        duel_shader.SetParamMat4("mvp", glm::value_ptr(mvp));
        duel_shader.Unuse();
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
    
    void DuelScene::UpdateBackground() {
        if(!update_bg)
            return;
        update_bg = false;
        auto ti = ImageMgr::Get().GetTexture("bg");
        std::array<glbase::v2ct, 4> verts;
        glbase::FillVertex(&verts[0], {-1.0f, 1.0f}, {2.0f, -2.0f}, ti);
        glBindBuffer(GL_ARRAY_BUFFER, back_buffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v2ct) * verts.size(), &verts[0]);
        GLCheckError(__FILE__, __LINE__);
    }
    
    void DuelScene::UpdateField() {
        
    }
    
    void DuelScene::UpdateMisc() {
        
    }
    
}

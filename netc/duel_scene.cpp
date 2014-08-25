#include "../common/common.h"

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
frag_color = mix(texcolor * color, vec4(hcolor.r, hcolor.g, hcolor.b, 0.0), hcolor.a);\n\
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
        duel_shader.Unload();
    }
    
    void DuelScene::Activate() {
        
    }
    
    bool DuelScene::Update() {
        //PullEvent();
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
        UpdateBackground();
        UpdateField();
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
        // field
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, -r * cosf(angle), r * sinf(angle)), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(45.0f, 1.0f * scene_size.x / scene_size.y, 0.1f, 100.0f);
        glm::mat4 trscreen;
        trscreen[3][0] = xoffset;
        trscreen[3][1] = yoffset;
        glm::mat4 mvp = trscreen * projection * view;
        duel_shader.SetParamMat4("mvp", glm::value_ptr(mvp));
        ImageMgr::Get().GetRawMiscTexture()->Bind();
        glBindVertexArray(field_vao);
        glDrawElements(GL_TRIANGLE_STRIP, 34 * 6 - 2, GL_UNSIGNED_SHORT, 0);
        // end
        glBindVertexArray(back_vao);
        duel_shader.Unuse();
    }
    
    void DuelScene::SetSceneSize(v2i sz) {
        scene_size = sz;
    }
    
    recti DuelScene::GetScreenshotClip() {
        return {0, 0, scene_size.x, scene_size.y};
    }
    
    void DuelScene::MouseMove(sgui::MouseMoveEvent evt) {
        if(btnDown[0]) {
            float ratex = (float)(evt.x - btnPos[0].x) / scene_size.x;
            float ratey = (float)(evt.y - btnPos[0].y) / scene_size.y;
            xoffset += ratex;
            yoffset -= ratey;
            btnPos[0] = {evt.x, evt.y};
        }
        if(btnDown[1]) {
            float rate = (float)(evt.y - btnPos[1].y) / scene_size.y;
            angle += 3.1415926f * 0.5f * rate;
            if(angle < 0.0f)
                angle = 0.0f;
            if(angle > 3.1415926f * 0.5f)
                angle = 3.1415926f * 0.5f;
            btnPos[1] = {evt.x, evt.y};
        }
    }
    
    void DuelScene::MouseButtonDown(sgui::MouseButtonEvent evt) {
        if(evt.button < 2) {
            btnDown[evt.button] = true;
            btnPos[evt.button] = {evt.x, evt.y};
        }
    }
    
    void DuelScene::MouseButtonUp(sgui::MouseButtonEvent evt) {
        if(evt.button < 2) {
            btnDown[evt.button] = false;
        }
    }
    
    void DuelScene::MouseWheel(sgui::MouseWheelEvent evt) {
        r += evt.deltay / 30.0f;
        if(r < 1.0f)
            r = 1.0f;
        if(r > 50.0f)
            r = 50.0f;
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
        std::array<v3hct, 4> verts;
        verts[0].vertex = {-1.0f, 1.0f, 0.0f};
        verts[0].texcoord = ti.vert[0];
        verts[1].vertex = {1.0f, 1.0f, 0.0f};
        verts[1].texcoord = ti.vert[1];
        verts[2].vertex = {-1.0f, -1.0f, 0.0f};
        verts[2].texcoord = ti.vert[2];
        verts[3].vertex = {1.0f, -1.0f, 0.0f};
        verts[3].texcoord = ti.vert[3];
        glBindBuffer(GL_ARRAY_BUFFER, back_buffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(v3hct) * verts.size(), &verts[0]);
        GLCheckError(__FILE__, __LINE__);
    }
    
    void DuelScene::UpdateField() {
        if(!update_field)
            return;
        update_field = false;
        auto FillVert3 = [](v3hct* vt, rectf center, ti4& hti, unsigned int cl = 0xffffffff, unsigned int hcl = 0) {
            vt[0].vertex = {center.left - center.width * 0.5f, center.top - center.height * 0.5f, 0.0f};
            vt[0].texcoord = hti.vert[0];
            vt[1].vertex = {center.left + center.width * 0.5f, center.top - center.height * 0.5f, 0.0f};
            vt[1].texcoord = hti.vert[1];
            vt[2].vertex = {center.left - center.width * 0.5f, center.top + center.height * 0.5f, 0.0f};
            vt[2].texcoord = hti.vert[2];
            vt[3].vertex = {center.left + center.width * 0.5f, center.top + center.height * 0.5f, 0.0f};
            vt[3].texcoord = hti.vert[3];
        };
        std::array<v3hct, 136> verts;
        FillVert3(&verts[0 ], SceneMgr::Get().LayoutRectConfig("mzone1"), ImageMgr::Get().GetTexture("numback"));
        FillVert3(&verts[4 ], SceneMgr::Get().LayoutRectConfig("mzone2"), ImageMgr::Get().GetTexture("numback"));
        FillVert3(&verts[8 ], SceneMgr::Get().LayoutRectConfig("mzone3"), ImageMgr::Get().GetTexture("numback"));
        FillVert3(&verts[12], SceneMgr::Get().LayoutRectConfig("mzone4"), ImageMgr::Get().GetTexture("numback"));
        FillVert3(&verts[16], SceneMgr::Get().LayoutRectConfig("mzone5"), ImageMgr::Get().GetTexture("numback"));
        FillVert3(&verts[20], SceneMgr::Get().LayoutRectConfig("szone1"), ImageMgr::Get().GetTexture("numback"));
        FillVert3(&verts[24], SceneMgr::Get().LayoutRectConfig("szone2"), ImageMgr::Get().GetTexture("numback"));
        FillVert3(&verts[28], SceneMgr::Get().LayoutRectConfig("szone3"), ImageMgr::Get().GetTexture("numback"));
        FillVert3(&verts[32], SceneMgr::Get().LayoutRectConfig("szone4"), ImageMgr::Get().GetTexture("numback"));
        FillVert3(&verts[36], SceneMgr::Get().LayoutRectConfig("szone5"), ImageMgr::Get().GetTexture("numback"));
        FillVert3(&verts[40], SceneMgr::Get().LayoutRectConfig("fdzone"), ImageMgr::Get().GetTexture("numback"));
        FillVert3(&verts[44], SceneMgr::Get().LayoutRectConfig("pzonel"), ImageMgr::Get().GetTexture("numback"));
        FillVert3(&verts[48], SceneMgr::Get().LayoutRectConfig("pzoner"), ImageMgr::Get().GetTexture("numback"));
        FillVert3(&verts[52], SceneMgr::Get().LayoutRectConfig("mdeck" ), ImageMgr::Get().GetTexture("numback"));
        FillVert3(&verts[56], SceneMgr::Get().LayoutRectConfig("exdeck"), ImageMgr::Get().GetTexture("numback"));
        FillVert3(&verts[60], SceneMgr::Get().LayoutRectConfig("grave" ), ImageMgr::Get().GetTexture("numback"));
        FillVert3(&verts[64], SceneMgr::Get().LayoutRectConfig("banish"), ImageMgr::Get().GetTexture("numback"));
        for(int i = 68; i < 136; ++i) {
            verts[i] = verts[i - 68];
            verts[i].vertex = {-verts[i - 68].vertex.x, -verts[i - 68].vertex.y, 0.0f};
        }
        glBindBuffer(GL_ARRAY_BUFFER, field_buffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(v3hct) * verts.size(), &verts[0]);
        GLCheckError(__FILE__, __LINE__);
    }
    
    void DuelScene::UpdateMisc() {
        if(!update_misc)
            update_misc = true;
    }
    
}

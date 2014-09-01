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
frag_color = mix(texcolor * color, vec4(hcolor.r, hcolor.g, hcolor.b, 1.0), hcolor.a);\n\
}\n\
";

namespace ygopro
{
    
    DuelScene::DuelScene() {
        glGenBuffers(1, &index_buffer);
        glGenBuffers(1, &card_index_buffer);
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
        index.resize(128 * 6);
        for(int i = 0; i < 128; ++i) {
            index[i * 6] = i * 4;
            index[i * 6 + 1] = i * 4 + 2;
            index[i * 6 + 2] = i * 4 + 1;
            index[i * 6 + 3] = i * 4 + 1;
            index[i * 6 + 4] = i * 4 + 2;
            index[i * 6 + 5] = i * 4 + 3;
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 128 * 6, &index[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, card_index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 512 * 4 * 6, nullptr, GL_STATIC_DRAW);
        GLCheckError(__FILE__, __LINE__);
        glGenVertexArrays(1, &field_vao);
        glGenVertexArrays(1, &back_vao);
        glGenVertexArrays(1, &card_vao);
        glGenVertexArrays(1, &misc_vao);
        GLCheckError(__FILE__, __LINE__);
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
            if(i != 2)
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
            else
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, card_index_buffer);
            glBindVertexArray(0);
        }
        GLCheckError(__FILE__, __LINE__);
        duel_shader.LoadVertShader(vert_shader);
        duel_shader.LoadFragShader(frag_shader);
        duel_shader.Link();
        field.InitBlocks();
        glBindBuffer(GL_ARRAY_BUFFER, field_buffer);
        field.RefreshBlocks();
    }
    
    DuelScene::~DuelScene() {
        glDeleteBuffers(1, &index_buffer);
        glDeleteBuffers(1, &card_index_buffer);
        glDeleteBuffers(1, &back_buffer);
        glDeleteBuffers(1, &field_buffer);
        glDeleteBuffers(1, &card_buffer);
        glDeleteBuffers(1, &misc_buffer);
        glDeleteVertexArrays(1, &back_vao);
        glDeleteVertexArrays(1, &field_vao);
        glDeleteVertexArrays(1, &card_vao);
        glDeleteVertexArrays(1, &misc_vao);
        duel_shader.Unload();
    }
    
    void DuelScene::Activate() {
        
    }
    
    bool DuelScene::Update() {
        //PullEvent();
        do {
            auto cmd = duel_commands.PullCommand();
            if(cmd == nullptr)
                break;
            if(!cmd->Handle())
                break;
            duel_commands.PopCommand();
        } while (duel_commands.IsEmpty());
        UpdateParams();
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
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
        GLCheckError(__FILE__, __LINE__);
        // field
        duel_shader.SetParamMat4("mvp", glm::value_ptr(camera.mvp));
        ImageMgr::Get().GetRawMiscTexture()->Bind();
        glBindVertexArray(field_vao);
        glDrawElements(GL_TRIANGLES, 34 * 6, GL_UNSIGNED_SHORT, 0);
        // end
        glBindVertexArray(back_vao);
        duel_shader.Unuse();
    }
    
    void DuelScene::SetSceneSize(v2i sz) {
        scene_size = sz;
        UpdateParams();
    }
    
    recti DuelScene::GetScreenshotClip() {
        return {0, 0, scene_size.x, scene_size.y};
    }
    
    void DuelScene::MouseMove(sgui::MouseMoveEvent evt) {
        bool update_param = false;
        if(btnDown[0]) {
            float ratex = (float)(evt.x - btnPos[0].x) / scene_size.x;
            float ratey = (float)(evt.y - btnPos[0].y) / scene_size.y;
            camera.xoffset += ratex;
            camera.yoffset -= ratey;
            btnPos[0] = {evt.x, evt.y};
            update_param = true;
        }
        if(btnDown[1]) {
            float rate = (float)(evt.y - btnPos[1].y) / scene_size.y;
            camera.angle += 3.1415926f * 0.5f * rate;
            if(camera.angle < 0.0f)
                camera.angle = 0.0f;
            if(camera.angle > 3.1415926f * 0.5f)
                camera.angle = 3.1415926f * 0.5f;
            btnPos[1] = {evt.x, evt.y};
            update_param = true;
        }
        if(update_param)
            UpdateParams();
        auto pre_obj = hover_obj.lock();
        std::shared_ptr<FieldObject> obj = nullptr;
        auto hp = GetHoverPos(evt.x, evt.y);
        if(hp.first != 0)
            obj = field.field_blocks[hp.first - 1][hp.second];
        if(pre_obj != obj) {
            if(pre_obj)
                pre_obj->hl.Reset(0.0f);
            if(obj) {
                obj->hl.SetAnimator(std::make_shared<LerpAnimator<float, TGenPeriodicRet>>(0.2f, 0.8f, SceneMgr::Get().GetGameTime(), 1.0));
                if(!obj->updating) {
                    updating_blocks.push_back(obj);
                    obj->updating = true;
                }
            }
            hover_obj = obj;
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
        camera.radius += evt.deltay / 30.0f;
        if(camera.radius < 1.0f)
            camera.radius = 1.0f;
        if(camera.radius > 50.0f)
            camera.radius = 50.0f;
        UpdateParams();
    }
    
    void DuelScene::KeyDown(sgui::KeyEvent evt) {
        
    }
    
    void DuelScene::KeyUp(sgui::KeyEvent evt) {
        
    }
    
    void DuelScene::UpdateParams() {
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, -camera.radius * cosf(camera.angle), camera.radius * sinf(camera.angle)),
                                     glm::vec3(0.0f, 0.0f, 0.0f),
                                     glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 projection = glm::perspective(camera.fovy, 1.0f * scene_size.x / scene_size.y, camera.cnear, camera.cfar);
        glm::mat4 trscreen;
        trscreen[3][0] = camera.xoffset;
        trscreen[3][1] = camera.yoffset;
        camera.mvp = trscreen * projection * view;
        camera.cameray = camera.radius * cosf(camera.angle);
        camera.cameraz = camera.radius * sinf(camera.angle);
        camera.scry = 2.0f * tanf(camera.fovy * 0.5f) * camera.cnear;
        camera.scrx = camera.scry * scene_size.x / scene_size.y;
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
        double tm = SceneMgr::Get().GetGameTime();
        glBindBuffer(GL_ARRAY_BUFFER, field_buffer);
        for(auto iter = updating_blocks.begin(); iter != updating_blocks.end();) {
            auto cur = iter++;
            auto ptr = (*cur).lock();
            if(ptr == nullptr) {
                updating_blocks.erase(cur);
                continue;
            }
            if(!ptr->UpdateVertices(tm)) {
                ptr->updating = false;
                updating_blocks.erase(cur);
            }
        }
        GLCheckError(__FILE__, __LINE__);
    }
    
    void DuelScene::UpdateMisc() {
        if(!update_misc)
            update_misc = true;
    }
    
    std::pair<int, int> DuelScene::GetHoverPos(int posx, int posy) {
        float x = (float)posx / scene_size.x * 2.0f - 1.0f - camera.xoffset;
        float y = 1.0f - (float)posy / scene_size.y * 2.0f - camera.yoffset;
        float projx = camera.scrx * 0.5f * x;
        float projy = camera.scry * 0.5f * y;
        float k = tanf(3.1415926f - camera.angle + atanf(projy / camera.cnear));
        float py = -camera.cameray - camera.cameraz / k;
        float nearx = sqrtf(camera.cnear * camera.cnear + projy * projy);
        float radiusx = sqrtf(camera.cameraz * camera.cameraz + (camera.cameray + py) * (camera.cameray + py));
        float px = projx * radiusx / nearx;
        auto hb = field.CheckHoverBlock(px, py);
        if(hb.first != 0)
            return hb;
        return std::make_pair(0, 0);
    }
}

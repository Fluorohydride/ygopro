#include "../common/common.h"

#include "gui_extra.h"
#include "duel_command.h"
#include "image_mgr.h"

#include "duel_scene.h"

namespace ygopro
{
    
    void FieldBlock::RefreshVertices() {
        std::array<glbase::v3hct, 4> vert;
        vert[0].vertex = vertex[0];
        vert[0].texcoord = texcoord[0];
        vert[1].vertex = vertex[1];
        vert[1].texcoord = texcoord[1];
        vert[2].vertex = vertex[2];
        vert[2].texcoord = texcoord[2];
        vert[3].vertex = vertex[3];
        vert[3].texcoord = texcoord[3];
        for(int i = 0; i < 4; ++i) {
            vert[i].color = ((int)(alpha.Get() * 255) << 24) | 0xffffff;
            vert[i].hcolor = ((int)(hl.Get() * 255) << 24) | hlcolor;
        }
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(glbase::v3hct) * vertex_index * 4, sizeof(glbase::v3hct) * 4, &vert[0]);
    }
    
    bool FieldBlock::UpdateVertices(double tm) {
        alpha.Update(tm);
        hl.Update(tm);
        RefreshVertices();
        return alpha.NeedUpdate() || hl.NeedUpdate();
    }
    
    void FieldBlock::Init(unsigned int idx, rectf center, ti4 ti) {
        vertex_index = idx;
        translation = {center.left, center.top, 0.0f};
        vertex.push_back({center.left - center.width * 0.5f, center.top + center.height * 0.5f, 0.0f});
        vertex.push_back({center.left + center.width * 0.5f, center.top + center.height * 0.5f, 0.0f});
        vertex.push_back({center.left - center.width * 0.5f, center.top - center.height * 0.5f, 0.0f});
        vertex.push_back({center.left + center.width * 0.5f, center.top - center.height * 0.5f, 0.0f});
        texcoord.push_back(ti.vert[0]);
        texcoord.push_back(ti.vert[1]);
        texcoord.push_back(ti.vert[2]);
        texcoord.push_back(ti.vert[3]);
        alpha = 1.0f;
        hl = 0.0f;
    }
    
    bool FieldBlock::CheckInside(float px, float py) {
        bool cx = (vertex[0].x < vertex[1].x) ? (vertex[0].x <= px && vertex[1].x >= px) : (vertex[0].x >= px && vertex[1].x <= px);
        bool cy = (vertex[0].y < vertex[2].y) ? (vertex[0].y <= py && vertex[2].y >= py) : (vertex[0].y >= py && vertex[2].y <= py);
        return cx && cy;
    }
    
    void FieldCard::RefreshVertices() {
        std::array<glbase::v3hct, 12> vert;
        if(update_rvert) {
            vertex_r.clear();
            auto q = rotation.Get();
            for(auto& iter : vertex) {
                glm::vec3 v(iter.x, iter.y + dy, iter.z);
                auto c = q * v;
                vertex_r.push_back({c.x, c.y, c.z});
            }
            update_rvert = false;
        }
        unsigned int cl = ((unsigned int)(alpha.Get() * 255) << 24) | 0xffffff;
        unsigned int hcl = ((unsigned int)(hl.Get() * 255) << 24) | hlcolor;
        auto& tl = translation.Get();
        for(size_t i = 0; i < 12; ++i) {
            vert[i].vertex = vertex_r[i] + tl;
            vert[i].texcoord = texcoord[i];
            vert[i].color = cl;
            vert[i].hcolor = hcl;
        }
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(glbase::v3hct) * vertex_index * 12, sizeof(glbase::v3hct) * 12, &vert[0]);
    }
    
    bool FieldCard::UpdateVertices(double tm) {
        alpha.Update(tm);
        hl.Update(tm);
        translation.Update(tm);
        if(dy.NeedUpdate()) {
            dy.Update(tm);
            update_rvert = true;
        }
        if(rotation.NeedUpdate()) {
            rotation.Update(tm);
            update_rvert = true;
        }
        RefreshVertices();
        return translation.NeedUpdate() || rotation.NeedUpdate() || dy.NeedUpdate() || alpha.NeedUpdate() || hl.NeedUpdate();
    }
    
    void FieldCard::Init(unsigned int idx, unsigned int code, int side) {
        rectf center = SceneMgr::Get().LayoutRectConfig("card");
        vertex_index = idx;
        this->code = code;
        vertex.resize(12);
        texcoord.resize(12);
        // front
        vertex[0] = {center.left - center.width * 0.5f, center.top + center.height * 0.5f, 0.0f};
        vertex[1] = {center.left + center.width * 0.5f, center.top + center.height * 0.5f, 0.0f};
        vertex[2] = {center.left - center.width * 0.5f, center.top - center.height * 0.5f, 0.0f};
        vertex[3] = {center.left + center.width * 0.5f, center.top - center.height * 0.5f, 0.0f};
        auto ti = code ? ImageMgr::Get().GetCardTexture(code) : ImageMgr::Get().GetTexture("unknown");
        texcoord[0] = ti.vert[0];
        texcoord[1] = ti.vert[1];
        texcoord[2] = ti.vert[2];
        texcoord[3] = ti.vert[3];
        // back
        vertex[4] = {center.left + center.width * 0.5f, center.top + center.height * 0.5f, 0.0f};
        vertex[5] = {center.left - center.width * 0.5f, center.top + center.height * 0.5f, 0.0f};
        vertex[6] = {center.left + center.width * 0.5f, center.top - center.height * 0.5f, 0.0f};
        vertex[7] = {center.left - center.width * 0.5f, center.top - center.height * 0.5f, 0.0f};
        auto sleeve = ImageMgr::Get().GetTexture((side == 0) ? "sleeve1" : "sleeve2");
        texcoord[4] = sleeve.vert[0];
        texcoord[5] = sleeve.vert[1];
        texcoord[6] = sleeve.vert[2];
        texcoord[7] = sleeve.vert[3];
        // icon
        for(int i = 8; i < 12; ++i) {
            vertex[i] = {0.0f, 0.0f, 0.0f};
            texcoord[i] = {0.0f, 0.0f};
        }
        alpha = 1.0f;
        hl = 0.0f;
    }
    
    void FieldCard::SetCode(unsigned int code, bool refresh) {
        if(this->code == code)
            return;
        ImageMgr::Get().UnloadCardTexture(this->code);
        this->code = code;
        auto ti = code ? ImageMgr::Get().GetCardTexture(code) : ImageMgr::Get().GetTexture("unknown");
        for(int i = 0; i < 4; ++i)
            texcoord[i] = ti.vert[i];
        if(refresh)
            RefreshVertices();
    }
    
    void FieldCard::SetIconTex(int iid, bool refresh) {
        if(iid == 0) {
            for(int i = 8; i < 12; ++i) {
                vertex[i] = {0.0f, 0.0f, 0.0f};
                texcoord[i] = {0.0f, 0.0f};
            }
        } else {
            ti4 ti;
            if(iid == 1)
                ti = ImageMgr::Get().GetTexture("negated");
            else if(iid == 2)
                ti = ImageMgr::Get().GetTexture("equip");
            else if(iid == 3)
                ti = ImageMgr::Get().GetTexture("target");
            rectf icon = SceneMgr::Get().LayoutRectConfig("icon");
            vertex[8 ] = {icon.left - icon.width * 0.5f, icon.top - icon.height * 0.5f, 0.0f};
            vertex[9 ] = {icon.left + icon.width * 0.5f, icon.top - icon.height * 0.5f, 0.0f};
            vertex[10] = {icon.left - icon.width * 0.5f, icon.top + icon.height * 0.5f, 0.0f};
            vertex[11] = {icon.left + icon.width * 0.5f, icon.top + icon.height * 0.5f, 0.0f};
            texcoord[8 ] = ti.vert[0];
            texcoord[9 ] = ti.vert[1];
            texcoord[10] = ti.vert[2];
            texcoord[11] = ti.vert[3];
        }
        if(refresh)
            RefreshVertices();
    }
    
    DuelScene::DuelScene() {
        glGenBuffers(1, &index_buffer);
        glGenBuffers(1, &card_index_buffer);
        glGenBuffers(1, &back_buffer);
        glGenBuffers(1, &field_buffer);
        glGenBuffers(1, &card_buffer);
        glGenBuffers(1, &misc_buffer);
        GLCheckError(__FILE__, __LINE__);
        glBindBuffer(GL_ARRAY_BUFFER, back_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v3hct) * 4, nullptr, GL_DYNAMIC_DRAW);
        GLCheckError(__FILE__, __LINE__);
        glBindBuffer(GL_ARRAY_BUFFER, field_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v3hct) * 36 * 4, nullptr, GL_DYNAMIC_DRAW);
        GLCheckError(__FILE__, __LINE__);
        glBindBuffer(GL_ARRAY_BUFFER, card_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v3hct) * 512 * 16, nullptr, GL_DYNAMIC_DRAW);
        GLCheckError(__FILE__, __LINE__);
        glBindBuffer(GL_ARRAY_BUFFER, misc_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v3hct) * 32 * 4, nullptr, GL_DYNAMIC_DRAW);
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
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 512 * 3 * 6, nullptr, GL_DYNAMIC_DRAW);
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
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glbase::v3hct), 0);
            glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(glbase::v3hct), (const GLvoid*)glbase::v3hct::color_offset);
            glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(glbase::v3hct), (const GLvoid*)glbase::v3hct::hcolor_offset);
            glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v3hct), (const GLvoid*)glbase::v3hct::tex_offset);
            if(i != 2)
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
            else
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, card_index_buffer);
            glBindVertexArray(0);
        }
        GLCheckError(__FILE__, __LINE__);
        TextFile vertf("./conf/vert.shader");
        TextFile fragf("./conf/frag.shader");
        duel_shader.LoadVertShader(vertf.Data());
        duel_shader.LoadFragShader(fragf.Data());
        duel_shader.Link();
        InitField();
        glBindBuffer(GL_ARRAY_BUFFER, field_buffer);
        RefreshBlocks();
        GLCheckError(__FILE__, __LINE__);
        
        vparam.fovy = SceneMgr::Get().LayoutFloatConfig("fovy");
        vparam.cnear = SceneMgr::Get().LayoutFloatConfig("near");
        vparam.cfar = SceneMgr::Get().LayoutFloatConfig("far");
        vparam.angle = SceneMgr::Get().LayoutFloatConfig("angle");
        vparam.radius = SceneMgr::Get().LayoutFloatConfig("radius");
        vparam.xoffset = SceneMgr::Get().LayoutFloatConfig("xoffset");
        vparam.yoffset = SceneMgr::Get().LayoutFloatConfig("yoffset");
        vparam.cardrect = SceneMgr::Get().LayoutRectConfig("card");
        vparam.handmin = SceneMgr::Get().LayoutFloatConfig("handmin");
        vparam.handmax = SceneMgr::Get().LayoutFloatConfig("handmax");
        vparam.handy[0] = SceneMgr::Get().LayoutFloatConfig("handy1");
        vparam.handy[1] = SceneMgr::Get().LayoutFloatConfig("handy2");
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
            if(!cmd->Handle(this))
                break;
            duel_commands.PopCommand();
        } while (duel_commands.IsEmpty());
        UpdateBackground();
        UpdateRegion();
        UpdateField();
        UpdateMisc();
        UpdateIndex();
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
        duel_shader.SetParamMat4("mvp", glm::value_ptr(vparam.mvp));
        ImageMgr::Get().GetRawMiscTexture()->Bind();
        glBindVertexArray(field_vao);
        glDrawElements(GL_TRIANGLES, 34 * 6, GL_UNSIGNED_SHORT, 0);
        GLCheckError(__FILE__, __LINE__);
        // card
        if(alloc_cards.size()) {
            ImageMgr::Get().GetRawCardTexture()->Bind();
            glBindVertexArray(card_vao);
            glDrawElements(GL_TRIANGLES, alloc_cards.size() * 18, GL_UNSIGNED_SHORT, 0);
            GLCheckError(__FILE__, __LINE__);
        }
        // misc
//        duel_shader.SetParamMat4("mvp", glm::value_ptr(glm::mat4(1.0f)));
//        ImageMgr::Get().GetRawMiscTexture()->Bind();
//        glBindVertexArray(misc_vao);
//        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_SHORT, 0);
        GLCheckError(__FILE__, __LINE__);
        // end
        glBindVertexArray(0);
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
            float ratex = (float)(evt.x - btnPos[0].x) / scene_size.x * 2.0f;
            float ratey = (float)(evt.y - btnPos[0].y) / scene_size.y * 2.0f;
            vparam.xoffset += ratex;
            vparam.yoffset -= ratey;
            btnPos[0] = {evt.x, evt.y};
            update_param = true;
        }
        if(btnDown[1]) {
            float rate = (float)(evt.y - btnPos[1].y) / scene_size.y;
            vparam.angle += 3.1415926f * 0.5f * rate;
            if(vparam.angle < 0.0f)
                vparam.angle = 0.0f;
            if(vparam.angle > 3.1415926f * 0.5f)
                vparam.angle = 3.1415926f * 0.5f;
            btnPos[1] = {evt.x, evt.y};
            update_param = true;
        }
        if(update_param)
            UpdateParams();
        auto pblock = pre_block.lock();
        auto pcard = pre_card.lock();
        std::shared_ptr<FieldBlock> hover_block;
        std::shared_ptr<FieldCard> hover_card;
        auto hp = GetHoverPos(evt.x, evt.y);
        if(hp.x == 1 || hp.x == 2)
            if(hp.y < 17)
                hover_block = field_blocks[hp.x - 1][hp.y];
        if(hp.x == 3 || hp.x == 4)
            if(hp.y < hand[hp.x - 3].size())
                hover_card = hand[hp.x - 3][hp.y];
        if(pblock != hover_block) {
            if(pblock)
                pblock->hl.Reset(0.0f);
            if(hover_block) {
                hover_block->hl.SetAnimator(std::make_shared<LerpAnimator<float, TGenPeriodicRet>>(0.2f, 0.8f, SceneMgr::Get().GetGameTime(), 1.0));
                if(!hover_block->updating) {
                    updating_blocks.push_back(hover_block);
                    hover_block->updating = true;
                }
            }
            pre_block = hover_block;
        }
        if(pcard != hover_card) {
            if(pcard) {
                pcard->dy.SetAnimator(std::make_shared<LerpAnimator<float, TGenMove>>(pcard->dy.Get(), 0.0f, SceneMgr::Get().GetGameTime(), 0.5, 10));
                AddUpdateCard(pcard);
            }
            if(hover_card) {
                hover_card->dy.SetAnimator(std::make_shared<LerpAnimator<float, TGenMove>>(0.0f, 0.2f, SceneMgr::Get().GetGameTime(), 0.5, 10));
                AddUpdateCard(hover_card);
            }
            pre_card = hover_card;
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
        duel_commands.PushCommand(std::make_shared<DuelCommandMove>(0, 0));
    }
    
    void DuelScene::MouseWheel(sgui::MouseWheelEvent evt) {
        vparam.radius += evt.deltay / 30.0f;
        if(vparam.radius < 1.0f)
            vparam.radius = 1.0f;
        if(vparam.radius > 50.0f)
            vparam.radius = 50.0f;
        UpdateParams();
    }
    
    void DuelScene::UpdateHandRect() {
        float hty = glm::abs(vparam.cardrect.height * 0.5f) * glm::sin(vparam.angle);
        float htz = glm::abs(vparam.cardrect.height * 0.5f) * glm::cos(vparam.angle);
        glm::vec4 vlt = vparam.mvp * glm::vec4(vparam.handmin, vparam.handy[0] + hty, htz, 1.0f);
        glm::vec4 vltw = vparam.mvp * glm::vec4(vparam.handmin + vparam.cardrect.width, vparam.handy[0] + hty, htz, 1.0f);
        glm::vec4 vrb = vparam.mvp * glm::vec4(vparam.handmax, vparam.handy[0] - hty, -htz, 1.0f);
        glm::vec4 vlt2 = vparam.mvp * glm::vec4(-vparam.handmin, vparam.handy[1] + hty, htz, 1.0f);
        glm::vec4 vltw2 = vparam.mvp * glm::vec4(-vparam.handmin - vparam.cardrect.width, vparam.handy[1] + hty, htz, 1.0f);
        glm::vec4 vrb2 = vparam.mvp * glm::vec4(-vparam.handmax, vparam.handy[1] - hty, -htz, 1.0f);
        vlt /= vlt.w;
        vltw /= vltw.w;
        vrb /= vrb.w;
        vlt2 /= vlt2.w;
        vltw2 /= vltw2.w;
        vrb2 /= vrb2.w;
        vparam.hand_rect[0] = {vlt.x, vlt.y, std::abs(vrb.x - vlt.x), std::abs(vrb.y - vlt.y)};
        vparam.hand_rect[1] = {vlt2.x, vlt2.y, std::abs(vrb2.x - vlt2.x), std::abs(vrb2.y - vlt2.y)};
        vparam.hand_width[0] = std::abs(vltw.x - vlt.x);
        vparam.hand_width[1] = std::abs(vltw2.x - vlt2.x);
        vparam.hand_quat[0] = glm::angleAxis(3.1415926f * 0.5f - vparam.angle, glm::vec3(1.0f, 0.0f, 0.0f));
        vparam.hand_quat[1] = vparam.hand_quat[0] * glm::angleAxis(3.1415926f, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    
    void DuelScene::KeyDown(sgui::KeyEvent evt) {
        
    }
    
    void DuelScene::KeyUp(sgui::KeyEvent evt) {
        
    }
    
    void DuelScene::UpdateParams() {
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, -vparam.radius * glm::cos(vparam.angle), vparam.radius * glm::sin(vparam.angle)),
                                     glm::vec3(0.0f, 0.0f, 0.0f),
                                     glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 projection = glm::perspective(vparam.fovy, 1.0f * scene_size.x / scene_size.y, vparam.cnear, vparam.cfar);
        glm::mat4 trscreen;
        trscreen[3][0] = vparam.xoffset;
        trscreen[3][1] = vparam.yoffset;
        vparam.mvp = trscreen * projection * view;
        vparam.cameray = vparam.radius * glm::cos(vparam.angle);
        vparam.cameraz = vparam.radius * glm::sin(vparam.angle);
        vparam.scry = 2.0f * tanf(vparam.fovy * 0.5f) * vparam.cnear;
        vparam.scrx = vparam.scry * scene_size.x / scene_size.y;
        UpdateHandRect();
        RefreshHand(0);
        RefreshHand(1);
    }
    
    void DuelScene::UpdateBackground() {
        if(!update_bg)
            return;
        update_bg = false;
        auto ti = ImageMgr::Get().GetTexture("bg");
        std::array<glbase::v3hct, 4> verts;
        verts[0].vertex = {-1.0f, 1.0f, 0.0f};
        verts[0].texcoord = ti.vert[0];
        verts[1].vertex = {1.0f, 1.0f, 0.0f};
        verts[1].texcoord = ti.vert[1];
        verts[2].vertex = {-1.0f, -1.0f, 0.0f};
        verts[2].texcoord = ti.vert[2];
        verts[3].vertex = {1.0f, -1.0f, 0.0f};
        verts[3].texcoord = ti.vert[3];
        glBindBuffer(GL_ARRAY_BUFFER, back_buffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v3hct) * verts.size(), &verts[0]);
        GLCheckError(__FILE__, __LINE__);
    }
    
    void DuelScene::UpdateRegion() {
        for(int side = 0; side < 1; ++side) {
            unsigned int region = (refresh_region >> (side * 16)) & 0xffff;
            if(region & 0x1)
                for(auto& iter : deck[side])
                    RefreshPos(iter, false, 0.5);
            if(region & 0x2)
                RefreshHand(side, false, 0.5);
            if(region & 0x4)
                for(auto& iter : grave[side])
                    RefreshPos(iter, false, 0.5);
            if(region & 0x8)
                for(auto& iter : banished[side])
                    RefreshPos(iter, false, 0.5);
            if(region & 0x10)
                for(auto& iter : extra[side])
                    RefreshPos(iter, false, 0.5);
            for(int seq = 0; seq < 5; ++seq) {
                if(region & (0x20 << seq)) {
                    auto pcard = m_zone[side][seq];
                    if(pcard) {
                        for(auto& iter : pcard->olcards)
                            RefreshPos(iter, false, 0.5);
                        RefreshPos(pcard);
                    }
                }
            }
        }
        refresh_region = 0;
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
        glBindBuffer(GL_ARRAY_BUFFER, card_buffer);
        for(auto iter = updating_cards.begin(); iter != updating_cards.end();) {
            auto cur = iter++;
            auto ptr = (*cur).lock();
            if(ptr == nullptr) {
                updating_cards.erase(cur);
                continue;
            }
            if(!ptr->UpdateVertices(tm)) {
                ptr->updating = false;
                updating_cards.erase(cur);
            }
        }
        GLCheckError(__FILE__, __LINE__);
    }
    
    void DuelScene::UpdateMisc() {
        if(!update_misc)
            return;
        update_misc = false;
        for(int i = 0; i < 5; ++i) {
            RefreshPos(AddCard((i % 2) ? 83764718 : 0, 0, 0x1, i, 1));
            RefreshPos(AddCard((i % 2) ? 83764718 : 0, 1, 0x2, i, 1));
        }
        RefreshHand(0);
        RefreshHand(1);
        update_index = true;
    }
    
    void DuelScene::UpdateIndex() {
        if(!update_index)
            return;
        update_index = false;
        std::vector<unsigned short> index;
        index.resize(alloc_cards.size() * 18);
        unsigned short istart = 0;
        auto push_index = [&index, &istart](std::vector<std::shared_ptr<FieldCard>>& vec) {
            for(auto& iter : vec) {
                if(iter == nullptr)
                    continue;
                unsigned int vstart = iter->vertex_index * 12;
                for(int i = 0; i < 3; ++i) {
                    index[istart + i * 6 + 0] = vstart + i * 4 + 0;
                    index[istart + i * 6 + 1] = vstart + i * 4 + 2;
                    index[istart + i * 6 + 2] = vstart + i * 4 + 1;
                    index[istart + i * 6 + 3] = vstart + i * 4 + 1;
                    index[istart + i * 6 + 4] = vstart + i * 4 + 2;
                    index[istart + i * 6 + 5] = vstart + i * 4 + 3;
                }
                istart += 18;
            }
        };
        push_index(hand[1]);
        push_index(deck[1]);
        push_index(extra[1]);
        push_index(s_zone[1]);
        for(auto& iter : m_zone[1]) {
            if(iter != nullptr)
                push_index(iter->olcards);
        }
        push_index(m_zone[1]);
        push_index(grave[1]);
        push_index(banished[1]);
        
        push_index(grave[0]);
        push_index(banished[0]);
        for(auto& iter : m_zone[0]) {
            if(iter != nullptr)
                push_index(iter->olcards);
        }
        push_index(m_zone[0]);
        push_index(s_zone[0]);
        push_index(deck[0]);
        push_index(extra[0]);
        push_index(hand[0]);
        if(index.size() > 0) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, card_index_buffer);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned short) * index.size(), &index[0]);
            GLCheckError(__FILE__, __LINE__);
        }
    }
    
    void DuelScene::InitField() {
        for(int i = 0 ; i < 17; ++i) {
            field_blocks[0][i] = std::make_shared<FieldBlock>();
            field_blocks[1][i] = std::make_shared<FieldBlock>();
        }
        field_blocks[0][0 ]->Init(0 , SceneMgr::Get().LayoutRectConfig("mzone1"), ImageMgr::Get().GetTexture("mzone"));
        field_blocks[0][1 ]->Init(1 , SceneMgr::Get().LayoutRectConfig("mzone2"), ImageMgr::Get().GetTexture("mzone"));
        field_blocks[0][2 ]->Init(2 , SceneMgr::Get().LayoutRectConfig("mzone3"), ImageMgr::Get().GetTexture("mzone"));
        field_blocks[0][3 ]->Init(3 , SceneMgr::Get().LayoutRectConfig("mzone4"), ImageMgr::Get().GetTexture("mzone"));
        field_blocks[0][4 ]->Init(4 , SceneMgr::Get().LayoutRectConfig("mzone5"), ImageMgr::Get().GetTexture("mzone"));
        field_blocks[0][5 ]->Init(5 , SceneMgr::Get().LayoutRectConfig("szone1"), ImageMgr::Get().GetTexture("szone"));
        field_blocks[0][6 ]->Init(6 , SceneMgr::Get().LayoutRectConfig("szone2"), ImageMgr::Get().GetTexture("szone"));
        field_blocks[0][7 ]->Init(7 , SceneMgr::Get().LayoutRectConfig("szone3"), ImageMgr::Get().GetTexture("szone"));
        field_blocks[0][8 ]->Init(8 , SceneMgr::Get().LayoutRectConfig("szone4"), ImageMgr::Get().GetTexture("szone"));
        field_blocks[0][9 ]->Init(9 , SceneMgr::Get().LayoutRectConfig("szone5"), ImageMgr::Get().GetTexture("szone"));
        field_blocks[0][10]->Init(10, SceneMgr::Get().LayoutRectConfig("fdzone"), ImageMgr::Get().GetTexture("fdzone"));
        field_blocks[0][11]->Init(11, SceneMgr::Get().LayoutRectConfig("pzonel"), ImageMgr::Get().GetTexture("pzonel"));
        field_blocks[0][12]->Init(12, SceneMgr::Get().LayoutRectConfig("pzoner"), ImageMgr::Get().GetTexture("pzoner"));
        field_blocks[0][13]->Init(13, SceneMgr::Get().LayoutRectConfig("mdeck" ), ImageMgr::Get().GetTexture("mdeck"));
        field_blocks[0][14]->Init(14, SceneMgr::Get().LayoutRectConfig("exdeck"), ImageMgr::Get().GetTexture("exdeck"));
        field_blocks[0][15]->Init(15, SceneMgr::Get().LayoutRectConfig("grave" ), ImageMgr::Get().GetTexture("grave"));
        field_blocks[0][16]->Init(16, SceneMgr::Get().LayoutRectConfig("banish"), ImageMgr::Get().GetTexture("banish"));
        for(int i = 0; i < 17; ++i) {
            *field_blocks[1][i] = *field_blocks[0][i];
            field_blocks[1][i]->translation = field_blocks[0][i]->translation * -1;
            for(auto& vert : field_blocks[1][i]->vertex)
                vert = vert * -1;
            field_blocks[1][i]->vertex_index = field_blocks[0][i]->vertex_index + 17;
        }
        m_zone[0].resize(5);
        m_zone[1].resize(5);
        s_zone[0].resize(8);
        s_zone[1].resize(8);
    }
    
    void DuelScene::RefreshBlocks() {
        for(int i = 0 ; i < 17; ++i) {
            field_blocks[0][i]->RefreshVertices();
            field_blocks[1][i]->RefreshVertices();
        }
    }
    
    std::shared_ptr<FieldCard> DuelScene::AddCard(unsigned int code, int side, int zone, int seq, int subs) {
        if(side > 1)
            return nullptr;
        if(zone & 0x4) {
            if(seq > 4)
                return nullptr;
            auto pre = m_zone[side][seq];
            if(pre != nullptr && zone == 0x4)
                return nullptr;
            if(pre == nullptr && (zone & 0x80))
                return nullptr;
        }
        if(zone & 0x8) {
            if(seq > 7)
                return nullptr;
            auto pre = s_zone[side][seq];
            if(pre != nullptr)
                return nullptr;
        }
        auto ptr = std::make_shared<FieldCard>();
        ptr->Init(alloc_cards.size(), code, side);
        alloc_cards.push_back(ptr);
        ptr->side = side;
        ptr->loc = zone;
        ptr->seq = seq;
        ptr->pos = subs;
        switch(zone & 0x7f) {
            case 0x1:
                ptr->seq = deck[side].size();
                deck[side].push_back(ptr);
                break;
            case 0x2:
                ptr->seq = hand[side].size();
                hand[side].push_back(ptr);
                break;
            case 0x4:
                if(zone & 0x80) {
                    auto pre = m_zone[side][seq];
                    pre->olcards.push_back(ptr);
                } else
                    m_zone[side][seq] = ptr;
                break;
            case 0x8:
                s_zone[side][seq] = ptr;
                break;
            case 0x10:
                ptr->seq = grave[side].size();
                grave[side].push_back(ptr);
                break;
            case 0x20:
                ptr->seq = banished[side].size();
                banished[side].push_back(ptr);
                break;
            case 0x40:
                ptr->seq = extra[side].size();
                extra[side].push_back(ptr);
                break;
        }
        return ptr;
    }
    
    std::shared_ptr<FieldCard> DuelScene::GetCard(int side, int zone, int seq, int subs) {
        switch(zone & 0x7f) {
            case 0x1:
                return deck[side][seq];
            case 0x2:
                return hand[side][seq];
            case 0x4:
                if(zone & 0x80) {
                    auto pcard = m_zone[side][seq];
                    return pcard->olcards[subs];
                } else
                    return m_zone[side][seq];
            case 0x8:
                return s_zone[side][seq];
            case 0x10:
                return grave[side][seq];
            case 0x20:
                return banished[side][seq];
            case 0x40:
                return extra[side][seq];
        }
        return nullptr;
    }
    
    std::shared_ptr<FieldCard> DuelScene::RemoveCard(int side, int zone, int seq, int subs) {
        std::shared_ptr<FieldCard> ret;
        switch(zone & 0x7f) {
            case 0x1:
                ret = deck[side][seq];
                deck[side].erase(deck[side].begin() + seq);
                if(seq != deck[side].size()) {
                    unsigned int index = 0;
                    for(auto& iter : deck[side])
                        iter->seq = index++;
                    refresh_region |= 0x1 << (side * 16);
                }
                break;
            case 0x2:
                ret = hand[side][seq];
                hand[side].erase(hand[side].begin() + seq);
                ret->dy.SetAnimator(std::make_shared<LerpAnimator<float, TGenMove>>(ret->dy.Get(), 0.0f, SceneMgr::Get().GetGameTime(), 0.5, 10));
                {
                    unsigned int index = 0;
                    for(auto& iter : hand[side])
                        iter->seq = index++;
                    refresh_region |= 0x2 << (side * 16);
                }
                break;
            case 0x4:
                if(zone & 0x80) {
                    auto pcard = m_zone[side][seq];
                    ret = pcard->olcards[subs];
                    pcard->olcards.erase(pcard->olcards.begin() + subs);
                    unsigned int index = 0;
                    for(auto& iter : pcard->olcards)
                        iter->seq = index++;
                    refresh_region |= 0x20 << (side * 16 + seq);
                } else {
                    ret = m_zone[side][seq];
                    m_zone[side][seq] = nullptr;
                }
                break;
            case 0x8:
                ret = s_zone[side][seq];
                s_zone[side][seq] = nullptr;
                break;
            case 0x10:
                ret = grave[side][seq];
                grave[side].erase(grave[side].begin() + seq);
                if(seq != grave[side].size()) {
                    unsigned int index = 0;
                    for(auto& iter : grave[side])
                        iter->seq = index++;
                    refresh_region |= 0x4 << (side * 16);
                }
                break;
            case 0x20:
                ret = banished[side][seq];
                banished[side].erase(banished[side].begin() + seq);
                if(seq != banished[side].size()) {
                    unsigned int index = 0;
                    for(auto& iter : banished[side])
                        iter->seq = index++;
                    refresh_region |= 0x8 << (side * 16);
                }
                break;
            case 0x40:
                ret = extra[side][seq];
                extra[side].erase(extra[side].begin() + seq);
                if(seq != extra[side].size()) {
                    unsigned int index = 0;
                    for(auto& iter : extra[side])
                        iter->seq = index++;
                    refresh_region |= 0x10 << (side * 16);
                }
                break;
        }
        if(ret != nullptr)
            update_index = true;
        return ret;
    }
    
    void DuelScene::RefreshPos(std::shared_ptr<FieldCard> pcard, bool update, float tm) {
        // POS_FACEUP_ATTACK		0x1
        // POS_FACEDOWN_ATTACK		0x2
        // POS_FACEUP_DEFENCE		0x4
        // POS_FACEDOWN_DEFENCE     0x8
        v3f tl;
        glm::quat rot;
        unsigned int side = pcard->side;
        unsigned int seq = pcard->seq;
        unsigned int subs = pcard->pos;
        switch(pcard->loc & 0x7f) {
            case 0x1: {
                tl = field_blocks[side][13]->translation;
                tl.z = 0.001f * subs;
                if(subs & 0x5) {
                    if(side == 0)
                        rot = glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
                    else
                        rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 0.0f, 1.0f));
                } else if(subs & 0xa) {
                    if(side == 0)
                        rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 1.0f, 0.0f));
                    else
                        rot = glm::angleAxis(3.1415926f, glm::vec3(1.0f, 0.0f, 0.0f));
                }
            }
                break;
            case 0x2: {
                int ct = hand[side].size();
                if(ct == 0)
                    return;
                if(side == 0) {
                    float wmax = vparam.handmax - vparam.handmin;
                    if(ct * vparam.cardrect.width + (ct - 1) * vparam.cardrect.width * 0.1f >= wmax) {
                        float wcard = (wmax - vparam.cardrect.width) / (ct - 1);
                        tl = {vparam.handmin + wcard * seq + vparam.cardrect.width * 0.5f, vparam.handy[0], 0.0f};
                    } else {
                        float whand = ct * vparam.cardrect.width + (ct - 1) * vparam.cardrect.width * 0.1f;
                        float lst = vparam.handmin + (wmax - whand) * 0.5f;
                        tl = {lst + seq * vparam.cardrect.width * 1.1f + vparam.cardrect.width * 0.5f, vparam.handy[0], 0.0f};
                    }
                } else {
                    float wmax = vparam.handmax - vparam.handmin;
                    if(ct * vparam.cardrect.width + (ct - 1) * vparam.cardrect.width * 0.1f >= wmax) {
                        float wcard = (wmax - vparam.cardrect.width) / (ct - 1);
                        tl = {-vparam.handmin - wcard * seq - vparam.cardrect.width * 0.5f, vparam.handy[1], 0.0f};
                    } else {
                        float whand = ct * vparam.cardrect.width + (ct - 1) * vparam.cardrect.width * 0.1f;
                        float lst = -vparam.handmin - (wmax - whand) * 0.5f;
                        tl = {lst - seq * vparam.cardrect.width * 1.1f - vparam.cardrect.width * 0.5f, vparam.handy[1], 0.0f};
                    }
                }
                if(pcard->code != 0)
                    rot = vparam.hand_quat[0];
                else
                    rot = vparam.hand_quat[1];
            }
                break;
            case 0x4: {
                tl = field_blocks[side][seq]->translation;
                if(pcard->side & 0x80) {
                    tl.z = 0.001f * subs;
                } else {
                    tl.z = 0.001f * pcard->olcards.size();
                    if(subs == 1) {
                        if(side == 0)
                            rot = glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
                        else
                            rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 0.0f, 1.0f));
                    } else if(subs == 0x2) {
                        if(side == 0)
                            rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 1.0f, 0.0f));
                        else
                            rot = glm::angleAxis(3.1415926f, glm::vec3(1.0f, 0.0f, 0.0f));
                    } else if(subs == 0x4) {
                        if(side == 0)
                            rot = glm::angleAxis(3.1415926f * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f));
                        else
                            rot = glm::angleAxis(3.1415926f * 0.5f, glm::vec3(0.0f, 0.0f, -1.0f));
                    } else if(subs == 0x8) {
                        if(side == 0)
                            rot = glm::angleAxis(3.1415926f, glm::vec3(-0.70710678f, 0.70710678f, 0.0f));
                        else
                            rot = glm::angleAxis(3.1415926f, glm::vec3(0.70710678f, 0.70710678f, 0.0f));
                    }
                }
                
            }
                break;
            case 0x8: {
                tl = field_blocks[side][seq + 5]->translation;
                if(subs & 0x5) {
                    if(side == 0)
                        rot = glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
                    else
                        rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 0.0f, 1.0f));
                } else if(subs & 0xa) {
                    if(side == 0)
                        rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 1.0f, 0.0f));
                    else
                        rot = glm::angleAxis(3.1415926f, glm::vec3(1.0f, 0.0f, 0.0f));
                }
            }
                break;
            case 0x10: {
                tl = field_blocks[side][15]->translation;
                tl.z = subs * 0.001f;
                if(side == 0)
                    rot = glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
                else
                    rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 0.0f, 1.0f));
            }
                break;
            case 0x20: {
                tl = field_blocks[side][16]->translation;
                tl.z = subs * 0.001f;
                if(subs & 0x5) {
                    if(side == 0)
                        rot = glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
                    else
                        rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 0.0f, 1.0f));
                } else if(subs & 0xa) {
                    if(side == 0)
                        rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 1.0f, 0.0f));
                    else
                        rot = glm::angleAxis(3.1415926f, glm::vec3(1.0f, 0.0f, 0.0f));
                }
            }
                break;
            case 0x40: {
                tl = field_blocks[side][14]->translation;
                tl.z = subs * 0.001f;
                if(subs & 0x5) {
                    if(side == 0)
                        rot = glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
                    else
                        rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 0.0f, 1.0f));
                } else if(subs & 0xa) {
                    if(side == 0)
                        rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 1.0f, 0.0f));
                    else
                        rot = glm::angleAxis(3.1415926f, glm::vec3(1.0f, 0.0f, 0.0f));
                }
            }
                break;
            default:
                break;
        }
        if(update) {
            pcard->translation = tl;
            pcard->rotation = rot;
            pcard->update_rvert = true;
        } else {
            float stm = SceneMgr::Get().GetGameTime();
            pcard->translation.SetAnimator(std::make_shared<LerpAnimator<v3f, TGenMove>>(pcard->translation.Get(), tl, stm, tm, 10));
            pcard->rotation.SetAnimator(std::make_shared<LerpAnimatorCB<glm::quat, TGenMove>>([](const glm::quat& st, const glm::quat& ed, float t)->glm::quat {
                return glm::slerp(st, ed, t);
            }, pcard->rotation.Get(), rot, stm, tm, 10));
        }
        AddUpdateCard(pcard);
    }

    void DuelScene::RefreshHand(int side, bool update, float tm) {
        int ct = hand[side].size();
        if(ct == 0)
            return;
        v3f tl;
        glm::quat rot;
        float stm = SceneMgr::Get().GetGameTime();
        for(size_t seq = 0; seq < hand[side].size(); ++seq) {
            auto& pcard = hand[side][seq];
            if(side == 0) {
                float wmax = vparam.handmax - vparam.handmin;
                if(ct * vparam.cardrect.width + (ct - 1) * vparam.cardrect.width * 0.1f >= wmax) {
                    float wcard = (wmax - vparam.cardrect.width) / (ct - 1);
                    tl = {vparam.handmin + wcard * seq + vparam.cardrect.width * 0.5f, vparam.handy[0], 0.0f};
                } else {
                    float whand = ct * vparam.cardrect.width + (ct - 1) * vparam.cardrect.width * 0.1f;
                    float lst = vparam.handmin + (wmax - whand) * 0.5f;
                    tl = {lst + seq * vparam.cardrect.width * 1.1f + vparam.cardrect.width * 0.5f, vparam.handy[0], 0.0f};
                }
            } else {
                float wmax = vparam.handmax - vparam.handmin;
                if(ct * vparam.cardrect.width + (ct - 1) * vparam.cardrect.width * 0.1f >= wmax) {
                    float wcard = (wmax - vparam.cardrect.width) / (ct - 1);
                    tl = {-vparam.handmin - wcard * seq - vparam.cardrect.width * 0.5f, vparam.handy[1], 0.0f};
                } else {
                    float whand = ct * vparam.cardrect.width + (ct - 1) * vparam.cardrect.width * 0.1f;
                    float lst = -vparam.handmin - (wmax - whand) * 0.5f;
                    tl = {lst - seq * vparam.cardrect.width * 1.1f - vparam.cardrect.width * 0.5f, vparam.handy[1], 0.0f};
                }
            }
            if(pcard->code != 0)
                rot = vparam.hand_quat[0];
            else
                rot = vparam.hand_quat[1];
            if(update) {
                pcard->translation = tl;
                pcard->rotation = rot;
                pcard->update_rvert = true;
            } else {
                pcard->translation.SetAnimator(std::make_shared<LerpAnimator<v3f, TGenMove>>(pcard->translation.Get(), tl, stm, tm, 10));
                pcard->rotation.SetAnimator(std::make_shared<LerpAnimatorCB<glm::quat, TGenMove>>([](const glm::quat& st, const glm::quat& ed, float t)->glm::quat {
                    return glm::slerp(st, ed, t);
                }, pcard->rotation.Get(), rot, stm, tm, 10));
            }
            AddUpdateCard(pcard);
        }
    }
    
    void DuelScene::MoveCard(std::shared_ptr<FieldCard> pcard, int toside, int tozone, int toseq, int tosubs, bool update, float tm) {
        if(pcard->side == toside && pcard->loc == tozone && pcard->seq == toseq && pcard->pos == tosubs)
            return;
        RemoveCard(pcard->side, pcard->loc, pcard->seq, pcard->pos);
        pcard->side = toside;
        pcard->loc = tozone;
        pcard->seq = toseq;
        pcard->pos = tosubs;
        switch(tozone & 0x7f) {
            case 0x1:
                pcard->seq = deck[toside].size();
                deck[toside].push_back(pcard);
                break;
            case 0x2:
                pcard->seq = hand[toside].size();
                hand[toside].push_back(pcard);
                refresh_region |= 2 << (toside * 16);
                break;
            case 0x4:
                if(tozone & 0x80) {
                    auto pre = m_zone[toside][toseq];
                    if(pre) {
                        pre->olcards.push_back(pcard);
                        refresh_region |= 0x4 << (toside * 16);
                    }
                } else {
                    m_zone[toside][toseq] = pcard;
                    refresh_region |= 0x4 << (toside * 16);
                }
                break;
            case 0x8:
                s_zone[toside][toseq] = pcard;
                break;
            case 0x10:
                pcard->seq = grave[toside].size();
                grave[toside].push_back(pcard);
                break;
            case 0x20:
                pcard->seq = banished[toside].size();
                banished[toside].push_back(pcard);
                break;
            case 0x40:
                pcard->seq = extra[toside].size();
                extra[toside].push_back(pcard);
                break;
        }
        RefreshPos(pcard, update, tm);
        update_index = true;
    }
    
    void DuelScene::ChangePos(std::shared_ptr<FieldCard> pcard, int pos, bool update, float tm) {
        if(pcard->loc != 0x4 && pcard->loc != 0x8)
            return;
        if(pcard->pos == pos)
            return;
        pcard->pos = pos;
        glm::quat rot;
        if(pcard->loc == 0x4) {
            if(pos == 1) {
                if(pcard->side == 0)
                    rot = glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
                else
                    rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 0.0f, 1.0f));
            } else if(pos == 0x2) {
                if(pcard->side == 0)
                    rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 1.0f, 0.0f));
                else
                    rot = glm::angleAxis(3.1415926f, glm::vec3(1.0f, 0.0f, 0.0f));
            } else if(pos == 0x4) {
                if(pcard->side == 0)
                    rot = glm::angleAxis(3.1415926f * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f));
                else
                    rot = glm::angleAxis(3.1415926f * 0.5f, glm::vec3(0.0f, 0.0f, -1.0f));
            } else if(pos == 0x8) {
                if(pcard->side == 0)
                    rot = glm::angleAxis(3.1415926f, glm::vec3(-0.70710678f, 0.70710678f, 0.0f));
                else
                    rot = glm::angleAxis(3.1415926f, glm::vec3(0.70710678f, 0.70710678f, 0.0f));
            }
        } else {
            if(pos & 0x5) {
                if(pcard->side == 0)
                    rot = glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
                else
                    rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 0.0f, 1.0f));
            } else if(pos & 0xa) {
                if(pcard->side == 0)
                    rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 1.0f, 0.0f));
                else
                    rot = glm::angleAxis(3.1415926f, glm::vec3(1.0f, 0.0f, 0.0f));
            }
        }
        if(update) {
            pcard->rotation = rot;
            pcard->update_rvert = true;
        } else {
            float stm = SceneMgr::Get().GetGameTime();
            pcard->rotation.SetAnimator(std::make_shared<LerpAnimatorCB<glm::quat, TGenLinear>>([](const glm::quat& st, const glm::quat& ed, float t)->glm::quat {
                return glm::slerp(st, ed, t);
            }, pcard->rotation.Get(), rot, stm, tm));
        }
        AddUpdateCard(pcard);
    }
    
    void DuelScene::ReleaseCard(std::shared_ptr<FieldCard> pcard) {
        if(pcard == nullptr)
            return;
        if(pcard->code)
            ImageMgr::Get().UnloadCardTexture(pcard->code);
        if(pcard->vertex_index == alloc_cards.size() - 1) {
            alloc_cards.pop_back();
        } else {
            auto last = *alloc_cards.rbegin();
            alloc_cards[pcard->vertex_index] = last;
            alloc_cards.pop_back();
            update_index = true;
            AddUpdateCard(last);
        }
    }
    
    void DuelScene::AddUpdateCard(std::shared_ptr<FieldCard> pcard) {
        if(!pcard->updating) {
            updating_cards.push_back(pcard);
            pcard->updating = true;
        }
    }
    
    void DuelScene::ClearField() {
        for(int i = 0; i < 2; ++i) {
            deck[i].clear();
            hand[i].clear();
            m_zone[i].clear();
            s_zone[i].clear();
            grave[i].clear();
            banished[i].clear();
            extra[i].clear();
            m_zone[i].resize(5);
            s_zone[i].resize(8);
        }
        ImageMgr::Get().UnloadAllCardTexture();
        alloc_cards.clear();
    }
    
    v2i DuelScene::CheckHoverBlock(float px, float py) {
        for(int i = 0 ; i < 17; ++i) {
            if(field_blocks[0][i]->CheckInside(px, py))
                return {1, i};
            if(field_blocks[1][i]->CheckInside(px, py))
                return {2, i};
        }
        return {0, 0};
    }
    
    v2f DuelScene::GetProjectXY(float sx, float sy) {
        float x = sx - vparam.xoffset;
        float y = sy - vparam.yoffset;
        float projx = vparam.scrx * 0.5f * x;
        float projy = vparam.scry * 0.5f * y;
        float k = tanf(3.1415926f - vparam.angle + atanf(projy / vparam.cnear));
        float py = -vparam.cameray - vparam.cameraz / k;
        float nearx = glm::sqrt(vparam.cnear * vparam.cnear + projy * projy);
        float radiusx = glm::sqrt(vparam.cameraz * vparam.cameraz + (vparam.cameray + py) * (vparam.cameray + py));
        float px = projx * radiusx / nearx;
        return {px, py};
    }
    
    v2i DuelScene::GetHoverPos(int posx, int posy) {
        float sx = (float)posx / scene_size.x * 2.0f - 1.0f;
        float sy = 1.0f - (float)posy / scene_size.y * 2.0f;
        if(sx > vparam.hand_rect[0].left && sx < vparam.hand_rect[0].left + vparam.hand_rect[0].width &&
           sy < vparam.hand_rect[0].top && sy > vparam.hand_rect[0].top - vparam.hand_rect[0].height) {
            float wwidth = vparam.hand_rect[0].width;
            int ct = hand[0].size();
            if(ct > 0) {
                float whand = ct * vparam.hand_width[0] + (ct - 1) * vparam.hand_width[0] * 0.1f;
                if(whand >= wwidth) {
                    float lastleft = vparam.hand_rect[0].left + wwidth - vparam.hand_width[0];
                    if(sx >= lastleft)
                        return {3, ct - 1};
                    else {
                        float front_width = (lastleft - vparam.hand_rect[0].left) / (ct - 1);
                        int index = (int)((sx - vparam.hand_rect[0].left) / front_width);
                        if(sx - vparam.hand_rect[0].left - front_width * index < vparam.hand_width[0])
                            return {3, index};
                    }
                } else {
                    float wleft = vparam.hand_rect[0].left + (wwidth - whand) * 0.5f;
                    float lastleft = wleft + whand - vparam.hand_width[0];
                    if(sx >= lastleft && sx < wleft + whand)
                        return {3, ct - 1};
                    else if(sx >= wleft && sx < lastleft) {
                        float front_width = vparam.hand_width[0] * 1.1f;
                        int index = (int)((sx - wleft) / front_width);
                        if(sx - wleft - front_width * index < vparam.hand_width[0])
                            return {3, index};
                    }
                }
            }
        }
        if(sx > vparam.hand_rect[1].left - vparam.hand_rect[1].width && sx < vparam.hand_rect[1].left &&
           sy < vparam.hand_rect[1].top && sy > vparam.hand_rect[1].top - vparam.hand_rect[1].height) {
            float wwidth = vparam.hand_rect[1].width;
            int ct = hand[1].size();
            if(ct > 0) {
                float whand = ct * vparam.hand_width[1] + (ct - 1) * vparam.hand_width[1] * 0.1f;
                if(whand >= wwidth) {
                    float lastleft = vparam.hand_rect[1].left - wwidth + vparam.hand_width[1];
                    if(sx <= lastleft)
                        return {4, ct - 1};
                    else {
                        float front_width = (wwidth - vparam.hand_width[1]) / (ct - 1);
                        int index = (int)((vparam.hand_rect[1].left - sx) / front_width);
                        if(vparam.hand_rect[1].left - sx - front_width * index < vparam.hand_width[1])
                            return {4, index};
                    }
                } else {
                    float wleft = vparam.hand_rect[1].left - (wwidth - whand) * 0.5f;
                    float lastleft = wleft - whand + vparam.hand_width[1];
                    if(sx >= wleft - whand && sx < lastleft)
                        return {4, ct - 1};
                    else if(sx >= lastleft && sx < wleft) {
                        float front_width = vparam.hand_width[1] * 1.1f;
                        int index = (int)((wleft - sx) / front_width);
                        if(wleft - sx - front_width * index < vparam.hand_width[1])
                            return {4, index};
                    }
                }
            }
        }
        auto proj = GetProjectXY(sx, sy);
        auto hb = CheckHoverBlock(proj.x, proj.y);
        if(hb.x != 0)
            return hb;
        return {0, 0};
    }
}

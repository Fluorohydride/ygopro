#include "utils/common.h"

#include "../gui_extra.h"
#include "../image_mgr.h"
#include "../config.h"

#include "duel_scene_handler.h"
#include "duel_scene.h"

namespace ygopro
{
    ViewParam vparam;
    
    int32_t FieldBlock::GetTextureId() {
        return ImageMgr::Get().GetRawMiscTexture()->GetTextureId();
    }
    
    void FieldBlock::RefreshVertices() {
        vertices.resize(4);
        indices.resize(6);
        vertices[0].vertex = {translation.x - quad_size.x * 0.5f, translation.y + quad_size.y * 0.5f, 0.0f};
        vertices[1].vertex = {translation.x + quad_size.x * 0.5f, translation.y + quad_size.y * 0.5f, 0.0f};
        vertices[2].vertex = {translation.x - quad_size.x * 0.5f, translation.y - quad_size.y * 0.5f, 0.0f};
        vertices[3].vertex = {translation.x + quad_size.x * 0.5f, translation.y - quad_size.y * 0.5f, 0.0f};
        for(int32_t i = 0; i < 4; ++i) {
            vertices[i].texcoord = block_texture.vert[i];;
            vertices[i].color = color;
            vertices[i].hcolor = hl;
        }
        vt3::GenQuadIndex(indices.data(), 1, vert_index);
    }
    
    bool FieldBlock::CheckInside(float px, float py) {
        return std::abs(px - translation.x) <= std::abs(quad_size.x / 2.0f) && std::abs(py - translation.y) <= std::abs(quad_size.y / 2.0f);
    }
    
    FieldCard::~FieldCard() {
        if(code)
            ImageMgr::Get().UnloadCardTexture(code);
    }
    
    int32_t FieldCard::GetTextureId() {
        return ImageMgr::Get().GetRawCardTexture()->GetTextureId();
    }
    
    void FieldCard::RefreshVertices() {
        vertices.resize(12);
        indices.resize(18);
        if(update_vert) {
            std::array<v3f, 12> origin_vert;
            origin_vert[0] = {vparam.cardrect.left, vparam.cardrect.top, 0.001f};
            origin_vert[1] = {vparam.cardrect.left + vparam.cardrect.width, vparam.cardrect.top, 0.001f};
            origin_vert[2] = {vparam.cardrect.left, vparam.cardrect.top - vparam.cardrect.height, 0.001f};
            origin_vert[3] = {vparam.cardrect.left + vparam.cardrect.width, vparam.cardrect.top - vparam.cardrect.height, 0.001f};
            
            origin_vert[4] = {vparam.cardrect.left + vparam.cardrect.width, vparam.cardrect.top, -0.001f};
            origin_vert[5] = {vparam.cardrect.left, vparam.cardrect.top, -0.001f};
            origin_vert[6] = {vparam.cardrect.left + vparam.cardrect.width, vparam.cardrect.top - vparam.cardrect.height, -0.001f};
            origin_vert[7] = {vparam.cardrect.left, vparam.cardrect.top - vparam.cardrect.height, -0.001f};
            
            origin_vert[8] = {vparam.iconrect.left, vparam.iconrect.top, 0.001f};
            origin_vert[9] = {vparam.iconrect.left + vparam.iconrect.width, vparam.iconrect.top, 0.001f};
            origin_vert[10] = {vparam.iconrect.left, vparam.iconrect.top - vparam.iconrect.height, 0.001f};
            origin_vert[11] = {vparam.iconrect.left + vparam.iconrect.width, vparam.iconrect.top - vparam.iconrect.height, 0.001f};
            for(int32_t i = 0; i < 12; ++i) {
                glm::vec3 v(origin_vert[i].x, origin_vert[i].y + yoffset, origin_vert[i].z);
                auto c = rotation * v;
                vertices[i].vertex = v3f{c.x, c.y, c.z} + translation;
            }
            update_vert = false;
        }
        for(size_t i = 0; i < 4; ++i)
            vertices[i].texcoord = card_texture.vert[i];
        for(size_t i = 0; i < 4; ++i)
            vertices[i + 4].texcoord = sleeve_texture.vert[i];
        for(size_t i = 0; i < 4; ++i)
            vertices[i + 8].texcoord = icon_texture.vert[i];
        for(size_t i = 0; i < 12; ++i) {
            vertices[i].color = color;
            vertices[i].hcolor = hl;
        }
        vt3::GenQuadIndex(indices.data(), 2, vert_index);
    }
    
    void FieldCard::SetCode(uint32_t code) {
        if(this->code == code)
            return;
        if(this->code)
            ImageMgr::Get().UnloadCardTexture(this->code);
        this->code = code;
        if(code) {
            std::weak_ptr<FieldCard> thiscard = shared_from_this();
            auto ti = ImageMgr::Get().GetCardTexture(code, [thiscard](texf4 tex) {
                if(!thiscard.expired())
                    thiscard.lock()->SetCardTex(tex);
            });
            SetCardTex(ti);
        } else {
            SetCardTex(ImageMgr::Get().GetTexture("unknown"));
        }
    }
    
    std::pair<v3f, glm::quat> FieldCard::GetPositionInfo() {
        // POS_FACEUP_ATTACK		0x1
        // POS_FACEDOWN_ATTACK		0x2
        // POS_FACEUP_DEFENCE		0x4
        // POS_FACEDOWN_DEFENCE     0x8
        v3f tl = {0.0f, 0.0f, 0.0f};
        glm::quat rot = {0.0f, 0.0f, 0.0f, 0.0f};
        int32_t side = attaching_card ? attaching_card->card_side : card_side;
        int32_t loc = attaching_card ? attaching_card->card_loc : card_loc;
        int32_t seq = attaching_card ? attaching_card->card_seq : card_seq;
        int32_t pos = attaching_card ? 0x5 : card_pos;
        int32_t subs = card_pos;
        switch(loc) {
            case 0x1: { //deck
                auto fb_pos = field_blocks[side][13]->GetCenter();
                tl = {fb_pos.x, fb_pos.y, 0.001f * subs};
                if(pos & 0x5) {
                    if(side == 0)
                        rot = glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
                    else
                        rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 0.0f, 1.0f));
                } else if(pos & 0xa) {
                    if(side == 0)
                        rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 1.0f, 0.0f));
                    else
                        rot = glm::angleAxis(3.1415926f, glm::vec3(1.0f, 0.0f, 0.0f));
                }
                break;
            }
            case 0x2: { //hand
                int32_t ct = (int32_t)hand[side].size();
                if(ct == 0)
                    break;;
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
                if(this->code != 0)
                    rot = vparam.hand_quat[0];
                else
                    rot = vparam.hand_quat[1];
                break;
            }
            case 0x4: { //mzone
                auto fb_pos = field_blocks[side][seq]->GetCenter();
                tl.x = fb_pos.x;
                tl.y = fb_pos.y;
                if(attaching_card != nullptr) {
                    tl.z = 0.001f * subs;
                } else {
                    tl.z = 0.001f * attached_cards.size();
                    if(pos == 1) {
                        if(side == 0)
                            rot = glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
                        else
                            rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 0.0f, 1.0f));
                    } else if(pos == 0x2) {
                        if(side == 0)
                            rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 1.0f, 0.0f));
                        else
                            rot = glm::angleAxis(3.1415926f, glm::vec3(1.0f, 0.0f, 0.0f));
                    } else if(pos == 0x4) {
                        if(side == 0)
                            rot = glm::angleAxis(3.1415926f * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f));
                        else
                            rot = glm::angleAxis(3.1415926f * 0.5f, glm::vec3(0.0f, 0.0f, -1.0f));
                    } else if(pos == 0x8) {
                        if(side == 0)
                            rot = glm::angleAxis(3.1415926f, glm::vec3(-0.70710678f, 0.70710678f, 0.0f));
                        else
                            rot = glm::angleAxis(3.1415926f, glm::vec3(0.70710678f, 0.70710678f, 0.0f));
                    }
                }
                break;
            }
            case 0x8: { //szone
                auto fb_pos = field_blocks[side][seq + 5]->GetCenter();
                tl.x = fb_pos.x;
                tl.y = fb_pos.y;
                if(pos & 0x5) {
                    if(side == 0)
                        rot = glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
                    else
                        rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 0.0f, 1.0f));
                } else if(pos & 0xa) {
                    if(side == 0)
                        rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 1.0f, 0.0f));
                    else
                        rot = glm::angleAxis(3.1415926f, glm::vec3(1.0f, 0.0f, 0.0f));
                }
                break;
            }
            case 0x10: { //grave
                auto fb_pos = field_blocks[side][15]->GetCenter();
                tl = {fb_pos.x, fb_pos.y, seq * 0.001f};
                if(side == 0)
                    rot = glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
                else
                    rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 0.0f, 1.0f));
                break;
            }
            case 0x20: { //banish
                auto fb_pos = field_blocks[side][16]->GetCenter();
                tl = {fb_pos.x, fb_pos.y, seq * 0.001f};
                if(pos & 0x5) {
                    if(side == 0)
                        rot = glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
                    else
                        rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 0.0f, 1.0f));
                } else if(pos & 0xa) {
                    if(side == 0)
                        rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 1.0f, 0.0f));
                    else
                        rot = glm::angleAxis(3.1415926f, glm::vec3(1.0f, 0.0f, 0.0f));
                }
                break;
            }
            case 0x40: { //extra
                auto fb_pos = field_blocks[side][14]->GetCenter();
                tl = {fb_pos.x, fb_pos.y, seq * 0.001f};
                if(pos & 0x5) {
                    if(side == 0)
                        rot = glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
                    else
                        rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 0.0f, 1.0f));
                } else if(pos & 0xa) {
                    if(side == 0)
                        rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 1.0f, 0.0f));
                    else
                        rot = glm::angleAxis(3.1415926f, glm::vec3(1.0f, 0.0f, 0.0f));
                }
                break;
            }
            default:
                break;
        }
        return std::make_pair(tl, rot);
    }
    
    void FieldCard::UpdatePosition(int32_t tm) {
        auto npos = GetPositionInfo();
        if(tm == 0) {
            SetTranslation(npos.first);
            SetRotation(npos.second);
            return;
        } else {
            auto ptr = shared_from_this();
            auto cpos = std::make_pair(translation, rotation);
            auto action = std::make_shared<LerpAnimator<int64_t, FieldCard>>(1000, ptr, [cpos, npos](FieldCard* fc, double t) ->bool {
                fc->SetTranslation(cpos.first + (npos.first - cpos.first) * t);
                fc->SetRotation(glm::slerp(cpos.second, npos.second, (float)t));
                return true;
            }, std::make_shared<TGenMove<int64_t>>(tm, 0.01));
            ptr->SetYOffset(0.0f);
            SceneMgr::Get().RemoveAction(ptr.get());
            SceneMgr::Get().PushAction(action, ptr.get(), 1);
        }
    }
    
    void FieldCard::Attach(std::shared_ptr<FieldCard> target) {
        if(attaching_card == target)
            return;
        if(attaching_card)
            Detach();
        attaching_card = target;
        target->attached_cards.push_back(shared_from_this());
        int32_t i = 0;
        for(auto pcard : target->attached_cards)
            pcard->card_pos = i++;
        card_pos = (int32_t)target->attached_cards.size() - 1;
    }
    
    void FieldCard::Detach() {
        if(!attaching_card)
            return;
        auto target = attaching_card;
        target->attached_cards.erase(target->attached_cards.begin() + card_pos);
        int32_t i = 0;
        for(auto pcard : target->attached_cards)
            pcard->card_pos = i++;
        attaching_card = nullptr;
    }

    void FieldRenderer::PushVerticesAll() {
        for(auto& iter : field_blocks[0])
            iter->PushVertices();
        for(auto& iter : field_blocks[1])
            iter->PushVertices();
    }
    
    void FieldCardRenderer::PushVerticesAll() {
        for(auto& iter : hand[1])
            iter->PushVertices();
        for(auto& iter : deck[1])
            iter->PushVertices();
        for(auto& iter : extra[1])
            iter->PushVertices();
        for(auto& iter : s_zone[1])
            if(iter)
                iter->PushVertices();
        for(auto& iter : m_zone[1]) {
            if(iter) {
                for(auto& att : iter->attached_cards)
                    att->PushVertices();
                iter->PushVertices();
            }
        }
        for(auto& iter : grave[1])
            iter->PushVertices();
        for(auto& iter : banished[1])
            iter->PushVertices();

        for(auto& iter : grave[0])
            iter->PushVertices();
        for(auto& iter : banished[0])
            iter->PushVertices();
        for(auto& iter : m_zone[0]) {
            if(iter) {
                for(auto& att : iter->attached_cards)
                    att->PushVertices();
                iter->PushVertices();
            }
        }
        for(auto& iter : s_zone[0])
            if(iter)
                iter->PushVertices();
        for(auto& iter : deck[0])
            iter->PushVertices();
        for(auto& iter : extra[0])
            iter->PushVertices();
        for(auto& iter : hand[0])
            iter->PushVertices();
    }
    
    DuelScene::DuelScene(base::Shader* _2dshader, base::Shader* _3dshader) {
        vparam.fovy = (float)layoutCfg["fovy"].to_double();
        vparam.cnear = (float)layoutCfg["near"].to_double();
        vparam.cfar = (float)layoutCfg["far"].to_double();
        vparam.angle = (float)layoutCfg["angle"].to_double();
        vparam.radius = (float)layoutCfg["radius"].to_double();
        vparam.xoffset = (float)layoutCfg["xoffset"].to_double();
        vparam.yoffset = (float)layoutCfg["yoffset"].to_double();
        vparam.cardrect = sgui::SGJsonUtil::ConvertRectf(layoutCfg["card"]);
        vparam.handmin = (float)layoutCfg["handmin"].to_double();
        vparam.handmax = (float)layoutCfg["handmax"].to_double();
        vparam.handy[0] = (float)layoutCfg["handy1"].to_double();
        vparam.handy[1] = (float)layoutCfg["handy2"].to_double();
        
        bg_renderer = std::make_shared<base::SimpleTextureRenderer>();
        bg_renderer->SetShader(_2dshader);
        field_renderer = std::make_shared<FieldRenderer>();
        field_renderer->SetShader(_3dshader);
        field_renderer->SetShaderSettingCallback([](base::Shader* shader) {
            shader->SetParam1i("texid", 0);
            shader->SetParamMat4("mvp", glm::value_ptr(vparam.mvp));
        });
        fieldcard_renderer = std::make_shared<FieldCardRenderer>();
        fieldcard_renderer->SetShader(_3dshader);
        fieldcard_renderer->SetShaderSettingCallback([](base::Shader* shader) {
            shader->SetParam1i("texid", 0);
            shader->SetParamMat4("mvp", glm::value_ptr(vparam.mvp));
        });
        PushObject(bg_renderer.get());
        PushObject(field_renderer.get());
        PushObject(fieldcard_renderer.get());
        PushObject(sgui::SGGUIRoot::GetSingleton());
        bg_renderer->ClearVertices();
        bg_renderer->AddVertices(ImageMgr::Get().GetRawBGTexture(), rectf{-1.0f, 1.0f, 2.0f, -2.0f}, ImageMgr::Get().GetTexture("bg"));
    }
    
    DuelScene::~DuelScene() {
    }
    
    void DuelScene::Activate() {
        if(scene_handler)
            scene_handler->BeginHandler();
    }
    
    void DuelScene::Terminate() {
        sgui::SGGUIRoot::GetSingleton().ClearChilds();
    }
    
    bool DuelScene::Update() {
        if(scene_handler)
            scene_handler->UpdateHandler();
        return IsActive();
    }
    
    bool DuelScene::Draw() {
        ImageMgr::Get().LoadCardTextureFromList(3);
        auto need_render = PrepareRender();
        if(need_render)
            Render();
        return need_render;
    }
    
    void DuelScene::SetSceneSize(v2i sz) {
        SetViewport({0, 0, sz.x, sz.y});
        UpdateParams();
    }
    
    recti DuelScene::GetScreenshotClip() {
        return {0, 0, viewport.width, viewport.height};
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
    
    void DuelScene::UpdateParams() {
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, -vparam.radius * glm::cos(vparam.angle), vparam.radius * glm::sin(vparam.angle)),
                                     glm::vec3(0.0f, 0.0f, 0.0f),
                                     glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 projection = glm::perspective(vparam.fovy, vparam.ratio * viewport.width / viewport.height, vparam.cnear, vparam.cfar);
        glm::mat4 trscreen;
        trscreen[3][0] = vparam.xoffset;
        trscreen[3][1] = vparam.yoffset;
        vparam.mvp = trscreen * projection * view;
        vparam.cameray = vparam.radius * glm::cos(vparam.angle);
        vparam.cameraz = vparam.radius * glm::sin(vparam.angle);
        vparam.scry = 2.0f * tanf(vparam.fovy * 0.5f) * vparam.cnear;
        vparam.scrx = vparam.scry * viewport.width / viewport.height;
        UpdateHandRect();
        for(auto& iter : hand[0])
            iter->UpdatePosition(0);
        for(auto& iter : hand[1])
            iter->UpdatePosition(0);
        
    }
    
    void DuelScene::UpdateViewOffset(v2f offset) {
        vparam.xoffset += offset.x;
        vparam.yoffset -= offset.y;
        UpdateParams();
    }
    
    void DuelScene::UpdateViewAngle(float angle) {
        vparam.angle += 3.1415926f * 0.5f * angle;
        if(vparam.angle < 0.0f)
            vparam.angle = 0.0f;
        if(vparam.angle > 3.1415926f * 0.5f)
            vparam.angle = 3.1415926f * 0.5f;
        UpdateParams();
    }
    
    void DuelScene::UpdateViewRadius(float radius) {
        vparam.radius += radius;
        if(vparam.radius < 1.0f)
            vparam.radius = 1.0f;
        if(vparam.radius > 50.0f)
            vparam.radius = 50.0f;
        UpdateParams();
    }
    
    v2i DuelScene::CheckHoverBlock(float px, float py) {
        for(int32_t i = 0 ; i < 17; ++i) {
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
    
    v2i DuelScene::GetHoverPos(int32_t posx, int32_t posy) {
        float sx = (float)posx / viewport.width * 2.0f - 1.0f;
        float sy = 1.0f - (float)posy / viewport.height * 2.0f;
        if(sx > vparam.hand_rect[0].left && sx < vparam.hand_rect[0].left + vparam.hand_rect[0].width &&
           sy < vparam.hand_rect[0].top && sy > vparam.hand_rect[0].top - vparam.hand_rect[0].height) {
            float wwidth = vparam.hand_rect[0].width;
            int32_t ct = (int32_t)hand[0].size();
            if(ct > 0) {
                float whand = ct * vparam.hand_width[0] + (ct - 1) * vparam.hand_width[0] * 0.1f;
                if(whand >= wwidth) {
                    float lastleft = vparam.hand_rect[0].left + wwidth - vparam.hand_width[0];
                    if(sx >= lastleft)
                        return {3, ct - 1};
                    else {
                        float front_width = (lastleft - vparam.hand_rect[0].left) / (ct - 1);
                        int32_t index = (int32_t)((sx - vparam.hand_rect[0].left) / front_width);
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
                        int32_t index = (int32_t)((sx - wleft) / front_width);
                        if(sx - wleft - front_width * index < vparam.hand_width[0])
                            return {3, index};
                    }
                }
            }
        }
        if(sx > vparam.hand_rect[1].left - vparam.hand_rect[1].width && sx < vparam.hand_rect[1].left &&
           sy < vparam.hand_rect[1].top && sy > vparam.hand_rect[1].top - vparam.hand_rect[1].height) {
            float wwidth = vparam.hand_rect[1].width;
            int32_t ct = (int32_t)hand[1].size();
            if(ct > 0) {
                float whand = ct * vparam.hand_width[1] + (ct - 1) * vparam.hand_width[1] * 0.1f;
                if(whand >= wwidth) {
                    float lastleft = vparam.hand_rect[1].left - wwidth + vparam.hand_width[1];
                    if(sx <= lastleft)
                        return {4, ct - 1};
                    else {
                        float front_width = (wwidth - vparam.hand_width[1]) / (ct - 1);
                        int32_t index = (int32_t)((vparam.hand_rect[1].left - sx) / front_width);
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
                        int32_t index = (int32_t)((wleft - sx) / front_width);
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
    
    v3i DuelScene::GetHoverCardPos(v2i hp) {
        if(hp.x == 0)
            return {0, 0, 0};
        if(hp.x > 2)
            return {hp.x - 3, 0x2, hp.y};
        int32_t side = hp.x - 1;
        int32_t loc = 0, seq = 0;
        if(hp.y < 5) {
            loc = 0x4;
            seq = hp.y;
        } else if(hp.y < 13) {
            loc = 0x8;
            seq = hp.y - 5;
        } else {
            switch(hp.y) {
                case 13: loc = 0x1; break;
                case 14: loc = 0x40; break;
                case 15: loc = 0x10; break;
                case 16: loc = 0x20; break;
            }
        }
        return {side, loc, seq};
    }
    
    std::shared_ptr<FieldCard> DuelScene::GetHoverCard(int32_t side, int32_t zone, int32_t seq) {
        if(zone == 0)
            return nullptr;
        switch(zone) {
            case 0x1: return deck[side].empty() ? nullptr : deck[side].back();
            case 0x2: return hand[side].empty() ? nullptr : hand[side][seq];
            case 0x4: return m_zone[side][seq];
            case 0x8: return s_zone[side][seq];
            case 0x10: return grave[side].empty() ? nullptr : grave[side].back();
            case 0x20: return banished[side].empty() ? nullptr : banished[side].back();
            case 0x40: return extra[side].empty() ? nullptr : extra[side].back();
        }
        return nullptr;
    }
    
    std::shared_ptr<FieldBlock> DuelScene::GetFieldBlock(int32_t x, int32_t y) {
        if(x == 1 || x == 2)
            if(y < 17)
                return field_blocks[x - 1][y];
        return nullptr;
    }
    
    void DuelScene::ClearAllCards() {
        fieldcard_renderer->ClearAllObjects();
        ImageMgr::Get().UnloadAllCardTexture();
    }
}

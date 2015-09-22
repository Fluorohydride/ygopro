#include "utils/common.h"

#include "../gui_extra.h"
#include "../image_mgr.h"
#include "../config.h"

#include "duel_scene_handler.h"
#include "duel_scene.h"

namespace ygopro
{
    ViewParam vparam;
    std::array<std::shared_ptr<FieldBlock>, 17> field_blocks[2];
    std::vector<std::shared_ptr<FieldCard>> m_zone[2];
    std::vector<std::shared_ptr<FieldCard>> s_zone[2];
    std::vector<std::shared_ptr<FieldCard>> deck[2];
    std::vector<std::shared_ptr<FieldCard>> hand[2];
    std::vector<std::shared_ptr<FieldCard>> extra[2];
    std::vector<std::shared_ptr<FieldCard>> grave[2];
    std::vector<std::shared_ptr<FieldCard>> banished[2];
    
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
        return std::abs(px - translation.x) <= quad_size.x / 2.0f && std::abs(py - translation.y) <= quad_size.y / 2.0f;
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
            origin_vert[0] = {card_quad.left, card_quad.top, 0.001f};
            origin_vert[1] = {card_quad.left + card_quad.width, card_quad.top, 0.001f};
            origin_vert[2] = {card_quad.left, card_quad.top - card_quad.height, 0.001f};
            origin_vert[3] = {card_quad.left + card_quad.width, card_quad.top - card_quad.height, 0.001f};
            origin_vert[4] = {card_quad.left + card_quad.width, card_quad.top, -0.001f};
            origin_vert[5] = {card_quad.left, card_quad.top, -0.001f};
            origin_vert[6] = {card_quad.left + card_quad.width, card_quad.top - card_quad.height, -0.001f};
            origin_vert[7] = {card_quad.left, card_quad.top - card_quad.height, -0.001f};
            origin_vert[8] = {icon_quad.left, icon_quad.top, 0.001f};
            origin_vert[9] = {icon_quad.left + icon_quad.width, icon_quad.top, 0.001f};
            origin_vert[10] = {icon_quad.left, icon_quad.top - icon_quad.height, 0.001f};
            origin_vert[11] = {icon_quad.left + icon_quad.width, icon_quad.top - icon_quad.height, 0.001f};
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
        vt3::GenQuadIndex(indices.data(), 3, vert_index);
    }
    
    void FieldCard::SetCode(uint32_t code) {
        if(this->code == code)
            return;
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
    }
    
    void FieldCardRenderer::PushVerticesAll() {
        for(auto& iter : hand[1])
            iter->PushVertices();
        for(auto& iter : deck[1])
            iter->PushVertices();
        for(auto& iter : extra[1])
            iter->PushVertices();
        for(auto& iter : s_zone[1])
            iter->PushVertices();
        for(auto& iter : m_zone[1]) {
            for(auto& att : iter->attached_cards)
                att->PushVertices();
            iter->PushVertices();
        }
        for(auto& iter : m_zone[1])
            iter->PushVertices();
        for(auto& iter : grave[1])
            iter->PushVertices();
        for(auto& iter : banished[1])
            iter->PushVertices();

        for(auto& iter : grave[0])
            iter->PushVertices();
        for(auto& iter : banished[0])
            iter->PushVertices();
        for(auto& iter : m_zone[0]) {
            for(auto& att : iter->attached_cards)
                att->PushVertices();
            iter->PushVertices();
        }
        for(auto& iter : s_zone[0])
            iter->PushVertices();
        for(auto& iter : deck[0])
            iter->PushVertices();
        for(auto& iter : extra[0])
            iter->PushVertices();
        for(auto& iter : hand[0])
            iter->PushVertices();
    }
    
    DuelScene::DuelScene() {
        InitField();
        vparam.fovy = (float)layoutCfg["fovy"].to_double();
        vparam.cnear = (float)layoutCfg["near"].to_double();
        vparam.cfar = (float)layoutCfg["far"].to_double();
        vparam.angle = (float)layoutCfg["angle"].to_double();
        vparam.radius = (float)layoutCfg["radius"].to_double();
        vparam.xoffset = (float)layoutCfg["xoffset"].to_double();
        vparam.yoffset = (float)layoutCfg["yoffset"].to_double();
        vparam.cardrect = sgui::SGJsonUtil::ConvertRectf(layoutCfg["yoffset"]);
        vparam.handmin = (float)layoutCfg["handmin"].to_double();
        vparam.handmax = (float)layoutCfg["handmax"].to_double();
        vparam.handy[0] = (float)layoutCfg["handy1"].to_double();
        vparam.handy[1] = (float)layoutCfg["handy2"].to_double();
        
        TextFile vert_shader("./conf/vert.shader");
        TextFile frag_shader("./conf/frag.shader");
        duel_shader = std::make_shared<base::Shader>();
        duel_shader->LoadVertShader(vert_shader.Data());
        duel_shader->LoadFragShader(frag_shader.Data());
        duel_shader->Link();
        
        bg_renderer = std::make_shared<base::SimpleTextureRenderer>();
        bg_renderer->SetShader(&base::Shader::GetDefaultShader());
        field_renderer = std::make_shared<FieldRenderer>();
        field_renderer->SetShader(duel_shader.get());
        field_renderer->SetShaderSettingCallback([this]() {
            duel_shader->SetParamMat4("mvp", glm::value_ptr(vparam.mvp));
        });
        fieldcard_renderer = std::make_shared<FieldCardRenderer>();
        fieldcard_renderer->SetShader(duel_shader.get());
        fieldcard_renderer->SetShaderSettingCallback([this](){
            duel_shader->SetParamMat4("mvp", glm::value_ptr(glm::mat4(1.0f)));
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
    
    void DuelScene::InitField() {
        for(int32_t i = 0 ; i < 17; ++i) {
            field_blocks[0][i] = field_renderer->NewSharedObject<FieldBlock>();
            field_blocks[1][i] = field_renderer->NewSharedObject<FieldBlock>();
        }
        field_blocks[0][0 ]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["mzone1"]));
        field_blocks[0][1 ]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["mzone2"]));
        field_blocks[0][2 ]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["mzone3"]));
        field_blocks[0][3 ]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["mzone4"]));
        field_blocks[0][4 ]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["mzone5"]));
        field_blocks[0][5 ]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["szone1"]));
        field_blocks[0][6 ]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["szone2"]));
        field_blocks[0][7 ]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["szone3"]));
        field_blocks[0][8 ]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["szone4"]));
        field_blocks[0][9 ]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["szone5"]));
        field_blocks[0][10]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["fdzone"]));
        field_blocks[0][11]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["pzonel"]));
        field_blocks[0][12]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["pzoner"]));
        field_blocks[0][13]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["mdeck"]));
        field_blocks[0][14]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["exdeck"]));
        field_blocks[0][15]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["grave"]));
        field_blocks[0][16]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["banish"]));
        field_blocks[0][0 ]->SetTexture(ImageMgr::Get().GetTexture("mzone"));
        field_blocks[0][1 ]->SetTexture(ImageMgr::Get().GetTexture("mzone"));
        field_blocks[0][2 ]->SetTexture(ImageMgr::Get().GetTexture("mzone"));
        field_blocks[0][3 ]->SetTexture(ImageMgr::Get().GetTexture("mzone"));
        field_blocks[0][4 ]->SetTexture(ImageMgr::Get().GetTexture("mzone"));
        field_blocks[0][5 ]->SetTexture(ImageMgr::Get().GetTexture("szone"));
        field_blocks[0][6 ]->SetTexture(ImageMgr::Get().GetTexture("szone"));
        field_blocks[0][7 ]->SetTexture(ImageMgr::Get().GetTexture("szone"));
        field_blocks[0][8 ]->SetTexture(ImageMgr::Get().GetTexture("szone"));
        field_blocks[0][9 ]->SetTexture(ImageMgr::Get().GetTexture("szone"));
        field_blocks[0][10]->SetTexture(ImageMgr::Get().GetTexture("fdzone"));
        field_blocks[0][11]->SetTexture(ImageMgr::Get().GetTexture("pzonel"));
        field_blocks[0][12]->SetTexture(ImageMgr::Get().GetTexture("pzoner"));
        field_blocks[0][13]->SetTexture(ImageMgr::Get().GetTexture("mdeck"));
        field_blocks[0][14]->SetTexture(ImageMgr::Get().GetTexture("exdeck"));
        field_blocks[0][15]->SetTexture(ImageMgr::Get().GetTexture("grave"));
        field_blocks[0][16]->SetTexture(ImageMgr::Get().GetTexture("banish"));
        for(int32_t i = 0; i < 17; ++i)
            field_blocks[1][i]->Mirror(field_blocks[0][i].get());
        m_zone[0].resize(5);
        m_zone[1].resize(5);
        s_zone[0].resize(8);
        s_zone[1].resize(8);
    }
    
    std::shared_ptr<FieldCard> DuelScene::AddCard(uint32_t code, int32_t side, int32_t zone, int32_t seq, int32_t subs) {
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
        auto ptr = fieldcard_renderer->NewSharedObject<FieldCard>();
        ptr->card_side = side;
        ptr->card_loc = zone;
        ptr->card_seq = seq;
        ptr->card_pos = subs;
        switch(zone & 0x7f) {
            case 0x1:
                ptr->card_seq = (int32_t)deck[side].size();
                deck[side].push_back(ptr);
                break;
            case 0x2:
                ptr->card_seq = (int32_t)hand[side].size();
                hand[side].push_back(ptr);
                break;
            case 0x4:
                if(zone & 0x80) {
                    auto pre = m_zone[side][seq];
                    pre->attached_cards.push_back(ptr);
                } else
                    m_zone[side][seq] = ptr;
                break;
            case 0x8:
                s_zone[side][seq] = ptr;
                break;
            case 0x10:
                ptr->card_seq = (int32_t)grave[side].size();
                grave[side].push_back(ptr);
                break;
            case 0x20:
                ptr->card_seq = (int32_t)banished[side].size();
                banished[side].push_back(ptr);
                break;
            case 0x40:
                ptr->card_seq = (int32_t)extra[side].size();
                extra[side].push_back(ptr);
                break;
        }
        fieldcard_renderer->RequestRedraw();
        return ptr;
    }
    
    std::shared_ptr<FieldCard> DuelScene::GetCard(int32_t side, int32_t zone, int32_t seq, int32_t subs) {
        switch(zone & 0x7f) {
            case 0x1:
                return deck[side][seq];
            case 0x2:
                return hand[side][seq];
            case 0x4:
                if(zone & 0x80) {
                    auto pcard = m_zone[side][seq];
                    return pcard->attached_cards[subs]->shared_from_this();
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
    
    std::shared_ptr<FieldCard> DuelScene::RemoveCard(int32_t side, int32_t zone, int32_t seq, int32_t subs) {
        std::shared_ptr<FieldCard> ret;
        switch(zone & 0x7f) {
            case 0x1: {
                ret = deck[side][seq];
                deck[side].erase(deck[side].begin() + seq);
                for(size_t i = seq; i < deck[side].size(); ++i) {
                    deck[side][i]->card_seq = (int32_t)i;
                    deck[side][i]->UpdatePosition(0);
                }
                break;
            }
            case 0x2: {
                ret = hand[side][seq];
                hand[side].erase(hand[side].begin() + seq);
                uint32_t index = 0;
                for(auto& iter : hand[side]) {
                    iter->card_seq = index++;
                    iter->UpdatePosition(0);
                }
                break;
            }
            case 0x4: {
                if(zone & 0x80) {
                    auto pcard = m_zone[side][seq];
                    ret = pcard->attached_cards[subs];
                    ret->Detach();
                    for(auto& iter : pcard->attached_cards)
                        iter->UpdatePosition(0);
                    pcard->UpdatePosition(0);
                } else {
                    ret = m_zone[side][seq];
                    m_zone[side][seq] = nullptr;
                }
                break;
            }
            case 0x8: {
                ret = s_zone[side][seq];
                s_zone[side][seq] = nullptr;
                break;
            }
            case 0x10: {
                ret = grave[side][seq];
                grave[side].erase(grave[side].begin() + seq);
                for(size_t i = seq; i < grave[side].size(); ++i) {
                    grave[side][i]->card_seq = (int32_t)i;
                    grave[side][i]->UpdatePosition(0);
                }
                break;
            }
            case 0x20: {
                ret = banished[side][seq];
                banished[side].erase(banished[side].begin() + seq);
                for(size_t i = seq; i < banished[side].size(); ++i) {
                    banished[side][i]->card_seq = (int32_t)i;
                    banished[side][i]->UpdatePosition(0);
                }
                break;
            }
            case 0x40: {
                ret = extra[side][seq];
                extra[side].erase(extra[side].begin() + seq);
                for(size_t i = seq; i < extra[side].size(); ++i) {
                    extra[side][i]->card_seq = (int32_t)i;
                    extra[side][i]->UpdatePosition(0);
                }
                break;
            }
        }
        return ret;
    }
    
    void DuelScene::MoveCard(std::shared_ptr<FieldCard> pcard, int32_t toside, int32_t tozone, int32_t toseq, int32_t tosubs, int32_t tm) {
        if(pcard->card_side == toside && pcard->card_loc == tozone && pcard->card_seq == toseq && pcard->card_pos == tosubs)
            return;
        RemoveCard(pcard->card_side, pcard->card_loc, pcard->card_seq, pcard->card_pos);
        pcard->card_side = toside;
        pcard->card_loc = tozone;
        pcard->card_seq = toseq;
        pcard->card_pos = tosubs;
        switch(tozone & 0x7f) {
            case 0x1:
                pcard->card_seq = (int32_t)deck[toside].size();
                deck[toside].push_back(pcard);
                break;
            case 0x2:
                pcard->card_seq = (int32_t)hand[toside].size();
                hand[toside].push_back(pcard);
                break;
            case 0x4:
                if(tozone & 0x80) {
                    auto target = m_zone[toside][toseq];
                    if(target) {
                        pcard->Attach(target);
                        if(target->card_loc == 0x4) {
                            for(auto& iter : target->attached_cards)
                                iter->UpdatePosition(0);
                            target->UpdatePosition(0);
                        }
                    }
                } else {
                    m_zone[toside][toseq] = pcard;
                }
                break;
            case 0x8:
                s_zone[toside][toseq] = pcard;
                break;
            case 0x10:
                pcard->card_seq = (int32_t)grave[toside].size();
                grave[toside].push_back(pcard);
                break;
            case 0x20:
                pcard->card_seq = (int32_t)banished[toside].size();
                banished[toside].push_back(pcard);
                break;
            case 0x40:
                pcard->card_seq = (int32_t)extra[toside].size();
                extra[toside].push_back(pcard);
                break;
        }
    }
    
    void DuelScene::ChangePos(std::shared_ptr<FieldCard> pcard, int32_t pos, int32_t tm) {
        
    }
    
    void DuelScene::ClearField() {
        for(int32_t i = 0; i < 2; ++i) {
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
        fieldcard_renderer->ClearAllObjects();
        ImageMgr::Get().UnloadAllCardTexture();
    }
    
    void DuelScene::InitHp(int32_t local_pl, int32_t hp) {
        
    }
    
    void DuelScene::AddChain(uint32_t code, int32_t side, int32_t zone, int32_t seq, int32_t subs, int32_t tside, int32_t tzone, int32_t tseq) {
        
    }
    
    void DuelScene::DrawCard(int32_t pl, int32_t data) {
        if(deck[pl].empty())
            return;
        auto ptr = deck[pl].back();
        MoveCard(ptr, pl, 0x2, 0, 0, 0);
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
    
    std::shared_ptr<FieldBlock> DuelScene::GetFieldBlock(int32_t x, int32_t y) {
        if(x == 1 || x == 2)
            if(y < 17)
                return field_blocks[x - 1][y];
        return nullptr;
    }
    
    std::shared_ptr<FieldCard> DuelScene::GetFieldCard(int32_t x, int32_t y) {
        if(x == 3 || x == 4)
            if(y < hand[x - 3].size())
                return hand[x - 3][y];
        return nullptr;
    }
    
}

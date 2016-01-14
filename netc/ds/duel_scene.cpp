#include "utils/common.h"

#include "../gui_extra.h"
#include "../image_mgr.h"
#include "../config.h"

#include "duel_scene.h"

namespace ygopro
{
    ViewParam vparam;
    
    int32_t FieldSprite::GetTextureId() {
        return texture ? texture->GetTextureId() : 0;
    }
    
    void FieldSprite::RefreshVertices() {
        vertices.resize(4);
        indices.resize(6);
        if(update_vert) {
            std::array<v3f, 4> origin_vert;
            origin_vert[0] = {-sprite_size.x / 2.0f, sprite_size.y / 2.0f, 0.0f};
            origin_vert[1] = {sprite_size.x / 2.0f, sprite_size.y / 2.0f, 0.0f};
            origin_vert[2] = {-sprite_size.x / 2.0f, -sprite_size.y / 2.0f, 0.0f};
            origin_vert[3] = {sprite_size.x / 2.0f, -sprite_size.y / 2.0f, 0.0f};
            for(int32_t i = 0; i < 4; ++i) {
                glm::vec3 v(origin_vert[i].x, origin_vert[i].y, origin_vert[i].z);
                auto c = rotation * v;
                vertices[i].vertex = v3f{c.x, c.y, c.z} + translation;
            }
            update_vert = false;
        }
        for(size_t i = 0; i < 4; ++i)
            vertices[i].texcoord = texcoord.vert[i];
        for(size_t i = 0; i < 4; ++i) {
            vertices[i].color = color;
            vertices[i].hcolor = hl;
        }
        vt3::GenQuadIndex(indices.data(), 1, vert_index);
    }
    
    void FieldSummonEffect::RefreshVertices() {
        vertices.resize(24);
        indices.resize(144);
        if(update_vert) {
            std::array<v3f, 24> origin_vert;
            for(int32_t i = 0; i < 12; ++i) {
                float angle = 3.1415925f / 6.0f * i;
                origin_vert[i * 2 + 0] = {sprite_size.x * cosf(angle), sprite_size.x * sinf(angle), 0.0f};
                origin_vert[i * 2 + 1] = {sprite_size.y * cosf(angle), sprite_size.y * sinf(angle), outer_z};
            }
            for(int32_t i = 0; i < 24; ++i) {
                glm::vec3 v(origin_vert[i].x, origin_vert[i].y, origin_vert[i].z);
                auto c = rotation * v;
                vertices[i].vertex = v3f{c.x, c.y, c.z} + translation;
            }
            update_vert = false;
        }
        for(size_t i = 0; i < 24; ++i) {
            v2f center = {(texcoord.vert[0].x + texcoord.vert[1].x) * 0.5f, (texcoord.vert[0].y + texcoord.vert[2].y) * 0.5f};
            float r1 = center.x - texcoord.vert[0].x;
            float r2 = center.y - texcoord.vert[0].y;
            for(int32_t i = 0; i < 12; ++i) {
                float angle = 3.1415925f / 6.0f * i;
                vertices[i * 2 + 0].texcoord = v2f{r1 * tex_r * cosf(angle), r2 * tex_r * sinf(angle)} + center;
                vertices[i * 2 + 1].texcoord = v2f{r1 * tex_o * cosf(angle), r2 * tex_o * sinf(angle)} + center;
            }
        }
        for(size_t i = 0; i < 24; ++i) {
            vertices[i].color = color;
            vertices[i].hcolor = hl;
        }
        static const int16_t idx[] = {0, 1, 2, 0, 2, 1};
        for(int32_t i = 0; i < 24; ++i) {
            for(int32_t j = 0; j < 6; ++j)
                indices[i * 6 + j] = (idx[j] + i) % 24 + vert_index;
        }
    }
    
    FieldCard::~FieldCard() {
        if(code)
            ImageMgr::Get().UnloadCardTexture(code);
        for(auto& att_cards : attached_cards)
            att_cards->attaching_card = nullptr;
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
            
            float icon_height = (pos_info.position & 0x5) ? 0.0015 : -0.0015;
            origin_vert[8] = {vparam.iconrect.left, vparam.iconrect.top, icon_height};
            origin_vert[9] = {vparam.iconrect.left + vparam.iconrect.width, vparam.iconrect.top, icon_height};
            origin_vert[10] = {vparam.iconrect.left, vparam.iconrect.top - vparam.iconrect.height, icon_height};
            origin_vert[11] = {vparam.iconrect.left + vparam.iconrect.width, vparam.iconrect.top - vparam.iconrect.height, icon_height};
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
            data = DataMgr::Get()[code];
        } else {
            SetCardTex(ImageMgr::Get().GetTexture("unknown"));
            data = nullptr;
        }
    }
    
    std::pair<v3f, glm::quat> FieldCard::GetPositionInfo(CardPosParam param) {
        // POS_FACEUP_ATTACK		0x1
        // POS_FACEDOWN_ATTACK		0x2
        // POS_FACEUP_DEFENCE		0x4
        // POS_FACEDOWN_DEFENCE     0x8
        v3f tl = {0.0f, 0.0f, 0.0f};
        glm::quat rot = {0.0f, 0.0f, 0.0f, 0.0f};
        int32_t side = attaching_card ? attaching_card->pos_info.controler : pos_info.controler;
        int32_t loc = attaching_card ? attaching_card->pos_info.location : pos_info.location;
        int32_t seq = attaching_card ? attaching_card->pos_info.sequence : pos_info.sequence;
        int32_t pos = attaching_card ? 0x5 : pos_info.position;
        int32_t subs = pos_info.subsequence;
        switch(loc) {
            case 0x1: { //deck
                auto fb_pos = g_player[side].field_blocks[13]->GetCenter();
                tl = {fb_pos.x, fb_pos.y, 0.005f * seq};
                bool faceup = g_duel.deck_reversed ? (pos & 0xa) : (pos & 0x5);
                if(param == CardPosParam::Confirm) {    // confirm deck
                    tl.x += -vparam.cardrect.width * 1.1f;
                    faceup = true;
                } else if(param == CardPosParam::Shuffle) // shuffle
                    tl.x += vparam.cardrect.width * SceneMgr::Get().GetRandomReal(-1.0f, 1.0f);
                if(faceup) {
                    if(side == 0)
                        rot = glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
                    else
                        rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 0.0f, 1.0f));
                } else {
                    if(side == 0)
                        rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 1.0f, 0.0f));
                    else
                        rot = glm::angleAxis(3.1415926f, glm::vec3(1.0f, 0.0f, 0.0f));
                }
                break;
            }
            case 0x2: { //hand
                int32_t ct = (int32_t)g_player[side].hand.size();
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
                        tl = {lst + seq * vparam.cardrect.width * 1.1f + vparam.cardrect.width * 0.5f, vparam.handy[0], 0.001f};
                    }
                } else {
                    float wmax = vparam.handmax - vparam.handmin;
                    if(ct * vparam.cardrect.width + (ct - 1) * vparam.cardrect.width * 0.1f >= wmax) {
                        float wcard = (wmax - vparam.cardrect.width) / (ct - 1);
                        tl = {-vparam.handmin - wcard * seq - vparam.cardrect.width * 0.5f, vparam.handy[1], 0.0f};
                    } else {
                        float whand = ct * vparam.cardrect.width + (ct - 1) * vparam.cardrect.width * 0.1f;
                        float lst = -vparam.handmin - (wmax - whand) * 0.5f;
                        tl = {lst - seq * vparam.cardrect.width * 1.1f - vparam.cardrect.width * 0.5f, vparam.handy[1], 0.001f};
                    }
                }
                if(this->code != 0 && (param == CardPosParam::None))
                    rot = vparam.hand_quat[0];
                else
                    rot = vparam.hand_quat[1];
                break;
            }
            case 0x4: { //mzone
                if(param != CardPosParam::Shuffle) {
                    auto fb_pos = g_player[side].field_blocks[seq]->GetCenter();
                    tl.x = fb_pos.x;
                    tl.y = fb_pos.y;
                    if(attaching_card != nullptr) {
                        tl.z = 0.005f * subs;
                        tl.x -= vparam.cardrect.width * 0.1f;
                    } else {
                        tl.z = 0.005f * attached_cards.size();
                        if(pos & 0x3) {
                            if(pos == 0x1 || param == CardPosParam::Confirm) {
                                if(side == 0)
                                    rot = glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
                                else
                                    rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 0.0f, 1.0f));
                            } else {
                                if(side == 0)
                                    rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 1.0f, 0.0f));
                                else
                                    rot = glm::angleAxis(3.1415926f, glm::vec3(1.0f, 0.0f, 0.0f));
                            }
                        } else {
                            if(pos == 0x4 || param == CardPosParam::Confirm) {
                                if(side == 0)
                                    rot = glm::angleAxis(3.1415926f * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f));
                                else
                                    rot = glm::angleAxis(3.1415926f * 0.5f, glm::vec3(0.0f, 0.0f, -1.0f));
                            } else {
                                if(side == 0)
                                    rot = glm::angleAxis(3.1415926f, glm::vec3(-0.70710678f, 0.70710678f, 0.0f));
                                else
                                    rot = glm::angleAxis(3.1415926f, glm::vec3(0.70710678f, 0.70710678f, 0.0f));
                            }
                        }
                    }
                } else {
                    auto fb_pos = g_player[side].field_blocks[2]->GetCenter();
                    tl = {fb_pos.x, fb_pos.y, 1};
                    if(side == 0)
                        rot = glm::angleAxis(3.1415926f, glm::vec3(-0.70710678f, 0.70710678f, 0.0f));
                    else
                        rot = glm::angleAxis(3.1415926f, glm::vec3(0.70710678f, 0.70710678f, 0.0f));
                }
                break;
            }
            case 0x8: { //szone
                auto fb_pos = g_player[side].field_blocks[seq + 5]->GetCenter();
                tl.x = fb_pos.x;
                tl.y = fb_pos.y;
                if(pos & 0x5 || param == CardPosParam::Confirm) {
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
                auto fb_pos = g_player[side].field_blocks[15]->GetCenter();
                tl = {fb_pos.x, fb_pos.y, seq * 0.005f};
                if(param == CardPosParam::Confirm)
                    tl.x += -vparam.cardrect.width * 1.1f;
                if(side == 0)
                    rot = glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
                else
                    rot = glm::angleAxis(3.1415926f, glm::vec3(0.0f, 0.0f, 1.0f));
                break;
            }
            case 0x20: { //banish
                auto fb_pos = g_player[side].field_blocks[16]->GetCenter();
                tl = {fb_pos.x, fb_pos.y, seq * 0.005f};
                if(param == CardPosParam::Confirm)
                    tl.x += -vparam.cardrect.width * 1.1f;
                if(pos & 0x5 || param == CardPosParam::Confirm) {
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
                auto fb_pos = g_player[side].field_blocks[14]->GetCenter();
                tl = {fb_pos.x, fb_pos.y, seq * 0.005f};
                if(param == CardPosParam::Confirm) // confirm extra
                    tl.x += vparam.cardrect.width * 1.1f;
                if(pos & 0x5 || param == CardPosParam::Confirm) {
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
    
    void FieldCard::UpdateTo(int32_t tm, std::pair<v3f, glm::quat> npos, CardActType act_type) {
        if(tm == 0) {
            SetTranslation(npos.first);
            SetRotation(npos.second);
            return;
        } else {
            auto ptr = shared_from_this();
            auto cpos = std::make_pair(translation, rotation);
            std::shared_ptr<TGen<int64_t>> tgen;
            if(act_type == CardActType::Linear)
                tgen = std::make_shared<TGenLinear<int64_t>>(tm);
            else
                tgen = std::make_shared<TGenMove<int64_t>>(tm, 0.01);
            auto action = std::make_shared<LerpAnimator<int64_t, FieldCard>>(tm, ptr, [cpos, npos](FieldCard* fc, double t) ->bool {
                fc->SetTranslation(cpos.first + (npos.first - cpos.first) * t);
                fc->SetRotation(glm::slerp(cpos.second, npos.second, (float)t));
                return true;
            }, tgen);
            ptr->SetYOffset(0.0f);
            SceneMgr::Get().RemoveAction(ptr.get());
            SceneMgr::Get().PushAction(action, ptr.get(), 1);
        }
    }
    
    void FieldCard::TranslateTo(int32_t tm, v3f tl, CardActType act_type) {
        if(tm == 0) {
            SetTranslation(tl);
            return;
        } else {
            auto ptr = shared_from_this();
            auto cur_tl = translation;
            std::shared_ptr<TGen<int64_t>> tgen;
            if(act_type == CardActType::Linear)
                tgen = std::make_shared<TGenLinear<int64_t>>(tm);
            else
                tgen = std::make_shared<TGenMove<int64_t>>(tm, 0.01);
            auto action = std::make_shared<LerpAnimator<int64_t, FieldCard>>(tm, ptr, [cur_tl, tl](FieldCard* fc, double t) ->bool {
                fc->SetTranslation(cur_tl + (tl - cur_tl) * t);
                return true;
            }, tgen);
            SceneMgr::Get().RemoveAction(ptr.get());
            SceneMgr::Get().PushAction(action, ptr.get(), 1);
        }
    }
    
    void FieldCard::RotateTo(int32_t tm, glm::quat rot) {
        if(tm == 0) {
            SetRotation(rot);
            return;
        } else {
            auto ptr = shared_from_this();
            auto crot = rotation;
            auto action = std::make_shared<LerpAnimator<int64_t, FieldCard>>(tm, ptr, [crot, rot](FieldCard* fc, double t) ->bool {
                fc->SetRotation(glm::slerp(crot, rot, (float)t));
                return true;
            }, std::make_shared<TGenLinear<int64_t>>(tm));
            SceneMgr::Get().RemoveAction(ptr.get());
            SceneMgr::Get().PushAction(action, ptr.get(), 1);
        }
    }
    
    void FieldCard::UpdatePosition(int32_t tm, CardActType act_type) {
        UpdateTo(tm, GetPositionInfo(), act_type);
    }
    
    void FieldCard::UpdateTranslation(int32_t tm, CardActType act_type) {
        TranslateTo(tm, GetPositionInfo().first, act_type);
    }
    
    void FieldCard::UpdateRotation(int32_t tm) {
        RotateTo(tm, GetPositionInfo().second);
    }
    
    void FieldCard::Attach(std::shared_ptr<FieldCard> target) {
        if(attaching_card == target.get())
            return;
        if(attaching_card)
            Detach();
        attaching_card = target.get();
        target->attached_cards.push_back(shared_from_this());
        pos_info.subsequence = (int32_t)target->attached_cards.size() - 1;
    }
    
    void FieldCard::Detach() {
        if(!attaching_card)
            return;
        auto target = attaching_card;
        target->attached_cards.erase(target->attached_cards.begin() + pos_info.subsequence);
        int32_t i = 0;
        for(auto pcard : target->attached_cards)
            pcard->pos_info.subsequence = i++;
        attaching_card = nullptr;
    }
    
    void FieldCard::AddCounter(uint16_t counter_type, uint32_t count) {
        counter_map[counter_type] += count;
    }
    
    void FieldCard::RemoveCounter(uint16_t counter_type, uint32_t count) {
        auto iter = counter_map.find(counter_type);
        if(iter != counter_map.end()) {
            if(iter->second > count)
                iter->second -= count;
            else
                counter_map.erase(iter);
        }
    }
    
    void FieldCard::ClearCounter() {
        counter_map.clear();
    }
    
    void FieldCard::AddContinuousTarget(FieldCard* target) {
        targeting_cards.insert(target);
        target->target_this.insert(this);
    }
    
    void FieldCard::RemoveContinuousTarget(FieldCard* target) {
        target->target_this.erase(this);
        targeting_cards.erase(target);
    }
    
    void FieldCard::ClearContinuousTarget() {
        for(auto target : targeting_cards)
            target->target_this.erase(this);
        for(auto target : target_this)
            target->targeting_cards.erase(this);
        targeting_cards.clear();
        target_this.clear();
    }
    
    void FieldCard::Equip(FieldCard* target) {
        if(equiping_card)
            equiping_card->equip_this.erase(this);
        target->equip_this.insert(this);
        equiping_card = target;
    }
    
    void FieldCard::Unequip() {
        if(!equiping_card)
            return;
        equiping_card->equip_this.erase(this);
        equiping_card = nullptr;
    }
    
    int32_t FloatingNumber::GetTextureId() {
        return ImageMgr::Get().GetRawMiscTexture()->GetTextureId();
    }
    
    void FloatingNumber::RefreshVertices() {
        if(val_string.empty())
            return;
        vertices.resize(val_string.length() * 8);
        indices.resize(val_string.length() * 12);
        base::RenderObject2DLayout* mgr = static_cast<base::RenderObject2DLayout*>(this->manager);
        v2i all_size = {(int32_t)val_string.length() * (char_size.x + 1) - 1, char_size.y};
        v2i center = CenterPosInScreen(mgr->GetScreenSize());
        v2i pos = {center.x - all_size.x / 2, center.y - all_size.y / 2};
        v2i off = {(int32_t)(char_size.x * 0.05f), (int32_t)(char_size.y * 0.05f)};
        off.x = off.x > 0 ? off.x : 1;
        off.y = off.y > 0 ? off.y : 1;
        int32_t i = 0;
        auto push_rotated_vert = [this, mgr](vt2* vt, v2i pos, v2i sz, v2i center, texf4 tex, uint32_t cl) {
            vt[0].vertex = mgr->ConvScreenCoord(RotPoint(pos, center));
            vt[1].vertex = mgr->ConvScreenCoord(RotPoint({pos.x + sz.x, pos.y}, center));
            vt[2].vertex = mgr->ConvScreenCoord(RotPoint({pos.x, pos.y + sz.y}, center));
            vt[3].vertex = mgr->ConvScreenCoord(RotPoint(pos + sz, center));
            for(int32_t i = 0; i < 4; ++i) {
                vt[i].texcoord = tex.vert[i];
                vt[i].color = cl;
                vt[i].hcolor = hl;
            }
        };
        for(auto iter : val_string) {
            auto tex = ImageMgr::Get().GetCharTex(iter);
            push_rotated_vert(&vertices[i], {pos.x - off.x, pos.y - off.y}, char_size, {center.x - off.x, center.y - off.y}, tex, scolor);
            push_rotated_vert(&vertices[i + val_string.length() * 4], pos, char_size, center, tex, color);
            i += 4;
            pos.x += char_size.x + 1;
        }
        vt2::GenQuadIndex(indices.data(), (int32_t)val_string.length() * 2, vert_index);
    }
    
    void FloatingNumber::SetValue(int32_t val) {
        std::string new_str = To<std::string>(val);
        bool redraw = val_string.length() != new_str.length();
        val_string = std::move(new_str);
        if(redraw)
            SetRedraw(true);
        else
            SetUpdate();
    }
    
    void FloatingNumber::SetValueStr(const std::string& val_str) {
        bool redraw = val_string.length() != val_str.length();
        val_string = val_str;
        if(redraw)
            SetRedraw(true);
        else
            SetUpdate();
    }
    
    int32_t FloatingSprite::GetTextureId() {
        return texture->GetTextureId();
    }
    
    void FloatingSprite::RefreshVertices() {
        vertices.resize(4);
        indices.resize(6);
        base::RenderObject2DLayout* mgr = static_cast<base::RenderObject2DLayout*>(this->manager);
        auto scr = mgr->GetScreenSize();
        v2i center = CenterPosInScreen(scr);
        for(int32_t i = 0; i < 4; ++i) {
            vertices[i].vertex = mgr->ConvScreenCoord(RotPoint(PosInScreen(points[i], points_prop[i], scr), center));
            vertices[i].texcoord = texcoord.vert[i];
            vertices[i].color = color;
            vertices[i].hcolor = hl;
        }
        vt2::GenQuadIndex(indices.data(), 1, vert_index);
    }
    
    void FloatingSprite::BuildSprite(recti rct, texf4 tex, rectf rct_prop) {
        points[0] = {rct.left, rct.top};
        points[1] = {rct.left + rct.width, rct.top};
        points[2] = {rct.left, rct.top + rct.height};
        points[3] = {rct.left + rct.width, rct.top + rct.height};
        points_prop[0] = {rct_prop.left, rct_prop.top};
        points_prop[1] = {rct_prop.left + rct_prop.width, rct_prop.top};
        points_prop[2] = {rct_prop.left, rct_prop.top + rct_prop.height};
        points_prop[3] = {rct_prop.left + rct_prop.width, rct_prop.top + rct_prop.height};
        texcoord = tex;
        center_pos = {rct.left + rct.width / 2, rct.top + rct.height / 2};
        center_prop = {rct_prop.left + rct_prop.width / 2, rct_prop.top + rct_prop.height / 2};
        SetUpdate();
    }
    
    void FloatingSprite::BuildSprite(v2i* verts, texf4 tex, v2f* vert_prop) {
        for(int32_t i = 0; i < 4; ++i) {
            points[i] = verts[i];
            if(vert_prop)
                points_prop[i] = vert_prop[i];
        }
        texcoord = tex;
        SetUpdate();
    }
    
    void FieldRenderer::PushVerticesAll() {
        base::RenderObject<vt3>::PushVerticesAll();
        for(auto& iter : g_player[0].field_blocks)
            iter->PushVertices();
        for(auto& iter : g_player[1].field_blocks)
            iter->PushVertices();
    }
    
    void FieldCardRenderer::PushVerticesAll() {
        base::RenderObject<vt3>::PushVerticesAll();
        for(auto& iter : g_player[1].hand)
            iter->PushVertices();
        for(auto& iter : g_player[1].deck)
            iter->PushVertices();
        for(auto& iter : g_player[1].extra)
            iter->PushVertices();
        for(auto& iter : g_player[1].s_zone)
            if(iter)
                iter->PushVertices();
        for(auto& iter : g_player[1].m_zone) {
            if(iter) {
                for(auto& att : iter->attached_cards)
                    att->PushVertices();
                iter->PushVertices();
            }
        }
        for(auto& iter : g_player[1].grave)
            iter->PushVertices();
        for(auto& iter : g_player[1].banished)
            iter->PushVertices();

        for(auto& iter : g_player[0].grave)
            iter->PushVertices();
        for(auto& iter : g_player[0].banished)
            iter->PushVertices();
        for(auto& iter : g_player[0].m_zone) {
            if(iter) {
                for(auto& att : iter->attached_cards)
                    att->PushVertices();
                iter->PushVertices();
            }
        }
        for(auto& iter : g_player[0].s_zone)
            if(iter)
                iter->PushVertices();
        for(auto& iter : g_player[0].deck)
            iter->PushVertices();
        for(auto& iter : g_player[0].extra)
            iter->PushVertices();
        for(auto& iter : g_player[0].hand)
            iter->PushVertices();
    }
    
    DuelScene::DuelScene(base::Shader* _2dshader, base::Shader* _3dshader) {
        vparam.fovy = layoutCfg["fovy"].to_value<float>();
        vparam.cnear = layoutCfg["near"].to_value<float>();
        vparam.cfar = layoutCfg["far"].to_value<float>();
        vparam.angle = layoutCfg["angle"].to_value<float>();
        vparam.radius = layoutCfg["radius"].to_value<float>();
        vparam.xoffset = layoutCfg["xoffset"].to_value<float>();
        vparam.yoffset = layoutCfg["yoffset"].to_value<float>();
        vparam.cardrect = sgui::SGJsonUtil::ConvertRect<float>(layoutCfg["card"]);
        vparam.handmin = layoutCfg["handmin"].to_value<float>();
        vparam.handmax = layoutCfg["handmax"].to_value<float>();
        vparam.handy[0] = layoutCfg["handy1"].to_value<float>();
        vparam.handy[1] = layoutCfg["handy2"].to_value<float>();
        
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
        fieldsprite_renderer = std::make_shared<FieldSpriteRenderer>();
        fieldsprite_renderer->SetShader(_3dshader);
        fieldsprite_renderer->SetShaderSettingCallback([](base::Shader* shader) {
            shader->SetParam1i("texid", 0);
            shader->SetParamMat4("mvp", glm::value_ptr(vparam.mvp));
        });
        floatingobject_renderer = std::make_shared<FloatingObjectRenderer>();
        floatingobject_renderer->SetShader(_2dshader);
        PushObject(bg_renderer.get());
        PushObject(field_renderer.get());
        PushObject(fieldcard_renderer.get());
        PushObject(fieldsprite_renderer.get());
        PushObject(floatingobject_renderer.get());
        PushObject(sgui::SGGUIRoot::GetSingleton());
        bg_renderer->ClearVertices();
        bg_renderer->AddVertices(ImageMgr::Get().GetRawBGTexture(), rectf{-1.0f, 1.0f, 2.0f, -2.0f}, ImageMgr::Get().GetTexture("bg"));
        scene_handler = std::make_shared<DuelSceneHandler>(this);
    }
    
    DuelScene::~DuelScene() {
    }
    
    void DuelScene::Activate() {
        scene_handler->BeginHandler();
    }
    
    void DuelScene::Terminate() {
        scene_handler->EndHandler();
        sgui::SGGUIRoot::GetSingleton().ClearChilds();
    }
    
    bool DuelScene::Update() {
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
        floatingobject_renderer->SetScreenSize(sz);
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
        for(auto& iter : g_player[0].hand)
            iter->UpdatePosition(0);
        for(auto& iter : g_player[1].hand)
            iter->UpdatePosition(0);
        auto set_number_pos = [this](FloatingNumber* num, FieldBlock* pb, int32_t rel, v2i off = {0, 0}) {
            v2f pos = pb->GetCenter() + v2f{0.0f, pb->sprite_size.y / 2.0f * rel};
            glm::vec4 scr = vparam.mvp * glm::vec4(pos.x, pos.y, 0.0f, 1.0f);
            scr /= scr.w;
            num->SetCenter({(int32_t)((scr.x + 1.0f) / 2.0f * viewport.width) + off.x, (int32_t)((1.0f - scr.y) / 2.0f * viewport.height) + off.y});
        };
        static const int32_t relp[] = {-1, 1};
        static const v2i offsetA[] = {{0, -7}, {0, -9}};
        static const v2i offsetD[] = {{0, 10}, {0, 8}};
        for(int32_t p = 0; p < 2; ++p) {
            set_number_pos(g_player[p].fixed_numbers[ 0].get(), g_player[p].field_blocks[13].get(), relp[p]);
            set_number_pos(g_player[p].fixed_numbers[ 1].get(), g_player[p].field_blocks[15].get(), relp[p]);
            set_number_pos(g_player[p].fixed_numbers[ 2].get(), g_player[p].field_blocks[16].get(), relp[p]);
            set_number_pos(g_player[p].fixed_numbers[ 3].get(), g_player[p].field_blocks[14].get(), relp[p]);
            set_number_pos(g_player[p].fixed_numbers[ 4].get(), g_player[p].field_blocks[11].get(), relp[p]);
            set_number_pos(g_player[p].fixed_numbers[ 5].get(), g_player[p].field_blocks[12].get(), relp[p]);
            set_number_pos(g_player[p].fixed_numbers[ 6].get(), g_player[p].field_blocks[ 0].get(), relp[1]);
            set_number_pos(g_player[p].fixed_numbers[ 7].get(), g_player[p].field_blocks[ 1].get(), relp[1]);
            set_number_pos(g_player[p].fixed_numbers[ 8].get(), g_player[p].field_blocks[ 2].get(), relp[1]);
            set_number_pos(g_player[p].fixed_numbers[ 9].get(), g_player[p].field_blocks[ 3].get(), relp[1]);
            set_number_pos(g_player[p].fixed_numbers[10].get(), g_player[p].field_blocks[ 4].get(), relp[1]);
            set_number_pos(g_player[p].fixed_numbers[11].get(), g_player[p].field_blocks[ 0].get(), relp[0], offsetA[p]);
            set_number_pos(g_player[p].fixed_numbers[12].get(), g_player[p].field_blocks[ 1].get(), relp[0], offsetA[p]);
            set_number_pos(g_player[p].fixed_numbers[13].get(), g_player[p].field_blocks[ 2].get(), relp[0], offsetA[p]);
            set_number_pos(g_player[p].fixed_numbers[14].get(), g_player[p].field_blocks[ 3].get(), relp[0], offsetA[p]);
            set_number_pos(g_player[p].fixed_numbers[15].get(), g_player[p].field_blocks[ 4].get(), relp[0], offsetA[p]);
            set_number_pos(g_player[p].fixed_numbers[16].get(), g_player[p].field_blocks[ 0].get(), relp[0], offsetD[p]);
            set_number_pos(g_player[p].fixed_numbers[17].get(), g_player[p].field_blocks[ 1].get(), relp[0], offsetD[p]);
            set_number_pos(g_player[p].fixed_numbers[18].get(), g_player[p].field_blocks[ 2].get(), relp[0], offsetD[p]);
            set_number_pos(g_player[p].fixed_numbers[19].get(), g_player[p].field_blocks[ 3].get(), relp[0], offsetD[p]);
            set_number_pos(g_player[p].fixed_numbers[20].get(), g_player[p].field_blocks[ 4].get(), relp[0], offsetD[p]);
        }
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
            if(g_player[0].field_blocks[i]->CheckInside(px, py))
                return {1, i};
            if(g_player[1].field_blocks[i]->CheckInside(px, py))
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
            int32_t ct = (int32_t)g_player[0].hand.size();
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
            int32_t ct = (int32_t)g_player[1].hand.size();
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
    
    v2i DuelScene::ProjectToScreen(v3f coods) {
        glm::vec4 scr = vparam.mvp * glm::vec4(coods.x, coods.y, coods.z, 1.0f);
        scr /= scr.w;
        int32_t x = (int32_t)((scr.x + 1.0f) / 2.0f * viewport.width);
        int32_t y = (int32_t)((-scr.y + 1.0f) / 2.0f * viewport.height);
        return {x, y};
    }
    
    CardPosInfo DuelScene::GetHoverCardPos(v2i hp) {
        if(hp.x == 0)
            return CardPosInfo(0, 0, 0, 0);
        if(hp.x > 2)
            return CardPosInfo(hp.x - 3, 0x2, hp.y, 0);
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
        return CardPosInfo(side, loc, seq, 0);
    }
    
    std::shared_ptr<FieldCard> DuelScene::GetHoverCard(CardPosInfo pos_info) {
        if(pos_info.location == 0)
            return nullptr;
        switch(pos_info.location) {
            case 0x1: return g_player[pos_info.controler].deck.empty() ? nullptr : g_player[pos_info.controler].deck.back();
            case 0x2: return g_player[pos_info.controler].hand.empty() ? nullptr : g_player[pos_info.controler].hand[pos_info.sequence];
            case 0x4: return g_player[pos_info.controler].m_zone[pos_info.sequence];
            case 0x8: return g_player[pos_info.controler].s_zone[pos_info.sequence];
            case 0x10: return g_player[pos_info.controler].grave.empty() ? nullptr : g_player[pos_info.controler].grave.back();
            case 0x20: return g_player[pos_info.controler].banished.empty() ? nullptr : g_player[pos_info.controler].banished.back();
            case 0x40: return g_player[pos_info.controler].extra.empty() ? nullptr : g_player[pos_info.controler].extra.back();
        }
        return nullptr;
    }
    
    std::shared_ptr<FieldBlock> DuelScene::GetFieldBlock(int32_t x, int32_t y) {
        if(x == 1 || x == 2)
            if(y < 17)
                return g_player[x - 1].field_blocks[y];
        return nullptr;
    }
    
    void DuelScene::ClearAllCards() {
        fieldcard_renderer->ClearAllObjects();
        ImageMgr::Get().UnloadAllCardTexture();
    }
    
}

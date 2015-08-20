#include "sgui.h"

namespace sgui
{

    std::pair<bool, bool> RegionObject::OnPositionSizeChange(bool re_pos, bool re_size) {
        auto pre_pos = area_pos.absolute;
        auto pre_size = area_size.absolute;
        if(container) {
            auto offset = container->GetAbsolutePosition();
            auto pprop = container->GetAbsoluteSize();
            if(re_size)
                area_size.absolute = area_size.relative + DotFactor(pprop, area_size.proportion);
            if(re_pos)
                area_pos.absolute = area_pos.relative + offset + DotFactor(pprop, area_pos.proportion) + DotFactor(area_size.absolute, self_factor);
        } else {
            if(re_size)
                area_size.absolute = area_size.relative;
            if(re_pos)
                area_pos.absolute = area_pos.relative + DotFactor(area_size.absolute, self_factor);
        }
        return std::make_pair(pre_pos != area_pos.absolute, pre_size != area_size.absolute);
    }
    
    std::pair<bool, bool> UIComponent::OnPositionSizeChange(bool re_pos, bool re_size) {
        auto ret = RegionObject::OnPositionSizeChange(re_pos, re_size);
        if(ret.first || ret.second)
            SetUpdate();
        return ret;
    }
    
    bool UIVertexList::CheckAvailable() { return texture != nullptr; }
    int32_t UIVertexList::GetTextureId() { return texture ? texture->GetTextureId() : 0; }
    
    int32_t UIConvexRegion::GetPrimitiveType() { return GL_TRIANGLES; }
    
    void UIConvexRegion::RefreshVertices() {
        size_t vsize = points.size();
        if(vsize < 3) {
            vertices.clear();
            indices.clear();
        } else {
            vertices.resize(vsize);
            indices.resize(vsize * 3 - 6);
            for(size_t i = 0; i < vsize; ++i) {
                vertices[i].vertex = CalUIVertex(points[i]);
                vertices[i].texcoord = points[i].texcoord;
                vertices[i].color = points[i].color;
            }
            for(size_t i = 0; i < vsize - 2; ++i) {
                indices[i * 3] = vert_index;
                indices[i * 3 + 1] = vert_index + i + 1;
                indices[i * 3 + 2] = vert_index + i + 2;
            }
        }
    }
    
    int32_t UISpriteList::GetPrimitiveType() { return GL_TRIANGLES; }
    
    void UISpriteList::RefreshVertices() {
        static const int16_t quad_idx[] = {0, 1, 2, 2, 1, 3};
        for(size_t i = 0; i < points.size(); ++i) {
            vertices[i].vertex = CalUIVertex(points[i]);
            vertices[i].texcoord = points[i].texcoord;
            vertices[i].color = points[i].color;
        }
        size_t pt_size = points.size() / 4;
        for(size_t i = 0; i < pt_size; ++i) {
            for(size_t j = 0; j < 6; ++j)
                indices[i * 6 + j] = vert_index + i * 4 + quad_idx[j];
        }
        for(size_t i = pt_size * 6; i < indices.size(); ++i)
            indices[i] = 0;
    }
    
    void UISpriteList::CheckCapacity(int32_t inc) {
        auto cur_size = points.size() / 4;
        if(cur_size + inc > capacity) {
            while(cur_size + inc > capacity) {
                if(cur_size == 0)
                    capacity = 8;
                else
                    capacity = capacity * 1.5;
            }
            vertices.resize(capacity * 4);
            indices.resize(capacity * 6);
            points.resize(capacity * 4);
            SetRedraw(true);
        } else
            SetUpdate();
    }
    
    bool UISprite::CheckAvailable() { return texture != nullptr; }
    int32_t UISprite::GetPrimitiveType() { return GL_TRIANGLES; }
    int32_t UISprite::GetTextureId() { return texture ? texture->GetTextureId() : 0; }
    
    void UISprite::RefreshVertices() {
        vertices.resize(4);
        indices.resize(6);
        FillQuad(&vertices[0], &indices[0]);
    }
    
    void UISprite::FillQuad(vt2* v, int16_t* idx) {
        static const int16_t quad_idx[] = {0, 1, 2, 2, 1, 3};
        v[0].vertex = ConvScreenCoord({area_pos.absolute.x, area_pos.absolute.y});
        v[1].vertex = ConvScreenCoord({area_pos.absolute.x + area_size.absolute.x, area_pos.absolute.y});
        v[2].vertex = ConvScreenCoord({area_pos.absolute.x, area_pos.absolute.y + area_size.absolute.y});
        v[3].vertex = ConvScreenCoord({area_pos.absolute.x + area_size.absolute.x, area_pos.absolute.y + area_size.absolute.y});
        v[0].texcoord = texture->ConvTexCoord({tex_rect.left, tex_rect.top});
        v[1].texcoord = texture->ConvTexCoord({tex_rect.left + tex_rect.width, tex_rect.top});
        v[2].texcoord = texture->ConvTexCoord({tex_rect.left, tex_rect.top + tex_rect.height});
        v[3].texcoord = texture->ConvTexCoord({tex_rect.left + tex_rect.width, tex_rect.top + tex_rect.height});
        for(int16_t i = 0; i < 6; ++i)
            idx[i] = vert_index + quad_idx[i];
        for(int16_t i = 0; i < 4; ++i)
            v[i].color = color;
    }
    
    void UISprite9::RefreshVertices() {
        vertices.resize(20);
        indices.resize(60);
        FillQuad9(&vertices[0], &indices[0]);
    }
    
    void UISprite9::FillQuad9(vt2* v, int16_t* idx) {
        static const int16_t quad9_idx[] = {    16,17,18,18,17,19,
            0, 1, 4, 4, 1, 5, 1, 2, 5, 5, 2, 6, 2, 3, 6, 6, 3, 7,
            4, 5, 8, 8, 5, 9, 5, 6, 9, 9, 6, 10,6, 7, 10,10,7, 11,
            8, 9, 12,12,9, 13,9, 10,13,13,10,14,10,11,14,14,11,15
        };
        v2f verts[4], texcoord[4];
        verts[0] = ConvScreenCoord(area_pos.absolute);
        verts[1] = ConvScreenCoord(area_pos.absolute + v2i{border.left, border.top});
        verts[2] = ConvScreenCoord(area_pos.absolute + area_size.absolute - v2i{border.width, border.height});
        verts[3] = ConvScreenCoord(area_pos.absolute + area_size.absolute);
        texcoord[0] = texture->ConvTexCoord({tex_rect.left, tex_rect.top});
        texcoord[1] = texture->ConvTexCoord({tex_rect.left + border_tex.left, tex_rect.top + border_tex.top});
        texcoord[2] = texture->ConvTexCoord({tex_rect.left + tex_rect.width - border_tex.width, tex_rect.top + tex_rect.height - border_tex.height});
        texcoord[3] = texture->ConvTexCoord({tex_rect.left + tex_rect.width, tex_rect.top + tex_rect.height});
        for(int32_t i = 0; i < 4; ++i) {
            for(int32_t j = 0; j < 4; ++j) {
                v[i * 4 + j].vertex = v2f{verts[j].x, verts[i].y};
                v[i * 4 + j].texcoord = v2f{texcoord[j].x, texcoord[i].y};
            }
        }
        v[16].vertex = ConvScreenCoord(area_pos.absolute + v2i{back.left, back.top});
        v[17].vertex = ConvScreenCoord(area_pos.absolute + v2i{area_size.absolute.x - back.width, back.top});
        v[18].vertex = ConvScreenCoord(area_pos.absolute + v2i{back.left, area_size.absolute.y - back.height});
        v[19].vertex = ConvScreenCoord(area_pos.absolute + area_size.absolute - v2i{back.width, back.height});
        v[16].texcoord = texture->ConvTexCoord({back_tex.left, back_tex.top});
        v[17].texcoord = texture->ConvTexCoord({back_tex.left + back_tex.width, back_tex.top});
        v[18].texcoord = texture->ConvTexCoord({back_tex.left, back_tex.top + back_tex.height});
        v[19].texcoord = texture->ConvTexCoord({back_tex.left + back_tex.width, back_tex.top + back_tex.height});
        for(int32_t i = 0; i < 60; ++i)
            idx[i] = vert_index + quad9_idx[i];
        for(int16_t i = 0; i < 20; ++i)
            v[i].color = color;
    }
    
    bool UIText::CheckAvailable() { return text_font != nullptr; }
    int32_t UIText::GetPrimitiveType() { return GL_TRIANGLES; }
    int32_t UIText::GetTextureId() { return text_font ? text_font->GetTexture().GetTextureId() : 0; }
    
    std::pair<bool, bool> UIText::OnPositionSizeChange(bool re_pos, bool re_size) {
        auto pre_pos = area_pos.absolute;
        if(re_pos) {
            if(container) {
                auto offset = container->GetAbsolutePosition();
                auto pprop = container->GetAbsoluteSize();
                area_pos.absolute = area_pos.relative + offset + DotFactor(pprop, area_pos.proportion) + DotFactor(area_size.absolute, self_factor);
            } else
                area_pos.absolute = area_pos.relative + DotFactor(area_size.absolute, self_factor);
            if(pre_pos != area_pos.absolute)
                SetUpdate();
        }
        return std::make_pair(pre_pos != area_pos.absolute, false);
    }
    
    void UIText::RefreshVertices() {
        static const int16_t quad_idx[] = {0, 1, 2, 2, 1, 3};
        advance.x = 0;
        advance.y = 0;
        line_spacing = 0;
        int32_t actual_size = 0;
        for(size_t i = 0; i < texts.length(); ++i) {
            auto ch = texts[i];
            if(ch < L' ') {
                if(ch == L'\n') {
                    advance.x = 0;
                    advance.y += line_spacing;
                    line_spacing = 0;
                }
                continue;
            }
            auto color = colors[i];
            auto& gl = text_font->GetGlyph(ch);
            if((uint32_t)(advance.x + gl.advance) > max_width) {
                advance.x = 0;
                advance.y += line_spacing;
                line_spacing = 0;
            }
            auto final_offset = area_pos.absolute + advance + offset;
            auto v = &vertices[actual_size * 4];
            auto idx = &indices[actual_size * 6];
            v[0].vertex = ConvScreenCoord(v2i{gl.bounds.left, gl.bounds.top} + final_offset);
            v[1].vertex = ConvScreenCoord(v2i{gl.bounds.left + gl.bounds.width, gl.bounds.top} + final_offset);
            v[2].vertex = ConvScreenCoord(v2i{gl.bounds.left, gl.bounds.top + gl.bounds.height} + final_offset);
            v[3].vertex = ConvScreenCoord(v2i{gl.bounds.left + gl.bounds.width, gl.bounds.top + gl.bounds.height} + final_offset);
            v[0].texcoord = text_font->GetTexture().ConvTexCoord({gl.textureRect.left, gl.textureRect.top});
            v[1].texcoord = text_font->GetTexture().ConvTexCoord({gl.textureRect.left + gl.textureRect.width, gl.textureRect.top});
            v[2].texcoord = text_font->GetTexture().ConvTexCoord({gl.textureRect.left, gl.textureRect.top + gl.textureRect.height});
            v[3].texcoord = text_font->GetTexture().ConvTexCoord({gl.textureRect.left + gl.textureRect.width, gl.textureRect.top + gl.textureRect.height});
            for(int16_t j = 0; j < 6; ++j)
                idx[j] = vert_index + actual_size * 4 + quad_idx[j];
            if(!text_font->IsEmoji(ch))
                for(int16_t j = 0; j < 4; ++j)
                    v[j].color = color;
            advance.x += gl.advance;
            line_spacing = std::max(text_font->GetLineSpacing(ch), line_spacing);
            actual_size++;
        }
        for(int32_t i = vert_size; i < vert_cap; ++i) {
            auto idx = &indices[i * 6];
            for(auto j = 0; j < 6; ++j)
                idx[j] = 0;
        }
    }
    
    void UIText::EvaluateSize() {
        area_size.absolute = v2i{0, 0};
        evaluate_info = v2i{0, 0};
        text_pos.clear();
        EvaluateSize(L"");
    }
    
    void UIText::EvaluateSize(const std::wstring& txt) {
        auto pre_size = area_size.absolute;
        for(auto ch : txt) {
            text_pos.push_back(v2i{evaluate_info.x, area_size.absolute.y - evaluate_info.y});
            if(ch < L' ') {
                if((ch == L'\n') && (max_width != 0xffffffff)) {
                    if(evaluate_info.x > area_size.absolute.x)
                        area_size.absolute.x = evaluate_info.x;
                    if(evaluate_info.x == 0)
                        area_size.absolute.y += text_font->GetFontSize();
                    evaluate_info = v2i{0, 0};
                }
                continue;
            }
            auto& gl = text_font->GetGlyph(ch);
            if((max_width != 0xffffffff) && ((uint32_t)(evaluate_info.x + gl.advance) > max_width)) {
                if(evaluate_info.x > area_size.absolute.x)
                    area_size.absolute.x = evaluate_info.x;
                evaluate_info = v2i{0, 0};
            }
            evaluate_info.x += gl.advance;
            if(text_font->GetLineSpacing(ch) > evaluate_info.y) {
                area_size.absolute.y += text_font->GetLineSpacing(ch) - evaluate_info.y;
                evaluate_info.y = text_font->GetLineSpacing(ch);
            }
        }
        last_text_pos = {evaluate_info.x, area_size.absolute.y - evaluate_info.y};
        if(evaluate_info.x > area_size.absolute.x)
            area_size.absolute.x = evaluate_info.x;
        if(area_size.absolute != pre_size) {
            OnPositionSizeChange(true, true);
            if(size_cb)
                size_cb(area_size.absolute);
        }
    }
    
    void UIText::InsertText(int32_t start, const std::wstring& txt, uint32_t cl) {
        if(start >= (int32_t)texts.size())
            start = (int32_t)texts.size();
        auto new_texts = std::move(texts);
        auto new_color = std::move(colors);
        new_texts.insert(new_texts.begin() + start, txt.begin(), txt.end());
        new_color.insert(new_color.begin() + start, txt.length(), cl);
        vert_size = 0;
        EvaluateSize();
        AppendText(new_texts, 0);
        colors = std::move(new_color);
    }
    
    void UIText::RemoveText(int32_t start, int32_t len) {
        if(start >= texts.size() || len <= 0)
            return;
        if(start + len > texts.size())
            len = (int32_t)texts.size() - start;
        auto new_texts = std::move(texts);
        auto new_color = std::move(colors);
        new_texts.erase(new_texts.begin() + start, new_texts.begin() + start + len);
        new_color.erase(new_color.begin() + start, new_color.begin() + start + len);
        vert_size = 0;
        EvaluateSize();
        AppendText(new_texts, 0);
        colors = std::move(new_color);
    }
    
    void UIText::ReplaceText(int32_t start, int32_t len, const std::wstring& txt, uint32_t cl) {
        if(start >= texts.size() || len <= 0)
            return;
        if(start + len > texts.size())
            len = (int32_t)texts.size() - start;
        auto new_texts = std::move(texts);
        auto new_color = std::move(colors);
        new_texts.erase(new_texts.begin() + start, new_texts.begin() + start + len);
        new_color.erase(new_color.begin() + start, new_color.begin() + start + len);
        new_texts.insert(new_texts.begin() + start, txt.begin(), txt.end());
        new_color.insert(new_color.begin() + start, txt.length(), cl);
        vert_size = 0;
        EvaluateSize();
        AppendText(new_texts, 0);
        colors = std::move(new_color);
    }
    
    void UIText::AppendText(const std::wstring& txt, uint32_t cl) {
        int32_t app_size = 0;
        for(auto& ch : txt) {
            if(ch >= ' ')
                app_size++;
        }
        auto redraw = false;
        if(vert_size + app_size > vert_cap) {
            while(vert_size + app_size > vert_cap) {
                if(vert_cap == 0)
                    vert_cap = 8;
                else
                    vert_cap *= 2;
            }
            vertices.resize(vert_cap * 4);
            indices.resize(vert_cap * 6);
            redraw = true;
        }
        texts.append(txt);
        colors.insert(colors.end(), txt.length(), cl);
        vert_size += app_size;
        EvaluateSize(txt);
        if(redraw)
            SetRedraw(true);
        else
            SetUpdate();
    }
    
    int32_t UIText::CheckHitPositionSingleLine(int32_t x) {
        for(size_t i = 0; i < texts.size(); ++i)
            if(text_pos[i].x > x)
                return (i > 0) ? ((int32_t)i - 1) : 0;
        if(last_text_pos.x > x)
            return (int32_t)texts.size() - 1;
        else
            return (int32_t)texts.size();;
    }
    
    v2i SGJsonUtil::ConvertV2i(jaweson::JsonNode<>& node, int32_t index) {
        v2i ret = {0, 0};
        if(node.is_array()) {
            ret.x = (int32_t)node[index].to_integer();
            ret.y = (int32_t)node[index + 1].to_integer();
        }
        return ret;
    }
    
    v2f SGJsonUtil::ConvertV2f(jaweson::JsonNode<>& node, int32_t index) {
        v2f ret = {0.0f, 0.0f};
        if(node.is_array()) {
            ret.x = node[index].to_double();
            ret.y = node[index + 1].to_double();
        }
        return ret;
    }
    
    recti SGJsonUtil::ConvertRect(jaweson::JsonNode<>& node) {
        recti ret = {0, 0, 0, 0};
        if(node.is_array()) {
            ret.left = (int32_t)node[0].to_integer();
            ret.top = (int32_t)node[1].to_integer();
            ret.width = (int32_t)node[2].to_integer();
            ret.height = (int32_t)node[3].to_integer();
        }
        return ret;
    }
    
    uint32_t SGJsonUtil::ConvertRGBA(jaweson::JsonNode<>& node) {
        std::string rgba = node.to_string();
        uint32_t agbr = To<uint32_t>(rgba);
        return ((agbr >> 24) & 0xff) | (((agbr >> 8) & 0xff00)) | ((agbr << 8) & 0xff0000) | ((agbr << 24) & 0xff000000);
    }
    
    void SGJsonUtil::SetUIPositionSize(jaweson::JsonNode<>& node, RegionObject* obj, v2i offset) {
        v2i pos_relative = SGJsonUtil::ConvertV2i(node, 0);
        v2i sz_relative = SGJsonUtil::ConvertV2i(node, 2);
        v2f pos_prop = SGJsonUtil::ConvertV2f(node, 4);
        v2f sz_prop = SGJsonUtil::ConvertV2f(node, 6);
        v2f self_factor = SGJsonUtil::ConvertV2f(node, 8);
        obj->SetPositionSize(pos_relative + offset, sz_relative, pos_prop, sz_prop, self_factor);
    }
    
    bool SGClickingMgr::DragBegin(std::shared_ptr<SGClickableObject> dr, int32_t x, int32_t y) {
        if(!draging_object.expired())
            draging_object.lock()->OnDragEnd(x, y);
        draging_object = dr;
        start_point = {x, y};
        diff_value = {0, 0};
        if(dr)
            return dr->OnDragBegin(x, y);
        return false;
    }
    
    bool SGClickingMgr::DragEnd(int32_t x, int32_t y) {
        if(!draging_object.expired()) {
            auto ret = draging_object.lock()->OnDragEnd(x, y);
            draging_object.reset();
            return ret;
        }
        return false;
    }
    
    bool SGClickingMgr::DragUpdate(int32_t dx, int32_t dy) {
        if(!draging_object.expired()) {
            auto ret = draging_object.lock()->OnDragUpdate(start_point.x, start_point.y, dx, dy);
            start_point = v2i{dx, dy};
            return ret;
        }
        return false;
    }
    
}

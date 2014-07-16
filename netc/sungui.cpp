#define GLEW_STATIC
#include <GL/glew.h>
#ifdef __WXMAC__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <array>
#include <iostream>
#include <regex>

#include <wx/xml/xml.h>
#include <wx/clipbrd.h>

#include "sungui.h"

namespace sgui
{
    
    static SGGUIRoot guiRoot;
    
    bool SGWidget::CheckInside(v2i pos) {
        return pos.x >= position_abs.x && pos.x <= (position_abs.x + size_abs.x) && pos.y >= position_abs.y && pos.y <= (position_abs.y + size_abs.y);
    }
    
    void SGWidget::PostRedraw() {
        vertices_dirty = true;
    }
    
    void SGWidget::PostResize(bool res, bool rep) {
        vertices_dirty = true;
        auto ptr = parent.lock();
        v2i ssize = (ptr == nullptr) ? guiRoot.GetSceneSize() : ptr->GetClientSize();
        if(res)
            size_abs = size + v2i(ssize.x * size_prop.x, ssize.y * size_prop.y);
        if(rep) {
            position_abs = position + v2i(ssize.x * position_prop.x, ssize.y * position_prop.y);
            if(ptr != nullptr)
                position_abs += ptr->GetClientPosition();
        }
    }
    
    void SGWidget::SetPosition(v2i pos, v2f prop) {
        position = pos;
        position_prop = prop;
        PostResize(false, true);
    }
    
    void SGWidget::SetSize(v2i sz, v2f prop) {
        size = sz;
        size_prop = prop;
        PostResize(true, false);
    }
    
    void SGWidget::SetX(int x, float propx) {
        position.x = x;
        position_prop.x = propx;
        PostResize(false, true);
    }
    
    void SGWidget::SetY(int y, float propy) {
        position.y = y;
        position_prop.y = propy;
        PostResize(false, true);
    }
    
    void SGWidget::SetWidth(int w, float propw) {
        size.x = w;
        size_prop.x = propw;
        PostResize(true, false);
    }
    
    void SGWidget::SetHeight(int h, float proph) {
        size.y = h;
        size_prop.y = proph;
        PostResize(true, false);
    }
    
    void SGWidget::SetColor(unsigned int c) {
        color = c;
        vertices_dirty = true;
    }
    
    void SGWidget::SetVisible(bool v) {
        if(visible == v)
            return;
        visible = v;
        if(!parent.expired())
            parent.lock()->PostRedraw();
        else
            PostRedraw();
    }
    
    bool SGWidget::IsVisible() const {
        return visible;
    }
    
    bool SGWidget::EventMouseMove(sf::Event::MouseMoveEvent evt) {
        return eventMouseMove.TriggerEvent(*this, evt);
    }
    
    bool SGWidget::EventMouseEnter() {
        return eventMouseEnter.TriggerEvent(*this);
    }
    
    bool SGWidget::EventMouseLeave() {
        return eventMouseLeave.TriggerEvent(*this);
    }
    
    bool SGWidget::EventMouseButtonDown(sf::Event::MouseButtonEvent evt) {
        auto dr = GetDragingTarget();
        if(dr && (evt.button == sf::Mouse::Button::Left)) {
            guiRoot.ObjectDragingBegin(dr, sf::Event::MouseMoveEvent{evt.x, evt.y});
        }
        return eventMouseButtonDown.TriggerEvent(*this, evt);
    }
    
    bool SGWidget::EventMouseButtonUp(sf::Event::MouseButtonEvent evt) {
        return eventMouseButtonUp.TriggerEvent(*this, evt);
    }
    
    bool SGWidget::EventMouseWheel(sf::Event::MouseWheelEvent evt) {
        return eventMouseWheel.TriggerEvent(*this, evt);
    }
    
    bool SGWidget::EventKeyDown(sf::Event::KeyEvent evt) {
        return eventKeyDown.TriggerEvent(*this, evt);
    }
    
    bool SGWidget::EventKeyUp(sf::Event::KeyEvent evt) {
        return eventKeyUp.TriggerEvent(*this, evt);
    }
    
    bool SGWidget::EventCharEnter(sf::Event::TextEvent evt) {
        return eventCharEnter.TriggerEvent(*this, evt);
    }
    
    bool SGWidget::EventGetFocus() {
        return eventGetFocus.TriggerEvent(*this);
    }
    
    bool SGWidget::EventLostFocus() {
        return eventLostFocus.TriggerEvent(*this);
    }
    
    bool SGWidget::DragingBegin(v2i evt) {
        position_drag = evt;
        return eventDragBegin.TriggerEvent(*this, evt);
    }
    
    bool SGWidget::DragingEnd(v2i evt) {
        return eventDragBegin.TriggerEvent(*this, evt);
    }
    
    bool SGWidget::DragingUpdate(v2i evt) {
        v2i delta = evt - position_drag;
        position_drag = evt;
        return eventDragUpdate.TriggerEvent(*this, delta);
    }
    
    SGTextBase::SGTextBase() {
        glGenBuffers(2, tbo);
    }
    
    SGTextBase::~SGTextBase() {
        glDeleteBuffers(2, tbo);
    }
    
    void SGTextBase::SetFont(sf::Font* ft, unsigned int sz) {
        font = ft;
        font_size = sz;
        auto tsz = ft->getTexture(sz).getSize();
        tex_size = v2i(tsz.x, tsz.y);
        text_update = true;
        EvaluateSize();
    }
    
    void SGTextBase::SetSpacing(unsigned int x, unsigned int y) {
        spacing_x = x;
        spacing_y = y;
        text_update = true;
        EvaluateSize();
    }
    
    int SGTextBase::GetLineSpacing() {
        return font_size + spacing_y;
    }
    
    void SGTextBase::ClearText() {
        text.clear();
        color_vec.clear();
        text_pos_array.clear();
        text_update = true;
        text_width = 0;
        text_height = 0;
        text_width_cur = 0;
    }
    
    unsigned int SGTextBase::GetTextColor(int index) {
        return color_vec[index];
    }
    
    void SGTextBase::SetText(const std::wstring& t, unsigned int cl) {
        if(t.length() == 0)
            return;
        text = t;
        color_vec.clear();
        for(size_t i = 0; i < text.length(); ++i)
            color_vec.push_back(cl);
        text_update = true;
        EvaluateSize();
    }
    
    void SGTextBase::AppendText(const std::wstring& t, unsigned int cl) {
        if(t.length() == 0)
            return;
        text += t;
        for(size_t i = 0; i < t.length(); ++i)
            color_vec.push_back(cl);
        text_update = true;
        EvaluateSize(t);
    }
    
    void SGTextBase::SetTextColor(unsigned int start, unsigned int end, unsigned int cl) {
        for(int i = start; i < end && i < color_vec.size(); ++i)
            color_vec[i] = cl;
        text_update = true;
    }
    
    void SGTextBase::EvaluateSize(const std::wstring& t) {
        if(t.length() == 0) {
            text_width = 0;
            text_height = font_size;
            text_width_cur = 0;
            text_pos_array.clear();
            if(text.length() == 0)
                return;
            EvaluateSize(text);
        } else {
            int max_width = GetMaxWidth();
            int ls = GetLineSpacing();
            for(auto ch : t) {
                if(ch < L' ') {
                    text_pos_array.push_back(v2i{text_width_cur, (int)(text_height - font_size)});
                    if(ch == L'\n' && IsMultiLine()) {
                        if(text_width_cur > text_width)
                            text_width = text_width_cur;
                        text_width_cur = 0;
                        text_height += ls;
                    }
                    continue;
                }
                auto& gl = font->getGlyph(ch, font_size, false);
                if(text_width_cur + gl.advance > max_width && IsMultiLine()) {
                    if(text_width_cur > text_width)
                        text_width = text_width_cur;
                    text_width_cur = 0;
                    text_height += ls;
                }
                text_pos_array.push_back(v2i{text_width_cur, (int)(text_height - font_size)});
                text_width_cur += gl.advance + spacing_x;
            }
        }
    }
    
    void SGTextBase::UpdateTextVertex() {
        if(!text_update || font == nullptr)
            return;
        text_update = false;
        vert_size = 0;
        if(text.length() == 0)
            return;
        std::vector<SGVertexVCT> charvtx;
        std::vector<unsigned short> index;
        auto tex = font->getTexture(font_size);
        unsigned int advx = 0, advy = font_size;
        SGVertexVCT cur_char;
        int max_width = GetMaxWidth();
        int ls = GetLineSpacing();
        v2i text_pos = GetTextOffset();
        for(size_t i = 0; i < text.length(); ++i) {
            wchar_t ch = text[i];
            unsigned int color = GetTextColor((int)i);
            if(ch < L' ') {
                if(ch == L'\n' && IsMultiLine()) {
                    advx = 0;
                    advy += ls;
                }
                continue;
            }
            auto& gl = font->getGlyph(ch, font_size, false);
            if(advx + gl.advance > max_width) {
                advx = 0;
                advy += ls;
            }
            guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx, text_pos.y + gl.bounds.top + advy, cur_char.vertex);
            cur_char.color = color;
            cur_char.texcoord[0] = (float)gl.textureRect.left / tex_size.x;
            cur_char.texcoord[1] = (float)gl.textureRect.top / tex_size.y;
            charvtx.push_back(cur_char);
            guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx + gl.bounds.width, text_pos.y + gl.bounds.top + advy, cur_char.vertex);
            cur_char.color = color;
            cur_char.texcoord[0] = (float)(gl.textureRect.left + gl.textureRect.width) / tex_size.x;
            cur_char.texcoord[1] = (float)gl.textureRect.top / tex_size.y;
            charvtx.push_back(cur_char);
            guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx, text_pos.y + gl.bounds.top + gl.bounds.height + advy, cur_char.vertex);
            cur_char.color = color;
            cur_char.texcoord[0] = (float)gl.textureRect.left / tex_size.x;
            cur_char.texcoord[1] = (float)(gl.textureRect.top + gl.textureRect.height) / tex_size.y;
            charvtx.push_back(cur_char);
            guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx + gl.bounds.width, text_pos.y + gl.bounds.top + gl.bounds.height + advy, cur_char.vertex);
            cur_char.color = color;
            cur_char.texcoord[0] = (float)(gl.textureRect.left + gl.textureRect.width) / tex_size.x;
            cur_char.texcoord[1] = (float)(gl.textureRect.top + gl.textureRect.height) / tex_size.y;
            charvtx.push_back(cur_char);
            advx += gl.advance + spacing_x;
            
            index.push_back(vert_size * 4);
            index.push_back(vert_size * 4 + 2);
            index.push_back(vert_size * 4 + 1);
            index.push_back(vert_size * 4 + 1);
            index.push_back(vert_size * 4 + 2);
            index.push_back(vert_size * 4 + 3);
            vert_size++;
        }
        if(vert_size > mem_size) {
            if(mem_size == 0)
                mem_size = 16;
            while(mem_size < vert_size)
                mem_size *= 2;
            glBindBuffer(GL_ARRAY_BUFFER, tbo[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(SGVertexVCT) * mem_size * 4, nullptr, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SGVertexVCT) * charvtx.size(), &charvtx[0]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tbo[1]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, mem_size * 6 * sizeof(unsigned short), nullptr, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, index.size() * sizeof(unsigned short), &index[0]);
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, tbo[0]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SGVertexVCT) * charvtx.size(), &charvtx[0]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tbo[1]);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, index.size() * sizeof(unsigned short), &index[0]);
        }
    }
    
    void SGTextBase::DrawText() {
        UpdateTextVertex();
        if(vert_size == 0)
            return;
        guiRoot.BindFontTexture(font_size);
        glBindBuffer(GL_ARRAY_BUFFER, tbo[0]);
        glVertexPointer(2, GL_FLOAT, sizeof(SGVertexVCT), 0);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(SGVertexVCT), (const GLvoid*)SGVertexVCT::color_offset);
        glTexCoordPointer(2, GL_FLOAT, sizeof(SGVertexVCT), (const GLvoid*)SGVertexVCT::tex_offset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tbo[1]);
        glDrawElements(GL_TRIANGLES, vert_size * 6, GL_UNSIGNED_SHORT, 0);
    }
    
    SGSpriteBase::~SGSpriteBase() {
        if(img_texture)
            glDeleteBuffers(2, imgvbo);
    }
    
    void SGSpriteBase::SetImage(sf::Texture* img, sf::IntRect varea) {
        if(img) {
            if(!img_texture)
                glGenBuffers(2, imgvbo);
        } else {
            if(img_texture)
                glDeleteBuffers(2, imgvbo);
        }
        img_texture = img;
        img_update = true;
        if(!img)
            return;
        verts.clear();
        texcoords.clear();
        verts.push_back(v2i{varea.left, varea.top});
        verts.push_back(v2i{varea.left, varea.top + varea.height});
        verts.push_back(v2i{varea.left + varea.width, varea.top + varea.height});
        verts.push_back(v2i{varea.left + varea.width, varea.top});
    }
    
    void SGSpriteBase::AddTexRect(sf::IntRect tarea) {
        if(!img_texture)
            return;
        img_update = true;
        img_dirty = true;
        std::vector<v2i> tex;
        tex.push_back(v2i{tarea.left, tarea.top});
        tex.push_back(v2i{tarea.left, tarea.top + tarea.height});
        tex.push_back(v2i{tarea.left + tarea.width, tarea.top + tarea.height});
        tex.push_back(v2i{tarea.left + tarea.width, tarea.top});
        texcoords.push_back(tex);
    }
    
    void SGSpriteBase::SetImage(sf::Texture* img, std::vector<v2i>& vtx) {
        if(img) {
            if(!img_texture)
                glGenBuffers(2, imgvbo);
        } else {
            if(img_texture)
                glDeleteBuffers(2, imgvbo);
        }
        img_texture = img;
        img_update = true;
        if(!img)
            return;
        verts.clear();
        texcoords.clear();
        verts = vtx;
    }
    
    void SGSpriteBase::AddTexcoord(std::vector<v2i>& tex) {
        if(!img_texture || verts.size() == 0 || tex.size() != verts.size())
            return;
        texcoords.push_back(tex);
        img_update = true;
        img_dirty = true;
    }
    
    void SGSpriteBase::UpdateImage() {
        if(!img_texture)
            return;
        if(!img_update)
            return;
        img_update = false;
        std::vector<SGVertexVCT> vert;
        std::vector<unsigned short> index;
        auto sz = img_texture->getSize();
        SGVertexVCT cur;
        v2i pos = GetImageOffset();
        for(int ti = 0; ti < texcoords.size(); ++ti) {
            for(int i = 0; i < verts.size(); ++i) {
                guiRoot.ConvertXY(verts[i].x + pos.x, verts[i].y + pos.y, cur.vertex);
                cur.texcoord[0] = (float)texcoords[ti][i].x / sz.x;
                cur.texcoord[1] = (float)texcoords[ti][i].y / sz.y;
                cur.color = 0xffffffff;
                vert.push_back(cur);
            }
        }
        for(int i = 0; i < verts.size(); ++i)
            index.push_back(i);
        if(img_dirty) {
            glBindBuffer(GL_ARRAY_BUFFER, imgvbo[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(SGVertexVCT) * vert.size() + texcoords.size(), &vert[0], GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, imgvbo[1]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(unsigned short), &index[0], GL_DYNAMIC_DRAW);
            img_dirty = false;
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, imgvbo[0]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SGVertexVCT) * vert.size() + texcoords.size(), &vert[0]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, imgvbo[1]);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, index.size() * sizeof(unsigned short), &index[0]);
        }
    }
    
    void SGSpriteBase::DrawImage() {
        if(!img_texture)
            return;
        UpdateImage();
        long voffset = 0;
        if(texcoords.size() > 1)
            voffset = ((int)(guiRoot.GetTime().asSeconds() / frame_time) % texcoords.size() * verts.size()) * sizeof(SGVertexVCT);
        guiRoot.BindTexture(img_texture);
        glBindBuffer(GL_ARRAY_BUFFER, imgvbo[0]);
        glVertexPointer(2, GL_FLOAT, sizeof(SGVertexVCT), 0);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(SGVertexVCT), (const GLvoid*)(SGVertexVCT::color_offset + voffset));
        glTexCoordPointer(2, GL_FLOAT, sizeof(SGVertexVCT), (const GLvoid*)(SGVertexVCT::tex_offset + voffset));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, imgvbo[1]);
        glDrawElements(GL_TRIANGLE_FAN, (int)verts.size(), GL_UNSIGNED_SHORT, 0);
    }
    
    SGWidgetContainer::~SGWidgetContainer() {
        children.clear();
    }
    
    std::shared_ptr<SGWidget> SGWidgetContainer::GetHovingWidget(v2i pos) {
        for(auto iter = children.rbegin(); iter != children.rend(); ++iter) {
            if((*iter)->IsVisible() && (*iter)->CheckInside(pos))
                return *iter;
        }
        return nullptr;
    }
    
    void SGWidgetContainer::PostResize(bool res, bool rep) {
        vertices_dirty = true;
        auto ptr = parent.lock();
        v2i ssize = (ptr == nullptr) ? guiRoot.GetSceneSize() : ptr->GetClientSize();
        if(res)
            size_abs = size + v2i(ssize.x * size_prop.x, ssize.y * size_prop.y);
        if(rep) {
            position_abs = position + v2i(ssize.x * position_prop.x, ssize.y * position_prop.y);
            if(ptr != nullptr)
                position_abs += ptr->GetClientPosition();
        }
        if(res)
            rep = true;
        for(auto& chd : children)
            chd->PostResize(res, rep);
    }
    
    void SGWidgetContainer::AddChild(std::shared_ptr<SGWidget> chd) {
        children.push_back(chd);
    }
    
    void SGWidgetContainer::RemoveChild(std::shared_ptr<SGWidget> chd) {
        for(auto iter = children.begin(); iter != children.end(); ++iter) {
            if(*iter == chd) {
                children.erase(iter);
                return;
            }
        }
    }
    
    void SGWidgetContainer::BringToTop(std::shared_ptr<SGWidget> chd) {
        for(auto iter = children.begin(); iter != children.end(); ++iter) {
            if(*iter == chd) {
                if(chd != children.back()) {
                    children.erase(iter);
                    children.push_back(chd);
                    return;
                }
            }
        }
    }
    
    bool SGWidgetContainer::EventMouseMove(sf::Event::MouseMoveEvent evt) {
        auto nhoving = GetHovingWidget(v2i{evt.x, evt.y});
        bool e = eventMouseMove.TriggerEvent(static_cast<SGWidget&>(*this), evt);
        auto choving = hoving.lock();
        if(nhoving) {
            if(choving) {
                if(choving == nhoving)
                    choving->EventMouseMove(evt);
                else {
                    choving->EventMouseLeave();
                    nhoving->EventMouseEnter();
                    nhoving->EventMouseMove(evt);
                }
            } else {
                nhoving->EventMouseEnter();
                nhoving->EventMouseMove(evt);
            }
        } else {
            if(choving)
                choving->EventMouseLeave();
        }
        hoving = nhoving;
        return e || (choving != nullptr);
    }
    
    bool SGWidgetContainer::EventMouseEnter() {
        hoving.reset();
        return eventMouseEnter.TriggerEvent(*this);
    }
    
    bool SGWidgetContainer::EventMouseLeave() {
        bool evt = eventMouseLeave.TriggerEvent(*this);
        if(!hoving.expired())
            hoving.lock()->EventMouseLeave();
        hoving.reset();
        return evt;
    }
    
    bool SGWidgetContainer::EventMouseButtonDown(sf::Event::MouseButtonEvent evt) {
        auto choving = hoving.lock();
        if(choving) {
            auto fwidget = focus_widget.lock();
            if(choving != fwidget && choving->AllowFocus()) {
                if(fwidget)
                    fwidget->EventLostFocus();
                choving->EventGetFocus();
                focus_widget = hoving;
            }
            choving->EventMouseButtonDown(evt);
            return true;
        } else {
            auto dr = GetDragingTarget();
            if(dr && (evt.button == sf::Mouse::Button::Left))
                guiRoot.ObjectDragingBegin(dr, sf::Event::MouseMoveEvent{evt.x, evt.y});
            return eventMouseButtonDown.TriggerEvent(*this, evt);
        }
    }
    
    bool SGWidgetContainer::EventMouseButtonUp(sf::Event::MouseButtonEvent evt) {
        if(!hoving.expired()) {
            hoving.lock()->EventMouseButtonUp(evt);
            return true;
        } else {
            return eventMouseButtonUp.TriggerEvent(*this, evt);
        }
    }
    
    bool SGWidgetContainer::EventMouseWheel(sf::Event::MouseWheelEvent evt) {
        if(!hoving.expired()) {
            hoving.lock()->EventMouseWheel(evt);
            return true;
        } else {
            return eventMouseWheel.TriggerEvent(*this, evt);
        }
        return true;
    }
    
    bool SGWidgetContainer::EventKeyDown(sf::Event::KeyEvent evt) {
        if(!focus_widget.expired())
            return focus_widget.lock()->EventKeyDown(evt);
        return false;
    }
    
    bool SGWidgetContainer::EventKeyUp(sf::Event::KeyEvent evt) {
        if(!focus_widget.expired())
            return focus_widget.lock()->EventKeyUp(evt);
        return false;
    }
    
    bool SGWidgetContainer::EventCharEnter(sf::Event::TextEvent evt) {
        if(!focus_widget.expired())
            return focus_widget.lock()->EventCharEnter(evt);
        return false;
    }
    
    bool SGWidgetWrapper::EventMouseMove(sf::Event::MouseMoveEvent evt) {
        SGWidgetContainer::EventMouseMove(evt);
        return true;
    }
    
    bool SGWidgetWrapper::EventMouseEnter() {
        SGWidgetContainer::EventMouseEnter();
        return true;
    }
    
    bool SGWidgetWrapper::EventMouseLeave() {
        SGWidgetContainer::EventMouseLeave();
        return true;
    }
    
    bool SGWidgetWrapper::EventMouseButtonDown(sf::Event::MouseButtonEvent evt) {
        SGWidgetContainer::EventMouseButtonDown(evt);
        return true;
    }
    
    bool SGWidgetWrapper::EventMouseButtonUp(sf::Event::MouseButtonEvent evt) {
        SGWidgetContainer::EventMouseButtonUp(evt);
        return true;
    }
    
    bool SGWidgetWrapper::EventMouseWheel(sf::Event::MouseWheelEvent evt) {
        SGWidgetContainer::EventMouseWheel(evt);
        return true;
    }
    
    bool SGWidgetWrapper::EventKeyDown(sf::Event::KeyEvent evt) {
        SGWidgetContainer::EventKeyDown(evt);
        return true;
    }
    
    bool SGWidgetWrapper::EventKeyUp(sf::Event::KeyEvent evt) {
        SGWidgetContainer::EventKeyUp(evt);
        return true;
    }
    
    bool SGWidgetWrapper::EventCharEnter(sf::Event::TextEvent evt) {
        SGWidgetContainer::EventCharEnter(evt);
        return true;
    }
    
    SGConfig SGGUIRoot::basic_config;
    
    SGGUIRoot::~SGGUIRoot() {
        
    }
    
    void SGGUIRoot::SetSceneSize(v2i size) {
        scene_size = size;
        PostRedraw();
        PostResize(true, true);
    }
    
    v2i SGGUIRoot::GetSceneSize() {
        return scene_size;
    }
    
    void SGGUIRoot::BeginScissor(sf::IntRect rt) {
        if(scissor_stack.size() == 0)
            glEnable(GL_SCISSOR_TEST);
        else {
            auto prt = scissor_stack.back();
            if(rt.left < prt.left)
                rt.left = prt.left;
            if(rt.top < prt.top)
                rt.top = prt.top;
            if(rt.left + rt.width > prt.left + prt.width)
                rt.width = prt.left + prt.width - rt.left;
            if(rt.width < 0)
                rt.width = 0;
            if(rt.top + rt.height > prt.top + prt.height)
                rt.height = prt.top + prt.height - rt.top;
            if(rt.height < 0)
                rt.height = 0;
        }
        scissor_stack.push_back(rt);
        glScissor(rt.left, scene_size.y - rt.top - rt.height, rt.width, rt.height);
    }
    
    void SGGUIRoot::EndScissor() {
        scissor_stack.pop_back();
        if(scissor_stack.size() == 0)
            glDisable(GL_SCISSOR_TEST);
        else {
            sf::IntRect rt = scissor_stack.back();
            glScissor(rt.left, scene_size.y - rt.top - rt.height, rt.width, rt.height);
        }
            
    }
    
    bool SGGUIRoot::CheckInside(v2i pos) {
        return true;
    }
    
    std::shared_ptr<SGWidget> SGGUIRoot::GetHovingWidget(v2i pos) {
        for(auto iter = children.rbegin(); iter != children.rend(); ++iter) {
            if((*iter)->IsVisible() && (*iter)->CheckInside(pos))
                return *iter;
        }
        return nullptr;
    }
    
    void SGGUIRoot::UpdateVertices() {
    }
    
    void SGGUIRoot::Draw() {
        glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_INDEX_ARRAY);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glLoadIdentity();
        for(auto& iter : children)
            if(iter->IsVisible())
                iter->Draw();
        cur_texture = nullptr;
        scissor_stack.clear();
        glPopClientAttrib();
        glPopAttrib();
    }
    
    void SGGUIRoot::ObjectDragingBegin(std::shared_ptr<SGWidget> dr, sf::Event::MouseMoveEvent evt) {
        if(!draging_object.expired())
            draging_object.lock()->DragingEnd(v2i{evt.x, evt.y});
        draging_object = dr;
        if(dr)
            dr->DragingBegin(v2i{evt.x, evt.y});
    }
    
    void SGGUIRoot::ObjectDragingEnd(sf::Event::MouseMoveEvent evt) {
        if(!draging_object.expired())
            draging_object.lock()->DragingEnd(v2i{evt.x, evt.y});
        draging_object.reset();
    }
    
    void SGGUIRoot::LoadConfigs() {
        AddConfig("basic", SGGUIRoot::basic_config);
        AddConfig("window", SGWindow::window_config);
        AddConfig("sprite", SGSprite::sprite_config);
        AddConfig("button", SGButton::button_config);
        AddConfig("checkbox", SGCheckbox::checkbox_config);
        AddConfig("radio", SGRadio::radio_config);
        AddConfig("scroll", SGScrollBar::scroll_config);
        AddConfig("textedit", SGTextEdit::textedit_config);
        AddConfig("iconlabel", SGIconLabel::iconlabel_config);
        AddConfig("listbox", SGListBox::listbox_config);
        AddConfig("combobox", SGComboBox::combobox_config);
        AddConfig("tabcontrol", SGTabControl::tab_config);
        
        wxXmlDocument doc;
		if(!doc.Load("./conf/gui.xml", wxT("UTF-8"), wxXMLDOC_KEEP_WHITESPACE_NODES))
			return;
		wxXmlNode* root = doc.GetRoot();
		wxXmlNode* subtype = root->GetChildren();
		while (subtype) {
            if(subtype->GetType() != wxXmlNodeType::wxXML_ELEMENT_NODE) {
                subtype = subtype->GetNext();
                continue;
            }
            auto iter = configs.find(subtype->GetName().ToStdString());
            if(iter != configs.end()) {
                wxXmlNode* child = subtype->GetChildren();
                while(child) {
                    if(child->GetType() != wxXmlNodeType::wxXML_ELEMENT_NODE) {
                        child = child->GetNext();
                        continue;
                    }
                    if (child->GetName() == wxT("integer")) {
                        wxString name = child->GetAttribute("name");
                        wxString value = child->GetAttribute("value");
                        long val = 0;
                        value.ToLong(&val, 0);
                        iter->second->int_config[name.ToStdString()] = (int)val;
                    } else if(child->GetName() == "string") {
                        wxString name = child->GetAttribute("name");
                        wxString value = child->GetAttribute("value");
                        iter->second->string_config[name.ToStdString()] = value.ToStdString();
                    } else if(child->GetName() == "rect") {
                        wxString name = child->GetAttribute("name");
                        wxString stru = child->GetAttribute("u");
                        wxString strv = child->GetAttribute("v");
                        wxString strw = child->GetAttribute("w");
                        wxString strh = child->GetAttribute("h");
                        long u, v, w, h;
                        stru.ToLong(&u, 0);
                        strv.ToLong(&v, 0);
                        strw.ToLong(&w, 0);
                        strh.ToLong(&h, 0);
                        iter->second->tex_config[name.ToStdString()] = sf::Rect<int>{(int)u, (int)v, (int)w, (int)h};
                    } else {
                        wxString name = child->GetAttribute("name");
                        auto& vec = iter->second->coord_config[name.ToStdString()];
                        wxXmlNode* coord_child = child->GetChildren();
                        while(coord_child) {
                            if(coord_child->GetType() == wxXmlNodeType::wxXML_ELEMENT_NODE) {
                                wxString stru = child->GetAttribute("u");
                                wxString strv = child->GetAttribute("v");
                                long u, v;
                                stru.ToLong(&u, 0);
                                strv.ToLong(&v, 0);
                                vec.push_back(v2i{(int)u, (int)v});
                            }
                            coord_child = coord_child->GetNext();
                        }
                    }
                    child = child->GetNext();
                }
            }
			subtype = subtype->GetNext();
		}
        gui_font.loadFromFile(basic_config.string_config["gui_font"]);
        gui_texture.loadFromFile(basic_config.string_config["gui_texture"]);
        gui_texture.setSmooth(true);
        tex_size = v2i(gui_texture.getSize());
    }
    
    bool SGGUIRoot::InjectMouseEnterEvent() {
        inside_scene = true;
        return EventMouseEnter();
    }
    
    bool SGGUIRoot::InjectMouseLeaveEvent() {
        inside_scene = false;
        return EventMouseLeave();
    }
    
    bool SGGUIRoot::InjectMouseMoveEvent(sf::Event::MouseMoveEvent evt) {
        if(!inside_scene)
            return InjectMouseEnterEvent();
        if(!draging_object.expired())
            draging_object.lock()->DragingUpdate(v2i{evt.x, evt.y});
        return EventMouseMove(evt);
    }
    
    bool SGGUIRoot::InjectMouseButtonDownEvent(sf::Event::MouseButtonEvent evt) {
        if(!inside_scene) {
            InjectMouseEnterEvent();
            InjectMouseMoveEvent(sf::Event::MouseMoveEvent{evt.x, evt.y});
        }
        if(!hoving.expired()) {
            auto hwidget = hoving.lock();
            if(hwidget != children.back()) {
                for(auto iter = children.begin(); iter != children.end(); ++iter) {
                    if(*iter == hwidget) {
                        children.erase(iter);
                        children.push_back(hwidget);
                        break;
                    }
                }
            }
        }
        return EventMouseButtonDown(evt);
    }
    
    bool SGGUIRoot::InjectMouseButtonUpEvent(sf::Event::MouseButtonEvent evt) {
        if(!draging_object.expired() && (evt.button == sf::Mouse::Button::Left))
            draging_object.lock()->DragingEnd(v2i{evt.x, evt.y});
        draging_object.reset();
        clicking_object.reset();
        return EventMouseButtonUp(evt);
    }
    
    bool SGGUIRoot::InjectMouseWheelEvent(sf::Event::MouseWheelEvent evt) {
        return EventMouseWheel(evt);
    }
    
    bool SGGUIRoot::InjectKeyDownEvent(sf::Event::KeyEvent evt) {
        return EventKeyDown(evt);
    }
    
    bool SGGUIRoot::InjectKeyUpEvent(sf::Event::KeyEvent evt) {
        return EventKeyUp(evt);
    }
    
    bool SGGUIRoot::InjectCharEvent(sf::Event::TextEvent evt) {
        return EventCharEnter(evt);
    }
    
    SGGUIRoot& SGGUIRoot::GetSingleton() {
        return guiRoot;
    }
    
    SGConfig SGWindow::window_config;

    SGWindow::~SGWindow() {
        glDeleteBuffers(2, vbo);
    }
    
    std::shared_ptr<SGWindow> SGWindow::Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, v2i size) {
        auto ptr = std::make_shared<SGWindow>();
        ptr->parent = p;
        ptr->position = pos;
        ptr->size = size;
        ptr->PostResize(true, true);
        auto iter1 = window_config.int_config.find("gui_color");
        if(iter1 != window_config.int_config.end())
            ptr->color = iter1->second;
        else
            ptr->color = guiRoot.GetDefaultInt("gui_color");
        auto iter2 = window_config.int_config.find("font_size");
        if(iter2 != window_config.int_config.end())
            ptr->SetFont(&guiRoot.GetGUIFont(), iter2->second);
        else
            ptr->SetFont(&guiRoot.GetGUIFont(), guiRoot.GetDefaultInt("font_size"));
        glGenBuffers(2, ptr->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(SGVertexVCT) * 36, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ptr->vbo[1]);
        unsigned short index[54];
        for(int i = 0; i < 9; ++i) {
            index[i * 6] = i * 4;
            index[i * 6 + 1] = i * 4 + 2;
            index[i * 6 + 2] = i * 4 + 1;
            index[i * 6 + 3] = i * 4 + 1;
            index[i * 6 + 4] = i * 4 + 2;
            index[i * 6 + 5] = i * 4 + 3;
        }
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
        if(p != nullptr)
            p->AddChild(ptr);
        else
            guiRoot.AddChild(ptr);
        auto rt = window_config.tex_config["closep"];
        auto cbtn = SGButton::Create(ptr, {size.x - rt.left, rt.top}, {rt.width, rt.height});
        cbtn->SetPosition({-rt.left, rt.top}, {1.0f, 0.0f});
        cbtn->eventButtonClick.Bind(ptr.get(), &SGWindow::CloseButtonClick);
        cbtn->SetTextureRect(window_config.tex_config["close1"], window_config.tex_config["close2"], window_config.tex_config["close3"],
                             window_config.int_config["closelw"], window_config.int_config["closerw"]);
        cbtn->SetColor(ptr->color);
        return ptr;
    }
    
    void SGWindow::UpdateVertices() {
        if(!vertices_dirty)
            return;
        vertices_dirty = false;
        text_update = true;
        int title_h = window_config.int_config["title_h"];
        int title_lw = window_config.int_config["title_lw"];
        int title_rw = window_config.int_config["title_rw"];
        int frame_w = window_config.int_config["frame_w"];
        int frame_h = window_config.int_config["frame_h"];
        int size_w = window_config.int_config["size_w"];
        int size_h = window_config.int_config["size_h"];
        
        std::array<SGVertexVCT, 36> vert;
        guiRoot.SetRectVertex(&vert[0], position_abs.x, position_abs.y, title_lw, title_h,
                              window_config.tex_config["titlel"]);
        guiRoot.SetRectVertex(&vert[4], position_abs.x + title_lw, position_abs.y, size_abs.x - title_lw - title_rw, title_h,
                              window_config.tex_config["titlem"]);
        guiRoot.SetRectVertex(&vert[8], position_abs.x + size_abs.x - title_rw, position_abs.y, title_rw, title_h,
                              window_config.tex_config["titler"]);
        guiRoot.SetRectVertex(&vert[12], position_abs.x, position_abs.y + title_h, frame_w, size_abs.y - title_h - size_h,
                              window_config.tex_config["framel"]);
        guiRoot.SetRectVertex(&vert[16], position_abs.x + frame_w, position_abs.y + title_h, size_abs.x - frame_w * 2, size_abs.y - title_h - frame_h,
                              window_config.tex_config["back"]);
        guiRoot.SetRectVertex(&vert[20], position_abs.x + size_abs.x - frame_w, position_abs.y + title_h, frame_w, size_abs.y - title_h - size_h,
                              window_config.tex_config["framer"]);
        guiRoot.SetRectVertex(&vert[24], position_abs.x, position_abs.y + size_abs.y - size_h, size_w, size_h,
                              window_config.tex_config["lc"]);
        guiRoot.SetRectVertex(&vert[28], position_abs.x + size_w, position_abs.y + size_abs.y - frame_h, size_abs.x - size_w * 2, frame_h,
                              window_config.tex_config["frameb"]);
        guiRoot.SetRectVertex(&vert[32], position_abs.x + size_abs.x - size_w, position_abs.y + size_abs.y - size_h, size_w, size_h,
                              window_config.tex_config["rc"]);
        for(int i = 0; i < 36; ++i)
            vert[i].color = color;
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SGVertexVCT) * 36, &vert);
    }
    
    void SGWindow::Draw() {
        UpdateVertices();
        guiRoot.BindGuiTexture();
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glVertexPointer(2, GL_FLOAT, sizeof(SGVertexVCT), 0);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(SGVertexVCT), (const GLvoid*)SGVertexVCT::color_offset);
        glTexCoordPointer(2, GL_FLOAT, sizeof(SGVertexVCT), (const GLvoid*)SGVertexVCT::tex_offset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
        glDrawElements(GL_TRIANGLES, 54, GL_UNSIGNED_SHORT, 0);
        DrawText();
        guiRoot.BeginScissor({position_abs.x, position_abs.y, size_abs.x, size_abs.y});
        for(auto& iter : children)
            if(iter->IsVisible())
                iter->Draw();
        guiRoot.EndScissor();
    }
    
    v2i SGWindow::GetTextOffset() {
        return position_abs + v2i{5, 1};
    }
    
    int SGWindow::GetMaxWidth() {
        return 0xffff;
    }
    
    bool SGWindow::IsMultiLine() {
        return false;
    }
    
    void SGWindow::Destroy() {
        auto p = parent.lock();
        if(p == nullptr)
            guiRoot.RemoveChild(shared_from_this());
        else
            p->RemoveChild(shared_from_this());
    }
    
    void SGWindow::SetTitle(const std::wstring& t) {
        ClearText();
        auto iter = window_config.int_config.find("font_color");
        if(iter != window_config.int_config.end())
            AppendText(t, iter->second);
        else
            AppendText(t, guiRoot.GetDefaultInt("font_color"));
        vertices_dirty = true;
    }
    
    bool SGWindow::DragingBegin(v2i evt) {
        position_drag = evt;
        int size_w = window_config.int_config["size_w"];
        int size_h = window_config.int_config["size_h"];
        if(evt.x > position_abs.x + size_abs.x - size_w && evt.y > position_abs.y + size_abs.y - size_h)
            drag_type = 1;
        else
            drag_type = 0;
        drag_diff = {0, 0};
        return eventDragBegin.TriggerEvent(*this, evt);
    }
    
    bool SGWindow::DragingUpdate(v2i evt) {
        v2i delta = evt - position_drag;
        position_drag = evt;
        if(drag_type == 0) {
            position = position_abs;
            position_prop = {0, 0};
            SetPosition(position + delta);
            text_update = true;
        } else if(drag_type == 1) {
            int minw = window_config.int_config["min_w"];
            int minh = window_config.int_config["min_h"];
            size = size_abs;
            size_prop = {0, 0};
            size += delta;
            size += drag_diff;
            if(size.x < minw) {
                drag_diff.x = size.x - minw;
                size.x = minw;
            } else
                drag_diff.x = 0;
            if(size.y < minh) {
                drag_diff.y = size.y - minh;
                size.y = minh;
            } else
                drag_diff.y = 0;
            PostResize(true, false);
        }
        return eventDragUpdate.TriggerEvent(*this, delta);
    }
    bool SGWindow::CloseButtonClick(SGWidget& sender) {
        Destroy();
        return true;
    }
    
    SGConfig SGLabel::label_config;
    
    SGLabel::~SGLabel() {
        
    }
    
    void SGLabel::PostResize(bool res, bool rep) {
        vertices_dirty = true;
        auto ptr = parent.lock();
        v2i ssize = (ptr == nullptr) ? guiRoot.GetSceneSize() : ptr->GetSize();
        if(res) {
            size_abs = v2i{std::max(text_width_cur, text_width), text_height};
        }
        if(rep) {
            position_abs = position + v2i(ssize.x * position_prop.x, ssize.y * position_prop.y);
            if(ptr != nullptr)
                position_abs += ptr->GetPosition();
        }
    }
    
    void SGLabel::UpdateVertices() {
        if(!vertices_dirty)
            return;
        vertices_dirty = false;
        text_update = true;
    }
    
    void SGLabel::Draw() {
        UpdateVertices();
        DrawText();
    }
    
    void SGLabel::SetFont(sf::Font* ft, unsigned int sz) {
        SGTextBase::SetFont(ft, sz);
        PostResize(true, false);
    }
    
    void SGLabel::SetSpacing(unsigned int x, unsigned int y) {
        SGTextBase::SetSpacing(x, y);
        PostResize(true, false);
    }
    
    v2i SGLabel::GetTextOffset() {
        return position_abs;
    }
    
    int SGLabel::GetMaxWidth() {
        if(max_width)
            return max_width;
        return 0xffff;
    }
    
    bool SGLabel::IsMultiLine() {
        return true;
    }
    
    void SGLabel::ClearText() {
        SGTextBase::ClearText();
        PostResize(true, false);
    }
    
    void SGLabel::SetText(const std::wstring& t, unsigned int cl) {
        SGTextBase::SetText(t, cl);
        PostResize(true, false);
    }
    
    void SGLabel::AppendText(const std::wstring& t, unsigned int cl) {
        SGTextBase::AppendText(t, cl);
        PostResize(true, false);
    }
    
    std::shared_ptr<SGLabel> SGLabel::Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, const std::wstring& t, int mw) {
        auto ptr = std::make_shared<SGLabel>();
        ptr->parent = p;
        ptr->position = pos;
        ptr->max_width = mw;
        auto iter2 = label_config.int_config.find("font_size");
        if(iter2 != label_config.int_config.end())
            ptr->SetFont(&guiRoot.GetGUIFont(), iter2->second);
        else
            ptr->SetFont(&guiRoot.GetGUIFont(), guiRoot.GetDefaultInt("font_size"));
        ptr->SetText(t, guiRoot.GetDefaultInt("font_color"));
        ptr->PostResize(false, true);
        if(p != nullptr)
            p->AddChild(ptr);
        else
            guiRoot.AddChild(ptr);
        return ptr;
    }
    
    SGConfig SGIconLabel::iconlabel_config;
    
    SGIconLabel::~SGIconLabel() {
        glDeleteBuffers(2, vbo);
    }
    
    void SGIconLabel::EvaluateSize(const std::wstring& t) {
        if(t.length() == 0) {
            text_width = 0;
            text_height = font_size;
            text_width_cur = 0;
            text_pos_array.clear();
            if(text.length() == 0)
                return;
            EvaluateSize(text);
        } else {
            int max_width = GetMaxWidth();
            auto iconoffset = iconlabel_config.tex_config["iconoffset"];
            int ls = GetLineSpacing();
            int ils = ls > (iconoffset.height + spacing_y) ? ls : (iconoffset.height + spacing_y);
            bool has_icon = false;
            for(auto ch : t) {
                if(ch < L' ') {
                    text_pos_array.push_back(v2i{text_width_cur, (int)(text_height - font_size)});
                    if(ch == L'\n') {
                        if(text_width_cur > text_width)
                            text_width = text_width_cur;
                        text_width_cur = 0;
                        text_height += has_icon ? ils : ls;
                        has_icon = false;
                    }
                    continue;
                }
                if(ch >= 0xe000 && ch <= 0xefff) { //private use zone
                    if(text_width_cur + iconoffset.width > max_width) {
                        if(text_width_cur > text_width)
                            text_width = text_width_cur;
                        text_width_cur = 0;
                        text_height += has_icon ? ils : ls;
                    }
                    has_icon = true;
                    text_width_cur += iconoffset.width + spacing_x;
                } else {
                    auto& gl = font->getGlyph(ch, font_size, false);
                    if(text_width_cur + gl.advance > max_width) {
                        if(text_width_cur > text_width)
                            text_width = text_width_cur;
                        text_width_cur = 0;
                        text_height += has_icon ? ils : ls;
                        has_icon = false;
                    }
                    text_pos_array.push_back(v2i{text_width_cur, (int)(text_height - font_size)});
                    text_width_cur += gl.advance + spacing_x;
                }
            }
        }
    }
    
    void SGIconLabel::UpdateTextVertex() {
        if(!text_update || font == nullptr)
            return;
        text_update = false;
        vert_size = 0;
        icon_size = 0;
        if(text.length() == 0)
            return;
        std::vector<SGVertexVCT> charvtx;
        std::vector<unsigned short> charidx;
        std::vector<SGVertexVCT> iconvtx;
        std::vector<unsigned short> iconidx;
        auto tex = font->getTexture(font_size);
        unsigned int advx = 0, advy = font_size;
        SGVertexVCT cur_char;
        int max_width = GetMaxWidth();
        v2i text_pos = GetTextOffset();
        auto iconoffset = guiRoot.GetDefaultRect("iconoffset");
        auto iconrc = guiRoot.GetDefaultInt("iconcolumn");
        int ls = GetLineSpacing();
        int ils = ls > (iconoffset.height + spacing_y) ? ls : (iconoffset.height + spacing_y);
        bool has_icon = false;
        for(size_t i = 0; i < text.length(); ++i) {
            wchar_t ch = text[i];
            unsigned int color = GetTextColor((int)i);
            if(ch < L' ') {
                if(ch == L'\n') {
                    advx = 0;
                    advy += has_icon ? ils : ls;
                    has_icon = false;
                }
                continue;
            }
            if(ch >= 0xe000 && ch <= 0xefff) {
                int index = ch - 0xe000;
                int x = index % iconrc;
                int y = index / iconrc;
                sf::IntRect rct = iconoffset;
                rct.left += x * iconoffset.width;
                rct.top += y * iconoffset.height;
                if(advx + iconoffset.width > max_width) {
                    advx = 0;
                    advy += has_icon ? ils : ls;
                }
                has_icon = true;
                iconvtx.resize(iconvtx.size() + 4);
                guiRoot.SetRectVertex(&iconvtx[icon_size * 4], text_pos.x + advx, text_pos.y + advy - font_size, iconoffset.width, iconoffset.height, rct);
                advx += iconoffset.width + spacing_x;
                
                iconidx.push_back(icon_size * 4);
                iconidx.push_back(icon_size * 4 + 2);
                iconidx.push_back(icon_size * 4 + 1);
                iconidx.push_back(icon_size * 4 + 1);
                iconidx.push_back(icon_size * 4 + 2);
                iconidx.push_back(icon_size * 4 + 3);
                icon_size++;
            } else {
                auto& gl = font->getGlyph(ch, font_size, false);
                if(advx + gl.advance > max_width) {
                    advx = 0;
                    advy += has_icon ? ils : ls;
                    has_icon = false;
                }
                guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx, text_pos.y + gl.bounds.top + advy, cur_char.vertex);
                cur_char.color = color;
                cur_char.texcoord[0] = (float)gl.textureRect.left / tex_size.x;
                cur_char.texcoord[1] = (float)gl.textureRect.top / tex_size.y;
                charvtx.push_back(cur_char);
                guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx + gl.bounds.width, text_pos.y + gl.bounds.top + advy, cur_char.vertex);
                cur_char.color = color;
                cur_char.texcoord[0] = (float)(gl.textureRect.left + gl.textureRect.width) / tex_size.x;
                cur_char.texcoord[1] = (float)gl.textureRect.top / tex_size.y;
                charvtx.push_back(cur_char);
                guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx, text_pos.y + gl.bounds.top + gl.bounds.height + advy, cur_char.vertex);
                cur_char.color = color;
                cur_char.texcoord[0] = (float)gl.textureRect.left / tex_size.x;
                cur_char.texcoord[1] = (float)(gl.textureRect.top + gl.textureRect.height) / tex_size.y;
                charvtx.push_back(cur_char);
                guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx + gl.bounds.width, text_pos.y + gl.bounds.top + gl.bounds.height + advy, cur_char.vertex);
                cur_char.color = color;
                cur_char.texcoord[0] = (float)(gl.textureRect.left + gl.textureRect.width) / tex_size.x;
                cur_char.texcoord[1] = (float)(gl.textureRect.top + gl.textureRect.height) / tex_size.y;
                charvtx.push_back(cur_char);
                advx += gl.advance + spacing_x;
                
                charidx.push_back(vert_size * 4);
                charidx.push_back(vert_size * 4 + 2);
                charidx.push_back(vert_size * 4 + 1);
                charidx.push_back(vert_size * 4 + 1);
                charidx.push_back(vert_size * 4 + 2);
                charidx.push_back(vert_size * 4 + 3);
                vert_size++;
            }
        }
        if(icon_size > icon_mem_size) {
            if(icon_mem_size == 0)
                icon_mem_size = 8;
            while(icon_mem_size < icon_size)
                icon_mem_size *= 2;
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(SGVertexVCT) * icon_mem_size * 4, nullptr, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SGVertexVCT) * iconvtx.size(), &iconvtx[0]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, icon_mem_size * 6 * sizeof(unsigned short), nullptr, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, iconidx.size() * sizeof(unsigned short), &iconidx[0]);
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SGVertexVCT) * iconvtx.size(), &iconvtx[0]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, iconidx.size() * sizeof(unsigned short), &iconidx[0]);
        }
        if(vert_size > mem_size) {
            if(mem_size == 0)
                mem_size = 16;
            while(mem_size < vert_size)
                mem_size *= 2;
            glBindBuffer(GL_ARRAY_BUFFER, tbo[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(SGVertexVCT) * mem_size * 4, nullptr, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SGVertexVCT) * charvtx.size(), &charvtx[0]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tbo[1]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, mem_size * 6 * sizeof(unsigned short), nullptr, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, charidx.size() * sizeof(unsigned short), &charidx[0]);
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, tbo[0]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SGVertexVCT) * charvtx.size(), &charvtx[0]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tbo[1]);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, charidx.size() * sizeof(unsigned short), &charidx[0]);
        }
    }
    
    void SGIconLabel::Draw() {
        UpdateVertices();
        UpdateTextVertex();
        if(icon_size) {
            guiRoot.BindGuiTexture();
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glVertexPointer(2, GL_FLOAT, sizeof(SGVertexVCT), 0);
            glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(SGVertexVCT), (const GLvoid*)SGVertexVCT::color_offset);
            glTexCoordPointer(2, GL_FLOAT, sizeof(SGVertexVCT), (const GLvoid*)SGVertexVCT::tex_offset);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
            glDrawElements(GL_TRIANGLES, icon_size * 6, GL_UNSIGNED_SHORT, 0);
        }
        if(vert_size) {
            guiRoot.BindFontTexture(font_size);
            glBindBuffer(GL_ARRAY_BUFFER, tbo[0]);
            glVertexPointer(2, GL_FLOAT, sizeof(SGVertexVCT), 0);
            glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(SGVertexVCT), (const GLvoid*)SGVertexVCT::color_offset);
            glTexCoordPointer(2, GL_FLOAT, sizeof(SGVertexVCT), (const GLvoid*)SGVertexVCT::tex_offset);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tbo[1]);
            glDrawElements(GL_TRIANGLES, vert_size * 6, GL_UNSIGNED_SHORT, 0);
        }
    }
    
    std::shared_ptr<SGIconLabel> SGIconLabel::Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, const std::wstring& t, int mw) {
        auto ptr = std::make_shared<SGIconLabel>();
        ptr->parent = p;
        ptr->position = pos;
        ptr->max_width = mw;
        auto iter2 = iconlabel_config.int_config.find("font_size");
        if(iter2 != iconlabel_config.int_config.end())
            ptr->SetFont(&guiRoot.GetGUIFont(), iter2->second);
        else
            ptr->SetFont(&guiRoot.GetGUIFont(), guiRoot.GetDefaultInt("font_size"));
        ptr->SetText(t, guiRoot.GetDefaultInt("font_color"));
        ptr->PostResize(false, true);
        glGenBuffers(2, ptr->vbo);
        if(p != nullptr)
            p->AddChild(ptr);
        else
            guiRoot.AddChild(ptr);
        return ptr;
    }
    
    SGConfig SGSprite::sprite_config;
    
    SGSprite::~SGSprite() {
        
    }
    
    void SGSprite::UpdateVertices() {
        if(!vertices_dirty)
            return;
        vertices_dirty = false;
        img_update = true;
    }
    
    void SGSprite::Draw() {
        UpdateVertices();
        DrawImage();
    }
    
    v2i SGSprite::GetImageOffset() {
        return position_abs;
    }
    
    std::shared_ptr<SGSprite> SGSprite::Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, v2i size) {
        auto ptr = std::make_shared<SGSprite>();
        ptr->parent = p;
        ptr->position = pos;
        ptr->size = size;
        ptr->PostResize(true, true);
        ptr->color = guiRoot.GetDefaultInt("gui_color");
        if(p != nullptr)
            p->AddChild(ptr);
        else
            guiRoot.AddChild(ptr);
        return ptr;
    }
    
    SGConfig SGButton::button_config;
    
    SGButton::~SGButton() {
        glDeleteBuffers(2, vbo);
    }
    
    void SGButton::UpdateVertices() {
        if(!vertices_dirty)
            return;
        vertices_dirty = false;
        img_update = true;
        text_update = true;
        std::array<SGVertexVCT, 36> vert;
        int bw = button_config.int_config["border"];
        auto back = tex_rect[state & 0xf];
        guiRoot.SetRectVertex(&vert[0], position_abs.x, position_abs.y, bw, bw,
                              sf::IntRect{back.left, back.top, bw, bw});
        guiRoot.SetRectVertex(&vert[4], position_abs.x + bw, position_abs.y, size_abs.x - bw * 2, bw,
                              sf::IntRect{back.left + bw, back.top, back.width - bw * 2, bw});
        guiRoot.SetRectVertex(&vert[8], position_abs.x + size_abs.x - bw, position_abs.y, bw, bw,
                              sf::IntRect{back.left + back.width - bw, back.top, bw, bw});
        guiRoot.SetRectVertex(&vert[12], position_abs.x, position_abs.y + bw, bw, size_abs.y - bw * 2,
                              sf::IntRect{back.left, back.top + bw, bw, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[16], position_abs.x + bw, position_abs.y + bw, size_abs.x - bw * 2, size_abs.y - bw * 2,
                              sf::IntRect{back.left + bw, back.top + bw, back.width - bw * 2, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[20], position_abs.x + size_abs.x - bw, position_abs.y + bw, bw, size_abs.y - bw * 2,
                              sf::IntRect{back.left + back.width - bw, back.top + bw, bw, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[24], position_abs.x, position_abs.y + size_abs.y - bw, bw, bw,
                              sf::IntRect{back.left, back.top + back.height - bw, bw, bw});
        guiRoot.SetRectVertex(&vert[28], position_abs.x + bw, position_abs.y + size_abs.y - bw, size_abs.x - bw * 2, bw,
                              sf::IntRect{back.left + bw, back.top + back.height - bw, back.width - bw * 2, bw});
        guiRoot.SetRectVertex(&vert[32], position_abs.x + size_abs.x - bw, position_abs.y + size_abs.y - bw, bw, bw,
                              sf::IntRect{back.left + back.width - bw, back.top + back.height - bw, bw, bw});
        for(int i = 0; i < 36; ++i)
            vert[i].color = color;
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SGVertexVCT) * 36, &vert);
    }
    
    void SGButton::Draw() {
        UpdateVertices();
        if(tex_texture)
            guiRoot.BindTexture(tex_texture);
        else
            guiRoot.BindGuiTexture();
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glVertexPointer(2, GL_FLOAT, sizeof(SGVertexVCT), 0);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(SGVertexVCT), (const GLvoid*)SGVertexVCT::color_offset);
        glTexCoordPointer(2, GL_FLOAT, sizeof(SGVertexVCT), (const GLvoid*)SGVertexVCT::tex_offset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
        glDrawElements(GL_TRIANGLES, 54, GL_UNSIGNED_SHORT, 0);
        DrawImage();
        DrawText();
    }
    
    v2i SGButton::GetTextOffset() {
        int offx = (size_abs.x - text_width_cur) / 2;
        int offy = (size_abs.y - text_height) / 2 - 3;
        if(state == 2)
            return position_abs + v2i{offx, offy + 1};
        else
            return position_abs + v2i{offx, offy};
    }
    
    int SGButton::GetMaxWidth() {
        return 0xffff;
    }
    
    bool SGButton::IsMultiLine() {
        return false;
    }
    
    v2i SGButton::GetImageOffset() {
        if(state == 2)
            return position_abs + v2i{0, 0 + 1};
        else
            return position_abs;
    }
    
    void SGButton::SetTextureRect(sf::IntRect r1, sf::IntRect r2, sf::IntRect r3, int lw, int rw) {
        tex_rect[0] = r1;
        tex_rect[1] = r2;
        tex_rect[2] = r3;
        lwidth = lw;
        rwidth = rw;
        UpdateVertices();
    }
    
    void SGButton::SetTexture(sf::Texture* tex) {
        tex_texture = tex;
    }
    
    std::shared_ptr<SGButton> SGButton::Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, v2i size, bool is_push) {
        auto ptr = std::make_shared<SGButton>();
        ptr->parent = p;
        ptr->position = pos;
        ptr->size = size;
        ptr->PostResize(true, true);
        auto iter1 = button_config.int_config.find("gui_color");
        if(iter1 != button_config.int_config.end())
            ptr->color = iter1->second;
        else
            ptr->color = guiRoot.GetDefaultInt("gui_color");
        auto iter2 = button_config.int_config.find("font_size");
        if(iter2 != button_config.int_config.end())
            ptr->SetFont(&guiRoot.GetGUIFont(), iter2->second);
        else
            ptr->SetFont(&guiRoot.GetGUIFont(), guiRoot.GetDefaultInt("font_size"));
        ptr->is_push = is_push;
        glGenBuffers(2, ptr->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(SGVertexVCT) * 36, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ptr->vbo[1]);
        unsigned short index[54];
        for(int i = 0; i < 9; ++i) {
            index[i * 6] = i * 4;
            index[i * 6 + 1] = i * 4 + 2;
            index[i * 6 + 2] = i * 4 + 1;
            index[i * 6 + 3] = i * 4 + 1;
            index[i * 6 + 4] = i * 4 + 2;
            index[i * 6 + 5] = i * 4 + 3;
        }
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
        if(p != nullptr)
            p->AddChild(ptr);
        else
            guiRoot.AddChild(ptr);
        ptr->tex_rect[0] = button_config.tex_config["normal"];
        ptr->tex_rect[1] = button_config.tex_config["hover"];
        ptr->tex_rect[2] = button_config.tex_config["down"];
        ptr->lwidth = button_config.int_config["lwidth"];
        ptr->rwidth = button_config.int_config["rwidth"];
        return ptr;
    }
    
    bool SGButton::EventMouseEnter() {
        if(!is_push) {
            if(guiRoot.GetClickObject().get() == this)
                state = 2;
            else
                state = 1;
            vertices_dirty = true;
        } else {
            if(state == 0) {
                if(guiRoot.GetClickObject().get() == this)
                    state = 2;
                else
                    state = 1;
                vertices_dirty = true;
            }
        }
        return SGWidget::EventMouseEnter();
    }
    
    bool SGButton::EventMouseLeave() {
        if(!is_push) {
            state = 0;
            vertices_dirty = true;
        } else {
            if(state != 0x12) {
                state = 0;
                vertices_dirty = true;
            }
        }
        return SGWidget::EventMouseLeave();
    }
    
    bool SGButton::EventMouseButtonDown(sf::Event::MouseButtonEvent evt) {
        if(evt.button == sf::Mouse::Left) {
            if(!is_push) {
                state = 2;
                vertices_dirty = true;
                guiRoot.SetClickingObject(shared_from_this());
            } else {
                if(state != 0x12) {
                    state = 2;
                    vertices_dirty = true;
                }
                guiRoot.SetClickingObject(shared_from_this());
            }
        }
        return SGWidget::EventMouseButtonDown(evt);
    }
    
    bool SGButton::EventMouseButtonUp(sf::Event::MouseButtonEvent evt) {
        bool click = false;
        if(evt.button == sf::Mouse::Left) {
            if(!is_push) {
                if(state == 2) {
                    state = 1;
                    vertices_dirty = true;
                    click = true;
                }
            } else {
                if(state == 2) {
                    state = 0x12;
                    click = true;
                } else if(state == 0x12) {
                    state = 1;
                    click = true;
                    vertices_dirty = true;
                }
            }
        }
        bool ret = SGWidget::EventMouseButtonUp(evt);
        if(click)
            eventButtonClick.TriggerEvent(*this);
        return ret;
    }
    
    SGConfig SGCheckbox::checkbox_config;
    
    SGCheckbox::~SGCheckbox() {
        glDeleteBuffers(2, vbo);
    }
    
    void SGCheckbox::PostResize(bool res, bool rep) {
        vertices_dirty = true;
        auto ptr = parent.lock();
        v2i ssize = (ptr == nullptr) ? guiRoot.GetSceneSize() : ptr->GetClientSize();
        if(res) {
            auto rec = checkbox_config.tex_config["offset"];
            size_abs = v2i{rec.left + rec.width + rec.top + text_width_cur, std::max(text_height , rec.height)};
        }
        if(rep) {
            position_abs = position + v2i(ssize.x * position_prop.x, ssize.y * position_prop.y);
            if(ptr != nullptr)
                position_abs += ptr->GetClientPosition();
        }
    }
    
    void SGCheckbox::UpdateVertices() {
        if(!vertices_dirty)
            return;
        vertices_dirty = false;
        text_update = true;
        auto rec = checkbox_config.tex_config["offset"];
        std::array<SGVertexVCT, 4> vert;
        if(!checked) {
            if(state == 0)
                guiRoot.SetRectVertex(&vert[0], position_abs.x + rec.left, position_abs.y, rec.width, rec.height, checkbox_config.tex_config["normal1"]);
            else if(state == 1)
                guiRoot.SetRectVertex(&vert[0], position_abs.x + rec.left, position_abs.y, rec.width, rec.height, checkbox_config.tex_config["hover1"]);
            else
                guiRoot.SetRectVertex(&vert[0], position_abs.x + rec.left, position_abs.y, rec.width, rec.height, checkbox_config.tex_config["down1"]);
        } else {
            if(state == 0)
                guiRoot.SetRectVertex(&vert[0], position_abs.x + rec.left, position_abs.y, rec.width, rec.height, checkbox_config.tex_config["normal2"]);
            else if(state == 1)
                guiRoot.SetRectVertex(&vert[0], position_abs.x + rec.left, position_abs.y, rec.width, rec.height, checkbox_config.tex_config["hover2"]);
            else
                guiRoot.SetRectVertex(&vert[0], position_abs.x + rec.left, position_abs.y, rec.width, rec.height, checkbox_config.tex_config["down2"]);
        }
        for(int i = 0; i < 4; ++i)
            vert[i].color = color;
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SGVertexVCT) * 4, &vert);
    }
    
    void SGCheckbox::Draw() {
        UpdateVertices();
        guiRoot.BindGuiTexture();
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glVertexPointer(2, GL_FLOAT, sizeof(SGVertexVCT), 0);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(SGVertexVCT), (const GLvoid*)SGVertexVCT::color_offset);
        glTexCoordPointer(2, GL_FLOAT, sizeof(SGVertexVCT), (const GLvoid*)SGVertexVCT::tex_offset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
        DrawText();
    }
    
    void SGCheckbox::SetFont(sf::Font* ft, unsigned int sz) {
        SGTextBase::SetFont(ft, sz);
        PostResize(true, false);
    }
    
    void SGCheckbox::SetSpacing(unsigned int x, unsigned int y) {
        SGTextBase::SetSpacing(x, y);
        PostResize(true, false);
    }
    
    v2i SGCheckbox::GetTextOffset() {
        auto rec = checkbox_config.tex_config["offset"];
        return position_abs + v2i{rec.left + rec.width + rec.top, (size_abs.y - text_height) / 2 - 2 };
    }
    
    int SGCheckbox::GetMaxWidth() {
        return 0xffff;
    }
    
    bool SGCheckbox::IsMultiLine() {
        return false;
    }
    
    void SGCheckbox::ClearText() {
        SGTextBase::ClearText();
        PostResize(true, false);
    }
    
    void SGCheckbox::SetText(const std::wstring& t, unsigned int cl) {
        SGTextBase::SetText(t, cl);
        PostResize(true, false);
    }
    
    void SGCheckbox::AppendText(const std::wstring& t, unsigned int cl) {
        SGTextBase::AppendText(t, cl);
        PostResize(true, false);
    }
    
    void SGCheckbox::SetChecked(bool chk) {
        if(checked == chk)
            return;
        checked = chk;
        vertices_dirty = true;
        eventCheckChange.TriggerEvent(*this, checked);
    }
    
    std::shared_ptr<SGCheckbox> SGCheckbox::Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, const std::wstring& t) {
        auto ptr = std::make_shared<SGCheckbox>();
        ptr->parent = p;
        ptr->position = pos;
        ptr->PostResize(false, true);
        auto iter1 = checkbox_config.int_config.find("gui_color");
        if(iter1 != checkbox_config.int_config.end())
            ptr->color = iter1->second;
        else
            ptr->color = guiRoot.GetDefaultInt("gui_color");
        auto iter2 = checkbox_config.int_config.find("font_size");
        if(iter2 != checkbox_config.int_config.end())
            ptr->SetFont(&guiRoot.GetGUIFont(), iter2->second);
        else
            ptr->SetFont(&guiRoot.GetGUIFont(), guiRoot.GetDefaultInt("font_size"));
        ptr->SetText(t, guiRoot.GetDefaultInt("font_color"));
        glGenBuffers(2, ptr->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(SGVertexVCT) * 12, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ptr->vbo[1]);
        unsigned short index[] = {0, 2, 1, 1, 2, 3};
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
        if(p != nullptr)
            p->AddChild(ptr);
        else
            guiRoot.AddChild(ptr);
        return ptr;
    }
    
    bool SGCheckbox::EventMouseEnter() {
        if(guiRoot.GetClickObject().get() == this)
            state = 2;
        else
            state = 1;
        vertices_dirty = true;
        return SGWidget::EventMouseEnter();
    }
    
    bool SGCheckbox::EventMouseLeave() {
        state = 0;
        vertices_dirty = true;
        return SGWidget::EventMouseLeave();
    }
    
    bool SGCheckbox::EventMouseButtonDown(sf::Event::MouseButtonEvent evt) {
        if(evt.button == sf::Mouse::Left) {
            state = 2;
            vertices_dirty = true;
            guiRoot.SetClickingObject(shared_from_this());
        }
        return SGWidget::EventMouseButtonDown(evt);
    }
    
    bool SGCheckbox::EventMouseButtonUp(sf::Event::MouseButtonEvent evt) {
        bool click = false;
        if(state == 2 && evt.button == sf::Mouse::Left) {
            vertices_dirty = true;
            state = 1;
            checked = !checked;
            click = true;
        }
        bool ret = SGWidget::EventMouseButtonUp(evt);
        if(click)
            eventCheckChange.TriggerEvent(*this, checked);
        return ret;
    }
    
    SGConfig SGRadio::radio_config;
    
    SGRadio::~SGRadio() {
        DetachGroup();
    }
    
    void SGRadio::UpdateVertices() {
        if(!vertices_dirty)
            return;
        vertices_dirty = false;
        text_update = true;
        auto rec = checkbox_config.tex_config["offset"];
        std::array<SGVertexVCT, 4> vert;
        if(!checked) {
            if(state == 0)
                guiRoot.SetRectVertex(&vert[0], position_abs.x + rec.left, position_abs.y, rec.width, rec.height, radio_config.tex_config["normal1"]);
            else if(state == 1)
                guiRoot.SetRectVertex(&vert[0], position_abs.x + rec.left, position_abs.y, rec.width, rec.height, radio_config.tex_config["hover1"]);
            else
                guiRoot.SetRectVertex(&vert[0], position_abs.x + rec.left, position_abs.y, rec.width, rec.height, radio_config.tex_config["down1"]);
        } else {
            if(state == 0)
                guiRoot.SetRectVertex(&vert[0], position_abs.x + rec.left, position_abs.y, rec.width, rec.height, radio_config.tex_config["normal2"]);
            else if(state == 1)
                guiRoot.SetRectVertex(&vert[0], position_abs.x + rec.left, position_abs.y, rec.width, rec.height, radio_config.tex_config["hover2"]);
            else
                guiRoot.SetRectVertex(&vert[0], position_abs.x + rec.left, position_abs.y, rec.width, rec.height, radio_config.tex_config["down2"]);
        }
        for(int i = 0; i < 4; ++i)
            vert[i].color = color;
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SGVertexVCT) * 4, &vert);
    }

    void SGRadio::SetChecked(bool chk) {
        if(checked == chk)
            return;
        if(chk) {
            auto pre = GetCheckedTarget();
            if(pre != nullptr)
                pre->SetChecked(false);
        }
        checked = chk;
        vertices_dirty = true;
        eventCheckChange.TriggerEvent(*this, checked);
    }
    
    void SGRadio::AttachGroup(std::shared_ptr<SGRadio> rdo) {
        if(rdo.get() == this)
            return;
        if(next_group_member != this)
            DetachGroup();
        next_group_member = rdo->next_group_member;
        rdo->next_group_member = this;
    }
    
    void SGRadio::DetachGroup() {
        auto next = next_group_member;
        if(next == this)
            return;
        while(next->next_group_member != this)
            next = next->next_group_member;
        next->next_group_member = next_group_member;
        next_group_member = this;
    }
    
    std::shared_ptr<SGRadio> SGRadio::GetCheckedTarget() {
        auto current = this;
        do {
            if(current->checked)
                return std::static_pointer_cast<SGRadio>(current->shared_from_this());
            current = current->next_group_member;
        } while (current != this);
        return nullptr;
    }
    
    bool SGRadio::EventMouseButtonUp(sf::Event::MouseButtonEvent evt) {
        bool click = false;
        if(state == 2 && evt.button == sf::Mouse::Left) {
            vertices_dirty = true;
            state = 1;
            if(!checked)
                click = true;
        }
        bool ret = SGWidget::EventMouseButtonUp(evt);
        if(click)
            SetChecked(true);
        return ret;
    }
    
    std::shared_ptr<SGRadio> SGRadio::Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, const std::wstring& t) {
        auto ptr = std::make_shared<SGRadio>();
        ptr->parent = p;
        ptr->position = pos;
        ptr->PostResize(false, true);
        auto iter1 = checkbox_config.int_config.find("gui_color");
        if(iter1 != checkbox_config.int_config.end())
            ptr->color = iter1->second;
        else
            ptr->color = guiRoot.GetDefaultInt("gui_color");
        auto iter2 = checkbox_config.int_config.find("font_size");
        if(iter2 != checkbox_config.int_config.end())
            ptr->SetFont(&guiRoot.GetGUIFont(), iter2->second);
        else
            ptr->SetFont(&guiRoot.GetGUIFont(), guiRoot.GetDefaultInt("font_size"));
        ptr->SetText(t, guiRoot.GetDefaultInt("font_color"));
        ptr->next_group_member = ptr.get();
        glGenBuffers(2, ptr->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(SGVertexVCT) * 12, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ptr->vbo[1]);
        unsigned short index[] = {0, 2, 1, 1, 2, 3};
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
        if(p != nullptr)
            p->AddChild(ptr);
        else
            guiRoot.AddChild(ptr);
        return ptr;
    }
    
    SGConfig SGScrollBar::scroll_config;
    
    SGScrollBar::~SGScrollBar() {
        glDeleteBuffers(2, vbo);
    }
    
    void SGScrollBar::PostResize(bool res, bool rep) {
        SGWidget::PostResize(res, rep);
        if(is_horizontal) {
            pos_min = position_abs.x;
            pos_max = position_abs.x + size_abs.x - slider_length;
            size_abs.y = scroll_config.int_config["hheight"];
        } else {
            pos_min = position_abs.y;
            pos_max = position_abs.y + size_abs.y - slider_length;
            size_abs.x = scroll_config.int_config["vwidth"];
        }
    }
    
    void SGScrollBar::UpdateVertices() {
        if(!vertices_dirty)
            return;
        vertices_dirty = false;
        std::array<SGVertexVCT, 16> vert;
        if(is_horizontal) {
            int hheight = scroll_config.int_config["hheight"];
            auto backh = scroll_config.tex_config["backh"];
            guiRoot.SetRectVertex(&vert[0], position_abs.x, position_abs.y, size_abs.x, hheight, backh);
            if(slider_length) {
                int slength = scroll_config.int_config["slength"];
                auto sliderh = scroll_config.tex_config[slider_moving ? "sliderh3" : slider_hoving ? "sliderh2" : "sliderh1"];
                auto rec = sliderh;
                rec.width = slength;
                guiRoot.SetRectVertex(&vert[4], position_abs.x + current_pos, position_abs.y, rec.width, hheight, rec);
                rec.left = sliderh.left + slength;
                rec.width = sliderh.width - slength * 2;
                guiRoot.SetRectVertex(&vert[8], position_abs.x + current_pos + slength, position_abs.y, slider_length - slength * 2, hheight, rec);
                rec.left = sliderh.left + sliderh.width - slength;
                rec.width = slength;
                guiRoot.SetRectVertex(&vert[12], position_abs.x + current_pos + slider_length - slength, position_abs.y, rec.width, hheight, rec);
            }
        } else {
            int vwidth = scroll_config.int_config["vwidth"];
            auto backv = scroll_config.tex_config["backv"];
            guiRoot.SetRectVertex(&vert[0], position_abs.x, position_abs.y, vwidth, size_abs.y, backv);
            if(slider_length) {
                int slength = scroll_config.int_config["slength"];
                auto sliderv = scroll_config.tex_config[slider_moving ? "sliderv3" : slider_hoving ? "sliderv2" : "sliderv1"];
                auto rec = sliderv;
                rec.height = slength;
                guiRoot.SetRectVertex(&vert[4], position_abs.x, position_abs.y + current_pos, vwidth, rec.height, rec);
                rec.top = sliderv.top + slength;
                rec.height = sliderv.height - slength * 2;
                guiRoot.SetRectVertex(&vert[8], position_abs.x, position_abs.y + current_pos + slength, vwidth, slider_length - slength * 2, rec);
                rec.top = sliderv.top + sliderv.height - slength;
                rec.height = slength;
                guiRoot.SetRectVertex(&vert[12], position_abs.x, position_abs.y + current_pos + slider_length - slength, vwidth, rec.height, rec);
            }
        }
        for(int i = 0; i < 16; ++i)
            vert[i].color = color;
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SGVertexVCT) * 16, &vert);
    }
    
    void SGScrollBar::Draw() {
        UpdateVertices();
        guiRoot.BindGuiTexture();
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glVertexPointer(2, GL_FLOAT, sizeof(SGVertexVCT), 0);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(SGVertexVCT), (const GLvoid*)SGVertexVCT::color_offset);
        glTexCoordPointer(2, GL_FLOAT, sizeof(SGVertexVCT), (const GLvoid*)SGVertexVCT::tex_offset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
        if(slider_length)
            glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_SHORT, 0);
        else
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    }
    
    void SGScrollBar::SetRange(float minv, float maxv, float cur) {
        minvalue = minv;
        maxvalue = maxv;
        current_pos = (cur - minv) / maxv * (pos_max - pos_min);
        if(current_pos < 0)
            current_pos = 0;
        if(current_pos > pos_max - slider_length)
            current_pos = pos_max - slider_length;
        vertices_dirty = true;
    }
    
    void SGScrollBar::SetValue(float cur) {
        int prepos = current_pos;
        current_pos = (cur - minvalue) / maxvalue * (pos_max - pos_min);
        if(current_pos < 0)
            current_pos = 0;
        if(current_pos > pos_max - slider_length)
            current_pos = pos_max - slider_length;
        if(prepos != current_pos) {
            vertices_dirty = true;
            float val = (float)current_pos / (pos_max - pos_min) * (maxvalue - minvalue) + minvalue;
            eventValueChange.TriggerEvent(*this, val);
        }
    }
    
    void SGScrollBar::SetSliderLength(int length) {
        int prel = slider_length;
        if(length == 0) {
            slider_length = 0;
        } else {
            int minl, maxl;
            if(is_horizontal) {
                minl = scroll_config.tex_config["sliderh1"].width;
                maxl = size_abs.x;
            } else {
                minl = scroll_config.tex_config["sliderv1"].height;
                maxl = size_abs.y - 1;
            }
            slider_length = length;
            if(slider_length < minl)
                slider_length = minl;
            if(slider_length > maxl)
                slider_length = maxl;
            if(is_horizontal)
                pos_max = position_abs.x + size_abs.x - slider_length;
            else
                pos_max = position_abs.y + size_abs.y - slider_length;
            if(current_pos + slider_length > pos_max)
                current_pos = pos_max - slider_length;
        }
        if(prel != slider_length)
            vertices_dirty = true;
    }
    
    bool SGScrollBar::EventMouseMove(sf::Event::MouseMoveEvent evt) {
        if(slider_length == 0)
            return false;
        bool preh = slider_hoving;
        if(is_horizontal) {
            if(evt.x >= position_abs.x + current_pos && evt.x <= position_abs.x + current_pos + slider_length)
                slider_hoving = true;
            else
                slider_hoving = false;
        } else {
            if(evt.y >= position_abs.y + current_pos && evt.y <= position_abs.y + current_pos + slider_length)
                slider_hoving = true;
            else
                slider_hoving = false;
        }
        if(preh != slider_hoving)
            vertices_dirty = true;
        return eventMouseMove.TriggerEvent(*this, evt);
    }
    
    bool SGScrollBar::EventMouseEnter() {
        return eventMouseEnter.TriggerEvent(*this);
    }
    
    bool SGScrollBar::EventMouseLeave() {
        if(slider_hoving) {
            slider_hoving = false;
            vertices_dirty = true;
        }
        return eventMouseLeave.TriggerEvent(*this);
    }
    
    bool SGScrollBar::EventMouseWheel(sf::Event::MouseWheelEvent evt) {
        if(slider_length == 0)
            return false;
        int prepos = current_pos;
        current_pos += (pos_max - pos_min) * evt.delta / 100;
        if(current_pos < 0)
            current_pos = 0;
        if(current_pos > pos_max - pos_min)
            current_pos = pos_max - pos_min;
        bool r1 = eventMouseWheel.TriggerEvent(*this, evt);
        if(prepos != current_pos) {
            vertices_dirty = true;
            float val = (float)current_pos / (pos_max - pos_min) * (maxvalue - minvalue) + minvalue;
            bool r2 = eventValueChange.TriggerEvent(*this, val);
            return r1 || r2;
        }
        return r1;
    }
                         
    bool SGScrollBar::DragingBegin(v2i evt) {
        if(slider_length == 0)
            return false;
        position_drag = evt;
        if(slider_hoving) {
            slider_moving = true;
            vertices_dirty = true;
            return eventDragBegin.TriggerEvent(*this, evt);
        } else {
            if(is_horizontal) {
                current_pos = evt.x;
                if(current_pos >= pos_max)
                    current_pos = pos_max;
                current_pos -= pos_min;
            } else {
                current_pos = evt.y;
                if(current_pos >= pos_max)
                    current_pos = pos_max;
                current_pos -= pos_min;
            }
            vertices_dirty = true;
            float val = (float)current_pos / (pos_max - pos_min) * (maxvalue - minvalue) + minvalue;
            bool r1 = eventDragBegin.TriggerEvent(*this, evt);
            bool r2 = eventValueChange.TriggerEvent(*this, val);
            return r1 || r2;
        }
    }
    
    bool SGScrollBar::DragingUpdate(v2i evt) {
        if(slider_length == 0)
            return false;
        v2i delta = evt - position_drag;
        position_drag = evt;
        if(slider_moving) {
            int prepos = current_pos;
            if(is_horizontal)
                current_pos += delta.x + slider_diff;
            else
                current_pos += delta.y + slider_diff;
            slider_diff = 0;
            if(current_pos < 0) {
                slider_diff = current_pos;
                current_pos = 0;
            }
            if(current_pos > pos_max - pos_min) {
                slider_diff = current_pos - (pos_max - pos_min);
                current_pos = pos_max - pos_min;
            }
            if(prepos != current_pos) {
                vertices_dirty = true;
                float val = (float)current_pos / (pos_max - pos_min) * (maxvalue - minvalue) + minvalue;
                bool r1 = eventDragBegin.TriggerEvent(*this, evt);
                bool r2 = eventValueChange.TriggerEvent(*this, val);
                return r1 || r2;
            }
        }
        return eventDragUpdate.TriggerEvent(*this, delta);
    }
    
    bool SGScrollBar::DragingEnd(v2i evt) {
        if(slider_length == 0)
            return false;
        slider_diff = 0;
        slider_moving = false;
        vertices_dirty = true;
        return eventDragEnd.TriggerEvent(*this, evt);
    }
    
    std::shared_ptr<SGScrollBar> SGScrollBar::Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, v2i sz, bool is_h) {
        auto ptr = std::make_shared<SGScrollBar>();
        ptr->parent = p;
        ptr->position = pos;
        ptr->size = sz;
        ptr->is_horizontal = is_h;
        if(is_h)
            ptr->slider_length = sz.x / 2;
        else
            ptr->slider_length = sz.y / 2;
        ptr->PostResize(true, true);
        auto iter = scroll_config.int_config.find("gui_color");
        if(iter != scroll_config.int_config.end())
            ptr->color = iter->second;
        else
            ptr->color = guiRoot.GetDefaultInt("gui_color");
        glGenBuffers(2, ptr->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(SGVertexVCT) * 16, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ptr->vbo[1]);
        unsigned short index[] = {0, 2, 1, 1, 2, 3, 4, 6, 5, 5, 6, 7, 8, 10, 9, 9, 10, 11, 12, 14, 13, 13, 14, 15};
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
        if(p != nullptr)
            p->AddChild(ptr);
        else
            guiRoot.AddChild(ptr);
        return ptr;
    }
    
    SGConfig SGTextEdit::textedit_config;
    
    SGTextEdit::~SGTextEdit() {
        glDeleteBuffers(2, vbo);
    }
    
    void SGTextEdit::PostResize(bool res, bool rep) {
        SGWidgetContainer::PostResize(res, rep);
        size_abs.y = textedit_config.int_config["height"];
    }
    
    void SGTextEdit::UpdateVertices() {
        if(!vertices_dirty)
            return;
        vertices_dirty = false;
        text_update = true;
        sel_change = true;
        std::array<SGVertexVCT, 40> vert;
        int bw = textedit_config.int_config["border"];
        auto back = textedit_config.tex_config[hoving ? "backh" : "back"];
        auto crect = textedit_config.tex_config["cursor"];
        auto curx = cursor_pos >= text.length() ? text_width_cur : text_pos_array[cursor_pos].x;
        int ht = textedit_config.int_config["sel_height"];
        guiRoot.SetRectVertex(&vert[0], position_abs.x, position_abs.y, bw, bw,
                              sf::IntRect{back.left, back.top, bw, bw});
        guiRoot.SetRectVertex(&vert[4], position_abs.x + bw, position_abs.y, size_abs.x - bw * 2, bw,
                              sf::IntRect{back.left + bw, back.top, back.width - bw * 2, bw});
        guiRoot.SetRectVertex(&vert[8], position_abs.x + size_abs.x - bw, position_abs.y, bw, bw,
                              sf::IntRect{back.left + back.width - bw, back.top, bw, bw});
        guiRoot.SetRectVertex(&vert[12], position_abs.x, position_abs.y + bw, bw, size_abs.y - bw * 2,
                              sf::IntRect{back.left, back.top + bw, bw, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[16], position_abs.x + bw, position_abs.y + bw, size_abs.x - bw * 2, size_abs.y - bw * 2,
                              sf::IntRect{back.left + bw, back.top + bw, back.width - bw * 2, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[20], position_abs.x + size_abs.x - bw, position_abs.y + bw, bw, size_abs.y - bw * 2,
                              sf::IntRect{back.left + back.width - bw, back.top + bw, bw, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[24], position_abs.x, position_abs.y + size_abs.y - bw, bw, bw,
                              sf::IntRect{back.left, back.top + back.height - bw, bw, bw});
        guiRoot.SetRectVertex(&vert[28], position_abs.x + bw, position_abs.y + size_abs.y - bw, size_abs.x - bw * 2, bw,
                              sf::IntRect{back.left + bw, back.top + back.height - bw, back.width - bw * 2, bw});
        guiRoot.SetRectVertex(&vert[32], position_abs.x + size_abs.x - bw, position_abs.y + size_abs.y - bw, bw, bw,
                              sf::IntRect{back.left + back.width - bw, back.top + back.height - bw, bw, bw});
        guiRoot.SetRectVertex(&vert[36], position_abs.x + curx + text_area.left - text_offset, position_abs.y + text_area.top, crect.width, ht, crect);
        for(int i = 0; i < 40; ++i)
            vert[i].color = color;
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SGVertexVCT) * 40, &vert);
    }
    
    void SGTextEdit::Draw() {
        if(draging) {
            float tm = guiRoot.GetTime().asSeconds();
            int chk = (int)((tm - cursor_time) / 0.3f);
            if(chk != drag_check) {
                drag_check = chk;
                CheckDragPos();
            }
        }
        UpdateVertices();
        guiRoot.BindGuiTexture();
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glVertexPointer(2, GL_FLOAT, sizeof(SGVertexVCT), 0);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(SGVertexVCT), (const GLvoid*)SGVertexVCT::color_offset);
        glTexCoordPointer(2, GL_FLOAT, sizeof(SGVertexVCT), (const GLvoid*)SGVertexVCT::tex_offset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
        float tm = guiRoot.GetTime().asSeconds();
        if(!focus || sel_start != sel_end || ((int)((tm - cursor_time) / 0.5f) % 2))
            glDrawElements(GL_TRIANGLES, 54, GL_UNSIGNED_SHORT, 0);
        else
            glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_SHORT, 0);
        int tw = size_abs.x - text_area.left - text_area.width;
        int th = size_abs.y - text_area.top - text_area.height;
        for(auto chd : children)
            chd->Draw();
        guiRoot.BeginScissor(sf::IntRect{position_abs.x + text_area.left, position_abs.y + text_area.top, tw, th});
        DrawSelectRegion();
        DrawText();
        guiRoot.EndScissor();
    }
    
    v2i SGTextEdit::GetTextOffset() {
        return position_abs + v2i{text_area.left - text_offset, text_area.top};
    }
    
    int SGTextEdit::GetMaxWidth() {
        return 0xffff;
    }
    
    unsigned int SGTextEdit::GetTextColor(int index) {
        if(index >= sel_start && index < sel_end)
            return sel_color;
        return color_vec[index];
    }
    
    void SGTextEdit::DrawSelectRegion() {
        UpdateSelRegion();
        if(sel_start != sel_end) {
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glVertexPointer(2, GL_FLOAT, sizeof(SGVertexVCT), 0);
            glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(SGVertexVCT), (const GLvoid*)SGVertexVCT::color_offset);
            glTexCoordPointer(2, GL_FLOAT, sizeof(SGVertexVCT), (const GLvoid*)SGVertexVCT::tex_offset);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (GLvoid*)(uintptr_t)(sizeof(unsigned short) * 60));
        }
    }
    
    void SGTextEdit::UpdateSelRegion() {
        if(!sel_change)
            return;
        sel_change = false;
        std::array<SGVertexVCT, 4> vert;
        int ht = textedit_config.int_config["sel_height"];
        if(sel_start != sel_end) {
            int ps = text_pos_array[sel_start].x;
            int pe = sel_end >= text.length() ? text_width_cur : text_pos_array[sel_end].x;
            v2i ta = position_abs + v2i{text_area.left - text_offset, text_area.top};
            guiRoot.SetRectVertex(&vert[0], ta.x + ps, ta.y, pe - ps, ht, textedit_config.tex_config["sel_rect"]);
        }
        for(int i = 0; i < 11; ++i)
            vert[i].color = sel_bcolor;
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(SGVertexVCT) * 40, sizeof(SGVertexVCT) * 4, &vert);
    }
    
    void SGTextEdit::SetSelRegion(unsigned int start, unsigned int end) {
        unsigned int pres = sel_start, pree = sel_end;
        sel_start = std::min(start, end);
        sel_end = std::max(start, end);
        if(pres != sel_start || pree != sel_end) {
            vertices_dirty = true;
            sel_change = true;
        }
    }
    
    void SGTextEdit::CheckCursorPos() {
        int preoff = text_offset;
        if(text_width_cur <= size_abs.x - text_area.left - text_area.width)
            text_offset = 0;
        else {
            int my = cursor_pos >= text.length() ? text_width_cur : text_pos_array[cursor_pos].x;
            int size_w = size_abs.x - text_area.left - text_area.width - 10;
            if(my < text_offset + 10) {
                text_offset = my - 10;
                if(text_offset < 0)
                    text_offset = 0;
            } else if(my - text_offset > size_w)
                text_offset = my - size_w;
        }
        if(text_offset != preoff) {
            vertices_dirty = true;
            sel_change = true;
        }
    }
    
    void SGTextEdit::CheckDragPos() {
        int cur = GetHitIndex(position_drag);
        SetSelRegion(cursor_pos, cur);
        int preoff = text_offset;
        if(text_width_cur <= size_abs.x - text_area.left - text_area.width)
            text_offset = 0;
        else {
            int my = cur >= text.length() ? text_width_cur : text_pos_array[cur].x;
            int size_w = size_abs.x - text_area.left - text_area.width - 10;
            if(my < text_offset + 10) {
                text_offset = my - 10;
                if(text_offset < 0)
                    text_offset = 0;
            } else if(my - text_offset > size_w)
                text_offset = my - size_w;
        }
        if(text_offset != preoff) {
            vertices_dirty = true;
            sel_change = true;
        }
    }
    
    std::shared_ptr<SGTextEdit> SGTextEdit::Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, v2i sz) {
        auto ptr = std::make_shared<SGTextEdit>();
        ptr->parent = p;
        ptr->position = pos;
        ptr->size = sz;
        ptr->text_area = textedit_config.tex_config["text_area"];
        ptr->PostResize(true, true);
        auto iter = textedit_config.int_config.find("gui_color");
        if(iter != textedit_config.int_config.end())
            ptr->color = iter->second;
        else
            ptr->color = guiRoot.GetDefaultInt("gui_color");
        auto iter2 = textedit_config.int_config.find("font_size");
        if(iter2 != textedit_config.int_config.end())
            ptr->SetFont(&guiRoot.GetGUIFont(), iter2->second);
        else
            ptr->SetFont(&guiRoot.GetGUIFont(), guiRoot.GetDefaultInt("font_size"));
        auto iter3 = textedit_config.int_config.find("font_color");
        if(iter3 != textedit_config.int_config.end())
            ptr->def_color = iter3->second;
        else
            ptr->def_color = guiRoot.GetDefaultInt("font_color");
        ptr->sel_color = textedit_config.int_config["sel_color"];
        ptr->sel_bcolor = textedit_config.int_config["sel_bcolor"];
        ptr->cursor_time = guiRoot.GetTime().asSeconds();
        glGenBuffers(2, ptr->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(SGVertexVCT) * 44, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ptr->vbo[1]);
        unsigned short index[66];
        for(int i = 0; i < 11; ++i) {
            index[i * 6] = i * 4;
            index[i * 6 + 1] = i * 4 + 2;
            index[i * 6 + 2] = i * 4 + 1;
            index[i * 6 + 3] = i * 4 + 1;
            index[i * 6 + 4] = i * 4 + 2;
            index[i * 6 + 5] = i * 4 + 3;
        }
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
        if(p != nullptr)
            p->AddChild(ptr);
        else
            guiRoot.AddChild(ptr);
        return ptr;
    }
    
    bool SGTextEdit::EventMouseEnter() {
        hoving = true;
        vertices_dirty = true;
        return SGWidgetContainer::EventMouseEnter();
    }
    
    bool SGTextEdit::EventMouseLeave() {
        hoving = false;
        vertices_dirty = true;
        return SGWidgetContainer::EventMouseLeave();
    }
    
    bool SGTextEdit::EventGetFocus() {
        focus = true;
        return SGWidget::EventGetFocus();
    }
    
    bool SGTextEdit::EventLostFocus() {
        focus = false;
        if(sel_start != sel_end) {
            sel_start = sel_end = 0;
            vertices_dirty = true;
            sel_change = true;
        }
        return SGWidget::EventLostFocus();
    }
    
    bool SGTextEdit::DragingBegin(v2i evt) {
        position_drag = evt;
        cursor_pos = GetHitIndex(evt);
        SetSelRegion(cursor_pos, cursor_pos);
        cursor_time = guiRoot.GetTime().asSeconds();
        draging = true;
        drag_check = 0;
        return eventDragBegin.TriggerEvent(*this, evt);
    }
    
    bool SGTextEdit::DragingUpdate(v2i evt) {
        v2i delta = evt - position_drag;
        position_drag = evt;
        CheckDragPos();
        return eventDragUpdate.TriggerEvent(*this, delta);
    }
    
    bool SGTextEdit::DragingEnd(v2i evt) {
        vertices_dirty = true;
        draging = false;
        return eventDragEnd.TriggerEvent(*this, evt);
    }
    
    bool SGTextEdit::EventKeyDown(sf::Event::KeyEvent evt) {
        switch(evt.code) {
            case sf::Keyboard::X:
                if(evt.control) {
                    if(sel_start == sel_end)
                        break;
                    std::wstring str = text.substr(sel_start, sel_end - sel_start);
                    wxTheClipboard->Open();
                    wxTheClipboard->SetData(new wxTextDataObject(str));
                    wxTheClipboard->Close();
                    text.erase(text.begin() + sel_start, text.begin() + sel_end);
                    color_vec.erase(color_vec.begin() + sel_start, color_vec.begin() + sel_end);
                    cursor_pos = sel_start;
                    EvaluateSize();
                    sel_end = sel_start;
                    vertices_dirty = true;
                    sel_change = true;
                }
                break;
            case sf::Keyboard::C:
                if(evt.control) {
                    if(sel_start == sel_end)
                        break;
                    std::wstring str = text.substr(sel_start, sel_end - sel_start);
                    wxTheClipboard->Open();
                    wxTheClipboard->SetData(new wxTextDataObject(str));
                    wxTheClipboard->Close();
                }
                break;
            case sf::Keyboard::V:
                if(evt.control) {
                    if(sel_start != sel_end) {
                        cursor_pos = sel_start;
                        text.erase(text.begin() + sel_start, text.begin() + sel_end);
                        color_vec.erase(color_vec.begin() + sel_start, color_vec.begin() + sel_end);
                    }
                    wxTextDataObject tdo;
                    wxTheClipboard->Open();
                    wxTheClipboard->GetData(tdo);
                    wxTheClipboard->Close();
                    std::wstring str = tdo.GetText().ToStdWstring();
                    text.insert(cursor_pos, str);
                    std::vector<unsigned int> nvec;
                    for(size_t i = 0; i < str.length(); ++i)
                        nvec.push_back(def_color);
                    color_vec.insert(color_vec.begin() + cursor_pos, nvec.begin(), nvec.end());
                    EvaluateSize();
                    sel_end = sel_start;
                    vertices_dirty = true;
                    sel_change = true;
                    cursor_pos += str.length();
                    CheckCursorPos();
                }
                break;
            case sf::Keyboard::BackSpace:
                if(sel_start == sel_end) {
                    if(cursor_pos == 0)
                        break;
                    text.erase(text.begin() + cursor_pos - 1, text.begin() + cursor_pos);
                    color_vec.erase(color_vec.begin() + cursor_pos - 1, color_vec.begin() + cursor_pos);
                    cursor_pos--;
                } else {
                    text.erase(text.begin() + sel_start, text.begin() + sel_end);
                    color_vec.erase(color_vec.begin() + sel_start, color_vec.begin() + sel_end);
                    cursor_pos = sel_start;
                    sel_start = sel_end = 0;
                }
                EvaluateSize();
                CheckCursorPos();
                vertices_dirty = true;
                sel_change = true;
                break;
            case sf::Keyboard::Delete:
                if(sel_start == sel_end) {
                    if(cursor_pos == text.length())
                        break;
                    text.erase(text.begin() + cursor_pos, text.begin() + cursor_pos + 1);
                    color_vec.erase(color_vec.begin() + cursor_pos, color_vec.begin() + cursor_pos + 1);
                } else {
                    text.erase(text.begin() + sel_start, text.begin() + sel_end);
                    color_vec.erase(color_vec.begin() + sel_start, color_vec.begin() + sel_end);
                    cursor_pos = sel_start;
                    sel_start = sel_end = 0;
                }
                EvaluateSize();
                CheckCursorPos();
                vertices_dirty = true;
                sel_change = true;
                break;
            case sf::Keyboard::Left:
                if(sel_start != sel_end) {
                    cursor_pos = sel_start;
                    sel_start = sel_end = 0;
                    vertices_dirty = true;
                    break;
                }
                if(cursor_pos > 0) {
                    cursor_pos--;
                    CheckCursorPos();
                    vertices_dirty = true;
                    cursor_time = guiRoot.GetTime().asSeconds();
                }
                break;
            case sf::Keyboard::Right:
                if(sel_start != sel_end) {
                    cursor_pos = sel_end;
                    sel_start = sel_end = 0;
                    CheckCursorPos();
                    vertices_dirty = true;
                    break;
                }
                if(cursor_pos < text.length()) {
                    cursor_pos++;
                    CheckCursorPos();
                    vertices_dirty = true;
                    cursor_time = guiRoot.GetTime().asSeconds();
                }
                break;
            case sf::Keyboard::Return:
                eventTextEnter.TriggerEvent(*this);
                break;
            default:
                break;
        }
        return eventKeyDown.TriggerEvent(*this, evt);
    }
    
    bool SGTextEdit::EventCharEnter(sf::Event::TextEvent evt) {
        if(evt.unicode < 32 || evt.unicode == 127) // control charactors
            return false;
        if(sel_start != sel_end) {
            text.erase(text.begin() + sel_start, text.begin() + sel_end);
            color_vec.erase(color_vec.begin() + sel_start, color_vec.begin() + sel_end);
            cursor_pos = sel_start;
        }
        text.insert(text.begin() + cursor_pos, evt.unicode);
        color_vec.insert(color_vec.begin() + cursor_pos, def_color);
        EvaluateSize();
        sel_end = sel_start;
        vertices_dirty = true;
        sel_change = true;
        cursor_pos += 1;
        CheckCursorPos();
        return true;
    }
    
    unsigned int SGTextEdit::GetHitIndex(v2i pos) {
        unsigned int index = 0;
        int offset = pos.x - position_abs.x - text_area.left + text_offset;
        for(auto& tpos : text_pos_array) {
            if(tpos.x > offset)
                return index ? index - 1 : 0;
            index++;
        }
        if(text_width_cur > offset)
            return index - 1;
        return index;
    }
    
    SGConfig SGListBox::listbox_config;
    
    SGListBox::~SGListBox() {
        glDeleteBuffers(2, vbo);
    }
    
    void SGListBox::UpdateVertices() {
        if(!vertices_dirty)
            return;
        vertices_dirty = false;
        text_update = true;
        std::vector<SGVertexVCT> vert;
        int m_item_count = (size_abs.y - text_area.top - text_area.height - 1) / line_spacing + 2;
        vert.resize(36 + m_item_count * 8);
        std::vector<unsigned short> index;
        index.resize(54 + m_item_count * 12);
        
        int bw = listbox_config.int_config["border"];
        auto back = listbox_config.tex_config[is_hoving ? "backh" : "back"];
        auto selrect = listbox_config.tex_config["sel_rect"];
        auto iconoffset = guiRoot.GetDefaultRect("iconoffset");
        auto iconrc = guiRoot.GetDefaultInt("iconcolumn");
        guiRoot.SetRectVertex(&vert[0], position_abs.x, position_abs.y, bw, bw,
                              sf::IntRect{back.left, back.top, bw, bw});
        guiRoot.SetRectVertex(&vert[4], position_abs.x + bw, position_abs.y, size_abs.x - bw * 2, bw,
                              sf::IntRect{back.left + bw, back.top, back.width - bw * 2, bw});
        guiRoot.SetRectVertex(&vert[8], position_abs.x + size_abs.x - bw, position_abs.y, bw, bw,
                              sf::IntRect{back.left + back.width - bw, back.top, bw, bw});
        guiRoot.SetRectVertex(&vert[12], position_abs.x, position_abs.y + bw, bw, size_abs.y - bw * 2,
                              sf::IntRect{back.left, back.top + bw, bw, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[16], position_abs.x + bw, position_abs.y + bw, size_abs.x - bw * 2, size_abs.y - bw * 2,
                              sf::IntRect{back.left + bw, back.top + bw, back.width - bw * 2, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[20], position_abs.x + size_abs.x - bw, position_abs.y + bw, bw, size_abs.y - bw * 2,
                              sf::IntRect{back.left + back.width - bw, back.top + bw, bw, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[24], position_abs.x, position_abs.y + size_abs.y - bw, bw, bw,
                              sf::IntRect{back.left, back.top + back.height - bw, bw, bw});
        guiRoot.SetRectVertex(&vert[28], position_abs.x + bw, position_abs.y + size_abs.y - bw, size_abs.x - bw * 2, bw,
                              sf::IntRect{back.left + bw, back.top + back.height - bw, back.width - bw * 2, bw});
        guiRoot.SetRectVertex(&vert[32], position_abs.x + size_abs.x - bw, position_abs.y + size_abs.y - bw, bw, bw,
                              sf::IntRect{back.left + back.width - bw, back.top + back.height - bw, bw, bw});
        for(int i = 0; i < 36; ++i)
            vert[i].color = color;
        int begini = text_offset / line_spacing;
        item_count = 0;
        unsigned int sel_color = listbox_config.int_config["sel_bcolor"];
        unsigned int color1 = listbox_config.int_config["color1"];
        unsigned int color2 = listbox_config.int_config["color2"];
        for(int i = begini; i < items.size() && i < begini + m_item_count; ++i) {
            int li = i - begini;
            guiRoot.SetRectVertex(&vert[36 + li * 8], position_abs.x + bw, position_abs.y + bw + i * line_spacing - text_offset,
                                  size_abs.x - bw * 2, line_spacing, selrect);
            unsigned int bcolor = (i == current_sel) ? sel_color : (i % 2) ? color1 : color2;
            for(int j = 0; j < 4; ++j)
                vert[36 + li * 8 + j].color = bcolor;
            int iconi = std::get<0>(items[i]);
            if(iconi) {
                int x = (iconi - 1) % iconrc;
                int y = (iconi - 1) / iconrc;
                sf::IntRect rct = iconoffset;
                rct.left += x * iconoffset.width;
                rct.top += y * iconoffset.height;
                guiRoot.SetRectVertex(&vert[36 + li * 8 + 4], position_abs.x + bw, position_abs.y + bw + i * line_spacing - text_offset,
                                      iconoffset.width, iconoffset.height, rct);
            }
            item_count++;
        }
        
        for(int i = 0; i < 9 + item_count * 2; ++i) {
            index[i * 6] = i * 4;
            index[i * 6 + 1] = i * 4 + 2;
            index[i * 6 + 2] = i * 4 + 1;
            index[i * 6 + 3] = i * 4 + 1;
            index[i * 6 + 4] = i * 4 + 2;
            index[i * 6 + 5] = i * 4 + 3;
        }
        
        if(m_item_count > max_item_count) {
            if(max_item_count == 0)
                max_item_count = 16;
            while(max_item_count < m_item_count)
                max_item_count *= 2;
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(SGVertexVCT) * (36 + max_item_count * 8), nullptr, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SGVertexVCT) * (36 + item_count * 8), &vert[0]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * (54 + max_item_count * 12), nullptr, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned short) * (54 + item_count * 12), &index[0]);
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SGVertexVCT) * (36 + item_count * 8), &vert[0]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned short) * (54 + item_count * 12), &index[0]);
        }
    }
    
    void SGListBox::Draw() {
        UpdateVertices();
        guiRoot.BindGuiTexture();
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glVertexPointer(2, GL_FLOAT, sizeof(SGVertexVCT), 0);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(SGVertexVCT), (const GLvoid*)SGVertexVCT::color_offset);
        glTexCoordPointer(2, GL_FLOAT, sizeof(SGVertexVCT), (const GLvoid*)SGVertexVCT::tex_offset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
        glDrawElements(GL_TRIANGLES, 54, GL_UNSIGNED_SHORT, 0);
        int tw = size_abs.x - text_area.left - text_area.width;
        int th = size_abs.y - text_area.top - text_area.height;
        for(auto chd : children)
            chd->Draw();
        guiRoot.BeginScissor(sf::IntRect{position_abs.x + text_area.left, position_abs.y + text_area.top, tw, th});
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glVertexPointer(2, GL_FLOAT, sizeof(SGVertexVCT), 0);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(SGVertexVCT), (const GLvoid*)SGVertexVCT::color_offset);
        glTexCoordPointer(2, GL_FLOAT, sizeof(SGVertexVCT), (const GLvoid*)SGVertexVCT::tex_offset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
        glDrawElements(GL_TRIANGLES, item_count * 12, GL_UNSIGNED_SHORT, (GLvoid*)(uintptr_t)(sizeof(unsigned short) * 54));
        DrawText();
        guiRoot.EndScissor();
    }
    
    v2i SGListBox::GetTextOffset() {
        return position_abs + v2i{text_area.left, text_area.top - text_offset};
    }
 
    void SGListBox::UpdateTextVertex() {
        if(!text_update || font == nullptr)
            return;
        text_update = false;
        vert_size = 0;
        if(items.size() == 0)
            return;
        std::vector<SGVertexVCT> charvtx;
        std::vector<unsigned short> charidx;
        std::vector<SGVertexVCT> iconvtx;
        std::vector<unsigned short> iconidx;
        auto tex = font->getTexture(font_size);
        SGVertexVCT cur_char;
        v2i text_pos = GetTextOffset();
        int itemcount = (size_abs.y - text_area.top - text_area.height - 1) / line_spacing + 2;
        int begini = text_offset / line_spacing;
        int scolor = listbox_config.int_config["sel_color"];
        int iconw = guiRoot.GetDefaultRect("iconoffset").width;
        for(int i = begini; i < items.size() && i < begini + itemcount; ++i) {
            unsigned int advx = std::get<0>(items[i]) ? iconw : 0;
            unsigned int advy = font_size + line_spacing * i;
            unsigned int color = (i == current_sel) ? scolor : std::get<2>(items[i]);
            for(auto ch : std::get<1>(items[i])) {
                if(ch < L' ')
                    continue;
                auto& gl = font->getGlyph(ch, font_size, false);
                guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx, text_pos.y + gl.bounds.top + advy, cur_char.vertex);
                cur_char.color = color;
                cur_char.texcoord[0] = (float)gl.textureRect.left / tex_size.x;
                cur_char.texcoord[1] = (float)gl.textureRect.top / tex_size.y;
                charvtx.push_back(cur_char);
                guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx + gl.bounds.width, text_pos.y + gl.bounds.top + advy, cur_char.vertex);
                cur_char.color = color;
                cur_char.texcoord[0] = (float)(gl.textureRect.left + gl.textureRect.width) / tex_size.x;
                cur_char.texcoord[1] = (float)gl.textureRect.top / tex_size.y;
                charvtx.push_back(cur_char);
                guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx, text_pos.y + gl.bounds.top + gl.bounds.height + advy, cur_char.vertex);
                cur_char.color = color;
                cur_char.texcoord[0] = (float)gl.textureRect.left / tex_size.x;
                cur_char.texcoord[1] = (float)(gl.textureRect.top + gl.textureRect.height) / tex_size.y;
                charvtx.push_back(cur_char);
                guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx + gl.bounds.width, text_pos.y + gl.bounds.top + gl.bounds.height + advy, cur_char.vertex);
                cur_char.color = color;
                cur_char.texcoord[0] = (float)(gl.textureRect.left + gl.textureRect.width) / tex_size.x;
                cur_char.texcoord[1] = (float)(gl.textureRect.top + gl.textureRect.height) / tex_size.y;
                charvtx.push_back(cur_char);
                advx += gl.advance + spacing_x;
                
                charidx.push_back(vert_size * 4);
                charidx.push_back(vert_size * 4 + 2);
                charidx.push_back(vert_size * 4 + 1);
                charidx.push_back(vert_size * 4 + 1);
                charidx.push_back(vert_size * 4 + 2);
                charidx.push_back(vert_size * 4 + 3);
                vert_size++;
            }
        }

        if(vert_size > mem_size) {
            if(mem_size == 0)
                mem_size = 16;
            while(mem_size < vert_size)
                mem_size *= 2;
            glBindBuffer(GL_ARRAY_BUFFER, tbo[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(SGVertexVCT) * mem_size * 4, nullptr, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SGVertexVCT) * charvtx.size(), &charvtx[0]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tbo[1]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, mem_size * 6 * sizeof(unsigned short), nullptr, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, charidx.size() * sizeof(unsigned short), &charidx[0]);
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, tbo[0]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SGVertexVCT) * charvtx.size(), &charvtx[0]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tbo[1]);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, charidx.size() * sizeof(unsigned short), &charidx[0]);
        }
    }
    
    void SGListBox::InsertItem(unsigned int index, unsigned short icon, const std::wstring& item, unsigned int color) {
        if(index >= items.size())
            items.push_back(std::make_tuple(icon, item, color));
        else
            items.insert(items.begin() + index, std::make_tuple(icon, item, color));
        if(items.size() * line_spacing > size_abs.y - text_area.top - text_area.height) {
            auto sptr = std::static_pointer_cast<SGScrollBar>(children[0]);
            sptr->SetSliderLength(sptr->GetSize().y * (size_abs.y - text_area.top - text_area.height) / (items.size() * line_spacing));
        }
        current_sel = -1;
        vertices_dirty = true;
    }
    
    void SGListBox::AddItem(unsigned short icon, const std::wstring& item, unsigned int color) {
        items.push_back(std::make_tuple(icon, item, color));
        if(items.size() * line_spacing > size_abs.y - text_area.top - text_area.height) {
            auto sptr = std::static_pointer_cast<SGScrollBar>(children[0]);
            sptr->SetSliderLength(sptr->GetSize().y * (size_abs.y - text_area.top - text_area.height) / (items.size() * line_spacing));
        }
        current_sel = -1;
        vertices_dirty = true;
    }
    
    void SGListBox::RemoveItem(unsigned int index) {
        if(index >= items.size())
            return;
        items.erase(items.begin() + index);
        auto sptr = std::static_pointer_cast<SGScrollBar>(children[0]);
        if(items.size() * line_spacing > size_abs.y - text_area.top - text_area.height)
            sptr->SetSliderLength(sptr->GetSize().y * (size_abs.y - text_area.top - text_area.height) / (items.size() * line_spacing));
        else {
            text_offset = 0;
            sptr->SetSliderLength(0);
        }
        current_sel = -1;
        vertices_dirty = true;
    }
    
    void SGListBox::ClearItem() {
        if(items.size() == 0)
            return;
        items.clear();
        std::static_pointer_cast<SGScrollBar>(children[0])->SetSliderLength(0);
        text_offset = 0;
        current_sel = -1;
        vertices_dirty = true;
    }
    
    void SGListBox::SetItemIcon(unsigned int index, unsigned short icon) {
        if(items.size() == 0)
            return;
        std::get<0>(items[index]) = icon;
    }
    
    void SGListBox::SetItemText(unsigned int index, const std::wstring& text, unsigned int color) {
        if(items.size() == 0)
            return;
        std::get<1>(items[index]) = text;
        std::get<2>(items[index]) = color;
    }
    
    const std::tuple<unsigned short, std::wstring, unsigned int>& SGListBox::GetItem(unsigned int index) {
        static std::tuple<unsigned short, std::wstring, unsigned int> st;
        if(index >= items.size())
            return st;
        return items[index];
    }
    
    void SGListBox::SetSelection(int sel) {
        if(sel < 0 || sel >= items.size())
            current_sel = -1;
        else {
            current_sel = sel;
            if(items.size() * line_spacing > size_abs.y - text_area.top - text_area.height) {
                int max_offset = items.size() * line_spacing - (size_abs.y - text_area.top - text_area.height);
                text_offset = current_sel * line_spacing;
                if(text_offset > max_offset)
                    text_offset = max_offset;
                std::static_pointer_cast<SGScrollBar>(children[0])->SetValue(text_offset * 100.0f / max_offset);
            }
        }
        vertices_dirty = true;
    }
    
    int SGListBox::GetSeletion() {
        return current_sel;
    }
    
    bool SGListBox::EventMouseButtonDown(sf::Event::MouseButtonEvent evt) {
        auto choving = std::static_pointer_cast<SGScrollBar>(hoving.lock());
        if(choving) {
            choving->EventMouseButtonDown(evt);
            return true;
        } else if(evt.button == sf::Mouse::Left
                  && evt.x >= position_abs.x + text_area.left && evt.x <= position_abs.x + size_abs.x - text_area.width
                  && evt.y >= position_abs.y + text_area.top && evt.y <= position_abs.y + size_abs.y - text_area.height) {
            int sel = (evt.y - position_abs.y - text_area.top + text_offset) / line_spacing;
            float now = guiRoot.GetTime().asSeconds();
            if(sel == current_sel) {
                if(now - click_time < 0.3f) {
                    eventDoubleClick.TriggerEvent(*this, sel);
                    click_time = 0.0f;
                } else
                    click_time = now;
            } else {
                current_sel = sel;
                vertices_dirty = true;
                click_time = now;
                eventSelChange.TriggerEvent(*this, sel);
            }
        }
        return true;
    }
    
    bool SGListBox::EventMouseEnter() {
        is_hoving = true;
        vertices_dirty = true;
        return SGWidgetContainer::EventMouseEnter();
    }
    
    bool SGListBox::EventMouseLeave() {
        is_hoving = false;
        vertices_dirty = true;
        return SGWidgetContainer::EventMouseLeave();
    }
    
    bool SGListBox::EventMouseWheel(sf::Event::MouseWheelEvent evt) {
        return children[0]->EventMouseWheel(evt);
    }
    
    bool SGListBox::ScrollBarChange(SGWidget& sender, float value) {
        text_offset = (items.size() * line_spacing - (size_abs.y - text_area.top - text_area.height)) * value / 100.0f;
        vertices_dirty = true;
        return true;
    }
    
    std::shared_ptr<SGListBox> SGListBox::Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, v2i sz) {
        auto ptr = std::make_shared<SGListBox>();
        ptr->parent = p;
        ptr->position = pos;
        ptr->size = sz;
        ptr->text_area = listbox_config.tex_config["text_area"];
        ptr->PostResize(true, true);
        auto iter = listbox_config.int_config.find("gui_color");
        if(iter != listbox_config.int_config.end())
            ptr->color = iter->second;
        else
            ptr->color = guiRoot.GetDefaultInt("gui_color");
        auto iter2 = listbox_config.int_config.find("font_size");
        if(iter2 != listbox_config.int_config.end())
            ptr->SetFont(&guiRoot.GetGUIFont(), iter2->second);
        else
            ptr->SetFont(&guiRoot.GetGUIFont(), guiRoot.GetDefaultInt("font_size"));
        ptr->color1 = listbox_config.int_config["color1"];
        ptr->color2 = listbox_config.int_config["color2"];
        ptr->sel_color = listbox_config.int_config["sel_color"];
        ptr->sel_bcolor = listbox_config.int_config["sel_bcolor"];
        ptr->line_spacing = listbox_config.int_config["spacing"];
        ptr->text_offset = 0;
        glGenBuffers(2, ptr->vbo);
        auto sarea = listbox_config.tex_config["scroll_area"];
        auto scr = SGScrollBar::Create(ptr, {0, 0}, {0, 0}, false);
        scr->SetPosition({-sarea.left, sarea.top}, {1.0f, 0.0f});
        scr->SetSize({0, -sarea.top - sarea.height}, {0.0f, 1.0f});
        scr->SetSliderLength(0);
        scr->eventValueChange.Bind(ptr.get(), &SGListBox::ScrollBarChange);
        if(p != nullptr)
            p->AddChild(ptr);
        else
            guiRoot.AddChild(ptr);
        return ptr;
    }
    
    class SGListBoxInner : public SGListBox {
    public:
        virtual bool EventMouseMove(sf::Event::MouseMoveEvent evt) {
            bool ret = SGWidgetContainer::EventMouseMove(evt);
            if(hoving.expired()) {
                int sel = (evt.y - position_abs.y - text_area.top + text_offset) / line_spacing;
                if(current_sel != sel) {
                    current_sel = sel;
                    vertices_dirty = true;
                }
            }
            return ret;
        }
        
        virtual bool EventMouseButtonDown(sf::Event::MouseButtonEvent evt) {
            auto choving = hoving.lock();
            if(choving) {
                choving->EventMouseButtonDown(evt);
                return true;
            } else if(evt.button == sf::Mouse::Left
                      && evt.x >= position_abs.x + text_area.left && evt.x <= position_abs.x + size_abs.x - text_area.width
                      && evt.y >= position_abs.y + text_area.top && evt.y <= position_abs.y + size_abs.y - text_area.height) {
                int sel = (evt.y - position_abs.y - text_area.top + text_offset) / line_spacing;
                std::static_pointer_cast<SGComboBox>(parent.lock())->SetSelection(sel);
                std::static_pointer_cast<SGComboBox>(parent.lock())->ShowList(false);
            }
            return true;
        }
        
        bool ScrollBarChange(SGWidget& sender, float value) {
            text_offset = (items.size() * line_spacing - (size_abs.y - text_area.top - text_area.height)) * value / 100.0f;
            vertices_dirty = true;
            return true;
        }
        
        static std::shared_ptr<SGListBoxInner> Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, v2i sz) {
            auto ptr = std::make_shared<SGListBoxInner>();
            ptr->parent = p;
            ptr->position = pos;
            ptr->size = sz;
            ptr->text_area = listbox_config.tex_config["text_area"];
            ptr->PostResize(true, true);
            auto iter = listbox_config.int_config.find("gui_color");
            if(iter != listbox_config.int_config.end())
                ptr->color = iter->second;
            else
                ptr->color = guiRoot.GetDefaultInt("gui_color");
            auto iter2 = listbox_config.int_config.find("font_size");
            if(iter2 != listbox_config.int_config.end())
                ptr->SetFont(&guiRoot.GetGUIFont(), iter2->second);
            else
                ptr->SetFont(&guiRoot.GetGUIFont(), guiRoot.GetDefaultInt("font_size"));
            ptr->color1 = listbox_config.int_config["color1"];
            ptr->color2 = listbox_config.int_config["color2"];
            ptr->sel_color = listbox_config.int_config["sel_color"];
            ptr->sel_bcolor = listbox_config.int_config["sel_bcolor"];
            ptr->line_spacing = listbox_config.int_config["spacing"];
            ptr->text_offset = 0;
            glGenBuffers(2, ptr->vbo);
            auto sarea = listbox_config.tex_config["scroll_area"];
            auto scr = SGScrollBar::Create(ptr, {0, 0}, {0, 0}, false);
            scr->SetPosition({-sarea.left, sarea.top}, {1.0f, 0.0f});
            scr->SetSize({0, -sarea.top - sarea.height}, {0.0f, 1.0f});
            scr->SetSliderLength(0);
            scr->eventValueChange.Bind(ptr.get(), &SGListBoxInner::ScrollBarChange);
            if(p != nullptr)
                p->AddChild(ptr);
            else
                guiRoot.AddChild(ptr);
            return ptr;
        }
    };
    
    SGConfig SGComboBox::combobox_config;
    
    SGComboBox::~SGComboBox() {
        glDeleteBuffers(2, vbo);
    }
    
    void SGComboBox::UpdateVertices() {
        if(!vertices_dirty)
            return;
        vertices_dirty = false;
        text_update = true;
        std::array<SGVertexVCT, 40> vert;
        int bw = combobox_config.int_config["border"];
        int ht = combobox_config.int_config["height"];
        auto back = combobox_config.tex_config[is_hoving ? "backh" : "back"];
        auto down = combobox_config.tex_config[show_item ? "down3" : is_hoving ? "down2" : "down1"];
        auto drop = combobox_config.tex_config["drop_area"];
        guiRoot.SetRectVertex(&vert[0], position_abs.x, position_abs.y, bw, bw,
                              sf::IntRect{back.left, back.top, bw, bw});
        guiRoot.SetRectVertex(&vert[4], position_abs.x + bw, position_abs.y, size_abs.x - bw * 2, bw,
                              sf::IntRect{back.left + bw, back.top, back.width - bw * 2, bw});
        guiRoot.SetRectVertex(&vert[8], position_abs.x + size_abs.x - bw, position_abs.y, bw, bw,
                              sf::IntRect{back.left + back.width - bw, back.top, bw, bw});
        guiRoot.SetRectVertex(&vert[12], position_abs.x, position_abs.y + bw, bw, ht - bw * 2,
                              sf::IntRect{back.left, back.top + bw, bw, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[16], position_abs.x + bw, position_abs.y + bw, size_abs.x - bw * 2, ht - bw * 2,
                              sf::IntRect{back.left + bw, back.top + bw, back.width - bw * 2, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[20], position_abs.x + size_abs.x - bw, position_abs.y + bw, bw, ht - bw * 2,
                              sf::IntRect{back.left + back.width - bw, back.top + bw, bw, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[24], position_abs.x, position_abs.y + ht - bw, bw, bw,
                              sf::IntRect{back.left, back.top + back.height - bw, bw, bw});
        guiRoot.SetRectVertex(&vert[28], position_abs.x + bw, position_abs.y + ht - bw, size_abs.x - bw * 2, bw,
                              sf::IntRect{back.left + bw, back.top + back.height - bw, back.width - bw * 2, bw});
        guiRoot.SetRectVertex(&vert[32], position_abs.x + size_abs.x - bw, position_abs.y + ht - bw, bw, bw,
                              sf::IntRect{back.left + back.width - bw, back.top + back.height - bw, bw, bw});
        guiRoot.SetRectVertex(&vert[36], position_abs.x + size_abs.x - drop.left, position_abs.y + drop.top, drop.width, drop.height, down);
        for(int i = 0; i < 40; ++i)
            vert[i].color = color;
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SGVertexVCT) * 40, &vert[0]);
    }
    
    void SGComboBox::Draw() {
        UpdateVertices();
        guiRoot.BindGuiTexture();
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glVertexPointer(2, GL_FLOAT, sizeof(SGVertexVCT), 0);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(SGVertexVCT), (const GLvoid*)SGVertexVCT::color_offset);
        glTexCoordPointer(2, GL_FLOAT, sizeof(SGVertexVCT), (const GLvoid*)SGVertexVCT::tex_offset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
        glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_SHORT, 0);
        int tw = size_abs.x - text_area.left - text_area.width;
        int th = size_abs.y - text_area.top - text_area.height;
        guiRoot.BeginScissor(sf::IntRect{position_abs.x + text_area.left, position_abs.y + text_area.top, tw, th});
        DrawText();
        guiRoot.EndScissor();
        if(show_item)
            for(auto chd : children)
                chd->Draw();
    }
    
    void SGComboBox::PostResize(bool res, bool rep) {
        SGWidgetContainer::PostResize(res, rep);
        if(show_item)
            size_abs.y = combobox_config.int_config["height"] + combobox_config.int_config["listheight"];
        else
            size_abs.y = combobox_config.int_config["height"];
    }
    
    v2i SGComboBox::GetTextOffset() {
        return position_abs + v2i{text_area.left, text_area.top};
    }
    
    void SGComboBox::InsertItem(unsigned int index, const std::wstring& item, unsigned int color) {
        std::static_pointer_cast<SGListBox>(children[0])->InsertItem(index, 0, item, color);
        item_count++;
        if(current_sel >= 0 && index <= current_sel)
            current_sel++;
    }
    
    void SGComboBox::AddItem(const std::wstring& item, unsigned int color) {
        std::static_pointer_cast<SGListBox>(children[0])->AddItem(0, item, color);
        item_count++;
    }
    
    void SGComboBox::RemoveItem(unsigned int index) {
        if(index >= item_count)
            return;
        std::static_pointer_cast<SGListBox>(children[0])->RemoveItem(index);
        item_count--;
        if(index == current_sel) {
            ClearText();
            current_sel = -1;
        } else if(index < current_sel)
            current_sel--;
    }
    
    void SGComboBox::ClearItem() {
        std::static_pointer_cast<SGListBox>(children[0])->ClearItem();
        item_count = 0;
        ClearText();
        current_sel = -1;
    }
    
    void SGComboBox::SetItemText(unsigned int index, const std::wstring& text, unsigned int color) {
        std::static_pointer_cast<SGListBox>(children[0])->SetItemText(index, text, color);
    }
    
    void SGComboBox::SetSelection(int sel) {
        if(sel < 0 || sel >= item_count)
            sel = -1;
        if(sel != current_sel) {
            current_sel = sel;
            auto item = std::static_pointer_cast<SGListBox>(children[0])->GetItem(sel);
            SetText(std::get<1>(item), std::get<2>(item));
            eventSelChange.TriggerEvent(*this, sel);
        }
    }
    
    int SGComboBox::GetSeletion() {
        return current_sel;
    }
    
    void SGComboBox::ShowList(bool show) {
        if(show_item == show)
            return;
        if(show)
            std::static_pointer_cast<SGListBox>(children[0])->SetSelection(current_sel);
        show_item = show;
        PostResize(true, false);
    }
    
    bool SGComboBox::EventMouseMove(sf::Event::MouseMoveEvent evt) {
        bool ret = SGWidgetContainer::EventMouseMove(evt);
        if(!hoving.lock()) {
            if(!is_hoving) {
                is_hoving = true;
                vertices_dirty = true;
            }
        } else {
            if(is_hoving) {
                is_hoving = false;
                vertices_dirty = true;
            }
        }
        return ret;
    }
    
    bool SGComboBox::EventMouseLeave() {
        is_hoving = false;
        vertices_dirty = true;
        return SGWidgetContainer::EventMouseLeave();
    }
    
    bool SGComboBox::EventMouseButtonDown(sf::Event::MouseButtonEvent evt) {
        auto choving = hoving.lock();
        if(choving) {
            choving->EventMouseButtonDown(evt);
            return true;
        } else if(evt.button == sf::Mouse::Left) {
            ShowList(!show_item);
            if(!parent.expired())
                parent.lock()->BringToTop(shared_from_this());
        }
        return true;
    }
    
    bool SGComboBox::EventLostFocus() {
        show_item = false;
        PostResize(true, false);
        return SGWidgetContainer::EventLostFocus();
    }
    
    std::shared_ptr<SGComboBox> SGComboBox::Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, v2i sz) {
        auto ptr = std::make_shared<SGComboBox>();
        ptr->parent = p;
        ptr->position = pos;
        ptr->size = sz;
        ptr->text_area = combobox_config.tex_config["text_area"];
        ptr->PostResize(true, true);
        auto iter = combobox_config.int_config.find("gui_color");
        if(iter != combobox_config.int_config.end())
            ptr->color = iter->second;
        else
            ptr->color = guiRoot.GetDefaultInt("gui_color");
        auto iter2 = combobox_config.int_config.find("font_size");
        if(iter2 != combobox_config.int_config.end())
            ptr->SetFont(&guiRoot.GetGUIFont(), iter2->second);
        else
            ptr->SetFont(&guiRoot.GetGUIFont(), guiRoot.GetDefaultInt("font_size"));
        glGenBuffers(2, ptr->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(SGVertexVCT) * 40, nullptr, GL_DYNAMIC_DRAW);
        unsigned short index[60];
        for(int i = 0; i < 10; ++i) {
            index[i * 6] = i * 4;
            index[i * 6 + 1] = i * 4 + 2;
            index[i * 6 + 2] = i * 4 + 1;
            index[i * 6 + 3] = i * 4 + 1;
            index[i * 6 + 4] = i * 4 + 2;
            index[i * 6 + 5] = i * 4 + 3;
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ptr->vbo[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 60, index, GL_STATIC_DRAW);
        auto lst = SGListBoxInner::Create(ptr, {0, combobox_config.int_config["listoffset"]}, {0, 0});
        lst->SetSize({0, combobox_config.int_config["listheight"]}, {1.0f, 0.0f});
        if(p != nullptr)
            p->AddChild(ptr);
        else
            guiRoot.AddChild(ptr);
        return ptr;
    }
    
    class SGTabInner : public SGWidgetContainer {
        friend class SGTabControl;
    public:
        virtual void PostResize(bool res, bool rep) {
            auto ptr = std::static_pointer_cast<SGTabControl>(parent.lock());
            v2i ssize = ptr->GetClientSize();
            if(res)
                size_abs = size + v2i(ssize.x * size_prop.x, ssize.y * size_prop.y);
            if(rep) {
                position_abs = position + v2i(ssize.x * position_prop.x, ssize.y * position_prop.y);
                position_abs += ptr->GetClientPosition();
            }
            if(res)
                rep = true;
            for(auto& chd : children)
                chd->PostResize(res, rep);
        }
        
        virtual void UpdateVertices() {
            
        }
        
        virtual void Draw() {
            for(auto& iter : children)
                if(iter->IsVisible())
                    iter->Draw();
        }
        
        std::wstring title;
        unsigned int width;
    };
    
    SGConfig SGTabControl::tab_config;
    
    SGTabControl::~SGTabControl() {
        glDeleteBuffers(2, vbo);
    }
    
    v2i SGTabControl::GetClientPosition() {
        return {position_abs.x, position_abs.y + tab_height};
    }
    
    v2i SGTabControl::GetClientSize() {
        return {size_abs.x, size_abs.y - tab_height};
    }
    
    void SGTabControl::UpdateVertices() {
        if(!vertices_dirty)
            return;
        vertices_dirty = false;
        text_update = true;
        std::vector<SGVertexVCT> vert;
        std::vector<unsigned short> index;
        
        int bw = tab_config.int_config["client_border"];
        int ht = tab_config.int_config["client_height"];
        auto back = tab_config.tex_config["client_area"];
        
        vert.resize((9 + item_count * 3) * 4);
        index.resize((9 + item_count * 3) * 6);
        
        guiRoot.SetRectVertex(&vert[0], position_abs.x, position_abs.y + ht, bw, bw,
                              sf::IntRect{back.left, back.top, bw, bw});
        guiRoot.SetRectVertex(&vert[4], position_abs.x + bw, position_abs.y + ht, size_abs.x - bw * 2, bw,
                              sf::IntRect{back.left + bw, back.top, back.width - bw * 2, bw});
        guiRoot.SetRectVertex(&vert[8], position_abs.x + size_abs.x - bw, position_abs.y + ht, bw, bw,
                              sf::IntRect{back.left + back.width - bw, back.top, bw, bw});
        guiRoot.SetRectVertex(&vert[12], position_abs.x, position_abs.y + ht + bw, bw, size_abs.y - ht - bw * 2,
                              sf::IntRect{back.left, back.top + bw, bw, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[16], position_abs.x + bw, position_abs.y + ht + bw, size_abs.x - bw * 2, size_abs.y - ht - bw * 2,
                              sf::IntRect{back.left + bw, back.top + bw, back.width - bw * 2, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[20], position_abs.x + size_abs.x - bw, position_abs.y + ht + bw, bw, size_abs.y - ht - bw * 2,
                              sf::IntRect{back.left + back.width - bw, back.top + bw, bw, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[24], position_abs.x, position_abs.y + size_abs.y - bw, bw, bw,
                              sf::IntRect{back.left, back.top + back.height - bw, bw, bw});
        guiRoot.SetRectVertex(&vert[28], position_abs.x + bw, position_abs.y + size_abs.y - bw, size_abs.x - bw * 2, bw,
                              sf::IntRect{back.left + bw, back.top + back.height - bw, back.width - bw * 2, bw});
        guiRoot.SetRectVertex(&vert[32], position_abs.x + size_abs.x - bw, position_abs.y + size_abs.y - bw, bw, bw,
                              sf::IntRect{back.left + back.width - bw, back.top + back.height - bw, bw, bw});
        
        int tb = tab_config.int_config["tab_border"];
        auto t1 = tab_config.tex_config["tab_title1"];
        auto t2 = tab_config.tex_config["tab_title2"];
        auto t3 = tab_config.tex_config["tab_title3"];
        int ti = tab_ol;
        int startv = 36;
        bool act = false;
        int actti = 0;
        for(int i = 0; i < item_count; ++i) {
            auto ptr = std::static_pointer_cast<SGTabInner>(children[i]);
            if(ptr == active_tab.lock()) {
                act = true;
                actti = ti;
                ti += ptr->width;
                continue;
            }
            auto& rct = (ptr == hover_tab.lock()) ? t2 : t3;
            guiRoot.SetRectVertex(&vert[startv], position_abs.x + ti, position_abs.y, tb, tab_height,
                                  sf::IntRect{rct.left, rct.top, tb, rct.height});
            guiRoot.SetRectVertex(&vert[startv + 4], position_abs.x + ti + tb, position_abs.y, ptr->width - tb * 2, tab_height,
                                  sf::IntRect{rct.left + tb, rct.top, rct.width - tb * 2, rct.height});
            guiRoot.SetRectVertex(&vert[startv + 8], position_abs.x + ti + ptr->width - tb, position_abs.y, tb, tab_height,
                                  sf::IntRect{rct.left + rct.width - tb, rct.top, tb, rct.height});
            ti += ptr->width;
            startv += 12;
        }
        if(act) {
            auto ptr = std::static_pointer_cast<SGTabInner>(active_tab.lock());
            ti = actti;
            guiRoot.SetRectVertex(&vert[startv], position_abs.x + ti, position_abs.y, tb, tab_height,
                                  sf::IntRect{t1.left, t1.top, tb, t1.height});
            guiRoot.SetRectVertex(&vert[startv + 4], position_abs.x + ti + tb, position_abs.y, ptr->width - tb * 2, tab_height,
                                  sf::IntRect{t1.left + tb, t1.top, t1.width - tb * 2, t1.height});
            guiRoot.SetRectVertex(&vert[startv + 8], position_abs.x + ti + ptr->width - tb, position_abs.y, tb, tab_height,
                                  sf::IntRect{t1.left + t1.width - tb, t1.top, tb, t1.height});
        }
        
        for(auto& vt : vert)
            vt.color = color;
        for(int i = 0; i < 9 + item_count * 3; ++i) {
            index[i * 6] = i * 4;
            index[i * 6 + 1] = i * 4 + 2;
            index[i * 6 + 2] = i * 4 + 1;
            index[i * 6 + 3] = i * 4 + 1;
            index[i * 6 + 4] = i * 4 + 2;
            index[i * 6 + 5] = i * 4 + 3;
        }
        
        if(max_item_count == 0 || item_count > max_item_count) {
            if(max_item_count == 0)
                max_item_count = 8;
            while(max_item_count < item_count)
                max_item_count *= 2;
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(SGVertexVCT) * ((9 + max_item_count * 3) * 4), nullptr, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SGVertexVCT) * ((9 + item_count * 3) * 4), &vert[0]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * ((9 + max_item_count * 3) * 6), nullptr, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned short) * ((9 + item_count * 3) * 6), &index[0]);
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SGVertexVCT) * ((9 + item_count * 3) * 4), &vert[0]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned short) * ((9 + item_count * 3) * 6), &index[0]);
        }
    }
    
    void SGTabControl::Draw() {
        EvaluateSize();
        UpdateVertices();
        guiRoot.BindGuiTexture();
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glVertexPointer(2, GL_FLOAT, sizeof(SGVertexVCT), 0);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(SGVertexVCT), (const GLvoid*)SGVertexVCT::color_offset);
        glTexCoordPointer(2, GL_FLOAT, sizeof(SGVertexVCT), (const GLvoid*)SGVertexVCT::tex_offset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
        guiRoot.BeginScissor(sf::IntRect{position_abs.x, position_abs.y, size_abs.x, size_abs.y});
        glDrawElements(GL_TRIANGLES, (9 + item_count * 3) * 6, GL_UNSIGNED_SHORT, 0);
        DrawText();
        guiRoot.EndScissor();
        guiRoot.BeginScissor(sf::IntRect{position_abs.x, position_abs.y + tab_height, size_abs.x, size_abs.y - tab_height});
        if(!active_tab.expired())
            active_tab.lock()->Draw();
        guiRoot.EndScissor();
    }
    
    void SGTabControl::PostResize(bool res, bool rep) {
        SGWidgetContainer::PostResize(res, rep);
        size_dirty = true;
    }
    
    v2i SGTabControl::GetTextOffset() {
        return position_abs;
    }
    
    void SGTabControl::EvaluateSize(const std::wstring& t) {
        if(!size_dirty)
            return;
        size_dirty = false;
        item_count = 0;
        int sz = children.size();
        if(sz == 0)
            return;
        int width = size_abs.x - tab_ol - tab_or;
        int w1 = tab_config.int_config["tab_min_width1"];
        int w2 = tab_config.int_config["tab_min_width2"];
        int wp = width / sz;
        int wh = wp;
        if(wh < w2) {
            wh = w2;
            if(sz > 1)
                wp = (width - wh) / (sz - 1);
        }
        if(wp < w1)
            wp = w1;
        auto atb = active_tab.lock();
        int max_w = 0;
        for(size_t i = 0; i < children.size(); ++i) {
            auto ptr = std::static_pointer_cast<SGTabInner>(children[i]);
            if(ptr == atb)
                ptr->width = wh;
            else
                ptr->width = wp;
            if(item_count == 0) {
                if(max_w + ptr->width > width)
                    item_count = i + 1;
                else
                    max_w += ptr->width;
            }
        }
        if(item_count == 0)
            item_count = sz;
    }
    
    void SGTabControl::UpdateTextVertex() {
        if(!text_update || font == nullptr)
            return;
        text_update = false;
        vert_size = 0;
        if(children.size() == 0)
            return;
        std::vector<SGVertexVCT> charvtx;
        std::vector<unsigned short> charidx;
        std::vector<SGVertexVCT> iconvtx;
        std::vector<unsigned short> iconidx;
        auto tex = font->getTexture(font_size);
        SGVertexVCT cur_char;
        v2i text_pos = GetTextOffset();
        int tb = tab_config.int_config["tab_border"];
        int ti = tab_ol;
        text_pos.y += font_size + tab_config.int_config["title_offset"];
        for(int i = 0; i < children.size(); ++i) {
            auto ptr = std::static_pointer_cast<SGTabInner>(children[i]);
            unsigned int advx = ti + tb;
            unsigned int color = ptr->color;
            ti += ptr->width;
            unsigned int tw = tb * 2;
            for(auto ch : ptr->title) {
                if(ch < L' ')
                    continue;
                auto& gl = font->getGlyph(ch, font_size, false);
                if(tw + gl.advance > ptr->width)
                    break;
                tw += gl.advance + spacing_x;
            }
            advx += (ptr->width > tw) ? (ptr->width - tw) / 2 : 0;
            tw = tb * 2;
            for(auto ch : ptr->title) {
                if(ch < L' ')
                    continue;
                auto& gl = font->getGlyph(ch, font_size, false);
                if(tw + gl.advance > ptr->width)
                    break;
                guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx, text_pos.y + gl.bounds.top, cur_char.vertex);
                cur_char.color = color;
                cur_char.texcoord[0] = (float)gl.textureRect.left / tex_size.x;
                cur_char.texcoord[1] = (float)gl.textureRect.top / tex_size.y;
                charvtx.push_back(cur_char);
                guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx + gl.bounds.width, text_pos.y + gl.bounds.top, cur_char.vertex);
                cur_char.color = color;
                cur_char.texcoord[0] = (float)(gl.textureRect.left + gl.textureRect.width) / tex_size.x;
                cur_char.texcoord[1] = (float)gl.textureRect.top / tex_size.y;
                charvtx.push_back(cur_char);
                guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx, text_pos.y + gl.bounds.top + gl.bounds.height, cur_char.vertex);
                cur_char.color = color;
                cur_char.texcoord[0] = (float)gl.textureRect.left / tex_size.x;
                cur_char.texcoord[1] = (float)(gl.textureRect.top + gl.textureRect.height) / tex_size.y;
                charvtx.push_back(cur_char);
                guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx + gl.bounds.width, text_pos.y + gl.bounds.top + gl.bounds.height, cur_char.vertex);
                cur_char.color = color;
                cur_char.texcoord[0] = (float)(gl.textureRect.left + gl.textureRect.width) / tex_size.x;
                cur_char.texcoord[1] = (float)(gl.textureRect.top + gl.textureRect.height) / tex_size.y;
                charvtx.push_back(cur_char);
                advx += gl.advance + spacing_x;
                tw += gl.advance + spacing_x;
                
                charidx.push_back(vert_size * 4);
                charidx.push_back(vert_size * 4 + 2);
                charidx.push_back(vert_size * 4 + 1);
                charidx.push_back(vert_size * 4 + 1);
                charidx.push_back(vert_size * 4 + 2);
                charidx.push_back(vert_size * 4 + 3);
                vert_size++;
            }
        }
        
        if(vert_size > mem_size) {
            if(mem_size == 0)
                mem_size = 16;
            while(mem_size < vert_size)
                mem_size *= 2;
            glBindBuffer(GL_ARRAY_BUFFER, tbo[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(SGVertexVCT) * mem_size * 4, nullptr, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SGVertexVCT) * charvtx.size(), &charvtx[0]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tbo[1]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, mem_size * 6 * sizeof(unsigned short), nullptr, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, charidx.size() * sizeof(unsigned short), &charidx[0]);
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, tbo[0]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SGVertexVCT) * charvtx.size(), &charvtx[0]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tbo[1]);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, charidx.size() * sizeof(unsigned short), &charidx[0]);
        }
    }
    
    void SGTabControl::AddTab(const std::wstring& title, unsigned int color) {
        auto ptr = std::make_shared<SGTabInner>();
        ptr->parent = std::static_pointer_cast<SGWidgetContainer>(shared_from_this());
        ptr->title = title;
        ptr->color = color;
        ptr->PostResize(true, true);
        children.push_back(ptr);
        if(children.size() == 1)
            active_tab = ptr;
        vertices_dirty = true;
        size_dirty = true;
    }
    
    void SGTabControl::RemoveTab(unsigned int index) {
        if(index >= children.size())
            return;
        if(children[index] == active_tab.lock()) {
            if(index < children.size() - 1)
                active_tab = children[index + 1];
            else if(index > 0)
                active_tab = children[index - 1];
        }
        children.erase(children.begin() + index);
        vertices_dirty = true;
        size_dirty = true;
    }
    
    void SGTabControl::SetTabTitle(unsigned int index, const std::wstring& title, unsigned int color) {
        if(index >= children.size())
            return;
        auto ptr = std::static_pointer_cast<SGTabInner>(children[index]);
        ptr->title = title;
        ptr->color = color;
        size_dirty = true;
        text_update = true;
    }
    
    std::shared_ptr<SGWidgetContainer> SGTabControl::GetTab(int index) {
        if(index >= children.size())
            return nullptr;
        return std::static_pointer_cast<SGWidgetContainer>(children[index]);
    }
    
    void SGTabControl::SetActiveTab(int index) {
        if(index >= children.size())
            return;
        active_tab = children[index];
        vertices_dirty = true;
    }
    
    int SGTabControl::GetTabCount() {
        return children.size();
    }
    
    int SGTabControl::GetActiveTab() {
        for(int i = 0; i < children.size(); ++i)
            if(children[i] == active_tab.lock())
                return i;
        return -1;
    }
    
    bool SGTabControl::EventMouseMove(sf::Event::MouseMoveEvent evt) {
        if(evt.y < position_abs.y + tab_height) {
            if(in_tab) {
                if(!active_tab.expired())
                    active_tab.lock()->EventMouseLeave();
                in_tab = false;
            }
            int ind = GetHovingTab(v2i{evt.x, evt.y});
            if(ind == -1)
                return true;
            auto ptr = children[ind];
            if(hover_tab.lock() != ptr) {
                hover_tab = ptr;
                vertices_dirty = true;
            }
            return true;
        } else {
            if(!in_tab) {
                if(active_tab.expired())
                    active_tab.lock()->EventMouseEnter();
                in_tab = true;
            }
            if(!hover_tab.expired()) {
                hover_tab.reset();
                vertices_dirty = true;
            }
            if(!active_tab.expired())
                active_tab.lock()->EventMouseMove(evt);
            return true;
        }
        return false;
    }
    
    bool SGTabControl::EventMouseEnter() {
        if(in_tab && active_tab.expired())
            return active_tab.lock()->EventMouseEnter();
        return false;
    }
    
    bool SGTabControl::EventMouseLeave() {
        if(!hover_tab.expired()) {
            hover_tab.reset();
            vertices_dirty = true;
        }
        if(in_tab && !active_tab.expired())
            return active_tab.lock()->EventMouseLeave();
        return false;
    }
    
    bool SGTabControl::EventMouseButtonDown(sf::Event::MouseButtonEvent evt) {
        if(!in_tab) {
            if(evt.button == sf::Mouse::Button::Left) {
                int ind = GetHovingTab(v2i{evt.x, evt.y});
                if(ind == -1)
                    return true;
                auto ptr = children[ind];
                if(hover_tab.lock() != ptr) {
                    hover_tab = ptr;
                    vertices_dirty = true;
                }
                if(active_tab.lock() != ptr) {
                    active_tab = ptr;
                    size_dirty = true;
                    vertices_dirty = true;
                }
                return true;
            }
        } else
            if(!active_tab.expired())
                return active_tab.lock()->EventMouseButtonDown(evt);
        return false;
    }
    
    bool SGTabControl::EventMouseButtonUp(sf::Event::MouseButtonEvent evt) {
        if(in_tab && !active_tab.expired())
            return active_tab.lock()->EventMouseButtonUp(evt);
        return false;
    }
    
    bool SGTabControl::EventMouseWheel(sf::Event::MouseWheelEvent evt) {
        if(!active_tab.expired())
            return active_tab.lock()->EventMouseWheel(evt);
        return false;
    }
    
    bool SGTabControl::EventKeyDown(sf::Event::KeyEvent evt) {
        if(!active_tab.expired())
            return active_tab.lock()->EventKeyDown(evt);
        return false;
    }
    
    bool SGTabControl::EventKeyUp(sf::Event::KeyEvent evt) {
        if(!active_tab.expired())
            return active_tab.lock()->EventKeyUp(evt);
        return false;
    }
    
    bool SGTabControl::EventCharEnter(sf::Event::TextEvent evt) {
        if(!active_tab.expired())
            return active_tab.lock()->EventCharEnter(evt);
        return false;
    }
    
    int SGTabControl::GetHovingTab(v2i pos) {
        if(children.size() == 0)
            return -1;
        int ti = position_abs.x + tab_ol;
        for(int i = 0; i < children.size(); ++i) {
            auto ptr = std::static_pointer_cast<SGTabInner>(children[i]);
            if(pos.x >= ti && pos.x <= ti + ptr->width)
                return i;
            ti += ptr->width;
        }
        return -1;
    }
    
    std::shared_ptr<SGTabControl> SGTabControl::Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, v2i sz) {
        auto ptr = std::make_shared<SGTabControl>();
        ptr->parent = p;
        ptr->position = pos;
        ptr->size = sz;
        ptr->PostResize(true, true);
        auto iter = tab_config.int_config.find("gui_color");
        if(iter != tab_config.int_config.end())
            ptr->color = iter->second;
        else
            ptr->color = guiRoot.GetDefaultInt("gui_color");
        auto iter2 = tab_config.int_config.find("font_size");
        if(iter2 != tab_config.int_config.end())
            ptr->SetFont(&guiRoot.GetGUIFont(), iter2->second);
        else
            ptr->SetFont(&guiRoot.GetGUIFont(), guiRoot.GetDefaultInt("font_size"));
        ptr->tab_height = tab_config.int_config["tab_height"];
        ptr->tab_ol = tab_config.int_config["tab_offsetl"];
        ptr->tab_or = tab_config.int_config["tab_offsetr"];
        glGenBuffers(2, ptr->vbo);
        if(p != nullptr)
            p->AddChild(ptr);
        else
            guiRoot.AddChild(ptr);
        return ptr;
    }
}

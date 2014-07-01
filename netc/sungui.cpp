#define GLEW_STATIC
#include <GL/glew.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <array>
#include <iostream>

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
        v2i ssize = (ptr == nullptr) ? guiRoot.GetSceneSize() : ptr->size_abs;
        if(res)
            size_abs = size + v2i(ssize.x * size_prop.x, ssize.y * size_prop.y);
        if(rep) {
            position_abs = position + v2i(ssize.x * position_prop.x, ssize.y * position_prop.y);
            if(ptr != nullptr)
                position_abs += ptr->position_abs;
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
    
    SGTextBase::~SGTextBase() {
        if(text.length())
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
        glDeleteBuffers(2, tbo);
        text_update = true;
        text_dirty = true;
        text_width = 0;
        text_height = 0;
        text_width_cur = font_size;
    }
    
    unsigned int SGTextBase::GetTextColor(int index) {
        return color_vec[index];
    }
    
    void SGTextBase::SetText(const std::wstring& t, unsigned int cl) {
        if(t.length() == 0)
            return;
        if(text.length() == 0)
            glGenBuffers(2, tbo);
        text = t;
        color_vec.clear();
        for(size_t i = 0; i < text.length(); ++i)
            color_vec.push_back(cl);
        text_update = true;
        text_dirty = true;
        EvaluateSize();
    }
    
    void SGTextBase::AppendText(const std::wstring& t, unsigned int cl) {
        if(t.length() == 0)
            return;
        if(text.length() == 0)
            glGenBuffers(2, tbo);
        text += t;
        for(size_t i = 0; i < t.length(); ++i)
            color_vec.push_back(cl);
        text_update = true;
        text_dirty = true;
        EvaluateSize(t);
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
        int pre_vert_size = vert_size;
        vert_size = 0;
        if(text.length() == 0) {
            text_dirty = false;
            return;
        }
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
        if(text_dirty && vert_size > pre_vert_size) {
            glBindBuffer(GL_ARRAY_BUFFER, tbo[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(SGVertexVCT) * charvtx.size(), &charvtx[0], GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tbo[1]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(unsigned short), &index[0], GL_DYNAMIC_DRAW);
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, tbo[0]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SGVertexVCT) * charvtx.size(), &charvtx[0]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tbo[1]);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, index.size() * sizeof(unsigned short), &index[0]);
        }
        text_dirty = false;
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
            voffset = ((int)(frame_clock.getElapsedTime().asSeconds() / frame_time) % texcoords.size() * verts.size()) * sizeof(SGVertexVCT);
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
        v2i ssize = (ptr == nullptr) ? guiRoot.GetSceneSize() : ptr->size_abs;
        if(res)
            size_abs = size + v2i(ssize.x * size_prop.x, ssize.y * size_prop.y);
        if(rep) {
            position_abs = position + v2i(ssize.x * position_prop.x, ssize.y * position_prop.y);
            if(ptr != nullptr)
                position_abs += ptr->position_abs;
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
    
    unsigned int SGLabel::GetHitIndex(v2i pos) {
        unsigned int index = 0;
//        int ls = GetLineSpacing();
//        v2i offset = pos - position_abs;
//        for(auto tpos : text_pos_array) {
//            if(tpos.y > offset.y)
//                break;
//            if(tpos.x > offset.x && tpos.y + ls > offset.y)
//                break;
//            index++;
//        }
//        if(index > 0)
//            index--;
        return index;
    }
    
    bool SGLabel::EventMouseLeave() {
        return SGWidget::EventMouseLeave();
    }
    
    bool SGLabel::EventMouseMove(sf::Event::MouseMoveEvent evt) {
        return SGWidget::EventMouseMove(evt);
    }
    
    bool SGLabel::EventMouseButtonDown(sf::Event::MouseButtonEvent evt) {
        return SGWidget::EventMouseButtonDown(evt);
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
        auto back = tex_rect[state];
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
    
    std::shared_ptr<SGButton> SGButton::Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, v2i size) {
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
        if(guiRoot.GetClickObject().get() == this)
            state = 2;
        else
            state = 1;
        vertices_dirty = true;
        return SGWidget::EventMouseEnter();
    }
    
    bool SGButton::EventMouseLeave() {
        state = 0;
        vertices_dirty = true;
        return SGWidget::EventMouseLeave();
    }
    
    bool SGButton::EventMouseButtonDown(sf::Event::MouseButtonEvent evt) {
        if(evt.button == sf::Mouse::Left) {
            state = 2;
            vertices_dirty = true;
            guiRoot.SetClickingObject(shared_from_this());
        }
        return SGWidget::EventMouseButtonDown(evt);
    }
    
    bool SGButton::EventMouseButtonUp(sf::Event::MouseButtonEvent evt) {
        bool click = false;
        if(state == 2 && evt.button == sf::Mouse::Left) {
            vertices_dirty = true;
            state = 1;
            click = true;
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
        v2i ssize = (ptr == nullptr) ? guiRoot.GetSceneSize() : ptr->GetSize();
        if(res) {
            auto rec = checkbox_config.tex_config["offset"];
            size_abs = v2i{rec.left + rec.width + rec.top + text_width_cur, std::max(text_height , rec.height)};
        }
        if(rep) {
            position_abs = position + v2i(ssize.x * position_prop.x, ssize.y * position_prop.y);
            if(ptr != nullptr)
                position_abs += ptr->GetPosition();
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
        if(group != nullptr)
            group->RemoveRadio(this);
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
        checked = chk;
        vertices_dirty = true;
        if(chk && group) {
            SGRadio* pre = group->GetCurrent();
            group->MakeCurrent(this);
            if(pre)
                pre->SetChecked(false);
        }
        eventCheckChange.TriggerEvent(*this, checked);
    }
    
    void SGRadio::SetGroup(std::shared_ptr<SGRadio> rdo) {
        if(rdo->group == nullptr) {
            rdo->group = std::make_shared<SGRadioGroup>();
            rdo->group->AddRadio(this);
        }
        if(group != nullptr)
            group->RemoveRadio(this);
        group = rdo->group;
        group->AddRadio(this);
    }
    
    std::shared_ptr<SGRadio> SGRadio::GetCheckedTarget() {
        if(group == nullptr) {
            if(checked)
                return std::static_pointer_cast<SGRadio>(shared_from_this());
            return nullptr;
        }
        SGRadio* rdo = group->GetCurrent();
        if(rdo)
            return std::static_pointer_cast<SGRadio>(rdo->shared_from_this());
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
            pos_max = position_abs.x + size_abs.x - scroll_config.tex_config["sliderh1"].width;
            size_abs.y = scroll_config.int_config["hheight"];
        } else {
            pos_min = position_abs.y;
            pos_max = position_abs.y + size_abs.y - scroll_config.tex_config["sliderv1"].height;
            size_abs.x = scroll_config.int_config["vwidth"];
        }
    }
    
    void SGScrollBar::UpdateVertices() {
        if(!vertices_dirty)
            return;
        vertices_dirty = false;
        std::array<SGVertexVCT, 8> vert;
        if(is_horizontal) {
            int hheight = scroll_config.int_config["hheight"];
            auto backh = scroll_config.tex_config["backh"];
            auto sliderh = scroll_config.tex_config[slider_moving ? "sliderh3" : slider_hoving ? "sliderh2" : "sliderh1"];
            guiRoot.SetRectVertex(&vert[0], position_abs.x, position_abs.y, size_abs.x, hheight, backh);
            guiRoot.SetRectVertex(&vert[4], position_abs.x + current_pos, position_abs.y, sliderh.width, hheight, sliderh);
        } else {
            int vwidth = scroll_config.int_config["vwidth"];
            auto backv = scroll_config.tex_config["backv"];
            auto sliderv = scroll_config.tex_config[slider_moving ? "sliderv3" : slider_hoving ? "sliderv2" : "sliderv1"];
            guiRoot.SetRectVertex(&vert[0], position_abs.x, position_abs.y, vwidth, size_abs.y, backv);
            guiRoot.SetRectVertex(&vert[4], position_abs.x, position_abs.y + current_pos, vwidth, sliderv.height, sliderv);
        }
        for(int i = 0; i < 8; ++i)
            vert[i].color = color;
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SGVertexVCT) * 8, &vert);
    }
    
    void SGScrollBar::Draw() {
        UpdateVertices();
        guiRoot.BindGuiTexture();
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glVertexPointer(2, GL_FLOAT, sizeof(SGVertexVCT), 0);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(SGVertexVCT), (const GLvoid*)SGVertexVCT::color_offset);
        glTexCoordPointer(2, GL_FLOAT, sizeof(SGVertexVCT), (const GLvoid*)SGVertexVCT::tex_offset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_SHORT, 0);
    }
    
    void SGScrollBar::SetRange(float minv, float maxv, float cur) {
        minvalue = minv;
        maxvalue = maxv;
        current_pos = (cur - minv) / maxv * (pos_max - pos_min);
        if(current_pos < 0)
            current_pos = 0;
        if(current_pos > pos_max - pos_min)
            current_pos = pos_max - pos_min;
        vertices_dirty = true;
    }
    
    void SGScrollBar::SetValue(float cur) {
        int prepos = current_pos;
        current_pos = (cur - minvalue) / maxvalue * (pos_max - pos_min);
        if(current_pos < 0)
            current_pos = 0;
        if(current_pos > pos_max - pos_min)
            current_pos = pos_max - pos_min;
        if(prepos != current_pos) {
            vertices_dirty = true;
            float val = (float)current_pos / (pos_max - pos_min) * (maxvalue - minvalue) + minvalue;
            eventValueChange.TriggerEvent(*this, val);
        }
    }
    
    bool SGScrollBar::EventMouseMove(sf::Event::MouseMoveEvent evt) {
        bool preh = slider_hoving;
        if(is_horizontal) {
            if(evt.x >= position_abs.x + current_pos && evt.x <= position_abs.x + current_pos + slider_range)
                slider_hoving = true;
            else
                slider_hoving = false;
        } else {
            if(evt.y >= position_abs.y + current_pos && evt.y <= position_abs.y + current_pos + slider_range)
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
            ptr->slider_range = scroll_config.tex_config["sliderh1"].width;
        else
            ptr->slider_range = scroll_config.tex_config["sliderv1"].height;
        ptr->PostResize(true, true);
        auto iter = scroll_config.int_config.find("gui_color");
        if(iter != scroll_config.int_config.end())
            ptr->color = iter->second;
        else
            ptr->color = guiRoot.GetDefaultInt("gui_color");
        glGenBuffers(2, ptr->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(SGVertexVCT) * 8, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ptr->vbo[1]);
        unsigned short index[] = {0, 2, 1, 1, 2, 3, 4, 6, 5, 5, 6, 7};
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
            float tm = frame_clock.getElapsedTime().asSeconds();
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
        float tm = frame_clock.getElapsedTime().asSeconds();
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
            text_dirty = true;
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
            text_dirty = true;
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
        ptr->cursor_time = ptr->frame_clock.getElapsedTime().asSeconds();
        glGenBuffers(2, ptr->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(SGVertexVCT) * 52, nullptr, GL_DYNAMIC_DRAW);
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
        cursor_time = frame_clock.getElapsedTime().asSeconds();
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
                    text_dirty = true;
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
                    text_dirty = true;
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
                text_dirty = true;
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
                text_dirty = true;
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
                    cursor_time = frame_clock.getElapsedTime().asSeconds();
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
                    cursor_time = frame_clock.getElapsedTime().asSeconds();
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
        }
        text.insert(text.begin() + cursor_pos, evt.unicode);
        color_vec.insert(color_vec.begin() + cursor_pos, def_color);
        EvaluateSize();
        sel_end = sel_start;
        vertices_dirty = true;
        text_dirty = true;
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
    
}

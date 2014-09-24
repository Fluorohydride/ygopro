#include "../common/common.h"

#include "../buildin/rapidxml.hpp"
#include "../buildin/rapidxml_print.hpp"

#include "sungui.h"

namespace sgui
{
    
    static SGGUIRoot guiRoot;
    
    SGWidget::~SGWidget() {
        eventDestroying.TriggerEvent(*this);
        if(vbo)
            glDeleteBuffers(1, &vbo);
        vbo = 0;
        if(vao)
            glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
    
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
            size_abs = size + v2i{(int)(ssize.x * size_prop.x), (int)(ssize.y * size_prop.y)};
        if(rep) {
            int posx = (position_prop.x < 0.0f) ? ((ssize.x - size_abs.x) / 2) : (position.x + (int)(ssize.x * position_prop.x));
            int posy = (position_prop.y < 0.0f) ? ((ssize.y - size_abs.y) / 2) : (position.y + (int)(ssize.y * position_prop.y));
            position_abs = {posx, posy};
            if(ptr != nullptr)
                position_abs += ptr->GetClientPosition();
        }
    }
    
    void SGWidget::Destroy() {
        auto p = parent.lock();
        if(p != nullptr)
            p->RemoveChild(shared_from_this());
        else
            guiRoot.RemoveChild(shared_from_this());
    }
    
    void SGWidget::SetFocus() {
        auto p = parent.lock();
        if(p != nullptr) {
            p->SetFocusWidget(shared_from_this());
            p->SetFocus();
        } else
            guiRoot.SetFocusWidget(shared_from_this());
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
    
    bool SGWidget::EventMouseMove(MouseMoveEvent evt) {
        return eventMouseMove.TriggerEvent(*this, evt);
    }
    
    bool SGWidget::EventMouseEnter() {
        return eventMouseEnter.TriggerEvent(*this);
    }
    
    bool SGWidget::EventMouseLeave() {
        return eventMouseLeave.TriggerEvent(*this);
    }
    
    bool SGWidget::EventMouseButtonDown(MouseButtonEvent evt) {
        auto dr = GetDragingTarget();
        if(dr && (evt.button == GLFW_MOUSE_BUTTON_LEFT)) {
            guiRoot.ObjectDragingBegin(dr, MouseMoveEvent{evt.x, evt.y});
        }
        return eventMouseButtonDown.TriggerEvent(*this, evt);
    }
    
    bool SGWidget::EventMouseButtonUp(MouseButtonEvent evt) {
        return eventMouseButtonUp.TriggerEvent(*this, evt);
    }
    
    bool SGWidget::EventMouseWheel(MouseWheelEvent evt) {
        return eventMouseWheel.TriggerEvent(*this, evt);
    }
    
    bool SGWidget::EventKeyDown(KeyEvent evt) {
        return eventKeyDown.TriggerEvent(*this, evt);
    }
    
    bool SGWidget::EventKeyUp(KeyEvent evt) {
        return eventKeyUp.TriggerEvent(*this, evt);
    }
    
    bool SGWidget::EventCharEnter(TextEvent evt) {
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
        glGenBuffers(1, &tbo);
        glGenVertexArrays(1, &tao);
        glBindVertexArray(tao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, tbo);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), 0);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::color_offset);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::tex_offset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, guiRoot.GetIndexBuffer());
        glBindVertexArray(0);
    }
    
    SGTextBase::~SGTextBase() {
        glDeleteBuffers(1, &tbo);
        glDeleteVertexArrays(1, &tao);
    }
    
    void SGTextBase::SetFont(glbase::Font* ft) {
        font = ft;
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
        return font->GetFontSize() + spacing_y;
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
        for(unsigned int i = start; i < end && i < color_vec.size(); ++i)
            color_vec[i] = cl;
        text_update = true;
    }
    
    void SGTextBase::EvaluateSize(const std::wstring& t) {
        if(t.length() == 0) {
            text_width = 0;
            text_height = font->GetFontSize();
            text_width_cur = 0;
            text_pos_array.clear();
            if(text.length() == 0)
                return;
            EvaluateSize(text);
        } else {
            int max_width = GetMaxWidth();
            int ls = GetLineSpacing();
            int font_size = font->GetFontSize();
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
                auto& gl = font->GetGlyph(ch);
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
        if(font == nullptr)
            return;
        if(!text_update)
            return;
        text_update = false;
        vert_size = 0;
        if(text.length() == 0)
            return;
        auto tex_size = font->GetTexture().GetSize();
        std::vector<glbase::v2ct> charvtx;
        unsigned int advx = 0, advy = font->GetFontSize();
        glbase::v2ct cur_char;
        unsigned int max_width = GetMaxWidth();
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
            auto& gl = font->GetGlyph(ch);
            if(advx + gl.advance > max_width) {
                advx = 0;
                advy += ls;
            }
            guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx, text_pos.y + gl.bounds.top + advy, cur_char.vertex);
            cur_char.color = color;
            cur_char.texcoord.x = (float)gl.textureRect.left / tex_size.x;
            cur_char.texcoord.y = (float)gl.textureRect.top / tex_size.y;
            charvtx.push_back(cur_char);
            guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx + gl.bounds.width, text_pos.y + gl.bounds.top + advy, cur_char.vertex);
            cur_char.color = color;
            cur_char.texcoord.x = (float)(gl.textureRect.left + gl.textureRect.width) / tex_size.x;
            cur_char.texcoord.y = (float)gl.textureRect.top / tex_size.y;
            charvtx.push_back(cur_char);
            guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx, text_pos.y + gl.bounds.top + gl.bounds.height + advy, cur_char.vertex);
            cur_char.color = color;
            cur_char.texcoord.x = (float)gl.textureRect.left / tex_size.x;
            cur_char.texcoord.y = (float)(gl.textureRect.top + gl.textureRect.height) / tex_size.y;
            charvtx.push_back(cur_char);
            guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx + gl.bounds.width, text_pos.y + gl.bounds.top + gl.bounds.height + advy, cur_char.vertex);
            cur_char.color = color;
            cur_char.texcoord.x = (float)(gl.textureRect.left + gl.textureRect.width) / tex_size.x;
            cur_char.texcoord.y = (float)(gl.textureRect.top + gl.textureRect.height) / tex_size.y;
            charvtx.push_back(cur_char);
            advx += gl.advance + spacing_x;
            vert_size++;
        }
        if(vert_size > mem_size) {
            if(mem_size == 0)
                mem_size = 16;
            while(mem_size < vert_size)
                mem_size *= 2;
            glBindBuffer(GL_ARRAY_BUFFER, tbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * mem_size * 4, nullptr, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v2ct) * charvtx.size(), &charvtx[0]);
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, tbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v2ct) * charvtx.size(), &charvtx[0]);
        }
    }
    
    void SGTextBase::DrawText() {
        UpdateTextVertex();
        if(vert_size == 0)
            return;
        guiRoot.BindTexture(&font->GetTexture());
        glBindVertexArray(tao);
        glDrawElements(GL_TRIANGLE_STRIP, vert_size * 6 - 2, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
    }
    
    SGSpriteBase::SGSpriteBase() {
        glGenBuffers(1, &imgbo);
        glGenVertexArrays(1, &imgao);
        glBindVertexArray(imgao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, imgbo);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), 0);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::color_offset);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::tex_offset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, guiRoot.GetIndexBuffer());
        glBindVertexArray(0);
    }
    
    SGSpriteBase::~SGSpriteBase() {
        glDeleteBuffers(1, &imgbo);
        glDeleteVertexArrays(1, &imgao);
    }
    
    void SGSpriteBase::SetImage(glbase::Texture* img, recti varea, unsigned int cl) {
        img_texture = img;
        img_update = true;
        if(!img)
            return;
        verts.clear();
        colors.clear();
        texcoords.clear();
        verts.push_back(v2i{varea.left, varea.top});
        verts.push_back(v2i{varea.left + varea.width, varea.top});
        verts.push_back(v2i{varea.left, varea.top + varea.height});
        verts.push_back(v2i{varea.left + varea.width, varea.top + varea.height});
        for(int i = 0; i < 4; ++i)
            colors.push_back(cl);
    }
    
    void SGSpriteBase::AddTexRect(recti tarea) {
        if(!img_texture)
            return;
        img_update = true;
        img_dirty = true;
        float w = img_texture->GetWidth();
        float h = img_texture->GetHeight();
        std::vector<v2f> tex;
        tex.push_back(v2f{tarea.left / w, tarea.top / h});
        tex.push_back(v2f{(tarea.left + tarea.width) / w, tarea.top / h});
        tex.push_back(v2f{tarea.left / w, (tarea.top + tarea.height) / h});
        tex.push_back(v2f{(tarea.left + tarea.width) / w, (tarea.top + tarea.height) / h});
        texcoords.push_back(tex);
    }
    
    void SGSpriteBase::SetImage(glbase::Texture* img, std::vector<v2i>& vtx, std::vector<unsigned int>& cl) {
        img_texture = img;
        img_update = true;
        if(!img)
            return;
        verts.clear();
        colors.clear();
        texcoords.clear();
        verts = vtx;
        colors = cl;
    }
    
    void SGSpriteBase::AddTexcoord(std::vector<v2f>& tex) {
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
        std::vector<glbase::v2ct> vert;
        glbase::v2ct cur;
        v2i pos = GetImageOffset();
        for(size_t ti = 0; ti < texcoords.size(); ++ti) {
            for(size_t i = 0; i < verts.size(); ++i) {
                guiRoot.ConvertXY(verts[i].x + pos.x, verts[i].y + pos.y, cur.vertex);
                cur.texcoord = texcoords[ti][i];
                cur.color = colors[i];
                vert.push_back(cur);
            }
        }
        if(img_dirty) {
            glBindBuffer(GL_ARRAY_BUFFER, imgbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * vert.size() * texcoords.size(), &vert[0], GL_DYNAMIC_DRAW);
            img_dirty = false;
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, imgbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v2ct) * vert.size() * texcoords.size(), &vert[0]);
        }
    }
    
    void SGSpriteBase::DrawImage() {
        if(!img_texture)
            return;
        UpdateImage();
        if(verts.size() == 0)
            return;
        int frame = 0;
        if(texcoords.size() > 1)
            frame = (int)(guiRoot.GetTime() / frame_time) % texcoords.size() * verts.size();
        guiRoot.BindTexture(img_texture);
        glBindVertexArray(imgao);
        glDrawElements(GL_TRIANGLE_STRIP, verts.size() / 4 * 6 - 2, GL_UNSIGNED_SHORT, (void*)(verts.size() / 4 * 6 * frame));
        glBindVertexArray(0);
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
        SGWidget::PostResize(res, rep);
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
                if(hoving.lock() == chd)
                    guiRoot.CheckMouseMove();
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
    
    void SGWidgetContainer::SetFocusWidget(std::shared_ptr<SGWidget> chd) {
        for(auto iter = children.begin(); iter != children.end(); ++iter) {
            if(*iter == chd) {
                auto f = focus_widget.lock();
                if(f != chd) {
                    if(f != nullptr)
                        f->EventLostFocus();
                    if(chd != nullptr)
                        chd->EventGetFocus();
                    focus_widget = chd;
                }
            }
        }
    }
    
    bool SGWidgetContainer::EventMouseMove(MouseMoveEvent evt) {
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
    
    bool SGWidgetContainer::EventMouseButtonDown(MouseButtonEvent evt) {
        auto choving = hoving.lock();
        bool e = eventMouseButtonDown.TriggerEvent(*this, evt);
        if(choving) {
            auto fwidget = focus_widget.lock();
            if(choving != fwidget && choving->AllowFocus()) {
                if(fwidget)
                    fwidget->EventLostFocus();
                choving->EventGetFocus();
                focus_widget = hoving;
            }
            return choving->EventMouseButtonDown(evt) || e;
        } else {
            auto dr = GetDragingTarget();
            if(dr && (evt.button == GLFW_MOUSE_BUTTON_LEFT))
                guiRoot.ObjectDragingBegin(dr, MouseMoveEvent{evt.x, evt.y});
            return e;
        }
    }
    
    bool SGWidgetContainer::EventMouseButtonUp(MouseButtonEvent evt) {
        bool e = eventMouseButtonUp.TriggerEvent(*this, evt);
        if(!hoving.expired())
            return hoving.lock()->EventMouseButtonUp(evt) || e;
        return e;
    }
    
    bool SGWidgetContainer::EventMouseWheel(MouseWheelEvent evt) {
        bool e = eventMouseWheel.TriggerEvent(*this, evt);
        if(!hoving.expired())
            return hoving.lock()->EventMouseWheel(evt) || e;
        return e;
    }
    
    bool SGWidgetContainer::EventKeyDown(KeyEvent evt) {
        bool e = eventKeyDown.TriggerEvent(*this, evt);
        if(!focus_widget.expired())
            return focus_widget.lock()->EventKeyDown(evt) || e;
        return e;
    }
    
    bool SGWidgetContainer::EventKeyUp(KeyEvent evt) {
        bool e = eventKeyUp.TriggerEvent(*this, evt);
        if(!focus_widget.expired())
            return focus_widget.lock()->EventKeyUp(evt) || e;
        return e;
    }
    
    bool SGWidgetContainer::EventCharEnter(TextEvent evt) {
        bool e = eventCharEnter.TriggerEvent(*this, evt);
        if(!focus_widget.expired())
            return focus_widget.lock()->EventCharEnter(evt) || e;
        return e;
    }
    
    bool SGWidgetContainer::EventLostFocus() {
        bool e = eventLostFocus.TriggerEvent(*this);
        if(!focus_widget.expired())
            e = focus_widget.lock()->EventLostFocus() || e;
        focus_widget.reset();
        return e;
    }
    
    bool SGWidgetWrapper::EventMouseMove(MouseMoveEvent evt) {
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
    
    bool SGWidgetWrapper::EventMouseButtonDown(MouseButtonEvent evt) {
        SGWidgetContainer::EventMouseButtonDown(evt);
        return true;
    }
    
    bool SGWidgetWrapper::EventMouseButtonUp(MouseButtonEvent evt) {
        SGWidgetContainer::EventMouseButtonUp(evt);
        return true;
    }
    
    bool SGWidgetWrapper::EventMouseWheel(MouseWheelEvent evt) {
        SGWidgetContainer::EventMouseWheel(evt);
        return true;
    }
    
    bool SGWidgetWrapper::EventKeyDown(KeyEvent evt) {
        SGWidgetContainer::EventKeyDown(evt);
        return true;
    }
    
    bool SGWidgetWrapper::EventKeyUp(KeyEvent evt) {
        SGWidgetContainer::EventKeyUp(evt);
        return true;
    }
    
    bool SGWidgetWrapper::EventCharEnter(TextEvent evt) {
        SGWidgetContainer::EventCharEnter(evt);
        return true;
    }
    
    bool SGWidgetWrapper::EventLostFocus() {
        if(!focus_widget.expired())
            focus_widget.lock()->EventLostFocus();
        focus_widget.reset();
        eventLostFocus.TriggerEvent(*this);
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
    
    void SGGUIRoot::BeginScissor(recti rt) {
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
            recti rt = scissor_stack.back();
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
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        gui_shader->Use();
        gui_shader->SetParam1i("texID", 0);
        for(auto& iter : children)
            if(iter->IsVisible())
                iter->Draw();
        cur_texture = nullptr;
        scissor_stack.clear();
        gui_shader->Unuse();
    }
    
    void SGGUIRoot::ObjectDragingBegin(std::shared_ptr<SGWidget> dr, MouseMoveEvent evt) {
        if(!draging_object.expired())
            draging_object.lock()->DragingEnd(v2i{evt.x, evt.y});
        draging_object = dr;
        if(dr)
            dr->DragingBegin(v2i{evt.x, evt.y});
    }
    
    void SGGUIRoot::ObjectDragingEnd(MouseMoveEvent evt) {
        if(!draging_object.expired())
            draging_object.lock()->DragingEnd(v2i{evt.x, evt.y});
        draging_object.reset();
    }
    
    bool SGGUIRoot::LoadConfigs(const std::wstring& gui_conf) {
        AddConfig("basic", SGGUIRoot::basic_config);
        AddConfig("panel", SGPanel::panel_config);
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
        
        TextFile f(To<std::string>(gui_conf));
        rapidxml::xml_document<> doc;
        doc.parse<0>(f.Data());
        rapidxml::xml_node<>* root = doc.first_node();
        rapidxml::xml_node<>* subtype_node = root->first_node();
        while(subtype_node) {
            std::string subtype = subtype_node->name();
            auto iter = configs.find(subtype);
            if(iter != configs.end()) {
                rapidxml::xml_node<>* config_node = subtype_node->first_node();
                while(config_node) {
                    std::string config_name = config_node->name();
                    rapidxml::xml_attribute<>* attr = config_node->first_attribute();
                    if(config_name == "integer") {
                        std::string name = attr->value();
                        int val = To<int>(attr->next_attribute()->value());
                        iter->second->int_config[name] = val;
                    } else if(config_name == "string") {
                        std::string name = attr->value();
                        std::string val = attr->next_attribute()->value();
                        iter->second->string_config[name] = val;
                    } else if(config_name == "rect") {
                        std::string name = attr->value();
                        attr = attr->next_attribute();
                        int u = To<int>(attr->value());
                        attr = attr->next_attribute();
                        int v = To<int>(attr->value());
                        attr = attr->next_attribute();
                        int w = To<int>(attr->value());
                        attr = attr->next_attribute();
                        int h = To<int>(attr->value());
                        iter->second->tex_config[name] = recti{u, v, w, h};
                    } else if(config_name == "font") {
                        std::string name = attr->value();
                        attr = attr->next_attribute();
                        std::string file = attr->value();
                        attr = attr->next_attribute();
                        int sz = To<int>(attr->value());
                        auto& ft = font_mgr[name];
                        if(!ft.Load(file, sz))
                            font_mgr.erase(name);
                    }
                    config_node = config_node->next_sibling();
                }
            }
            subtype_node = subtype_node->next_sibling();
        }
        
        if(font_mgr.find("default") == font_mgr.end())
            return false;
        glbase::Image img;
        if(!img.LoadFile(basic_config.string_config["gui_texture"]))
            return false;
        gui_texture.Load(img.GetRawData(), img.GetWidth(), img.GetHeight());
        tex_size = v2i{gui_texture.GetWidth(), gui_texture.GetHeight()};
        glGenBuffers(1, &index_buffer);
        std::vector<unsigned short> index;
        index.resize(1024 * 4 * 6);
        for(int i = 0; i < 1024 * 4; ++i) {
            index[i * 6] = i * 4;
            index[i * 6 + 1] = i * 4 + 2;
            index[i * 6 + 2] = i * 4 + 1;
            index[i * 6 + 3] = i * 4 + 3;
            index[i * 6 + 4] = i * 4 + 3;
            index[i * 6 + 5] = i * 4 + 4;
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 1024 * 4 * 6, &index[0], GL_STATIC_DRAW);
        start_time = std::chrono::system_clock::now().time_since_epoch().count();
        gui_shader = &glbase::Shader::GetDefaultShader();
        return true;
    }
    
    void SGGUIRoot::Unload() {
        ClearChild();
        if(index_buffer)
            glDeleteBuffers(1, &index_buffer);
        for(auto& ft : font_mgr)
            ft.second.Unload();
        index_buffer = 0;
    }
    
    void SGGUIRoot::CheckMouseMove() {
        InjectMouseMoveEvent({mouse_pos.x, mouse_pos.y});
    }
    
    double SGGUIRoot::GetTime() {
        unsigned long long now = std::chrono::system_clock::now().time_since_epoch().count();
        return (double)(now - start_time) * std::chrono::system_clock::period::num / std::chrono::system_clock::period::den;
    }
    
    bool SGGUIRoot::InjectMouseEnterEvent() {
        inside_scene = true;
        return EventMouseEnter();
    }
    
    bool SGGUIRoot::InjectMouseLeaveEvent() {
        inside_scene = false;
        return EventMouseLeave();
    }
    
    bool SGGUIRoot::InjectMouseMoveEvent(MouseMoveEvent evt) {
        if(!inside_scene)
            InjectMouseEnterEvent();
        if(!draging_object.expired())
            draging_object.lock()->DragingUpdate(v2i{evt.x, evt.y});
        mouse_pos = {evt.x, evt.y};
        return EventMouseMove(evt);
    }
    
    bool SGGUIRoot::InjectMouseButtonDownEvent(MouseButtonEvent evt) {
        if(!inside_scene) {
            InjectMouseEnterEvent();
            InjectMouseMoveEvent(MouseMoveEvent{evt.x, evt.y});
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
            while(popup_objects.size() > 0) {
                auto ptr = popup_objects.back().lock();
                if(ptr == nullptr) {
                    popup_objects.pop_back();
                    continue;
                } else if(hwidget != ptr) {
                    ptr->Destroy();
                    popup_objects.pop_back();
                    continue;
                } else
                    break;
            }
        } else {
            for(auto& pop : popup_objects) {
                auto ptr = pop.lock();
                if(ptr != nullptr)
                    ptr->Destroy();
            }
            popup_objects.clear();
        }
        return EventMouseButtonDown(evt);
    }
    
    bool SGGUIRoot::InjectMouseButtonUpEvent(MouseButtonEvent evt) {
        if(!draging_object.expired() && (evt.button == GLFW_MOUSE_BUTTON_LEFT))
            draging_object.lock()->DragingEnd(v2i{evt.x, evt.y});
        draging_object.reset();
        clicking_object.reset();
        return EventMouseButtonUp(evt);
    }
    
    bool SGGUIRoot::InjectMouseWheelEvent(MouseWheelEvent evt) {
        return EventMouseWheel(evt);
    }
    
    bool SGGUIRoot::InjectKeyDownEvent(KeyEvent evt) {
        return EventKeyDown(evt);
    }
    
    bool SGGUIRoot::InjectKeyUpEvent(KeyEvent evt) {
        return EventKeyUp(evt);
    }
    
    bool SGGUIRoot::InjectCharEvent(TextEvent evt) {
        return EventCharEnter(evt);
    }
    
    SGGUIRoot& SGGUIRoot::GetSingleton() {
        return guiRoot;
    }
    
    SGConfig SGPanel::panel_config;
    
    SGPanel::~SGPanel() {

    }
    
    void SGPanel::UpdateVertices() {
        if(!vertices_dirty)
            return;
        vertices_dirty = false;
        std::array<glbase::v2ct, 36> vert;
        
        int bw = panel_config.int_config["client_border"];
        auto back = panel_config.tex_config["client_area"];
        
        guiRoot.SetRectVertex(&vert[0], position_abs.x, position_abs.y, bw, bw,
                              recti{back.left, back.top, bw, bw});
        guiRoot.SetRectVertex(&vert[4], position_abs.x + bw, position_abs.y, size_abs.x - bw * 2, bw,
                              recti{back.left + bw, back.top, back.width - bw * 2, bw});
        guiRoot.SetRectVertex(&vert[8], position_abs.x + size_abs.x - bw, position_abs.y, bw, bw,
                              recti{back.left + back.width - bw, back.top, bw, bw});
        guiRoot.SetRectVertex(&vert[12], position_abs.x, position_abs.y + bw, bw, size_abs.y - bw * 2,
                              recti{back.left, back.top + bw, bw, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[16], position_abs.x + bw, position_abs.y + bw, size_abs.x - bw * 2, size_abs.y - bw * 2,
                              recti{back.left + bw, back.top + bw, back.width - bw * 2, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[20], position_abs.x + size_abs.x - bw, position_abs.y + bw, bw, size_abs.y - bw * 2,
                              recti{back.left + back.width - bw, back.top + bw, bw, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[24], position_abs.x, position_abs.y + size_abs.y - bw, bw, bw,
                              recti{back.left, back.top + back.height - bw, bw, bw});
        guiRoot.SetRectVertex(&vert[28], position_abs.x + bw, position_abs.y + size_abs.y - bw, size_abs.x - bw * 2, bw,
                              recti{back.left + bw, back.top + back.height - bw, back.width - bw * 2, bw});
        guiRoot.SetRectVertex(&vert[32], position_abs.x + size_abs.x - bw, position_abs.y + size_abs.y - bw, bw, bw,
                              recti{back.left + back.width - bw, back.top + back.height - bw, bw, bw});
        for(auto& vt : vert)
            vt.color = color;
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v2ct) * 36, &vert[0]);
    }
    
    void SGPanel::Draw() {
        UpdateVertices();
        guiRoot.BindGuiTexture();
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLE_STRIP, 9 * 6 - 2, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
        guiRoot.BeginScissor(recti{position_abs.x, position_abs.y, size_abs.x, size_abs.y});
        for(auto& iter : children)
            if(iter->IsVisible())
                iter->Draw();
        guiRoot.EndScissor();
    }
    
    std::shared_ptr<SGPanel> SGPanel::Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, v2i size) {
        auto ptr = std::make_shared<SGPanel>();
        ptr->parent = p;
        ptr->position = pos;
        ptr->size = size;
        ptr->PostResize(true, true);
        auto iter1 = panel_config.int_config.find("gui_color");
        if(iter1 != panel_config.int_config.end())
            ptr->color = iter1->second;
        else
            ptr->color = guiRoot.GetDefaultInt("gui_color");
        glGenBuffers(1, &ptr->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * 36, nullptr, GL_DYNAMIC_DRAW);
        glGenVertexArrays(1, &ptr->vao);
        glBindVertexArray(ptr->vao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), 0);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::color_offset);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::tex_offset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, guiRoot.GetIndexBuffer());
        glBindVertexArray(0);
        if(p != nullptr)
            p->AddChild(ptr);
        else
            guiRoot.AddChild(ptr);
        return ptr;
    }
    
    SGConfig SGWindow::window_config;

    SGWindow::~SGWindow() {

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
        auto iter2 = window_config.string_config.find("font_name");
        if(iter2 != window_config.string_config.end())
            ptr->SetFont(&guiRoot.GetGUIFont(iter2->second));
        else
            ptr->SetFont(&guiRoot.GetGUIFont("default"));
        glGenBuffers(1, &ptr->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * 36, nullptr, GL_DYNAMIC_DRAW);
        glGenVertexArrays(1, &ptr->vao);
        glBindVertexArray(ptr->vao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), 0);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::color_offset);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::tex_offset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, guiRoot.GetIndexBuffer());
        glBindVertexArray(0);
        if(p != nullptr)
            p->AddChild(ptr);
        else
            guiRoot.AddChild(ptr);
        auto rt = window_config.tex_config["closep"];
        auto cbtn = SGButton::Create(ptr, {size.x - rt.left, rt.top}, {rt.width, rt.height});
        cbtn->SetPosition({-rt.left, rt.top}, {1.0f, 0.0f});
        cbtn->eventButtonClick.Bind(ptr.get(), &SGWindow::CloseButtonClick);
        cbtn->SetTextureRect(window_config.tex_config["close1"], window_config.tex_config["close2"], window_config.tex_config["close3"]);
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
        
        std::array<glbase::v2ct, 36> vert;
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
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v2ct) * 36, &vert);
    }
    
    void SGWindow::Draw() {
        UpdateVertices();
        guiRoot.BindGuiTexture();
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLE_STRIP, 9 * 6 - 2, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
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
        if(onClosing.TriggerEvent(sender) == false)
            Destroy();
        return true;
    }
    
    SGConfig SGLabel::label_config;
    
    SGLabel::~SGLabel() {
        
    }
    
    void SGLabel::PostResize(bool res, bool rep) {
        if(res)
            size_abs = v2i{std::max(text_width_cur, text_width), text_height};
        if(position_prop.x < 0.0f || position_prop.y < 0.0f)
            rep = true;
        SGWidget::PostResize(false, rep);
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
    
    void SGLabel::SetFont(glbase::Font* ft) {
        SGTextBase::SetFont(ft);
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
        auto iter2 = label_config.string_config.find("font_name");
        if(iter2 != label_config.string_config.end())
            ptr->SetFont(&guiRoot.GetGUIFont(iter2->second));
        else
            ptr->SetFont(&guiRoot.GetGUIFont("default"));
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

    }
    
    void SGIconLabel::EvaluateSize(const std::wstring& t) {
        if(t.length() == 0) {
            text_width = 0;
            text_height = font->GetFontSize();
            text_width_cur = 0;
            text_pos_array.clear();
            if(text.length() == 0)
                return;
            EvaluateSize(text);
        } else {
            int max_width = GetMaxWidth();
            auto iconoffset = iconlabel_config.tex_config["iconoffset"];
            int ls = GetLineSpacing();
            int ils = (ls > (int)(iconoffset.height + spacing_y)) ? ls : (iconoffset.height + spacing_y);
            bool has_icon = false;
            int font_size = font->GetFontSize();
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
                    auto& gl = font->GetGlyph(ch);
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
        if(font == nullptr)
            return;
        if(!text_update)
            return;
        text_update = false;
        vert_size = 0;
        icon_size = 0;
        if(text.length() == 0)
            return;
        auto tex_size = font->GetTexture().GetSize();
        std::vector<glbase::v2ct> charvtx;
        std::vector<glbase::v2ct> iconvtx;
        int font_size = font->GetFontSize();
        unsigned int advx = 0, advy = font_size;
        glbase::v2ct cur_char;
        unsigned int max_width = GetMaxWidth();
        v2i text_pos = GetTextOffset();
        auto iconoffset = guiRoot.GetDefaultRect("iconoffset");
        auto iconrc = guiRoot.GetDefaultInt("iconcolumn");
        int ls = GetLineSpacing();
        int ils = (ls > (int)(iconoffset.height + spacing_y)) ? ls : (iconoffset.height + spacing_y);
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
                recti rct = iconoffset;
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
                icon_size++;
            } else {
                auto& gl = font->GetGlyph(ch);
                if(advx + gl.advance > max_width) {
                    advx = 0;
                    advy += has_icon ? ils : ls;
                    has_icon = false;
                }
                guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx, text_pos.y + gl.bounds.top + advy, cur_char.vertex);
                cur_char.color = color;
                cur_char.texcoord.x = (float)gl.textureRect.left / tex_size.x;
                cur_char.texcoord.y = (float)gl.textureRect.top / tex_size.y;
                charvtx.push_back(cur_char);
                guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx + gl.bounds.width, text_pos.y + gl.bounds.top + advy, cur_char.vertex);
                cur_char.color = color;
                cur_char.texcoord.x = (float)(gl.textureRect.left + gl.textureRect.width) / tex_size.x;
                cur_char.texcoord.y = (float)gl.textureRect.top / tex_size.y;
                charvtx.push_back(cur_char);
                guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx, text_pos.y + gl.bounds.top + gl.bounds.height + advy, cur_char.vertex);
                cur_char.color = color;
                cur_char.texcoord.x = (float)gl.textureRect.left / tex_size.x;
                cur_char.texcoord.y = (float)(gl.textureRect.top + gl.textureRect.height) / tex_size.y;
                charvtx.push_back(cur_char);
                guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx + gl.bounds.width, text_pos.y + gl.bounds.top + gl.bounds.height + advy, cur_char.vertex);
                cur_char.color = color;
                cur_char.texcoord.x = (float)(gl.textureRect.left + gl.textureRect.width) / tex_size.x;
                cur_char.texcoord.y = (float)(gl.textureRect.top + gl.textureRect.height) / tex_size.y;
                charvtx.push_back(cur_char);
                advx += gl.advance + spacing_x;
                vert_size++;
            }
        }
        if(icon_size > icon_mem_size) {
            if(icon_mem_size == 0)
                icon_mem_size = 8;
            while(icon_mem_size < icon_size)
                icon_mem_size *= 2;
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * icon_mem_size * 4, nullptr, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v2ct) * iconvtx.size(), &iconvtx[0]);
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v2ct) * iconvtx.size(), &iconvtx[0]);
        }
        if(vert_size > mem_size) {
            if(mem_size == 0)
                mem_size = 16;
            while(mem_size < vert_size)
                mem_size *= 2;
            glBindBuffer(GL_ARRAY_BUFFER, tbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * mem_size * 4, nullptr, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v2ct) * charvtx.size(), &charvtx[0]);
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, tbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v2ct) * charvtx.size(), &charvtx[0]);
        }
    }
    
    void SGIconLabel::Draw() {
        UpdateVertices();
        UpdateTextVertex();
        if(icon_size) {
            guiRoot.BindGuiTexture();
            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLE_STRIP, icon_size * 6 - 2, GL_UNSIGNED_SHORT, 0);
            glBindVertexArray(0);
        }
        if(vert_size) {
            guiRoot.BindTexture(&font->GetTexture());
            glBindVertexArray(tao);
            glDrawElements(GL_TRIANGLE_STRIP, vert_size * 6 - 2, GL_UNSIGNED_SHORT, 0);
            glBindVertexArray(0);
        }
    }
    
    std::shared_ptr<SGIconLabel> SGIconLabel::Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, const std::wstring& t, int mw) {
        auto ptr = std::make_shared<SGIconLabel>();
        ptr->parent = p;
        ptr->position = pos;
        ptr->max_width = mw;
        auto iter2 = iconlabel_config.string_config.find("font_name");
        if(iter2 != iconlabel_config.string_config.end())
            ptr->SetFont(&guiRoot.GetGUIFont(iter2->second));
        else
            ptr->SetFont(&guiRoot.GetGUIFont("default"));
        ptr->SetText(t, guiRoot.GetDefaultInt("font_color"));
        ptr->PostResize(false, true);
        glGenBuffers(1, &ptr->vbo);
        glGenVertexArrays(1, &ptr->vao);
        glBindVertexArray(ptr->vao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), 0);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::color_offset);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::tex_offset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, guiRoot.GetIndexBuffer());
        glBindVertexArray(0);
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
    }
    
    void SGButton::UpdateVertices() {
        if(!vertices_dirty)
            return;
        vertices_dirty = false;
        img_update = true;
        text_update = true;
        std::array<glbase::v2ct, 36> vert;
        int bw = button_config.int_config["border"];
        auto back = tex_rect[state & 0xf];
        guiRoot.SetRectVertex(&vert[0], position_abs.x, position_abs.y, bw, bw,
                              recti{back.left, back.top, bw, bw});
        guiRoot.SetRectVertex(&vert[4], position_abs.x + bw, position_abs.y, size_abs.x - bw * 2, bw,
                              recti{back.left + bw, back.top, back.width - bw * 2, bw});
        guiRoot.SetRectVertex(&vert[8], position_abs.x + size_abs.x - bw, position_abs.y, bw, bw,
                              recti{back.left + back.width - bw, back.top, bw, bw});
        guiRoot.SetRectVertex(&vert[12], position_abs.x, position_abs.y + bw, bw, size_abs.y - bw * 2,
                              recti{back.left, back.top + bw, bw, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[16], position_abs.x + bw, position_abs.y + bw, size_abs.x - bw * 2, size_abs.y - bw * 2,
                              recti{back.left + bw, back.top + bw, back.width - bw * 2, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[20], position_abs.x + size_abs.x - bw, position_abs.y + bw, bw, size_abs.y - bw * 2,
                              recti{back.left + back.width - bw, back.top + bw, bw, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[24], position_abs.x, position_abs.y + size_abs.y - bw, bw, bw,
                              recti{back.left, back.top + back.height - bw, bw, bw});
        guiRoot.SetRectVertex(&vert[28], position_abs.x + bw, position_abs.y + size_abs.y - bw, size_abs.x - bw * 2, bw,
                              recti{back.left + bw, back.top + back.height - bw, back.width - bw * 2, bw});
        guiRoot.SetRectVertex(&vert[32], position_abs.x + size_abs.x - bw, position_abs.y + size_abs.y - bw, bw, bw,
                              recti{back.left + back.width - bw, back.top + back.height - bw, bw, bw});
        for(int i = 0; i < 36; ++i)
            vert[i].color = color;
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v2ct) * 36, &vert);
    }
    
    void SGButton::Draw() {
        UpdateVertices();
        if(tex_texture)
            guiRoot.BindTexture(tex_texture);
        else
            guiRoot.BindGuiTexture();
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLE_STRIP, 9 * 6 - 2, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
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
    
    void SGButton::SetTextureRect(recti r1, recti r2, recti r3) {
        tex_rect[0] = r1;
        tex_rect[1] = r2;
        tex_rect[2] = r3;
        UpdateVertices();
    }
    
    void SGButton::SetTexture(glbase::Texture* tex) {
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
        auto iter2 = button_config.string_config.find("font_name");
        if(iter2 != button_config.string_config.end())
            ptr->SetFont(&guiRoot.GetGUIFont(iter2->second));
        else
            ptr->SetFont(&guiRoot.GetGUIFont("default"));
        ptr->is_push = is_push;
        glGenBuffers(1, &ptr->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * 36, nullptr, GL_DYNAMIC_DRAW);
        glGenVertexArrays(1, &ptr->vao);
        glBindVertexArray(ptr->vao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), 0);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::color_offset);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::tex_offset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, guiRoot.GetIndexBuffer());
        glBindVertexArray(0);
        if(p != nullptr)
            p->AddChild(ptr);
        else
            guiRoot.AddChild(ptr);
        ptr->tex_rect[0] = button_config.tex_config["normal"];
        ptr->tex_rect[1] = button_config.tex_config["hover"];
        ptr->tex_rect[2] = button_config.tex_config["down"];
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
    
    bool SGButton::EventMouseButtonDown(MouseButtonEvent evt) {
        if(evt.button == GLFW_MOUSE_BUTTON_LEFT) {
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
    
    bool SGButton::EventMouseButtonUp(MouseButtonEvent evt) {
        bool click = false;
        if(evt.button == GLFW_MOUSE_BUTTON_LEFT) {
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

    }
    
    void SGCheckbox::PostResize(bool res, bool rep) {
        if(res) {
            auto rec = checkbox_config.tex_config["offset"];
            size_abs = v2i{rec.left + rec.width + rec.top + text_width_cur, std::max(text_height , rec.height)};
        }
        if(position_prop.x < 0.0f || position_prop.y < 0.0f)
            rep = true;
        SGWidget::PostResize(false, rep);
    }
    
    void SGCheckbox::UpdateVertices() {
        if(!vertices_dirty)
            return;
        vertices_dirty = false;
        text_update = true;
        auto rec = checkbox_config.tex_config["offset"];
        std::array<glbase::v2ct, 4> vert;
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
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v2ct) * 4, &vert);
    }
    
    void SGCheckbox::Draw() {
        UpdateVertices();
        guiRoot.BindGuiTexture();
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLE_STRIP, 6 - 2, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
        DrawText();
    }
    
    void SGCheckbox::SetFont(glbase::Font* ft) {
        SGTextBase::SetFont(ft);
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
        auto iter2 = checkbox_config.string_config.find("font_name");
        if(iter2 != checkbox_config.string_config.end())
            ptr->SetFont(&guiRoot.GetGUIFont(iter2->second));
        else
            ptr->SetFont(&guiRoot.GetGUIFont("default"));
        ptr->SetText(t, guiRoot.GetDefaultInt("font_color"));
        glGenBuffers(1, &ptr->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * 12, nullptr, GL_DYNAMIC_DRAW);
        glGenVertexArrays(1, &ptr->vao);
        glBindVertexArray(ptr->vao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), 0);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::color_offset);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::tex_offset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, guiRoot.GetIndexBuffer());
        glBindVertexArray(0);
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
    
    bool SGCheckbox::EventMouseButtonDown(MouseButtonEvent evt) {
        if(evt.button == GLFW_MOUSE_BUTTON_LEFT) {
            state = 2;
            vertices_dirty = true;
            guiRoot.SetClickingObject(shared_from_this());
        }
        return SGWidget::EventMouseButtonDown(evt);
    }
    
    bool SGCheckbox::EventMouseButtonUp(MouseButtonEvent evt) {
        bool click = false;
        if(state == 2 && evt.button == GLFW_MOUSE_BUTTON_LEFT) {
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
        std::array<glbase::v2ct, 4> vert;
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
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v2ct) * 4, &vert);
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
    
    bool SGRadio::EventMouseButtonUp(MouseButtonEvent evt) {
        bool click = false;
        if(state == 2 && evt.button == GLFW_MOUSE_BUTTON_LEFT) {
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
        auto iter2 = checkbox_config.string_config.find("font_name");
        if(iter2 != checkbox_config.string_config.end())
            ptr->SetFont(&guiRoot.GetGUIFont(iter2->second));
        else
            ptr->SetFont(&guiRoot.GetGUIFont("default"));
        ptr->SetText(t, guiRoot.GetDefaultInt("font_color"));
        ptr->next_group_member = ptr.get();
        glGenBuffers(1, &ptr->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * 12, nullptr, GL_DYNAMIC_DRAW);
        glGenVertexArrays(1, &ptr->vao);
        glBindVertexArray(ptr->vao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), 0);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::color_offset);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::tex_offset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, guiRoot.GetIndexBuffer());
        glBindVertexArray(0);
        if(p != nullptr)
            p->AddChild(ptr);
        else
            guiRoot.AddChild(ptr);
        return ptr;
    }
    
    SGConfig SGScrollBar::scroll_config;
    
    SGScrollBar::~SGScrollBar() {

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
        std::array<glbase::v2ct, 16> vert;
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
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v2ct) * 16, &vert);
    }
    
    void SGScrollBar::Draw() {
        UpdateVertices();
        guiRoot.BindGuiTexture();
        glBindVertexArray(vao);
        if(slider_length)
            glDrawElements(GL_TRIANGLE_STRIP, 4 * 6 - 2, GL_UNSIGNED_SHORT, 0);
        else
            glDrawElements(GL_TRIANGLE_STRIP, 6 - 2, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
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
    
    bool SGScrollBar::EventMouseMove(MouseMoveEvent evt) {
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
    
    bool SGScrollBar::EventMouseWheel(MouseWheelEvent evt) {
        if(slider_length == 0)
            return false;
        int prepos = current_pos;
        current_pos += (pos_max - pos_min) * evt.deltay / 50;
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
        glGenBuffers(1, &ptr->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * 16, nullptr, GL_DYNAMIC_DRAW);
        glGenVertexArrays(1, &ptr->vao);
        glBindVertexArray(ptr->vao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), 0);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::color_offset);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::tex_offset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, guiRoot.GetIndexBuffer());
        glBindVertexArray(0);
        if(p != nullptr)
            p->AddChild(ptr);
        else
            guiRoot.AddChild(ptr);
        return ptr;
    }
    
    SGConfig SGTextEdit::textedit_config;
    
    SGTextEdit::~SGTextEdit() {

    }
    
    void SGTextEdit::PostResize(bool res, bool rep) {
        SGWidgetContainer::PostResize(res, rep);
        if(res)
            size_abs.y = textedit_config.int_config["height"];
    }
    
    void SGTextEdit::UpdateVertices() {
        if(!vertices_dirty)
            return;
        vertices_dirty = false;
        text_update = true;
        sel_change = true;
        std::array<glbase::v2ct, 40> vert;
        int bw = textedit_config.int_config["border"];
        auto back = textedit_config.tex_config[hoving ? "backh" : "back"];
        auto crect = textedit_config.tex_config["cursor"];
        auto curx = cursor_pos >= text.length() ? text_width_cur : text_pos_array[cursor_pos].x;
        int ht = textedit_config.int_config["sel_height"];
        guiRoot.SetRectVertex(&vert[0], position_abs.x, position_abs.y, bw, bw,
                              recti{back.left, back.top, bw, bw});
        guiRoot.SetRectVertex(&vert[4], position_abs.x + bw, position_abs.y, size_abs.x - bw * 2, bw,
                              recti{back.left + bw, back.top, back.width - bw * 2, bw});
        guiRoot.SetRectVertex(&vert[8], position_abs.x + size_abs.x - bw, position_abs.y, bw, bw,
                              recti{back.left + back.width - bw, back.top, bw, bw});
        guiRoot.SetRectVertex(&vert[12], position_abs.x, position_abs.y + bw, bw, size_abs.y - bw * 2,
                              recti{back.left, back.top + bw, bw, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[16], position_abs.x + bw, position_abs.y + bw, size_abs.x - bw * 2, size_abs.y - bw * 2,
                              recti{back.left + bw, back.top + bw, back.width - bw * 2, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[20], position_abs.x + size_abs.x - bw, position_abs.y + bw, bw, size_abs.y - bw * 2,
                              recti{back.left + back.width - bw, back.top + bw, bw, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[24], position_abs.x, position_abs.y + size_abs.y - bw, bw, bw,
                              recti{back.left, back.top + back.height - bw, bw, bw});
        guiRoot.SetRectVertex(&vert[28], position_abs.x + bw, position_abs.y + size_abs.y - bw, size_abs.x - bw * 2, bw,
                              recti{back.left + bw, back.top + back.height - bw, back.width - bw * 2, bw});
        guiRoot.SetRectVertex(&vert[32], position_abs.x + size_abs.x - bw, position_abs.y + size_abs.y - bw, bw, bw,
                              recti{back.left + back.width - bw, back.top + back.height - bw, bw, bw});
        guiRoot.SetRectVertex(&vert[36], position_abs.x + curx + text_area.left - text_offset, position_abs.y + text_area.top, crect.width, ht, crect);
        for(int i = 0; i < 40; ++i)
            vert[i].color = color;
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v2ct) * 40, &vert);
    }
    
    void SGTextEdit::Draw() {
        if(draging) {
            double tm = guiRoot.GetTime();
            int chk = (int)((tm - cursor_time) / 0.3);
            if(chk != drag_check) {
                drag_check = chk;
                CheckDragPos();
            }
        }
        UpdateVertices();
        guiRoot.BindGuiTexture();
        glBindVertexArray(vao);
        double tm = guiRoot.GetTime();
        if(read_only || !focus || sel_start != sel_end || ((int)((tm - cursor_time) / 0.5) % 2))
            glDrawElements(GL_TRIANGLE_STRIP, 9 * 6 - 2, GL_UNSIGNED_SHORT, 0);
        else
            glDrawElements(GL_TRIANGLE_STRIP, 10 * 6 - 2, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
        int tw = size_abs.x - text_area.left - text_area.width;
        int th = size_abs.y - text_area.top - text_area.height;
        for(auto chd : children)
            chd->Draw();
        guiRoot.BeginScissor(recti{position_abs.x + text_area.left, position_abs.y + text_area.top, tw, th});
        if(!read_only)
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
        if(index >= (int)sel_start && index < (int)sel_end)
            return sel_color;
        return color_vec[index];
    }
    
    void SGTextEdit::DrawSelectRegion() {
        UpdateSelRegion();
        if(sel_start != sel_end) {
            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLE_STRIP, 6 - 2, GL_UNSIGNED_SHORT, (GLvoid*)(uintptr_t)(sizeof(unsigned short) * 60));
            glBindVertexArray(0);
        }
    }
    
    void SGTextEdit::UpdateSelRegion() {
        if(!sel_change)
            return;
        sel_change = false;
        std::array<glbase::v2ct, 4> vert;
        int ht = textedit_config.int_config["sel_height"];
        if(sel_start != sel_end) {
            int ps = text_pos_array[sel_start].x;
            int pe = sel_end >= text.length() ? text_width_cur : text_pos_array[sel_end].x;
            v2i ta = position_abs + v2i{text_area.left - text_offset, text_area.top};
            guiRoot.SetRectVertex(&vert[0], ta.x + ps, ta.y, pe - ps, ht, textedit_config.tex_config["sel_rect"]);
        }
        for(int i = 0; i < 4; ++i)
            vert[i].color = sel_bcolor;
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * 40, sizeof(glbase::v2ct) * 4, &vert);
    }
    
    void SGTextEdit::SetSelRegion(unsigned int start, unsigned int end) {
        if(read_only)
            return;
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
            int my = (cur >= (int)text.length()) ? text_width_cur : text_pos_array[cur].x;
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
    
    void SGTextEdit::SetReadOnly(bool ro) {
        if(read_only != ro)
            read_only = ro;
        if(sel_start != sel_end) {
            sel_start = sel_end = 0;
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
        auto iter2 = textedit_config.string_config.find("font_name");
        if(iter2 != textedit_config.string_config.end())
            ptr->SetFont(&guiRoot.GetGUIFont(iter2->second));
        else
            ptr->SetFont(&guiRoot.GetGUIFont("default"));
        auto iter3 = textedit_config.int_config.find("font_color");
        if(iter3 != textedit_config.int_config.end())
            ptr->def_color = iter3->second;
        else
            ptr->def_color = guiRoot.GetDefaultInt("font_color");
        ptr->sel_color = textedit_config.int_config["sel_color"];
        ptr->sel_bcolor = textedit_config.int_config["sel_bcolor"];
        ptr->cursor_time = guiRoot.GetTime();
        glGenBuffers(1, &ptr->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * 44, nullptr, GL_DYNAMIC_DRAW);
        glGenVertexArrays(1, &ptr->vao);
        glBindVertexArray(ptr->vao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), 0);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::color_offset);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::tex_offset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, guiRoot.GetIndexBuffer());
        glBindVertexArray(0);
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
        if(read_only)
            return false;
        if(text.size() == 0)
            return false;
        position_drag = evt;
        cursor_pos = GetHitIndex(evt);
        SetSelRegion(cursor_pos, cursor_pos);
        cursor_time = guiRoot.GetTime();
        draging = true;
        drag_check = 0;
        return eventDragBegin.TriggerEvent(*this, evt);
    }
    
    bool SGTextEdit::DragingUpdate(v2i evt) {
        if(!draging || read_only)
            return false;
        v2i delta = evt - position_drag;
        position_drag = evt;
        CheckDragPos();
        return eventDragUpdate.TriggerEvent(*this, delta);
    }
    
    bool SGTextEdit::DragingEnd(v2i evt) {
        if(read_only)
            return false;
        vertices_dirty = true;
        draging = false;
        return eventDragEnd.TriggerEvent(*this, evt);
    }
    
    bool SGTextEdit::EventKeyDown(KeyEvent evt) {
        if(read_only)
            return false;
        switch(evt.key) {
            case GLFW_KEY_X:
                if(evt.mods & GLFW_MOD_CONTROL) {
                    if(sel_start == sel_end)
                        break;
                    std::wstring str = text.substr(sel_start, sel_end - sel_start);
                    glfwSetClipboardString(nullptr, To<std::string>(str).c_str());
                    text.erase(text.begin() + sel_start, text.begin() + sel_end);
                    color_vec.erase(color_vec.begin() + sel_start, color_vec.begin() + sel_end);
                    cursor_pos = sel_start;
                    EvaluateSize();
                    sel_end = sel_start;
                    vertices_dirty = true;
                    sel_change = true;
                }
                break;
            case GLFW_KEY_C:
                if(evt.mods & GLFW_MOD_CONTROL) {
                    if(sel_start == sel_end)
                        break;
                    std::wstring str = text.substr(sel_start, sel_end - sel_start);
                    glfwSetClipboardString(nullptr, To<std::string>(str).c_str());
                }
                break;
            case GLFW_KEY_V:
                if(evt.mods & GLFW_MOD_CONTROL) {
                    if(sel_start != sel_end) {
                        cursor_pos = sel_start;
                        text.erase(text.begin() + sel_start, text.begin() + sel_end);
                        color_vec.erase(color_vec.begin() + sel_start, color_vec.begin() + sel_end);
                    }
                    std::string utf8str = glfwGetClipboardString(nullptr);
                    std::wstring str = To<std::wstring>(utf8str);
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
            case GLFW_KEY_BACKSPACE:
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
            case GLFW_KEY_DELETE:
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
            case GLFW_KEY_LEFT:
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
                    cursor_time = guiRoot.GetTime();
                }
                break;
            case GLFW_KEY_RIGHT:
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
                    cursor_time = guiRoot.GetTime();
                }
                break;
            case GLFW_KEY_ENTER:
                eventTextEnter.TriggerEvent(*this);
                break;
            default:
                break;
        }
        return eventKeyDown.TriggerEvent(*this, evt);
    }
    
    bool SGTextEdit::EventCharEnter(TextEvent evt) {
        if(read_only)
            return false;
        if(evt.unichar < 32 || evt.unichar == 127 || evt.unichar >= 0xe000) // control charactors
            return false;
        if(sel_start != sel_end) {
            text.erase(text.begin() + sel_start, text.begin() + sel_end);
            color_vec.erase(color_vec.begin() + sel_start, color_vec.begin() + sel_end);
            cursor_pos = sel_start;
        }
        text.insert(text.begin() + cursor_pos, evt.unichar);
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

    }
    
    void SGListBox::UpdateVertices() {
        if(!vertices_dirty)
            return;
        vertices_dirty = false;
        text_update = true;
        std::vector<glbase::v2ct> vert;
        int m_item_count = (size_abs.y - text_area.top - text_area.height - 1) / line_spacing + 2;
        vert.resize(36 + m_item_count * 8);
        
        int bw = listbox_config.int_config["border"];
        auto back = listbox_config.tex_config[is_hoving ? "backh" : "back"];
        auto selrect = listbox_config.tex_config["sel_rect"];
        auto iconoffset = guiRoot.GetDefaultRect("iconoffset");
        auto iconrc = guiRoot.GetDefaultInt("iconcolumn");
        guiRoot.SetRectVertex(&vert[0], position_abs.x, position_abs.y, bw, bw,
                              recti{back.left, back.top, bw, bw});
        guiRoot.SetRectVertex(&vert[4], position_abs.x + bw, position_abs.y, size_abs.x - bw * 2, bw,
                              recti{back.left + bw, back.top, back.width - bw * 2, bw});
        guiRoot.SetRectVertex(&vert[8], position_abs.x + size_abs.x - bw, position_abs.y, bw, bw,
                              recti{back.left + back.width - bw, back.top, bw, bw});
        guiRoot.SetRectVertex(&vert[12], position_abs.x, position_abs.y + bw, bw, size_abs.y - bw * 2,
                              recti{back.left, back.top + bw, bw, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[16], position_abs.x + bw, position_abs.y + bw, size_abs.x - bw * 2, size_abs.y - bw * 2,
                              recti{back.left + bw, back.top + bw, back.width - bw * 2, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[20], position_abs.x + size_abs.x - bw, position_abs.y + bw, bw, size_abs.y - bw * 2,
                              recti{back.left + back.width - bw, back.top + bw, bw, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[24], position_abs.x, position_abs.y + size_abs.y - bw, bw, bw,
                              recti{back.left, back.top + back.height - bw, bw, bw});
        guiRoot.SetRectVertex(&vert[28], position_abs.x + bw, position_abs.y + size_abs.y - bw, size_abs.x - bw * 2, bw,
                              recti{back.left + bw, back.top + back.height - bw, back.width - bw * 2, bw});
        guiRoot.SetRectVertex(&vert[32], position_abs.x + size_abs.x - bw, position_abs.y + size_abs.y - bw, bw, bw,
                              recti{back.left + back.width - bw, back.top + back.height - bw, bw, bw});
        for(int i = 0; i < 36; ++i)
            vert[i].color = color;
        int begini = text_offset / line_spacing;
        item_count = 0;
        unsigned int sel_color = listbox_config.int_config["sel_bcolor"];
        unsigned int color1 = listbox_config.int_config["color1"];
        unsigned int color2 = listbox_config.int_config["color2"];
        for(size_t i = begini; i < items.size() && i < (size_t)(begini + m_item_count); ++i) {
            int li = i - begini;
            guiRoot.SetRectVertex(&vert[36 + li * 8], position_abs.x + bw, position_abs.y + bw + i * line_spacing - text_offset,
                                  size_abs.x - bw * 2, line_spacing, selrect);
            unsigned int bcolor = (i == (size_t)current_sel) ? sel_color : (i % 2) ? color1 : color2;
            for(int j = 0; j < 4; ++j)
                vert[36 + li * 8 + j].color = bcolor;
            int iconi = std::get<0>(items[i]);
            if(iconi) {
                int x = (iconi - 1) % iconrc;
                int y = (iconi - 1) / iconrc;
                recti rct = iconoffset;
                rct.left += x * iconoffset.width;
                rct.top += y * iconoffset.height;
                guiRoot.SetRectVertex(&vert[36 + li * 8 + 4], position_abs.x + bw, position_abs.y + bw + i * line_spacing - text_offset,
                                      iconoffset.width, iconoffset.height, rct);
            }
            item_count++;
        }
        
        if(m_item_count > max_item_count) {
            if(max_item_count == 0)
                max_item_count = 16;
            while(max_item_count < m_item_count)
                max_item_count *= 2;
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * (36 + max_item_count * 8), nullptr, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v2ct) * (36 + item_count * 8), &vert[0]);
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v2ct) * (36 + item_count * 8), &vert[0]);
        }
    }
    
    void SGListBox::Draw() {
        UpdateVertices();
        guiRoot.BindGuiTexture();
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLE_STRIP, 9 * 6 - 2, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
        int tw = size_abs.x - text_area.left - text_area.width;
        int th = size_abs.y - text_area.top - text_area.height;
        for(auto chd : children)
            chd->Draw();
        guiRoot.BeginScissor(recti{position_abs.x + text_area.left, position_abs.y + text_area.top, tw, th});
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLE_STRIP, item_count * 2 * 6 - 2, GL_UNSIGNED_SHORT, (GLvoid*)(uintptr_t)(sizeof(unsigned short) * 54));
        glBindVertexArray(0);
        DrawText();
        guiRoot.EndScissor();
    }
    
    v2i SGListBox::GetTextOffset() {
        return position_abs + v2i{text_area.left, text_area.top - text_offset};
    }
 
    void SGListBox::UpdateTextVertex() {
        if(font == nullptr)
            return;
        if(!text_update)
            return;
        text_update = false;
        vert_size = 0;
        if(items.size() == 0)
            return;
        auto tex_size = font->GetTexture().GetSize();
        int font_size = font->GetFontSize();
        std::vector<glbase::v2ct> charvtx;
        std::vector<glbase::v2ct> iconvtx;
        glbase::v2ct cur_char;
        v2i text_pos = GetTextOffset();
        int itemcount = (size_abs.y - text_area.top - text_area.height - 1) / line_spacing + 2;
        int begini = text_offset / line_spacing;
        int scolor = listbox_config.int_config["sel_color"];
        int iconw = guiRoot.GetDefaultRect("iconoffset").width;
        for(size_t i = begini; i < items.size() && i < (size_t)(begini + itemcount); ++i) {
            unsigned int advx = std::get<0>(items[i]) ? iconw : 0;
            unsigned int advy = font_size + line_spacing * i;
            unsigned int color = (i == (size_t)current_sel) ? scolor : std::get<2>(items[i]);
            for(auto ch : std::get<1>(items[i])) {
                if(ch < L' ')
                    continue;
                auto& gl = font->GetGlyph(ch);
                guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx, text_pos.y + gl.bounds.top + advy, cur_char.vertex);
                cur_char.color = color;
                cur_char.texcoord.x = (float)gl.textureRect.left / tex_size.x;
                cur_char.texcoord.y = (float)gl.textureRect.top / tex_size.y;
                charvtx.push_back(cur_char);
                guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx + gl.bounds.width, text_pos.y + gl.bounds.top + advy, cur_char.vertex);
                cur_char.color = color;
                cur_char.texcoord.x = (float)(gl.textureRect.left + gl.textureRect.width) / tex_size.x;
                cur_char.texcoord.y = (float)gl.textureRect.top / tex_size.y;
                charvtx.push_back(cur_char);
                guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx, text_pos.y + gl.bounds.top + gl.bounds.height + advy, cur_char.vertex);
                cur_char.color = color;
                cur_char.texcoord.x = (float)gl.textureRect.left / tex_size.x;
                cur_char.texcoord.y = (float)(gl.textureRect.top + gl.textureRect.height) / tex_size.y;
                charvtx.push_back(cur_char);
                guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx + gl.bounds.width, text_pos.y + gl.bounds.top + gl.bounds.height + advy, cur_char.vertex);
                cur_char.color = color;
                cur_char.texcoord.x = (float)(gl.textureRect.left + gl.textureRect.width) / tex_size.x;
                cur_char.texcoord.y = (float)(gl.textureRect.top + gl.textureRect.height) / tex_size.y;
                charvtx.push_back(cur_char);
                advx += gl.advance + spacing_x;
                vert_size++;
            }
        }

        if(vert_size > mem_size) {
            if(mem_size == 0)
                mem_size = 16;
            while(mem_size < vert_size)
                mem_size *= 2;
            glBindBuffer(GL_ARRAY_BUFFER, tbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * mem_size * 4, nullptr, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v2ct) * charvtx.size(), &charvtx[0]);
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, tbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v2ct) * charvtx.size(), &charvtx[0]);
        }
    }
    
    void SGListBox::InsertItem(unsigned int index, unsigned short icon, const std::wstring& item, unsigned int color, int val) {
        if(index >= items.size())
            items.push_back(std::make_tuple(icon, item, color, val));
        else
            items.insert(items.begin() + index, std::make_tuple(icon, item, color, val));
        if(items.size() * line_spacing > (size_t)(size_abs.y - text_area.top - text_area.height)) {
            auto sptr = std::static_pointer_cast<SGScrollBar>(children[0]);
            sptr->SetSliderLength(sptr->GetSize().y * (size_abs.y - text_area.top - text_area.height) / (items.size() * line_spacing));
        }
        current_sel = -1;
        vertices_dirty = true;
    }
    
    void SGListBox::AddItem(unsigned short icon, const std::wstring& item, unsigned int color, int val) {
        items.push_back(std::make_tuple(icon, item, color, val));
        if(items.size() * line_spacing > (size_t)(size_abs.y - text_area.top - text_area.height)) {
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
        if(items.size() * line_spacing > (size_t)(size_abs.y - text_area.top - text_area.height))
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
    
    void SGListBox::SetItemValue(unsigned int index, int val) {
        if(items.size() == 0)
            return;
        std::get<3>(items[index]) = val;
    }
    
    const std::tuple<unsigned short, std::wstring, unsigned int, int>& SGListBox::GetItem(unsigned int index) {
        static std::tuple<unsigned short, std::wstring, unsigned int, int> st;
        if(index >= items.size())
            return st;
        return items[index];
    }
    
    void SGListBox::SetSelection(int sel) {
        if(sel < 0 || sel >= (int)items.size())
            current_sel = -1;
        else {
            current_sel = sel;
            if(items.size() * line_spacing > (size_t)(size_abs.y - text_area.top - text_area.height)) {
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
    
    int SGListBox::GetHoverItem(int offsetx, int offsety) {
        if(offsetx >= position_abs.x + text_area.left
           && offsetx <= position_abs.x + size_abs.x - text_area.width
           && offsety >= position_abs.y + text_area.top
           && offsety <= position_abs.y + size_abs.y - text_area.height)
            return (offsety - position_abs.y - text_area.top + text_offset) / line_spacing;
        return -1;
    }
    
    bool SGListBox::EventMouseButtonDown(MouseButtonEvent evt) {
        eventMouseButtonDown.TriggerEvent(*this, evt);
        auto choving = std::static_pointer_cast<SGScrollBar>(hoving.lock());
        if(choving) {
            choving->EventMouseButtonDown(evt);
            return true;
        } else {
            if(evt.button == GLFW_MOUSE_BUTTON_LEFT
               && evt.x >= position_abs.x + text_area.left && evt.x <= position_abs.x + size_abs.x - text_area.width
               && evt.y >= position_abs.y + text_area.top && evt.y <= position_abs.y + size_abs.y - text_area.height) {
                int sel = (evt.y - position_abs.y - text_area.top + text_offset) / line_spacing;
                double now = guiRoot.GetTime();
                if(sel == current_sel) {
                    if(now - click_time < 0.3) {
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
    
    bool SGListBox::EventMouseWheel(MouseWheelEvent evt) {
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
        auto iter2 = listbox_config.string_config.find("font_name");
        if(iter2 != listbox_config.string_config.end())
            ptr->SetFont(&guiRoot.GetGUIFont(iter2->second));
        else
            ptr->SetFont(&guiRoot.GetGUIFont("default"));
        ptr->color1 = listbox_config.int_config["color1"];
        ptr->color2 = listbox_config.int_config["color2"];
        ptr->sel_color = listbox_config.int_config["sel_color"];
        ptr->sel_bcolor = listbox_config.int_config["sel_bcolor"];
        ptr->line_spacing = listbox_config.int_config["spacing"];
        ptr->text_offset = 0;
        glGenBuffers(1, &ptr->vbo);
        glGenVertexArrays(1, &ptr->vao);
        glBindVertexArray(ptr->vao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), 0);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::color_offset);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::tex_offset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, guiRoot.GetIndexBuffer());
        glBindVertexArray(0);
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
        virtual bool EventMouseMove(MouseMoveEvent evt) {
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
        
        virtual bool EventMouseButtonDown(MouseButtonEvent evt) {
            auto choving = hoving.lock();
            if(choving) {
                choving->EventMouseButtonDown(evt);
                return true;
            } else if(evt.button == GLFW_MOUSE_BUTTON_LEFT
                      && evt.x >= position_abs.x + text_area.left && evt.x <= position_abs.x + size_abs.x - text_area.width
                      && evt.y >= position_abs.y + text_area.top && evt.y <= position_abs.y + size_abs.y - text_area.height) {
                int sel = (evt.y - position_abs.y - text_area.top + text_offset) / line_spacing;
                auto cb = combo_box.lock();
                if(cb != nullptr) {
                    cb->SetSelection(sel);
                    Destroy();
                }
            }
            return true;
        }
        
        bool ScrollBarChange(SGWidget& sender, float value) {
            text_offset = (items.size() * line_spacing - (size_abs.y - text_area.top - text_area.height)) * value / 100.0f;
            vertices_dirty = true;
            return true;
        }
        
        static std::shared_ptr<SGListBoxInner> Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, v2i sz, std::shared_ptr<SGComboBox> c) {
            auto ptr = std::make_shared<SGListBoxInner>();
            ptr->parent = p;
            ptr->position = pos;
            ptr->size = sz;
            ptr->text_area = listbox_config.tex_config["text_area"];
            ptr->PostResize(true, true);
            ptr->combo_box = c;
            auto iter = listbox_config.int_config.find("gui_color");
            if(iter != listbox_config.int_config.end())
                ptr->color = iter->second;
            else
                ptr->color = guiRoot.GetDefaultInt("gui_color");
            auto iter2 = listbox_config.string_config.find("font_name");
            if(iter2 != listbox_config.string_config.end())
                ptr->SetFont(&guiRoot.GetGUIFont(iter2->second));
            else
                ptr->SetFont(&guiRoot.GetGUIFont("default"));
            ptr->color1 = listbox_config.int_config["color1"];
            ptr->color2 = listbox_config.int_config["color2"];
            ptr->sel_color = listbox_config.int_config["sel_color"];
            ptr->sel_bcolor = listbox_config.int_config["sel_bcolor"];
            ptr->line_spacing = listbox_config.int_config["spacing"];
            ptr->text_offset = 0;
            glGenBuffers(1, &ptr->vbo);
            glGenVertexArrays(1, &ptr->vao);
            glBindVertexArray(ptr->vao);
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), 0);
            glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::color_offset);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::tex_offset);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, guiRoot.GetIndexBuffer());
            glBindVertexArray(0);
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
        
    private:
        std::weak_ptr<SGComboBox> combo_box;
    };
    
    SGConfig SGComboBox::combobox_config;
    
    SGComboBox::~SGComboBox() {

    }
    
    void SGComboBox::UpdateVertices() {
        if(!vertices_dirty)
            return;
        vertices_dirty = false;
        text_update = true;
        std::array<glbase::v2ct, 40> vert;
        int bw = combobox_config.int_config["border"];
        int ht = combobox_config.int_config["height"];
        auto back = combobox_config.tex_config[is_hoving ? "backh" : "back"];
        auto down = combobox_config.tex_config[show_item ? "down3" : is_hoving ? "down2" : "down1"];
        auto drop = combobox_config.tex_config["drop_area"];
        guiRoot.SetRectVertex(&vert[0], position_abs.x, position_abs.y, bw, bw,
                              recti{back.left, back.top, bw, bw});
        guiRoot.SetRectVertex(&vert[4], position_abs.x + bw, position_abs.y, size_abs.x - bw * 2, bw,
                              recti{back.left + bw, back.top, back.width - bw * 2, bw});
        guiRoot.SetRectVertex(&vert[8], position_abs.x + size_abs.x - bw, position_abs.y, bw, bw,
                              recti{back.left + back.width - bw, back.top, bw, bw});
        guiRoot.SetRectVertex(&vert[12], position_abs.x, position_abs.y + bw, bw, ht - bw * 2,
                              recti{back.left, back.top + bw, bw, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[16], position_abs.x + bw, position_abs.y + bw, size_abs.x - bw * 2, ht - bw * 2,
                              recti{back.left + bw, back.top + bw, back.width - bw * 2, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[20], position_abs.x + size_abs.x - bw, position_abs.y + bw, bw, ht - bw * 2,
                              recti{back.left + back.width - bw, back.top + bw, bw, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[24], position_abs.x, position_abs.y + ht - bw, bw, bw,
                              recti{back.left, back.top + back.height - bw, bw, bw});
        guiRoot.SetRectVertex(&vert[28], position_abs.x + bw, position_abs.y + ht - bw, size_abs.x - bw * 2, bw,
                              recti{back.left + bw, back.top + back.height - bw, back.width - bw * 2, bw});
        guiRoot.SetRectVertex(&vert[32], position_abs.x + size_abs.x - bw, position_abs.y + ht - bw, bw, bw,
                              recti{back.left + back.width - bw, back.top + back.height - bw, bw, bw});
        guiRoot.SetRectVertex(&vert[36], position_abs.x + size_abs.x - drop.left, position_abs.y + drop.top, drop.width, drop.height, down);
        for(int i = 0; i < 40; ++i)
            vert[i].color = color;
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v2ct) * 40, &vert[0]);
    }
    
    void SGComboBox::Draw() {
        UpdateVertices();
        guiRoot.BindGuiTexture();
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLE_STRIP, 10 * 6 - 2, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
        int tw = size_abs.x - text_area.left - text_area.width;
        int th = size_abs.y - text_area.top - text_area.height;
        guiRoot.BeginScissor(recti{position_abs.x + text_area.left, position_abs.y + text_area.top, tw, th});
        DrawText();
        guiRoot.EndScissor();
    }
    
    void SGComboBox::PostResize(bool res, bool rep) {
        SGWidget::PostResize(res, rep);
        size_abs.y = combobox_config.int_config["height"];
    }
    
    v2i SGComboBox::GetTextOffset() {
        return position_abs + v2i{text_area.left, text_area.top};
    }
    
    void SGComboBox::InsertItem(unsigned int index, const std::wstring& item, unsigned int color, int val) {
        items.insert(items.begin() + index, std::make_tuple(item, color, val));
        if(current_sel >= 0 && (int)index <= current_sel)
            current_sel++;
    }
    
    void SGComboBox::AddItem(const std::wstring& item, unsigned int color, int val) {
        items.push_back(std::make_tuple(item, color, val));
    }
    
    void SGComboBox::RemoveItem(unsigned int index) {
        if((size_t)index >= items.size())
            return;
        items.erase(items.begin() + index);
        if((int)index == current_sel) {
            ClearText();
            current_sel = -1;
        } else if((int)index < current_sel)
            current_sel--;
    }
    
    void SGComboBox::ClearItem() {
        items.clear();
        ClearText();
        current_sel = -1;
    }
    
    void SGComboBox::SetItemText(unsigned int index, const std::wstring& text, unsigned int color) {
        if((size_t)index >= items.size())
            return;
        auto& it = items[index];
        std::get<0>(it) = text;
        std::get<1>(it) = color;
        if(index == (unsigned int)current_sel)
            SetText(text, color);
    }
    
    void SGComboBox::SetItemValue(unsigned int index, int val) {
        if((size_t)index >= items.size())
            return;
        std::get<2>(items[index]) = val;
    }
    
    void SGComboBox::SetSelection(int sel) {
        if(sel < 0 || (size_t)sel >= items.size())
            sel = -1;
        if(sel != current_sel) {
            current_sel = sel;
            auto& item = items[sel];
            SetText(std::get<0>(item), std::get<1>(item));
            eventSelChange.TriggerEvent(*this, sel);
        }
    }
    
    int SGComboBox::GetSelection() {
        return current_sel;
    }
    
    int SGComboBox::GetSelectedValue() {
        if(current_sel == -1 || (size_t)current_sel >= items.size())
            return 0;
        return std::get<2>(items[current_sel]);
    }
    
    void SGComboBox::ShowList(bool show) {
        if(show_item == show)
            return;
        if(show) {
            auto lst = SGListBoxInner::Create(nullptr, {position_abs.x, position_abs.y + combobox_config.int_config["listoffset"]}, {0, 0},
                                              std::static_pointer_cast<SGComboBox>(this->shared_from_this()));
            lst->SetSize({size_abs.x, combobox_config.int_config["listheight"]});
            for(auto& it : items)
                lst->AddItem(0, std::get<0>(it), std::get<1>(it));
            lst->SetSelection(current_sel);
            lst->eventDestroying.Bind([this](SGWidget& sender)->bool {
                ShowList(false);
                return true;
            });
            guiRoot.SetPopupObject(lst);
        }
        show_item = show;
        vertices_dirty = true;
    }
    
    bool SGComboBox::EventMouseEnter() {
        if(!is_hoving) {
            is_hoving = true;
            vertices_dirty = true;
        }
        return SGWidget::EventMouseEnter();
    }
    
    bool SGComboBox::EventMouseLeave() {
        is_hoving = false;
        vertices_dirty = true;
        return SGWidget::EventMouseLeave();
    }
    
    bool SGComboBox::EventMouseButtonDown(MouseButtonEvent evt) {
        if(evt.button == GLFW_MOUSE_BUTTON_LEFT)
            ShowList(!show_item);
        return SGWidget::EventMouseButtonDown(evt);
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
        auto iter2 = combobox_config.string_config.find("font_name");
        if(iter2 != combobox_config.string_config.end())
            ptr->SetFont(&guiRoot.GetGUIFont(iter2->second));
        else
            ptr->SetFont(&guiRoot.GetGUIFont("default"));
        glGenBuffers(1, &ptr->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * 40, nullptr, GL_DYNAMIC_DRAW);
        glGenVertexArrays(1, &ptr->vao);
        glBindVertexArray(ptr->vao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), 0);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::color_offset);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::tex_offset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, guiRoot.GetIndexBuffer());
        glBindVertexArray(0);
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
                size_abs = size + v2i{(int)(ssize.x * size_prop.x), (int)(ssize.y * size_prop.y)};
            if(rep) {
                position_abs = position + v2i{(int)(ssize.x * position_prop.x), (int)(ssize.y * position_prop.y)};
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
        std::vector<glbase::v2ct> vert;
        
        int bw = tab_config.int_config["client_border"];
        int ht = tab_config.int_config["client_height"];
        auto back = tab_config.tex_config["client_area"];
        
        vert.resize((9 + item_count * 3) * 4);
        
        guiRoot.SetRectVertex(&vert[0], position_abs.x, position_abs.y + ht, bw, bw,
                              recti{back.left, back.top, bw, bw});
        guiRoot.SetRectVertex(&vert[4], position_abs.x + bw, position_abs.y + ht, size_abs.x - bw * 2, bw,
                              recti{back.left + bw, back.top, back.width - bw * 2, bw});
        guiRoot.SetRectVertex(&vert[8], position_abs.x + size_abs.x - bw, position_abs.y + ht, bw, bw,
                              recti{back.left + back.width - bw, back.top, bw, bw});
        guiRoot.SetRectVertex(&vert[12], position_abs.x, position_abs.y + ht + bw, bw, size_abs.y - ht - bw * 2,
                              recti{back.left, back.top + bw, bw, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[16], position_abs.x + bw, position_abs.y + ht + bw, size_abs.x - bw * 2, size_abs.y - ht - bw * 2,
                              recti{back.left + bw, back.top + bw, back.width - bw * 2, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[20], position_abs.x + size_abs.x - bw, position_abs.y + ht + bw, bw, size_abs.y - ht - bw * 2,
                              recti{back.left + back.width - bw, back.top + bw, bw, back.height - bw * 2});
        guiRoot.SetRectVertex(&vert[24], position_abs.x, position_abs.y + size_abs.y - bw, bw, bw,
                              recti{back.left, back.top + back.height - bw, bw, bw});
        guiRoot.SetRectVertex(&vert[28], position_abs.x + bw, position_abs.y + size_abs.y - bw, size_abs.x - bw * 2, bw,
                              recti{back.left + bw, back.top + back.height - bw, back.width - bw * 2, bw});
        guiRoot.SetRectVertex(&vert[32], position_abs.x + size_abs.x - bw, position_abs.y + size_abs.y - bw, bw, bw,
                              recti{back.left + back.width - bw, back.top + back.height - bw, bw, bw});
        
        int tb = tab_config.int_config["tab_border"];
        auto t1 = tab_config.tex_config["tab_title1"];
        auto t2 = tab_config.tex_config["tab_title2"];
        auto t3 = tab_config.tex_config["tab_title3"];
        int ew = tab_config.int_config["extra_width"];
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
            guiRoot.SetRectVertex(&vert[startv], position_abs.x + ti - ew, position_abs.y, tb, tab_height,
                                  recti{rct.left, rct.top, tb, rct.height});
            guiRoot.SetRectVertex(&vert[startv + 4], position_abs.x + ti - ew + tb, position_abs.y, ptr->width + ew * 2 - tb * 2, tab_height,
                                  recti{rct.left + tb, rct.top, rct.width - tb * 2, rct.height});
            guiRoot.SetRectVertex(&vert[startv + 8], position_abs.x + ti + ptr->width + ew - tb, position_abs.y, tb, tab_height,
                                  recti{rct.left + rct.width - tb, rct.top, tb, rct.height});
            ti += ptr->width;
            startv += 12;
        }
        if(act) {
            auto ptr = std::static_pointer_cast<SGTabInner>(active_tab.lock());
            ti = actti;
            guiRoot.SetRectVertex(&vert[startv], position_abs.x + ti - ew, position_abs.y, tb, tab_height,
                                  recti{t1.left, t1.top, tb, t1.height});
            guiRoot.SetRectVertex(&vert[startv + 4], position_abs.x + ti - ew + tb, position_abs.y, ptr->width + ew * 2 - tb * 2, tab_height,
                                  recti{t1.left + tb, t1.top, t1.width - tb * 2, t1.height});
            guiRoot.SetRectVertex(&vert[startv + 8], position_abs.x + ti + ptr->width + ew - tb, position_abs.y, tb, tab_height,
                                  recti{t1.left + t1.width - tb, t1.top, tb, t1.height});
        }
        
        for(auto& vt : vert)
            vt.color = color;
        
        if(max_item_count == 0 || item_count > max_item_count) {
            if(max_item_count == 0)
                max_item_count = 8;
            while(max_item_count < item_count)
                max_item_count *= 2;
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * ((9 + max_item_count * 3) * 4), nullptr, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v2ct) * ((9 + item_count * 3) * 4), &vert[0]);
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v2ct) * ((9 + item_count * 3) * 4), &vert[0]);
        }
    }
    
    void SGTabControl::Draw() {
        EvaluateSize();
        UpdateVertices();
        guiRoot.BindGuiTexture();
        glBindVertexArray(vao);
        guiRoot.BeginScissor(recti{position_abs.x, position_abs.y, size_abs.x, size_abs.y});
        glDrawElements(GL_TRIANGLE_STRIP, (9 + item_count * 3) * 6 - 2, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
        DrawText();
        guiRoot.EndScissor();
        guiRoot.BeginScissor(recti{position_abs.x, position_abs.y + tab_height, size_abs.x, size_abs.y - tab_height});
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
                if((int)(max_w + ptr->width) > width)
                    item_count = i + 1;
                else
                    max_w += ptr->width;
            }
        }
        if(item_count == 0)
            item_count = sz;
    }
    
    void SGTabControl::UpdateTextVertex() {
        if(font == nullptr)
            return;
        if(!text_update)
            return;
        if(children.size() == 0)
            return;
        text_update = false;
        vert_size = 0;
        auto tex_size = font->GetTexture().GetSize();
        std::vector<glbase::v2ct> charvtx;
        std::vector<glbase::v2ct> iconvtx;
        glbase::v2ct cur_char;
        v2i text_pos = GetTextOffset();
        int tb = tab_config.int_config["tab_border"];
        int ti = tab_ol;
        int font_size = font->GetFontSize();
        text_pos.y += font_size + tab_config.int_config["title_offset"];
        for(size_t i = 0; i < children.size(); ++i) {
            auto ptr = std::static_pointer_cast<SGTabInner>(children[i]);
            unsigned int advx = ti + tb;
            unsigned int color = ptr->color;
            ti += ptr->width;
            unsigned int tw = tb * 2;
            for(auto ch : ptr->title) {
                if(ch < L' ')
                    continue;
                auto& gl = font->GetGlyph(ch);
                if(tw + gl.advance > ptr->width)
                    break;
                tw += gl.advance + spacing_x;
            }
            advx += (ptr->width > tw) ? (ptr->width - tw) / 2 : 0;
            tw = tb * 2;
            for(auto ch : ptr->title) {
                if(ch < L' ')
                    continue;
                auto& gl = font->GetGlyph(ch);
                if(tw + gl.advance > ptr->width)
                    break;
                guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx, text_pos.y + gl.bounds.top, cur_char.vertex);
                cur_char.color = color;
                cur_char.texcoord.x = (float)gl.textureRect.left / tex_size.x;
                cur_char.texcoord.y = (float)gl.textureRect.top / tex_size.y;
                charvtx.push_back(cur_char);
                guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx + gl.bounds.width, text_pos.y + gl.bounds.top, cur_char.vertex);
                cur_char.color = color;
                cur_char.texcoord.x = (float)(gl.textureRect.left + gl.textureRect.width) / tex_size.x;
                cur_char.texcoord.y = (float)gl.textureRect.top / tex_size.y;
                charvtx.push_back(cur_char);
                guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx, text_pos.y + gl.bounds.top + gl.bounds.height, cur_char.vertex);
                cur_char.color = color;
                cur_char.texcoord.x = (float)gl.textureRect.left / tex_size.x;
                cur_char.texcoord.y = (float)(gl.textureRect.top + gl.textureRect.height) / tex_size.y;
                charvtx.push_back(cur_char);
                guiRoot.ConvertXY(text_pos.x + gl.bounds.left + advx + gl.bounds.width, text_pos.y + gl.bounds.top + gl.bounds.height, cur_char.vertex);
                cur_char.color = color;
                cur_char.texcoord.x = (float)(gl.textureRect.left + gl.textureRect.width) / tex_size.x;
                cur_char.texcoord.y = (float)(gl.textureRect.top + gl.textureRect.height) / tex_size.y;
                charvtx.push_back(cur_char);
                advx += gl.advance + spacing_x;
                tw += gl.advance + spacing_x;
                vert_size++;
            }
        }
        
        if(vert_size > mem_size) {
            if(mem_size == 0)
                mem_size = 16;
            while(mem_size < vert_size)
                mem_size *= 2;
            glBindBuffer(GL_ARRAY_BUFFER, tbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * mem_size * 4, nullptr, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v2ct) * charvtx.size(), &charvtx[0]);
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, tbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v2ct) * charvtx.size(), &charvtx[0]);
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
        if((size_t)index >= children.size())
            return;
        auto ptr = std::static_pointer_cast<SGTabInner>(children[index]);
        ptr->title = title;
        ptr->color = color;
        size_dirty = true;
        text_update = true;
    }
    
    std::shared_ptr<SGWidgetContainer> SGTabControl::GetTab(int index) {
        if(index >= (int)children.size())
            return nullptr;
        return std::static_pointer_cast<SGWidgetContainer>(children[index]);
    }
    
    void SGTabControl::SetActiveTab(int index) {
        if(index >= (int)children.size())
            return;
        active_tab = children[index];
        vertices_dirty = true;
    }
    
    int SGTabControl::GetTabCount() {
        return children.size();
    }
    
    int SGTabControl::GetActiveTab() {
        for(size_t i = 0; i < children.size(); ++i)
            if(children[i] == active_tab.lock())
                return i;
        return -1;
    }
    
    bool SGTabControl::EventMouseMove(MouseMoveEvent evt) {
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
    
    bool SGTabControl::EventMouseButtonDown(MouseButtonEvent evt) {
        if(!in_tab) {
            if(evt.button == GLFW_MOUSE_BUTTON_LEFT) {
                int ind = GetHovingTab(v2i{evt.x, evt.y});
                if(ind == -1)
                    return true;
                auto ptr = children[ind];
                if(hover_tab.lock() != ptr) {
                    hover_tab = ptr;
                    vertices_dirty = true;
                }
                if(active_tab.lock() != ptr) {
                    if(!active_tab.expired())
                        active_tab.lock()->EventLostFocus();
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
    
    bool SGTabControl::EventMouseButtonUp(MouseButtonEvent evt) {
        if(in_tab && !active_tab.expired())
            return active_tab.lock()->EventMouseButtonUp(evt);
        return false;
    }
    
    bool SGTabControl::EventMouseWheel(MouseWheelEvent evt) {
        if(!active_tab.expired())
            return active_tab.lock()->EventMouseWheel(evt);
        return false;
    }
    
    bool SGTabControl::EventKeyDown(KeyEvent evt) {
        if(!active_tab.expired())
            return active_tab.lock()->EventKeyDown(evt);
        return false;
    }
    
    bool SGTabControl::EventKeyUp(KeyEvent evt) {
        if(!active_tab.expired())
            return active_tab.lock()->EventKeyUp(evt);
        return false;
    }
    
    bool SGTabControl::EventCharEnter(TextEvent evt) {
        if(!active_tab.expired())
            return active_tab.lock()->EventCharEnter(evt);
        return false;
    }
    
    int SGTabControl::GetHovingTab(v2i pos) {
        if(children.size() == 0)
            return -1;
        int ti = position_abs.x + tab_ol;
        for(size_t i = 0; i < children.size(); ++i) {
            auto ptr = std::static_pointer_cast<SGTabInner>(children[i]);
            if(pos.x >= ti && pos.x <= (int)(ti + ptr->width))
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
        auto iter2 = tab_config.string_config.find("font_name");
        if(iter2 != tab_config.string_config.end())
            ptr->SetFont(&guiRoot.GetGUIFont(iter2->second));
        else
            ptr->SetFont(&guiRoot.GetGUIFont("default"));
        ptr->tab_height = tab_config.int_config["tab_height"];
        ptr->tab_ol = tab_config.int_config["tab_offsetl"];
        ptr->tab_or = tab_config.int_config["tab_offsetr"];
        glGenBuffers(1, &ptr->vbo);
        glGenVertexArrays(1, &ptr->vao);
        glBindVertexArray(ptr->vao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), 0);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::color_offset);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::tex_offset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, guiRoot.GetIndexBuffer());
        glBindVertexArray(0);
        if(p != nullptr)
            p->AddChild(ptr);
        else
            guiRoot.AddChild(ptr);
        return ptr;
    }
}

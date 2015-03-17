#ifndef _SUNGUI_H_
#define _SUNGUI_H_

#include "render_base.h"
#include <GLFW/glfw3.h>

namespace sgui
{

    struct MouseMoveEvent {
        int32_t x;
        int32_t y;
    };
    
    struct MouseButtonEvent {
        int32_t button;
        int32_t mods;
        int32_t x;
        int32_t y;
    };
    
    struct MouseWheelEvent {
        float deltax;
        float deltay;
    };
    
    struct KeyEvent {
        int32_t key;
        int32_t mods;
    };
    
    struct TextEvent {
        uint32_t unichar;
    };
    
    // ===== Delegate Implement =====
    // OT: Object Type
    // ST: Sender Type
    // ET: Event Type
    // MT: Method Type
    
    template<typename ST, typename... ET>
    class SGDelegate {
        typedef std::function<bool (ST&, ET...)> MT;
    public:
        SGDelegate(const MT& fun): invoke_method(fun) {}
        virtual bool Invoke(ST& sender, ET... evt) {
            return invoke_method(sender, evt...);
        }
        
    protected:
        MT invoke_method = nullptr;
    };
    
    template<typename ST, typename... ET>
    class SGEventHandler {
    public:
        template<typename OT>
        void Bind(OT* obj, bool (OT::*fun)(ST&, ET...)) {
            auto lambda = [obj, fun](ST& sender, ET... evt)->bool { return (obj->*fun)(sender, evt...); };
            auto ptr = std::make_shared<SGDelegate<ST, ET...>>(lambda);
            delegate_ptrs.push_back(ptr);
        }
        
        template<typename FT>
        void Bind(FT f) {
            auto ptr = std::make_shared<SGDelegate<ST, ET...>>(f);
            delegate_ptrs.push_back(ptr);
        }
        
        void Reset() {
            delegate_ptrs.clear();
        }
        
        bool TriggerEvent(ST& sender, ET... evt) {
            if(delegate_ptrs.size() == 0)
                return false;
            bool ret = false;
            for(auto& ptr : delegate_ptrs)
                ret = ptr->Invoke(sender, evt...) || ret;
            return ret;
        }
        
    protected:
        std::vector<std::shared_ptr<SGDelegate<ST, ET...>>> delegate_ptrs;
    };
    
    // ===== Delegate Implement End =====
    
    // == basic ui elements ==
    struct UICommonStatus {
        v2i screen_size;
        bool need_update;
    };
    
    class UIBase {
    public:
        inline static v2f ConvScreenCoord(v2i pos) {
            auto& status = CommonStatus();
            return v2f{pos.x * 2.0f / status.screen_size.x - 1.0f, 1.0f - pos.y * 2.0f / status.screen_size.y};
        }
        
        inline static void SetScreenSize(v2i sz) {
            auto& status = CommonStatus();
            if(sz == status.screen_size)
                return;
            status.screen_size = sz;
            status.need_update = true;
        }
        
        inline static void EndUpdate() { CommonStatus().need_update = false; }
        inline static UICommonStatus& CommonStatus() { static UICommonStatus status; return status; }
    };
    
    class UIComponent : public UIBase, public base::RenderUnit<base::v2ct> {
    public:
        virtual int32_t GetPrimitiveType() = 0;
        virtual int32_t GetTextureId() = 0;
        
        virtual void RefreshVertices() {}
        
        virtual void PushVertices(base::RenderObject<base::v2ct>& render_obj) {
            std::tie(vert_index, index_index) = render_obj.BeginPrimitive(GetPrimitiveType(), GetTextureId());
            Update();
            render_obj.PushVertices(&vertices[0], &indices[0], vertices.size(), indices.size());
        }
        
        virtual void UpdateVertices(base::RenderObject<base::v2ct>& render_obj) {
            Update();
            render_obj.UpdateVertices(&vertices[0], vert_index, vertices.size());
            render_obj.Updateindices(&indices[0], index_index, indices.size());
        }
        
        inline void Update() {
            if(!(need_update || CommonStatus().need_update))
                return;
            RefreshVertices();
            need_update = 0;
        }
        
        inline void SetPosition(v2i pos) {
            if(pos == position)
                return;
            position = pos;
            need_update = true;
        }
        
        inline void SetSize(v2i sz) {
            if(sz == size)
                return;
            size = sz;
            need_update = true;
        }
        
        inline void SetColor(uint32_t cl) {
            if(color == cl)
                return;
            color = cl;
            need_update = true;
        }
        
    protected:
        v2i position;
        v2i size;
        uint32_t color;
    };
    
    class UISprite : public UIComponent {
    public:
        virtual int32_t GetPrimitiveType() { return GL_TRIANGLES; }
        virtual int32_t GetTextureId() { return texture->GetTextureId(); }
        
        virtual void RefreshVertices() {
            vertices.resize(4);
            indices.resize(6);
            FillQuad(&vertices[0], &indices[0]);
        }
        
        inline void SetTexure(recti trct, base::Texture* tex) {
            tex_rect = trct;
            texture = tex;
            need_update = true;
        }
        
        void FillQuad(base::v2ct* v, int16_t* idx) {
            static const int16_t quad_idx[] = {0, 1, 2, 2, 1, 3};
            v[0].vertex = ConvScreenCoord({position.x, position.y});
            v[1].vertex = ConvScreenCoord({position.x + size.x, position.y});
            v[2].vertex = ConvScreenCoord({position.x, position.y + size.y});
            v[3].vertex = ConvScreenCoord({position.x + size.x, position.y + size.y});
            v[0].texcoord = texture->ConvTexCoord({tex_rect.left, tex_rect.top});
            v[1].texcoord = texture->ConvTexCoord({tex_rect.left + tex_rect.width, tex_rect.top});
            v[2].texcoord = texture->ConvTexCoord({tex_rect.left, tex_rect.top + tex_rect.height});
            v[3].texcoord = texture->ConvTexCoord({tex_rect.left + tex_rect.width, tex_rect.top + tex_rect.height});
            for(int16_t i = 0; i < 6; ++i)
                idx[i] = index_index + quad_idx[i];
            for(int16_t i = 0; i < 4; ++i)
                v[i].color = color;
        }
        
    protected:
        recti tex_rect;
        base::Texture* texture = nullptr;
    };
    
    class UISprite9 : UISprite {
    public:
        virtual void RefreshVertices() {
            vertices.resize(20);
            indices.resize(60);
            FillQuad9(&vertices[0], &indices[0]);
        }
        
        inline void SetFrameRect(recti frct, recti ftrct, recti brct) {
            frame = frct;
            frame_tex = ftrct;
            back_tex = brct;
            need_update = true;
        }
        
        void FillQuad9(base::v2ct* v, int16_t* idx) {
            static const int16_t quad9_idx[] = {    16,17,18,18,17,19,
                0, 1, 4, 4, 1, 5, 1, 2, 5, 5, 2, 6, 2, 3, 6, 6, 3, 7,
                4, 5, 8, 8, 5, 9, 5, 6, 9, 9, 6, 10,6, 7, 10,10,7, 11,
                8, 9, 12,12,9, 13,9, 10,13,13,10,14,10,11,14,14,11,15
            };
            v2f verts[4], texcoord[4];
            verts[0] = ConvScreenCoord({position.x, position.y});
            verts[1] = ConvScreenCoord({position.x + frame.left, position.y + frame.top});
            verts[2] = ConvScreenCoord({position.x + size.x - frame.width, position.y + size.y - frame.height});
            verts[3] = ConvScreenCoord({position.x + size.x, position.y + size.y});
            texcoord[0] = texture->ConvTexCoord({tex_rect.left, tex_rect.top});
            texcoord[1] = texture->ConvTexCoord({tex_rect.left + frame_tex.left, tex_rect.top + frame_tex.top});
            texcoord[2] = texture->ConvTexCoord({tex_rect.left + tex_rect.width - frame_tex.width, tex_rect.top + tex_rect.height - frame_tex.height});
            texcoord[3] = texture->ConvTexCoord({tex_rect.left + tex_rect.width, tex_rect.top + tex_rect.height});
            for(int32_t i = 0; i < 4; ++i) {
                for(int32_t j = 0; j < 4; ++j) {
                    v[i * 4 + j].vertex = v2f{verts[j].x, verts[i].y};
                    v[i * 4 + j].texcoord = v2f{texcoord[j].x, texcoord[i].y};
                }
            }
            v[16].vertex = v[0].vertex;
            v[17].vertex = v[3].vertex;
            v[18].vertex = v[12].vertex;
            v[19].vertex = v[15].vertex;
            v[16].texcoord = texture->ConvTexCoord({back_tex.left, back_tex.top});
            v[17].texcoord = texture->ConvTexCoord({back_tex.left + back_tex.width, back_tex.top});
            v[18].texcoord = texture->ConvTexCoord({back_tex.left, back_tex.top + back_tex.height});
            v[19].texcoord = texture->ConvTexCoord({back_tex.left + back_tex.width, back_tex.top + back_tex.height});
            for(int32_t i = 0; i < 60; ++i)
                idx[i] = quad9_idx[i];
            for(int16_t i = 0; i < 20; ++i)
                v[i].color = color;
        }
        
    protected:
        recti frame;
        recti frame_tex;
        recti back_tex;
    };
    
    class UITextBase : UIComponent {
    public:
        UITextBase(int32_t capacity) {
            vertices.resize((capacity + 4) * 4);
            indices.resize((capacity + 4) * 6);
            vert_cap = capacity;
        }
        
        virtual int32_t GetPrimitiveType() { return GL_TRIANGLES; }
        virtual int32_t GetTextureId() { return text_font->GetTexture().GetTextureId(); }
        
        virtual void RefreshVertices() {
            static const int16_t quad_idx[] = {0, 1, 2, 2, 1, 3};
            advance.x = 0;
            advance.y = 0;
            line_spacing = 0;
            int32_t actual_size = 0;
            auto len = texts.length();
            for(size_t i = 0; i < len; ++i) {
                auto ch = texts[i];
                if(ch < L' ') {
                    if(ch == L'\n') {
                        advance.y += line_spacing;
                        line_spacing = 0;
                    }
                    continue;
                }
                auto color = colors[i];
                auto& gl = text_font->GetGlyph(ch);
                if(advance.x + gl.advance > max_width) {
                    advance.y += line_spacing;
                    line_spacing = 0;
                }
                line_spacing = std::max(text_font->GetLineSpacing(ch), line_spacing);
                auto v = &vertices[actual_size * 4];
                auto idx = &indices[actual_size * 6];
                actual_size++;
                v[0].vertex = ConvScreenCoord({position.x + advance.x + gl.bounds.left, position.y + advance.y + gl.bounds.top});
                v[1].vertex = ConvScreenCoord({position.x + advance.x + gl.bounds.left + gl.bounds.width, position.y + advance.y + gl.bounds.top});
                v[2].vertex = ConvScreenCoord({position.x + advance.x + gl.bounds.left, position.y + advance.y + gl.bounds.top + gl.bounds.height});
                v[3].vertex = ConvScreenCoord({position.x + advance.x + gl.bounds.left + gl.bounds.width, position.y + advance.y + gl.bounds.top + gl.bounds.height});
                v[0].texcoord = text_font->GetTexture().ConvTexCoord({gl.textureRect.left, gl.textureRect.top});
                v[1].texcoord = text_font->GetTexture().ConvTexCoord({gl.textureRect.left + gl.textureRect.width, gl.textureRect.top});
                v[2].texcoord = text_font->GetTexture().ConvTexCoord({gl.textureRect.left, gl.textureRect.top + gl.textureRect.height});
                v[3].texcoord = text_font->GetTexture().ConvTexCoord({gl.textureRect.left + gl.textureRect.width, gl.textureRect.top + gl.textureRect.height});
                for(int16_t i = 0; i < 6; ++i)
                    idx[i] = index_index + quad_idx[i];
                if(!text_font->IsEmoji(ch))
                    for(int16_t i = 0; i < 4; ++i) {
                        v[i].color = color;
                    }
            }
            for(size_t i = len; i < vert_cap; ++i) {
                auto idx = &indices[i * 6];
                for(auto j = 0; j < 6; ++j)
                    idx[j] = 0;
            }
        }
        
        inline void SetFont(base::Font* ft) {
            if(!ft)
                return;
            text_font = ft;
            need_update = true;
        }
        
        inline void SetMaxWidth(int32_t mw) {
            if(max_width == mw)
                return;
            max_width = mw;
            need_update = true;
        }
        
        inline bool SetText(std::wstring& t, int32_t cl) {
            vertices.clear();
            indices.clear();
            need_update = true;
            return AppendText(t, cl);
        }
        
        inline bool AppendText(std::wstring& t, int32_t cl) {
            int32_t app_size = 0;
            for(auto& ch : t) {
                if(ch >= ' ')
                    app_size++;
            }
            auto redraw = false;
            if(vertices.size() + app_size > vert_cap) {
                while(vertices.size() + app_size > vert_cap)
                    vert_cap *= 2;
                vertices.resize((vert_cap + 4) * 4);
                indices.resize((vert_cap + 4) * 6);
                redraw = true;
            }
            texts.append(t);
            for(auto i = 0; i < app_size; ++i)
                colors.push_back(cl);
            need_update = true;
            return redraw;
        }
        
        inline void Clear() {
            vertices.clear();
            indices.clear();
            need_update = true;
        }
        
    protected:
        base::Font* text_font = nullptr;
        int32_t max_width = 0xffffffff;
        int32_t vert_cap = 0;
        int32_t line_spacing = 0;
        v2i advance = {0, 0};
        v2i selection = {0, 0};
        std::wstring texts;
        std::vector<uint32_t> colors;
    };
    
    // ===== GUI Components =====
    
    class SGWidgetContainer;
    class SGGUIRoot;
    
    class SGWidget : public std::enable_shared_from_this<SGWidget> {
        
    public:
        virtual ~SGWidget();
        virtual bool CheckInside(v2i pos);
        virtual void UpdateVertices() = 0;
        virtual void Draw() = 0;
        virtual void PostRedraw();
        virtual void PostResize(bool res, bool rep);
        virtual bool AllowFocus() const { return true; }
        virtual std::shared_ptr<SGWidget> GetDragingTarget() { return nullptr; }
        virtual void Destroy();
        virtual void SetFocus();
        
        void SetPosition(v2i pos, v2f = v2f{0.0f, 0.0f});
        void SetSize(v2i sz, v2f = v2f{0.0f, 0.0f});
        void SetX(int32_t x, float propx = 0.0f);
        void SetY(int32_t y, float propy = 0.0f);
        void SetWidth(int32_t w, float propw = 0.0f);
        void SetHeight(int32_t h, float proph = 0.0f);
        void SetColor(uint32_t c);
        v2i GetPosition() const { return position_abs; }
        v2i GetSize() const { return size_abs; }
        void SetVisible(bool v);
        bool IsVisible() const;
        void SetCustomValue(uint32_t v) { cvalue = v; }
        uint32_t GetCustomValue() const { return cvalue; }
        void SetCustomObject(void* v) { cobject = v; }
        void* GetCustomObject() const { return cobject; }
        
        SGEventHandler<SGWidget, MouseMoveEvent> eventMouseMove;
        SGEventHandler<SGWidget> eventMouseEnter;
        SGEventHandler<SGWidget> eventMouseLeave;
        SGEventHandler<SGWidget, MouseButtonEvent> eventMouseButtonDown;
        SGEventHandler<SGWidget, MouseButtonEvent> eventMouseButtonUp;
        SGEventHandler<SGWidget, MouseWheelEvent> eventMouseWheel;
        SGEventHandler<SGWidget, KeyEvent> eventKeyDown;
        SGEventHandler<SGWidget, KeyEvent> eventKeyUp;
        SGEventHandler<SGWidget, TextEvent> eventCharEnter;
        SGEventHandler<SGWidget> eventGetFocus;
        SGEventHandler<SGWidget> eventLostFocus;
        
        SGEventHandler<SGWidget, v2i> eventDragBegin;
        SGEventHandler<SGWidget, v2i> eventDragEnd;
        SGEventHandler<SGWidget, v2i> eventDragUpdate;

        SGEventHandler<SGWidget> eventDestroying;
        
    public:
        virtual bool EventMouseMove(MouseMoveEvent evt);
        virtual bool EventMouseEnter();
        virtual bool EventMouseLeave();
        virtual bool EventMouseButtonDown(MouseButtonEvent evt);
        virtual bool EventMouseButtonUp(MouseButtonEvent evt);
        virtual bool EventMouseWheel(MouseWheelEvent evt);
        virtual bool EventKeyDown(KeyEvent evt);
        virtual bool EventKeyUp(KeyEvent evt);
        virtual bool EventCharEnter(TextEvent evt);
        virtual bool EventGetFocus();
        virtual bool EventLostFocus();
        
        virtual bool DragingBegin(v2i evt);
        virtual bool DragingEnd(v2i evt);
        virtual bool DragingUpdate(v2i evt);
        
    protected:
        v2i position = {0, 0};
        v2i position_abs = {0, 0};
        v2f position_prop = {0, 0};
        v2i size = {0, 0};
        v2i size_abs = {0, 0};
        v2f size_prop = {0, 0};
        v2i position_drag = {0, 0};
        bool visible = true;
        uint32_t cvalue = 0;
        uint32_t color = 0xffffffff;
        void* cobject = nullptr;
        std::weak_ptr<SGWidgetContainer> parent;
        bool vertices_dirty = true;
        uint32_t vbo = 0;
        uint32_t vao = 0;
    };
    
    class SGTextBase {
    public:
        SGTextBase();
        virtual ~SGTextBase();
        virtual void SetFont(base::Font* ft);
        virtual void SetSpacing(uint32_t x, uint32_t y);
        virtual int32_t GetLineSpacing();
        virtual v2i GetTextOffset() = 0;
        virtual int32_t GetMaxWidth() = 0;
        virtual bool IsMultiLine() = 0;
        virtual void ClearText();
        virtual uint32_t GetTextColor(int32_t index);
        virtual void SetText(const std::wstring& t, uint32_t cl);
        virtual void AppendText(const std::wstring& t, uint32_t cl);
        virtual void SetTextColor(uint32_t start, uint32_t end, uint32_t cl);
        virtual void EvaluateSize(const std::wstring& t = L"");
        virtual void UpdateTextVertex();
        virtual void DrawText();
        virtual std::wstring GetText() { return text; }
        
    protected:
        bool text_update = true;
        uint16_t mem_size = 0;
        uint32_t tbo = 0;
        uint32_t tao = 0;
        base::Font* font = nullptr;
        uint32_t spacing_x = 1;
        uint32_t spacing_y = 1;
        uint32_t vert_size = 0;
        int32_t text_width = 0;
        int32_t text_height = 0;
        int32_t text_width_cur = 0;
        std::wstring text;
        std::vector<uint32_t> color_vec;
        std::vector<v2i> text_pos_array;
    };
    
    class SGSpriteBase {
    public:
        SGSpriteBase();
        virtual ~SGSpriteBase();
        virtual void SetImage(base::Texture* img, recti varea, uint32_t cl = 0xffffffff);
        virtual void AddTexRect(recti tarea);
        virtual void SetImage(base::Texture* img, std::vector<v2i>& verts, std::vector<uint32_t>& cl);
        virtual void AddTexcoord(std::vector<v2f>& texcoords);
        virtual v2i GetImageOffset() = 0;
        void SetFrameTime(float ft) { frame_time = ft; }
        virtual void UpdateImage();
        virtual void DrawImage();
        
    protected:
        uint32_t imgbo = 0;
        uint32_t imgao = 0;
        bool img_update = true;
        bool img_dirty = true;
        double frame_time = 0.016;
        base::Texture* img_texture = nullptr;
        v2i img_offset = {0, 0};
        std::vector<v2i> verts;
        std::vector<uint32_t> colors;
        std::vector<std::vector<v2f>> texcoords;
    };
    
    class SGWidgetContainer : public SGWidget {
        
    public:
        virtual ~SGWidgetContainer();
        virtual void PostResize(bool res, bool rep);
        virtual v2i GetClientPosition() { return position_abs; }
        virtual v2i GetClientSize() { return size_abs; }
        
        virtual std::shared_ptr<SGWidget> GetHovingWidget(v2i pos);
        virtual void AddChild(std::shared_ptr<SGWidget> chd);
        virtual void RemoveChild(std::shared_ptr<SGWidget> chd);
        virtual void BringToTop(std::shared_ptr<SGWidget> chd);
        virtual void ClearChild() { children.clear(); }
        virtual void SetFocusWidget(std::shared_ptr<SGWidget> chd);
        
    public:
        virtual bool EventMouseMove(MouseMoveEvent evt);
        virtual bool EventMouseEnter();
        virtual bool EventMouseLeave();
        virtual bool EventMouseButtonDown(MouseButtonEvent evt);
        virtual bool EventMouseButtonUp(MouseButtonEvent evt);
        virtual bool EventMouseWheel(MouseWheelEvent evt);
        virtual bool EventKeyDown(KeyEvent evt);
        virtual bool EventKeyUp(KeyEvent evt);
        virtual bool EventCharEnter(TextEvent evt);
        virtual bool EventLostFocus();
        
    protected:
        std::weak_ptr<SGWidget> hoving;
        std::weak_ptr<SGWidget> focus_widget;
        std::vector<std::shared_ptr<SGWidget>> children;
    };
    
    struct SGConfig {
        std::unordered_map<std::string, int32_t> int_config;
        std::unordered_map<std::string, std::string> string_config;
        std::unordered_map<std::string, recti> tex_config;
    };
    
    class SGWidgetWrapper : public SGWidgetContainer {
    public:
        virtual bool EventMouseMove(MouseMoveEvent evt);
        virtual bool EventMouseEnter();
        virtual bool EventMouseLeave();
        virtual bool EventMouseButtonDown(MouseButtonEvent evt);
        virtual bool EventMouseButtonUp(MouseButtonEvent evt);
        virtual bool EventMouseWheel(MouseWheelEvent evt);
        virtual bool EventKeyDown(KeyEvent evt);
        virtual bool EventKeyUp(KeyEvent evt);
        virtual bool EventCharEnter(TextEvent evt);
        virtual bool EventLostFocus();
    };
    
    class SGGUIRoot : public SGWidgetContainer {
    public:
        SGGUIRoot() {}
        virtual ~SGGUIRoot();
        SGGUIRoot(const SGGUIRoot&) = delete;
        
        void SetSceneSize(v2i size);
        v2i GetSceneSize();
        
        template<typename T>
        void ConvertXY(int32_t x, int32_t y, T& v) {
            v.x = (float)x / scene_size.x * 2.0f - 1.0f;
            v.y = 1.0f - (float)y / scene_size.y * 2.0f;
        }
        template<typename T>
        void TexUV(int32_t u, int32_t v, T& t) {
            t.x = (float)u / tex_size.x;
            t.y = (float)v / tex_size.y;
        }
        template<typename T>
        void SetRectVertex(T* v, int32_t x, int32_t y, int32_t w, int32_t h, recti uv) {
            v[0].vertex.x = (float)x / scene_size.x * 2.0f - 1.0f;
            v[0].vertex.y = 1.0f - (float)y / scene_size.y * 2.0f;
            v[1].vertex.x = (float)(x + w) / scene_size.x * 2.0f - 1.0f;
            v[1].vertex.y = 1.0f - (float)y / scene_size.y * 2.0f;
            v[2].vertex.x = (float)x / scene_size.x * 2.0f - 1.0f;
            v[2].vertex.y = 1.0f - (float)(y + h) / scene_size.y * 2.0f;
            v[3].vertex.x = (float)(x + w) / scene_size.x * 2.0f - 1.0f;
            v[3].vertex.y = 1.0f - (float)(y + h) / scene_size.y * 2.0f;
            v[0].texcoord.x = (float)uv.left / tex_size.x;
            v[0].texcoord.y = (float)uv.top / tex_size.y;
            v[1].texcoord.x = (float)(uv.left + uv.width) / tex_size.x;
            v[1].texcoord.y = (float)uv.top / tex_size.y;
            v[2].texcoord.x = (float)uv.left / tex_size.x;
            v[2].texcoord.y = (float)(uv.top + uv.height) / tex_size.y;
            v[3].texcoord.x = (float)(uv.left + uv.width) / tex_size.x;
            v[3].texcoord.y = (float)(uv.top + uv.height) / tex_size.y;
        }
        
        void BindGuiTexture() {
            if(cur_texture != &gui_texture) {
                gui_texture.Bind();
                cur_texture = &gui_texture;
            }
        }
        
        void BindTexture(base::Texture* t = nullptr) {
            if(t != cur_texture) {
                t->Bind();
                cur_texture = t;
            }
        }
        
        void BeginScissor(recti rt);
        void EndScissor();
        
        virtual bool CheckInside(v2i pos);
        virtual std::shared_ptr<SGWidget> GetHovingWidget(v2i pos);
        virtual void UpdateVertices();
        virtual void Draw();
        virtual void Destroy() {}
        void ObjectDragingBegin(std::shared_ptr<SGWidget> d, MouseMoveEvent evt);
        void ObjectDragingEnd(MouseMoveEvent evt);
        void SetClickingObject(std::shared_ptr<SGWidget> ptr) { clicking_object = ptr; }
        std::shared_ptr<SGWidget> GetClickObject() { return clicking_object.lock(); }
        void SetPopupObject(std::shared_ptr<SGWidget> ptr) { popup_objects.push_back(ptr); }
        bool LoadConfigs(const std::wstring& gui_conf);
        void Unload();
        void AddConfig(const std::string& wtype, SGConfig& conf) { configs[wtype] = &conf; }
        base::Font& GetGUIFont(const std::string& name) { return font_mgr[name]; }
        base::Texture& GetGUITexture() { return gui_texture; }
        uint32_t GetDefaultInt(const std::string& key) { return basic_config.int_config[key]; }
        recti& GetDefaultRect(const std::string& key) { return basic_config.tex_config[key]; }
        void CheckMouseMove();
        double GetTime();
        uint32_t GetIndexBuffer() { return index_buffer; }
        
        bool InjectMouseMoveEvent(MouseMoveEvent evt);
        bool InjectMouseEnterEvent();
        bool InjectMouseLeaveEvent();
        bool InjectMouseButtonDownEvent(MouseButtonEvent evt);
        bool InjectMouseButtonUpEvent(MouseButtonEvent evt);
        bool InjectMouseWheelEvent(MouseWheelEvent evt);
        bool InjectKeyDownEvent(KeyEvent evt);
        bool InjectKeyUpEvent(KeyEvent evt);
        bool InjectCharEvent(TextEvent evt);
        
        static SGGUIRoot& GetSingleton();
        
    protected:
        bool inside_scene = false;
        v2i scene_size = {0, 0};
        v2i tex_size = {0, 0};
        v2i mouse_pos = {0, 0};
        std::weak_ptr<SGWidget> draging_object;
        std::weak_ptr<SGWidget> clicking_object;
        std::list<std::weak_ptr<SGWidget>> popup_objects;
        base::Texture gui_texture;
        base::Texture* cur_texture = nullptr;
        std::unordered_map<std::string, SGConfig*> configs;
        std::list<recti> scissor_stack;
        uint64_t start_time = 0;
        uint32_t index_buffer = 0;
        std::unordered_map<std::string, base::Font> font_mgr;
        base::Shader* gui_shader;
        
    public:
        static SGConfig basic_config;
    };
    
    class SGPanel : public SGWidgetWrapper {
    public:
        virtual ~SGPanel();
        virtual void UpdateVertices();
        virtual void Draw();
        
    public:
        static std::shared_ptr<SGPanel> Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, v2i size);
        static SGConfig panel_config;
    };
    
    class SGWindow : public SGWidgetWrapper, public SGTextBase {
    public:
        virtual ~SGWindow();
        virtual void UpdateVertices();
        virtual void Draw();
        virtual std::shared_ptr<SGWidget> GetDragingTarget() { return shared_from_this(); }
        virtual v2i GetTextOffset();
        virtual int32_t GetMaxWidth();
        virtual bool IsMultiLine();
        std::shared_ptr<SGWidget> GetCloseButton() { return children[0]; }
        void SetTitle(const std::wstring& t);
        
        SGEventHandler<SGWidget> onClosing;
        
    protected:
        virtual bool DragingBegin(v2i evt);
        virtual bool DragingUpdate(v2i evt);
        bool CloseButtonClick(SGWidget& sender);
        uint32_t drag_type = 0;
        v2i drag_diff = {0, 0};
        
    public:
        static std::shared_ptr<SGWindow> Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, v2i size);
        static SGConfig window_config;
    };

    class SGLabel : public SGWidget, public SGTextBase {
    public:
        virtual ~SGLabel();
        virtual void PostResize(bool res, bool rep);
        virtual bool AllowFocus() const { return false; }
        virtual std::shared_ptr<SGWidget> GetDragingTarget() { return parent.lock(); }
        virtual void UpdateVertices();
        virtual void Draw();
        virtual void SetFont(base::Font* ft);
        virtual void SetSpacing(uint32_t x, uint32_t y);
        virtual v2i GetTextOffset();
        virtual int32_t GetMaxWidth();
        virtual bool IsMultiLine();
        virtual void ClearText();
        virtual void SetText(const std::wstring& t, uint32_t cl);
        virtual void AppendText(const std::wstring& t, uint32_t cl);
        
    protected:
        uint32_t max_width = 0;
        
    public:
        static std::shared_ptr<SGLabel> Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, const std::wstring& t, int32_t mw = 0xffff);
        static SGConfig label_config;
    };

    class SGIconLabel : public SGLabel {
    public:
        virtual ~SGIconLabel();
        virtual void EvaluateSize(const std::wstring& t = L"");
        virtual void UpdateTextVertex();
        virtual void Draw();
        
    protected:
        int32_t icon_size = 0;
        int32_t icon_mem_size = 0;
        
    public:
        static std::shared_ptr<SGIconLabel> Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, const std::wstring& t, int32_t mw = 0xffff);
        static SGConfig iconlabel_config;
    };
    
    class SGSprite : public SGWidget, public SGSpriteBase {
    public:
        virtual ~SGSprite();
        virtual void UpdateVertices();
        virtual void Draw();
        virtual v2i GetImageOffset();
        
    public:
        static std::shared_ptr<SGSprite> Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, v2i size);
        static SGConfig sprite_config;
    };

    class SGButton : public SGWidget, public SGSpriteBase, public SGTextBase {
    public:
        virtual ~SGButton();
        virtual void UpdateVertices();
        virtual void Draw();
        virtual v2i GetTextOffset();
        virtual int32_t GetMaxWidth();
        virtual bool IsMultiLine();
        virtual v2i GetImageOffset();
        void SetTextureRect(recti r1, recti r2, recti r3);
        void SetTexture(base::Texture* tex);
        bool IsPushed() { return is_push && (state == 0x12); }
        
        SGEventHandler<SGWidget> eventButtonClick;
        
    protected:
        virtual bool EventMouseEnter();
        virtual bool EventMouseLeave();
        virtual bool EventMouseButtonDown(MouseButtonEvent evt);
        virtual bool EventMouseButtonUp(MouseButtonEvent evt);
        
        bool is_push = false;
        uint32_t state = 0;
        int32_t lwidth = 0;
        int32_t rwidth = 0;
        recti tex_rect[3] = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        base::Texture* tex_texture = nullptr;
        
    public:
        static std::shared_ptr<SGButton> Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, v2i size, bool is_push = false);
        static SGConfig button_config;
    };
    
    class SGCheckbox : public SGWidget, public SGTextBase {
    public:
        virtual ~SGCheckbox();
        virtual void PostResize(bool res, bool rep);
        virtual void UpdateVertices();
        virtual void Draw();
        virtual void SetFont(base::Font* ft);
        virtual void SetSpacing(uint32_t x, uint32_t y);
        virtual v2i GetTextOffset();
        virtual int32_t GetMaxWidth();
        virtual bool IsMultiLine();
        virtual void ClearText();
        virtual void SetText(const std::wstring& t, uint32_t cl);
        virtual void AppendText(const std::wstring& t, uint32_t cl);
        
        virtual void SetChecked(bool chk);
        virtual bool IsChecked() { return checked; }
        SGEventHandler<SGWidget, bool> eventCheckChange;
        
    protected:
        virtual bool EventMouseEnter();
        virtual bool EventMouseLeave();
        virtual bool EventMouseButtonDown(MouseButtonEvent evt);
        virtual bool EventMouseButtonUp(MouseButtonEvent evt);
        
        uint32_t state = 0;
        bool checked = false;
    
    public:
        static std::shared_ptr<SGCheckbox> Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, const std::wstring& t);
        static SGConfig checkbox_config;
    };

    class SGRadio : public SGCheckbox {
    public:
        virtual ~SGRadio();
        virtual void UpdateVertices();
        virtual void SetChecked(bool chk);
        void AttachGroup(std::shared_ptr<SGRadio> rdo);
        void DetachGroup();
        std::shared_ptr<SGRadio> GetCheckedTarget();
        
    protected:
        virtual bool EventMouseButtonUp(MouseButtonEvent evt);
        SGRadio* next_group_member;
        
    public:
        static std::shared_ptr<SGRadio> Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, const std::wstring& t);
        static SGConfig radio_config;
    };

    class SGScrollBar : public SGWidget {
    public:
        virtual ~SGScrollBar();
        virtual void PostResize(bool res, bool rep);
        virtual std::shared_ptr<SGWidget> GetDragingTarget() { return shared_from_this(); }
        virtual void UpdateVertices();
        virtual void Draw();
        void SetRange(float minv, float maxv, float cur);
        void SetValue(float cur);
        void SetSliderLength(int32_t length);
        
        SGEventHandler<SGWidget, float> eventValueChange;
        
    protected:
        virtual bool EventMouseMove(MouseMoveEvent evt);
        virtual bool EventMouseEnter();
        virtual bool EventMouseLeave();
        virtual bool EventMouseWheel(MouseWheelEvent evt);
        virtual bool DragingBegin(v2i evt);
        virtual bool DragingUpdate(v2i evt);
        virtual bool DragingEnd(v2i evt);
        
        float minvalue = 0.0f;
        float maxvalue = 100.0f;
        int32_t current_pos = 0;
        int32_t pos_min = 0;
        int32_t pos_max = 0;
        int32_t slider_diff = 0;
        int32_t slider_length = 0;
        bool slider_hoving = false;
        bool slider_moving = false;
        bool is_horizontal = true;
        
    public:
        static std::shared_ptr<SGScrollBar> Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, v2i size, bool is_h);
        static SGConfig scroll_config;
    };

    class SGTextEdit : public SGWidgetWrapper, public SGTextBase {
    public:
        ~SGTextEdit();
        virtual void PostResize(bool res, bool rep);
        virtual std::shared_ptr<SGWidget> GetDragingTarget() { return shared_from_this(); }
        virtual void UpdateVertices();
        virtual void Draw();
        virtual v2i GetTextOffset();
        virtual int32_t GetMaxWidth();
        virtual bool IsMultiLine() { return false; };
        virtual uint32_t GetTextColor(int32_t index);
        void DrawSelectRegion();
        void UpdateSelRegion();
        void SetSelRegion(uint32_t start, uint32_t end);
        void CheckCursorPos();
        void CheckDragPos();
        void SetDefaultColor(uint32_t color) { def_color = color; }
        void SetReadOnly(bool ro);
        
        SGEventHandler<SGWidget> eventTextEnter;
        
    protected:

        virtual bool EventMouseEnter();
        virtual bool EventMouseLeave();
        virtual bool EventKeyDown(KeyEvent evt);
        virtual bool EventCharEnter(TextEvent evt);
        virtual bool EventGetFocus();
        virtual bool EventLostFocus();
        virtual bool DragingBegin(v2i evt);
        virtual bool DragingUpdate(v2i evt);
        virtual bool DragingEnd(v2i evt);
        
        uint32_t GetHitIndex(v2i pos);
        
        uint32_t def_color = 0xff000000;
        uint32_t sel_color = 0xffffffff;
        uint32_t sel_bcolor = 0xff000000;
        bool hoving = false;
        bool focus = false;
        bool draging = false;
        bool read_only = false;
        int32_t text_offset = 0;
        int32_t drag_check = 0;
        recti text_area = {0, 0, 0, 0};
        double cursor_time = 0.0;
        uint32_t cursor_pos = 0;
        bool sel_change = true;
        uint32_t sel_start = 0;
        uint32_t sel_end = 0;
        
    public:
        static std::shared_ptr<SGTextEdit> Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, v2i size);
        static SGConfig textedit_config;
    };

    class SGListBox : public SGWidgetWrapper, public SGTextBase {
    public:
        ~SGListBox();
        virtual void UpdateVertices();
        virtual void Draw();
        virtual v2i GetTextOffset();
        virtual int32_t GetMaxWidth() { return 0xffff; }
        virtual bool IsMultiLine() { return true; };
        virtual void UpdateTextVertex();
        void InsertItem(uint32_t index, uint16_t icon, const std::wstring& item, uint32_t color, int32_t val = 0);
        void AddItem(uint16_t icon, const std::wstring& item, uint32_t color, int32_t val = 0);
        void RemoveItem(uint32_t index);
        void ClearItem();
        void SetItemIcon(uint32_t index, uint16_t icon);
        void SetItemText(uint32_t index, const std::wstring& text, uint32_t color);
        void SetItemValue(uint32_t index, int32_t val);
        const std::tuple<uint16_t, std::wstring, uint32_t, int32_t>& GetItem(uint32_t index);
        void SetSelection(int32_t sel);
        int32_t GetSeletion();
        int32_t GetItemCount() { return (int32_t)items.size(); }
        int32_t GetHoverItem(int32_t offsetx, int32_t offsety);
        
        SGEventHandler<SGWidget, int32_t> eventSelChange;
        SGEventHandler<SGWidget, int32_t> eventDoubleClick;
        
    protected:
        
        virtual bool EventMouseEnter();
        virtual bool EventMouseLeave();
        virtual bool EventMouseButtonDown(MouseButtonEvent evt);
        virtual bool EventMouseWheel(MouseWheelEvent evt);
        bool ScrollBarChange(SGWidget& sender, float value);
        
        bool is_hoving = false;
        uint32_t sel_color = 0xffffffff;
        uint32_t sel_bcolor = 0xff000000;
        uint32_t color1 = 0xffffffff;
        uint32_t color2 = 0xffeeeeee;
        int32_t current_sel = -1;
        int32_t line_spacing = 0;
        int32_t text_offset = 0;
        int32_t item_count = 0;
        int32_t max_item_count = 0;
        recti text_area = {0, 0, 0, 0};
        double click_time = 0.0;
        std::vector<std::tuple<uint16_t, std::wstring, uint32_t, int32_t>> items;
        
    public:
        static std::shared_ptr<SGListBox> Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, v2i size);
        static SGConfig listbox_config;
    };
    
    class SGComboBox : public SGWidget, public SGTextBase {
    public:
        ~SGComboBox();
        virtual void UpdateVertices();
        virtual void Draw();
        virtual void PostResize(bool res, bool rep);
        virtual v2i GetTextOffset();
        virtual int32_t GetMaxWidth() { return 0xffff; }
        virtual bool IsMultiLine() { return true; };
        void InsertItem(uint32_t index, const std::wstring& item, uint32_t color, int32_t val = 0);
        void AddItem(const std::wstring& item, uint32_t color, int32_t val = 0);
        void RemoveItem(uint32_t index);
        void ClearItem();
        void SetItemText(uint32_t index, const std::wstring& text, uint32_t color);
        void SetItemValue(uint32_t index, int32_t val);
        void SetSelection(int32_t sel);
        int32_t GetSelection();
        int32_t GetSelectedValue();
        void ShowList(bool show);
        
        SGEventHandler<SGWidget, int32_t> eventSelChange;
        
    protected:
        virtual bool EventMouseEnter();
        virtual bool EventMouseLeave();
        virtual bool EventMouseButtonDown(MouseButtonEvent evt);
        
        bool show_item = false;
        bool is_hoving = false;
        int32_t current_sel = -1;
        recti text_area = {0, 0, 0, 0};
        std::vector<std::tuple<std::wstring, uint32_t, int32_t>> items;
        
    public:
        static std::shared_ptr<SGComboBox> Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, v2i size);
        static SGConfig combobox_config;
    };
    
    class SGTabControl : public SGWidgetWrapper , public SGTextBase {
    public:
        ~SGTabControl();
        virtual v2i GetClientPosition();
        virtual v2i GetClientSize();
        virtual void UpdateVertices();
        virtual void Draw();
        virtual void PostResize(bool res, bool rep);
        virtual v2i GetTextOffset();
        virtual int32_t GetMaxWidth() { return 0xffff; }
        virtual bool IsMultiLine() { return false; }
        virtual void EvaluateSize(const std::wstring& t = L"");
        virtual void UpdateTextVertex();;
        void AddTab(const std::wstring& title, uint32_t color);
        void RemoveTab(uint32_t index);
        void SetTabTitle(uint32_t index, const std::wstring& title, uint32_t color);
        std::shared_ptr<SGWidgetContainer> GetTab(int32_t index);
        void SetActiveTab(int32_t index);
        int32_t GetTabCount();
        int32_t GetActiveTab();
        
    protected:
        virtual bool EventMouseMove(MouseMoveEvent evt);
        virtual bool EventMouseEnter();
        virtual bool EventMouseLeave();
        virtual bool EventMouseButtonDown(MouseButtonEvent evt);
        virtual bool EventMouseButtonUp(MouseButtonEvent evt);
        virtual bool EventMouseWheel(MouseWheelEvent evt);
        virtual bool EventKeyDown(KeyEvent evt);
        virtual bool EventKeyUp(KeyEvent evt);
        virtual bool EventCharEnter(TextEvent evt);
        
        int32_t GetHovingTab(v2i pos);
        
        std::weak_ptr<SGWidget> active_tab;
        std::weak_ptr<SGWidget> hover_tab;
        int32_t max_item_count = 0;
        int32_t item_count = 0;
        int32_t tab_height = 0;
        int32_t tab_ol = 0;
        int32_t tab_or = 0;
        bool in_tab = false;
        bool size_dirty = true;
        
    public:
        static std::shared_ptr<SGTabControl> Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, v2i size);
        static SGConfig tab_config;
    };
    
    // ===== GUI Components End =====
    
}

#endif

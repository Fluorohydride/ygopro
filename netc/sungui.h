#ifndef _SUNGUI_H_
#define _SUNGUI_H_

#include "glbase.h"
#include <GLFW/glfw3.h>

namespace sgui
{

    struct MouseMoveEvent {
        int x;
        int y;
    };
    
    struct MouseButtonEvent {
        int button;
        int mods;
        int x;
        int y;
    };
    
    struct MouseWheelEvent {
        float deltax;
        float deltay;
    };
    
    struct KeyEvent {
        int key;
        int mods;
    };
    
    struct TextEvent {
        unsigned int unichar;
    };
    
    // ===== Delegate Implement =====
    // OT: Object Type
    // ST: Sender Type
    // ET: Event Type
    // MT: Method Type
    // FT: Lambda Type
    
    template<typename ST, typename ET = void>
    class SGDelegate {
    public:
        virtual bool Invoke(ST& sender, ET evt) = 0;
    };
    
    template<typename ST>
    class SGDelegate<ST, void> {
    public:
        virtual bool Invoke(ST& sender) = 0;
    };
    
    template<typename OT, typename ST, typename ET = void>
    class SGDelegateObject : public SGDelegate<ST, ET> {
        typedef bool (OT::*MT)(ST&, ET);
        
    public:
        SGDelegateObject(OT* obj, MT fun): invoke_object(obj), invoke_method(fun) {}
        virtual bool Invoke(ST& sender, ET evt) {
            return (invoke_object->*invoke_method)(sender, evt);
        }
        
    protected:
        OT* invoke_object = nullptr;
        MT invoke_method = nullptr;
    };
    
    template<typename OT, typename ST>
    class SGDelegateObject<OT, ST, void> : public SGDelegate<ST, void> {
        typedef bool (OT::*MT)(ST&);
        
    public:
        SGDelegateObject(OT* obj, MT fun): invoke_object(obj), invoke_method(fun) {}
        virtual bool Invoke(ST& sender) {
            return (invoke_object->*invoke_method)(sender);
        }
        
    protected:
        OT* invoke_object = nullptr;
        MT invoke_method = nullptr;
    };
    
    template<typename ST, typename ET = void>
    class SGDelegateStdFunction : public SGDelegate<ST, ET> {
        
    public:
        SGDelegateStdFunction(const std::function<bool (ST&, ET)>& fun): invoke_method(fun) {}
        virtual bool Invoke(ST& sender, ET evt) {
            return invoke_method(sender, evt);
        }
        
    protected:
        std::function<bool (ST&, ET)> invoke_method = nullptr;
    };
    
    template<typename ST>
    class SGDelegateStdFunction<ST, void> : public SGDelegate<ST, void> {
        
    public:
        SGDelegateStdFunction(const std::function<bool (ST&)>& fun): invoke_method(fun) {}
        virtual bool Invoke(ST& sender) {
            return invoke_method(sender);
        }
        
    protected:
        std::function<bool (ST&)> invoke_method = nullptr;
    };
    
    template<typename ST, typename ET = void>
    class SGEventHandler {
    public:
        template<typename OT>
        void Bind(OT* obj, bool (OT::*fun)(ST&, ET)) {
            delegate_ptrs.push_back(std::make_shared<SGDelegateObject<OT, ST, ET>>(obj, fun));
        }
        
        template<typename FT>
        void Bind(FT f) {
            auto ptr = std::make_shared<SGDelegateStdFunction<ST, ET>>(std::function<bool (ST&, ET)>(f));
            delegate_ptrs.push_back(ptr);
        }
        
        void Reset() {
            delegate_ptrs.clear();
        }
        
        bool TriggerEvent(ST& sender, ET evt) {
            if(delegate_ptrs.size() == 0)
                return false;
            bool ret = false;
            for(auto& ptr : delegate_ptrs)
                ret = ptr->Invoke(sender, evt) || ret;
            return ret;
        }
        
    protected:
        std::vector<std::shared_ptr<SGDelegate<ST, ET>>> delegate_ptrs;
    };
    
    template<typename ST>
    class SGEventHandler<ST, void> {
    public:
        template<typename OT>
        void Bind(OT* obj, bool (OT::*fun)(ST&)) {
            delegate_ptrs.push_back(std::make_shared<SGDelegateObject<OT, ST, void>>(obj, fun));
        }
        
        template<typename FT>
        void Bind(FT f) {
            auto ptr = std::make_shared<SGDelegateStdFunction<ST, void>>(std::function<bool (ST&)>(f));
            delegate_ptrs.push_back(ptr);
        }
        
        void Reset() {
            delegate_ptrs.clear();
        }
        
        bool TriggerEvent(ST& sender) {
            if(delegate_ptrs.size() == 0)
                return false;
            bool ret = false;
            for(auto& ptr : delegate_ptrs)
                ret = ptr->Invoke(sender) || ret;
            return ret;
        }
        
    protected:
        std::vector<std::shared_ptr<SGDelegate<ST, void>>> delegate_ptrs;
    };
    
    // ===== Delegate Implement End =====
    
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
        void SetX(int x, float propx = 0.0f);
        void SetY(int y, float propy = 0.0f);
        void SetWidth(int w, float propw = 0.0f);
        void SetHeight(int h, float proph = 0.0f);
        void SetColor(unsigned int c);
        v2i GetPosition() const { return position_abs; }
        v2i GetSize() const { return size_abs; }
        void SetVisible(bool v);
        bool IsVisible() const;
        void SetCustomValue(unsigned int v) { cvalue = v; }
        unsigned int GetCustomValue() const { return cvalue; }
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
        unsigned int cvalue = 0;
        unsigned int color = 0xffffffff;
        void* cobject = nullptr;
        std::weak_ptr<SGWidgetContainer> parent;
        bool vertices_dirty = true;
        unsigned int vbo = 0;
        unsigned int vao = 0;
    };
    
    class SGTextBase {
    public:
        SGTextBase();
        virtual ~SGTextBase();
        virtual void SetFont(glbase::Font* ft);
        virtual void SetSpacing(unsigned int x, unsigned int y);
        virtual int GetLineSpacing();
        virtual v2i GetTextOffset() = 0;
        virtual int GetMaxWidth() = 0;
        virtual bool IsMultiLine() = 0;
        virtual void ClearText();
        virtual unsigned int GetTextColor(int index);
        virtual void SetText(const std::wstring& t, unsigned int cl);
        virtual void AppendText(const std::wstring& t, unsigned int cl);
        virtual void SetTextColor(unsigned int start, unsigned int end, unsigned int cl);
        virtual void EvaluateSize(const std::wstring& t = L"");
        virtual void UpdateTextVertex();
        virtual void DrawText();
        virtual std::wstring GetText() { return text; }
        
    protected:
        bool text_update = true;
        unsigned short mem_size = 0;
        unsigned int tbo = 0;
        unsigned int tao = 0;
        glbase::Font* font = nullptr;
        unsigned int spacing_x = 1;
        unsigned int spacing_y = 1;
        unsigned int vert_size = 0;
        int text_width = 0;
        int text_height = 0;
        int text_width_cur = 0;
        std::wstring text;
        std::vector<unsigned int> color_vec;
        std::vector<v2i> text_pos_array;
    };
    
    class SGSpriteBase {
    public:
        SGSpriteBase();
        virtual ~SGSpriteBase();
        virtual void SetImage(glbase::Texture* img, recti varea, unsigned int cl = 0xffffffff);
        virtual void AddTexRect(recti tarea);
        virtual void SetImage(glbase::Texture* img, std::vector<v2i>& verts, std::vector<unsigned int>& cl);
        virtual void AddTexcoord(std::vector<v2f>& texcoords);
        virtual v2i GetImageOffset() = 0;
        void SetFrameTime(float ft) { frame_time = ft; }
        virtual void UpdateImage();
        virtual void DrawImage();
        
    protected:
        unsigned int imgbo = 0;
        unsigned int imgao = 0;
        bool img_update = true;
        bool img_dirty = true;
        double frame_time = 0.016;
        glbase::Texture* img_texture = nullptr;
        v2i img_offset = {0, 0};
        std::vector<v2i> verts;
        std::vector<unsigned int> colors;
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
        std::unordered_map<std::string, int> int_config;
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
        void ConvertXY(int x, int y, T& v) {
            v.x = (float)x / scene_size.x * 2.0f - 1.0f;
            v.y = 1.0f - (float)y / scene_size.y * 2.0f;
        }
        template<typename T>
        void TexUV(int u, int v, T& t) {
            t.x = (float)u / tex_size.x;
            t.y = (float)v / tex_size.y;
        }
        template<typename T>
        void SetRectVertex(T* v, int x, int y, int w, int h, recti uv) {
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
        
        void BindTexture(glbase::Texture* t = nullptr) {
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
        glbase::Font& GetGUIFont(const std::string& name) { return font_mgr[name]; }
        glbase::Texture& GetGUITexture() { return gui_texture; }
        unsigned int GetDefaultInt(const std::string& key) { return basic_config.int_config[key]; }
        recti& GetDefaultRect(const std::string& key) { return basic_config.tex_config[key]; }
        void CheckMouseMove();
        double GetTime();
        unsigned int GetIndexBuffer() { return index_buffer; }
        
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
        glbase::Texture gui_texture;
        glbase::Texture* cur_texture = nullptr;
        std::unordered_map<std::string, SGConfig*> configs;
        std::list<recti> scissor_stack;
        unsigned long long start_time = 0;
        unsigned int index_buffer = 0;
        std::unordered_map<std::string, glbase::Font> font_mgr;
        glbase::Shader* gui_shader;
        
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
        virtual int GetMaxWidth();
        virtual bool IsMultiLine();
        std::shared_ptr<SGWidget> GetCloseButton() { return children[0]; }
        void SetTitle(const std::wstring& t);
        
        SGEventHandler<SGWidget> onClosing;
        
    protected:
        virtual bool DragingBegin(v2i evt);
        virtual bool DragingUpdate(v2i evt);
        bool CloseButtonClick(SGWidget& sender);
        unsigned int drag_type = 0;
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
        virtual void SetFont(glbase::Font* ft);
        virtual void SetSpacing(unsigned int x, unsigned int y);
        virtual v2i GetTextOffset();
        virtual int GetMaxWidth();
        virtual bool IsMultiLine();
        virtual void ClearText();
        virtual void SetText(const std::wstring& t, unsigned int cl);
        virtual void AppendText(const std::wstring& t, unsigned int cl);
        
    protected:
        unsigned int max_width = 0;
        
    public:
        static std::shared_ptr<SGLabel> Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, const std::wstring& t, int mw = 0xffff);
        static SGConfig label_config;
    };

    class SGIconLabel : public SGLabel {
    public:
        virtual ~SGIconLabel();
        virtual void EvaluateSize(const std::wstring& t = L"");
        virtual void UpdateTextVertex();
        virtual void Draw();
        
    protected:
        int icon_size = 0;
        int icon_mem_size = 0;
        
    public:
        static std::shared_ptr<SGIconLabel> Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, const std::wstring& t, int mw = 0xffff);
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
        virtual int GetMaxWidth();
        virtual bool IsMultiLine();
        virtual v2i GetImageOffset();
        void SetTextureRect(recti r1, recti r2, recti r3);
        void SetTexture(glbase::Texture* tex);
        bool IsPushed() { return is_push && (state == 0x12); }
        
        SGEventHandler<SGWidget> eventButtonClick;
        
    protected:
        virtual bool EventMouseEnter();
        virtual bool EventMouseLeave();
        virtual bool EventMouseButtonDown(MouseButtonEvent evt);
        virtual bool EventMouseButtonUp(MouseButtonEvent evt);
        
        bool is_push = false;
        unsigned int state = 0;
        int lwidth = 0;
        int rwidth = 0;
        recti tex_rect[3] = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        glbase::Texture* tex_texture = nullptr;
        
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
        virtual void SetFont(glbase::Font* ft);
        virtual void SetSpacing(unsigned int x, unsigned int y);
        virtual v2i GetTextOffset();
        virtual int GetMaxWidth();
        virtual bool IsMultiLine();
        virtual void ClearText();
        virtual void SetText(const std::wstring& t, unsigned int cl);
        virtual void AppendText(const std::wstring& t, unsigned int cl);
        
        virtual void SetChecked(bool chk);
        virtual bool IsChecked() { return checked; }
        SGEventHandler<SGWidget, bool> eventCheckChange;
        
    protected:
        virtual bool EventMouseEnter();
        virtual bool EventMouseLeave();
        virtual bool EventMouseButtonDown(MouseButtonEvent evt);
        virtual bool EventMouseButtonUp(MouseButtonEvent evt);
        
        unsigned int state = 0;
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
        void SetSliderLength(int length);
        
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
        int current_pos = 0;
        int pos_min = 0;
        int pos_max = 0;
        int slider_diff = 0;
        int slider_length = 0;
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
        virtual int GetMaxWidth();
        virtual bool IsMultiLine() { return false; };
        virtual unsigned int GetTextColor(int index);
        void DrawSelectRegion();
        void UpdateSelRegion();
        void SetSelRegion(unsigned int start, unsigned int end);
        void CheckCursorPos();
        void CheckDragPos();
        void SetDefaultColor(unsigned int color) { def_color = color; }
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
        
        unsigned int GetHitIndex(v2i pos);
        
        unsigned int def_color = 0xff000000;
        unsigned int sel_color = 0xffffffff;
        unsigned int sel_bcolor = 0xff000000;
        bool hoving = false;
        bool focus = false;
        bool draging = false;
        bool read_only = false;
        int text_offset = 0;
        int drag_check = 0;
        recti text_area = {0, 0, 0, 0};
        double cursor_time = 0.0;
        unsigned int cursor_pos = 0;
        bool sel_change = true;
        unsigned int sel_start = 0;
        unsigned int sel_end = 0;
        
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
        virtual int GetMaxWidth() { return 0xffff; }
        virtual bool IsMultiLine() { return true; };
        virtual void UpdateTextVertex();
        void InsertItem(unsigned int index, unsigned short icon, const std::wstring& item, unsigned int color, int val = 0);
        void AddItem(unsigned short icon, const std::wstring& item, unsigned int color, int val = 0);
        void RemoveItem(unsigned int index);
        void ClearItem();
        void SetItemIcon(unsigned int index, unsigned short icon);
        void SetItemText(unsigned int index, const std::wstring& text, unsigned int color);
        void SetItemValue(unsigned int index, int val);
        const std::tuple<unsigned short, std::wstring, unsigned int, int>& GetItem(unsigned int index);
        void SetSelection(int sel);
        int GetSeletion();
        int GetItemCount() { return (int)items.size(); }
        int GetHoverItem(int offsetx, int offsety);
        
        SGEventHandler<SGWidget, int> eventSelChange;
        SGEventHandler<SGWidget, int> eventDoubleClick;
        
    protected:
        
        virtual bool EventMouseEnter();
        virtual bool EventMouseLeave();
        virtual bool EventMouseButtonDown(MouseButtonEvent evt);
        virtual bool EventMouseWheel(MouseWheelEvent evt);
        bool ScrollBarChange(SGWidget& sender, float value);
        
        bool is_hoving = false;
        unsigned int sel_color = 0xffffffff;
        unsigned int sel_bcolor = 0xff000000;
        unsigned int color1 = 0xffffffff;
        unsigned int color2 = 0xffeeeeee;
        int current_sel = -1;
        int line_spacing = 0;
        int text_offset = 0;
        int item_count = 0;
        int max_item_count = 0;
        recti text_area = {0, 0, 0, 0};
        double click_time = 0.0;
        std::vector<std::tuple<unsigned short, std::wstring, unsigned int, int>> items;
        
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
        virtual int GetMaxWidth() { return 0xffff; }
        virtual bool IsMultiLine() { return true; };
        void InsertItem(unsigned int index, const std::wstring& item, unsigned int color, int val = 0);
        void AddItem(const std::wstring& item, unsigned int color, int val = 0);
        void RemoveItem(unsigned int index);
        void ClearItem();
        void SetItemText(unsigned int index, const std::wstring& text, unsigned int color);
        void SetItemValue(unsigned int index, int val);
        void SetSelection(int sel);
        int GetSelection();
        int GetSelectedValue();
        void ShowList(bool show);
        
        SGEventHandler<SGWidget, int> eventSelChange;
        
    protected:
        virtual bool EventMouseEnter();
        virtual bool EventMouseLeave();
        virtual bool EventMouseButtonDown(MouseButtonEvent evt);
        
        bool show_item = false;
        bool is_hoving = false;
        int current_sel = -1;
        recti text_area = {0, 0, 0, 0};
        std::vector<std::tuple<std::wstring, unsigned int, int>> items;
        
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
        virtual int GetMaxWidth() { return 0xffff; }
        virtual bool IsMultiLine() { return false; }
        virtual void EvaluateSize(const std::wstring& t = L"");
        virtual void UpdateTextVertex();;
        void AddTab(const std::wstring& title, unsigned int color);
        void RemoveTab(unsigned int index);
        void SetTabTitle(unsigned int index, const std::wstring& title, unsigned int color);
        std::shared_ptr<SGWidgetContainer> GetTab(int index);
        void SetActiveTab(int index);
        int GetTabCount();
        int GetActiveTab();
        
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
        
        int GetHovingTab(v2i pos);
        
        std::weak_ptr<SGWidget> active_tab;
        std::weak_ptr<SGWidget> hover_tab;
        int max_item_count = 0;
        int item_count = 0;
        int tab_height = 0;
        int tab_ol = 0;
        int tab_or = 0;
        bool in_tab = false;
        bool size_dirty = true;
        
    public:
        static std::shared_ptr<SGTabControl> Create(std::shared_ptr<SGWidgetContainer> p, v2i pos, v2i size);
        static SGConfig tab_config;
    };
    
    // ===== GUI Components End =====
    
}

#endif

#ifndef _RENDER_UTIL_H_
#define _RENDER_UTIL_H_

#include "utils/clock.h"

#include "render_base.h"

namespace base
{
    enum class BlendMode {
        Normal = 0,
        FrameBuffer = 1,
        DrawFrameBuffer = 2,
    };
    
    class IRenderer {
    public:
        virtual bool PrepareRender() = 0;
        virtual void Render() = 0;
        inline void SetBlendMode(BlendMode bm) { blend_mode = bm; }
        
    protected:
        BlendMode blend_mode = BlendMode::Normal;
    };
    
    template<typename VTYPE>
    class RenderObject;
    
    template<typename VTYPE>
    class RenderUnit {
        friend class RenderObject<VTYPE>;
    public:
        virtual ~RenderUnit() {}
        virtual void PushVertices();
        virtual void UpdateVertices();
        virtual void Update(int16_t vidx, int16_t iidx);
        virtual void ShowPushInfo() {}
        virtual void ShowUpdateInfo() {}
        
        inline void SetUpdate();
        inline void SetRedraw(bool up);
        inline void SyncVersion();
        inline bool CheckUpdateVersion();
        
        virtual bool CheckAvailable() = 0;
        virtual int32_t GetPrimitiveType() = 0;
        virtual int32_t GetTextureId() = 0;
        virtual void RefreshVertices() = 0;
        
    protected:
        bool need_update = true;
        int16_t vert_index = 0;
        int16_t index_index = 0;
        int32_t render_version = 0;
        std::vector<VTYPE> vertices;
        std::vector<int16_t> indices;
        RenderObject<VTYPE>* manager = nullptr;
    };
    
    template<typename VTYPE>
    class IRenderState {
    public:
        virtual ~IRenderState() {
            if(vbo_id)
                glDeleteBuffers(1, &vbo_id);
            if(vbo_idx)
                glDeleteBuffers(1, &vbo_idx);
            if(vao_id)
                glDeleteVertexArrays(1, &vao_id);
        }
        
        void InitGLState(bool with_vao) {
            glGenBuffers(1, &vbo_id);
            glGenBuffers(1, &vbo_idx);
            if(with_vao)
                vao_id = VTYPE::InitVao(vbo_id, vbo_idx);
        }
        
        inline void SetShader(base::Shader* shader) {
            render_shader = shader;
            if(shader_setting_callback == nullptr)
                shader_setting_callback = shader->GetDefaultParamCallback();
        }
        inline base::Shader* GetShader() { return render_shader; }
        inline void SetShaderSettingCallback(std::function<void(base::Shader*)> cb) { shader_setting_callback = cb; }
        
        std::vector<recti> scissor_stack;
        std::vector<float> global_alpha;
        uint32_t vbo_id = 0;
        uint32_t vbo_idx = 0;
        uint32_t vao_id = 0;
        base::Shader* render_shader = nullptr;
        std::function<void(base::Shader*)> shader_setting_callback;
    };
    
    template<typename VTYPE>
    struct RenderCmd {
        virtual ~RenderCmd() {}
        virtual bool CheckPrimitive(int16_t pri_type, int16_t texid) { return false; }
        virtual void Execute(IRenderState<VTYPE>& state) = 0;
    };
    
    template<typename VTYPE>
    struct RenderCmdDraw : public RenderCmd<VTYPE> {
        RenderCmdDraw(int16_t pri_type, int16_t texid, int16_t sindex) {
            primitive_type = pri_type;
            texture_id = texid;
            start_index = sindex;
            count = 0;
        }
        virtual bool CheckPrimitive(int16_t pri_type, int16_t texid) {
            return (primitive_type == pri_type) && (texture_id == texid);
        }
        virtual void Execute(IRenderState<VTYPE>& state) {
            if(count > 0) {
                glBindTexture(GL_TEXTURE_2D, texture_id);
                glDrawElements(primitive_type, count, GL_UNSIGNED_SHORT, (const void*)(intptr_t)(start_index * sizeof(int16_t)));
            }
        }
        int16_t primitive_type;
        int16_t texture_id;
        int16_t start_index;
        int16_t count;
    };

    template<typename VTYPE>
    struct RenderCmdBeginScissor : public RenderCmd<VTYPE> {
        RenderCmdBeginScissor(recti rect) : scissor_rect(rect) {}
        virtual void Execute(IRenderState<VTYPE>& state) {
            if(state.scissor_stack.size() == 0)
                glEnable(GL_SCISSOR_TEST);
            else {
                auto prt = state.scissor_stack.back();
                if(scissor_rect.left < prt.left)
                    scissor_rect.left = prt.left;
                if(scissor_rect.top < prt.top)
                    scissor_rect.top = prt.top;
                if(scissor_rect.left + scissor_rect.width > prt.left + prt.width)
                    scissor_rect.width = prt.left + prt.width - scissor_rect.left;
                if(scissor_rect.width < 0)
                    scissor_rect.width = 0;
                if(scissor_rect.top + scissor_rect.height > prt.top + prt.height)
                    scissor_rect.height = prt.top + prt.height - scissor_rect.top;
                if(scissor_rect.height < 0)
                    scissor_rect.height = 0;
            }
            state.scissor_stack.push_back(scissor_rect);
            glScissor(scissor_rect.left, scissor_rect.top, scissor_rect.width, scissor_rect.height);
        }
        recti scissor_rect;
    };

    template<typename VTYPE>
    struct RenderCmdEndScissor  : public RenderCmd<VTYPE> {
        virtual void Execute(IRenderState<VTYPE>& state) {
            state.scissor_stack.pop_back();
            if(state.scissor_stack.size() == 0)
                glDisable(GL_SCISSOR_TEST);
            else {
                recti rt = state.scissor_stack.back();
                glScissor(rt.left, rt.top, rt.width, rt.height);
            }
        }
    };
    
    template<typename VTYPE>
    struct RenderCmdBeginGlobalAlpha : public RenderCmd<VTYPE> {
        RenderCmdBeginGlobalAlpha(base::Shader* s, float alpha) : shader(s), global_alpha(alpha) {}
        virtual void Execute(IRenderState<VTYPE>& state) {
            float alpha = state.global_alpha.empty() ? global_alpha : (state.global_alpha.back() * global_alpha);
            shader->SetParam1f("alpha", alpha);
            state.global_alpha.push_back(alpha);
        }
        base::Shader* shader = nullptr;
        float global_alpha;
    };
    
    template<typename VTYPE>
    struct RenderCmdEndGlobalAlpha  : public RenderCmd<VTYPE> {
        RenderCmdEndGlobalAlpha(base::Shader* s) : shader(s) {}
        virtual void Execute(IRenderState<VTYPE>& state) {
            state.global_alpha.pop_back();
            float alpha = state.global_alpha.empty() ? 1.0f : state.global_alpha.back();
            shader->SetParam1f("alpha", alpha);
        }
        base::Shader* shader = nullptr;
    };
    
    template<typename VTYPE>
    class RenderObject : public IRenderer, public IRenderState<VTYPE> {
    public:
        RenderObject() {
            vertex_buffer.reserve(4096);
            index_buffer.reserve(6144);
        }
        
        virtual ~RenderObject() {
        }
        
        std::tuple<int16_t, int16_t> BeginPrimitive(int16_t pri_type, int16_t texid) {
            bool merge = false;
            if(!render_commands.empty() && render_commands.back()->CheckPrimitive(pri_type, texid))
                merge = true;
            if(!merge)
                render_commands.push_back(std::make_shared<RenderCmdDraw<VTYPE>>(pri_type, texid, (int16_t)index_buffer.size()));
            return std::make_tuple((int16_t)vertex_buffer.size(), (int16_t)index_buffer.size());
        }
        
        void PushVertices(const VTYPE* vert_data, const int16_t* index_data, int16_t vcount, int16_t icount) {
            if(vcount > 0)
                vertex_buffer.insert(vertex_buffer.end(), vert_data, vert_data + vcount);
            if(icount > 0)
                index_buffer.insert(index_buffer.end(), index_data, index_data + icount);
            std::static_pointer_cast<RenderCmdDraw<VTYPE>>(render_commands.back())->count += icount;
        }
        
        void BeginUpdate() {
            glBindBuffer(GL_ARRAY_BUFFER, IRenderState<VTYPE>::vbo_id);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IRenderState<VTYPE>::vbo_idx);
        }
        
        void UpdateVertices(const VTYPE* vert_data, int16_t start_index, int16_t count) {
            if(count > 0)
                glBufferSubData(GL_ARRAY_BUFFER, sizeof(VTYPE) * start_index, sizeof(VTYPE) * count, vert_data);
        }
        
        void UpdateIndices(const int16_t* index_data, int16_t start_index, int16_t count) {
            if(count > 0)
                glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int16_t) * start_index, sizeof(int16_t) * count, index_data);
        }
        
        virtual void PushVerticesAll() { ClearCommandVertices(); }
        
        void UploadVertices() {
            if(vertex_buffer.empty())
                return;
            glBindBuffer(GL_ARRAY_BUFFER, IRenderState<VTYPE>::vbo_id);
            if(vertex_buffer.size() > pre_vert_size) {
                glBufferData(GL_ARRAY_BUFFER, sizeof(VTYPE) * vertex_buffer.size(), &vertex_buffer[0], GL_DYNAMIC_DRAW);
                pre_vert_size = (int32_t)vertex_buffer.size();
            } else
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VTYPE) * vertex_buffer.size(), &vertex_buffer[0]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IRenderState<VTYPE>::vbo_idx);
            if(index_buffer.size() > pre_index_size) {
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int16_t) * index_buffer.size(), &index_buffer[0], GL_DYNAMIC_DRAW);
                pre_index_size = (int32_t)index_buffer.size();
            } else
                glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(int16_t) * index_buffer.size(), &index_buffer[0]);
        }
        
        template<template<typename> class CMD_TYPE, typename... TR>
        std::shared_ptr<CMD_TYPE<VTYPE>> PushCommand(TR... tr) {
            static_assert(std::is_base_of<RenderCmd<VTYPE>, CMD_TYPE<VTYPE>>::value, "command type should be subclass of RenderCmd.");
            auto ptr = std::make_shared<CMD_TYPE<VTYPE>>(std::forward<TR>(tr)...);
            render_commands.push_back(ptr);
            return ptr;
        }

        virtual bool PrepareRender() {
            if(need_redraw) {
                render_version++;
                PushVerticesAll();
                UploadVertices();
                ClearUpdatingUnits();
                return true;
            } else
                return UpdateUnits();
        }
        
        virtual void Render() {
            RenderSetting();
            GLCheckError(__FILE__, __LINE__);
            VTYPE::DrawBegin(IRenderState<VTYPE>::vbo_id, IRenderState<VTYPE>::vbo_idx, IRenderState<VTYPE>::vao_id);
            GLCheckError(__FILE__, __LINE__);
            for(auto& rc : render_commands)
                rc->Execute(*this);
            GLCheckError(__FILE__, __LINE__);
            VTYPE::DrawEnd(IRenderState<VTYPE>::vbo_id, IRenderState<VTYPE>::vbo_idx, IRenderState<VTYPE>::vao_id);
            GLCheckError(__FILE__, __LINE__);
            if(IRenderState<VTYPE>::render_shader)
                IRenderState<VTYPE>::render_shader->Unuse();
            GLCheckError(__FILE__, __LINE__);
            need_redraw = false;
            all_need_update = false;
        }
        
        virtual void RenderSetting() {
            glEnable(GL_BLEND);
            glDisable(GL_DEPTH_TEST);
            glFrontFace(GL_CW);
            SetBlendFunc(blend_mode);
            glActiveTexture(GL_TEXTURE0);
            if(IRenderState<VTYPE>::render_shader) {
                IRenderState<VTYPE>::render_shader->Use();
                if(IRenderState<VTYPE>::shader_setting_callback)
                    IRenderState<VTYPE>::shader_setting_callback(IRenderState<VTYPE>::render_shader);
            }
            GLCheckError(__FILE__, __LINE__);
        }
        
        void ClearCommandVertices() {
            vertex_buffer.clear();
            index_buffer.clear();
            render_commands.clear();
            update_units.clear();
        }
        
        template<typename ALLOC_TYPE, typename... TR>
        ALLOC_TYPE* NewObject(TR... tr) {
            static_assert(std::is_base_of<RenderUnit<VTYPE>, ALLOC_TYPE>::value, "object type should be subclass of RenderUnit.");
            auto ptr = std::make_shared<ALLOC_TYPE>(std::forward<TR>(tr)...);
            ptr->manager = this;
            all_units[ptr.get()] = ptr;
            return ptr.get();
        }
        
        template<typename ALLOC_TYPE, typename... TR>
        std::shared_ptr<ALLOC_TYPE> NewSharedObject(TR... tr) {
            static_assert(std::is_base_of<RenderUnit<VTYPE>, ALLOC_TYPE>::value, "object type should be subclass of RenderUnit.");
            auto ptr = std::make_shared<ALLOC_TYPE>(std::forward<TR>(tr)...);
            ptr->manager = this;
            all_units[ptr.get()] = ptr;
            return ptr;
        }
        
        template<typename ALLOC_TYPE>
        bool DeleteObject(ALLOC_TYPE* at) {
            static_assert(std::is_base_of<RenderUnit<VTYPE>, ALLOC_TYPE>::value, "object type should be subclass of RenderUnit.");
            auto iter = all_units.find(at);
            if(iter != all_units.end()) {
                update_units.erase(at);
                all_units.erase(iter);
                return true;
            }
            return false;
        }
        
        void ClearAllObjects() {
            all_units.clear();
            update_units.clear();
            need_redraw = true;
        }
        
        inline void AddUpdateUnit(RenderUnit<VTYPE>* unit) { if(!need_redraw) update_units.insert(unit); }
        inline bool UpdateUnits() {
            bool up = !update_units.empty();
            if(up) {
                BeginUpdate();
                for(auto& punit : update_units)
                    punit->UpdateVertices();
                update_units.clear();
            }
            bool ret = up || need_update;
            need_update = false;
            return ret;
        }
        inline void ClearUpdatingUnits() { update_units.clear(); }
        inline void RequestRedraw() { need_redraw = true; }
        inline void RequestUpdate() { need_update = true; }
        inline bool AllUnitNeedUpdate() { return all_need_update; }
        inline int32_t GetRenderVersion() { return render_version; }
        
        static void SetBlendFunc(BlendMode bm) {
            switch(bm) {
                case BlendMode::Normal: glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
                case BlendMode::FrameBuffer: glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA); break;
                case BlendMode::DrawFrameBuffer: glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); break;
            }
        }
        
    protected:
        std::vector<VTYPE> vertex_buffer;
        std::vector<int16_t> index_buffer;
        int32_t pre_vert_size = 0;
        int32_t pre_index_size = 0;
        int32_t render_version = 0;
        bool need_redraw = true;
        bool need_update = false;
        bool all_need_update = false;
        std::vector<std::shared_ptr<RenderCmd<VTYPE>>> render_commands;
        std::map<RenderUnit<VTYPE>*, std::shared_ptr<RenderUnit<VTYPE>>> all_units;     // owner
        std::set<RenderUnit<VTYPE>*> update_units;  // weak reference
    };
    
    template<typename VTYPE>
    void RenderUnit<VTYPE>::PushVertices() {
        if(!CheckAvailable())
            return;
        SyncVersion();
        auto new_idx = manager->BeginPrimitive(GetPrimitiveType(), GetTextureId());
        Update(std::get<0>(new_idx), std::get<1>(new_idx));
        manager->PushVertices(vertices.data(), indices.data(), vertices.size(), indices.size());
        ShowPushInfo();
    }
    
    template<typename VTYPE>
    void RenderUnit<VTYPE>::UpdateVertices() {
        if(!CheckUpdateVersion())
            return;
        Update(vert_index, index_index);
        manager->UpdateVertices(vertices.data(), vert_index, vertices.size());
        manager->UpdateIndices(indices.data(), index_index, indices.size());
        ShowUpdateInfo();
    }
    
    template<typename VTYPE>
    void RenderUnit<VTYPE>::Update(int16_t vidx, int16_t iidx) {
        index_index = iidx;
        if(!need_update && !manager->AllUnitNeedUpdate()) {
            if(vert_index != vidx) {
                for(auto& ind : indices)
                    ind += vidx - vert_index;
                vert_index = vidx;
            }
            return;
        }
        vert_index = vidx;
        RefreshVertices();
        need_update = false;
    }
    
    template<typename VTYPE>
    inline void RenderUnit<VTYPE>::SetUpdate() {
        if(need_update)
            return;
        need_update = true;
        if(this->render_version != manager->GetRenderVersion())
            return;
        manager->AddUpdateUnit(this);
    }
    
    template<typename VTYPE>
    inline void RenderUnit<VTYPE>::SetRedraw(bool up) {
        if(up)
            need_update = true;
        manager->RequestRedraw();
    }

    template<typename VTYPE>
    inline void RenderUnit<VTYPE>::SyncVersion() {
        render_version = manager->GetRenderVersion();
    }
    
    template<typename VTYPE>
    inline bool RenderUnit<VTYPE>::CheckUpdateVersion() {
        return render_version == manager->GetRenderVersion();
    }
    
    class RenderObject2DLayout : public base::RenderObject<vt2> {
    public:
        virtual void SetScreenSize(v2i sz) {
            if(sz == screen_size)
                return;
            screen_size = sz;
            all_need_update = true;
            need_redraw = true;
        }
        
        inline v2i GetScreenSize() { return screen_size; }
        inline void SetFlip(bool f) { flip_ycoord = f; }
        inline v2f ConvScreenCoord(v2i pos) {
            if(flip_ycoord)
                return v2f{pos.x * 2.0f / screen_size.x - 1.0f, pos.y * 2.0f / screen_size.y - 1.0f};
            return v2f{pos.x * 2.0f / screen_size.x - 1.0f, 1.0f - pos.y * 2.0f / screen_size.y};
        }
        inline v2f ConvScreenSize(v2i sz) {
            if(flip_ycoord)
                return v2f{sz.x * 2.0f / screen_size.x, sz.y * 2.0f / screen_size.y};
            return v2f{sz.x * 2.0f / screen_size.x, -sz.y * 2.0f / screen_size.y};
        }
        
    protected:
        bool flip_ycoord = false;
        v2i screen_size = {1, 1};
    };
    
    class FrameControler : public SysClock {
    public:
        inline void InitFrameControler() { InitSysClock(); }
        
        void SetFrameRate(int32_t rate) {
            frame_interval = 1000L / rate;
        }
        
        void CheckFrameRate() {
            auto now = GetSysClock();
            frame_check += frame_interval - (now - frame_time);
            if(frame_check >= 0)
                std::this_thread::sleep_for(std::chrono::milliseconds(frame_interval));
            frame_time = now;
        }
        
    protected:
        int64_t frame_interval = 16L;
        int64_t frame_time = 0;
        int64_t frame_check = 0;
    };
    
    class RenderCompositor : public IRenderer {
    public:
        inline void PushObject(IRenderer* obj) { renderer.push_back(obj); }
        inline void PushObject(IRenderer& obj) { renderer.push_back(&obj); }
        inline void Clear() { renderer.clear(); }
        inline void SetClearColor(float r, float g, float b, float a) {
            clear_color[0] = r;
            clear_color[1] = g;
            clear_color[2] = b;
            clear_color[3] = a;
        }
        inline void SetRenderCountRequired(int16_t ct) { render_count_required = 2; }
        
        virtual bool PrepareRender() {
            bool need_render = false;
            for(auto& r : renderer)
                need_render = r->PrepareRender() || need_render;
            if(need_render) {
                render_count = 0;
                return true;
            } else
                return render_count < render_count_required;
        }
        
        virtual void Render() {
            GLCheckError(__FILE__, __LINE__);
            glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
            glClear(GL_COLOR_BUFFER_BIT);
            GLCheckError(__FILE__, __LINE__);
            for(auto& r : renderer)
                r->Render();
            render_count++;
        }
        
    protected:
        int16_t render_count = 0;
        int16_t render_count_required = 2;
        float clear_color[4] = {0.2f, 0.2f, 0.2f, 1.0f};
        std::vector<IRenderer*> renderer;
    };
    
    class RenderCompositorWithViewport : public RenderCompositor {
    public:
        void SetViewport(recti v) { viewport = v; }
        virtual void Render() {
            GLCheckError(__FILE__, __LINE__);
            glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
            glClear(GL_COLOR_BUFFER_BIT);
            glViewport(viewport.left, viewport.top, viewport.width, viewport.height);
            GLCheckError(__FILE__, __LINE__);
            for(auto& r : renderer)
                r->Render();
        }
        
    protected:
        recti viewport = {0, 0, 1, 1};
    };
    
    class FrameBufferRenderer : public IRenderer {
    public:
        FrameBufferRenderer(int32_t texw, int32_t texh, bool nc = false, bool auto_revert_tex = true) {
            render_tex = new Texture();
            render_tex->Load(nullptr, texw, texh);
            render_tex->SetFrameBufferTexture(auto_revert_tex);
            frame_size = {texw, texh};
            need_clear = nc;
            glGenFramebuffers(1, &frame_buffer);
            glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render_tex->GetTextureId(), 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            GLCheckError(__FILE__, __LINE__);
        }
        
        ~FrameBufferRenderer() {
            glDeleteFramebuffers(1, &frame_buffer);
            delete render_tex;
        }
        
        virtual bool PrepareRender() {
            if(renderer)
                need_update = renderer->PrepareRender();
            else
                need_update = false;
            GLCheckError(__FILE__, __LINE__);
            return need_update;
        }
        
        virtual void Render() {
            if(need_update) {
                glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
                GLCheckError(__FILE__, __LINE__);
                glViewport(0, 0, frame_size.x, frame_size.y);
                if(need_clear) {
                    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
                    glClear(GL_COLOR_BUFFER_BIT);
                }
                renderer->Render();
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                GLCheckError(__FILE__, __LINE__);
            }
        }
        
        inline void SetRenderObject(IRenderer* r) { renderer = r; if(r) r->SetBlendMode(BlendMode::FrameBuffer); }
        inline Texture* GetFrameBufferTexture() { return render_tex; }
        
    protected:
        bool need_update = true;
        bool need_clear = false;
        uint32_t frame_buffer = 0;
        v2i frame_size = {0, 0};
        Texture* render_tex = nullptr;
        IRenderer* renderer = nullptr;
    };
    
    class SimpleTextureRenderer : public RenderObject2DLayout {
    public:
        SimpleTextureRenderer(BlendMode bm = BlendMode::Normal, bool with_vao = true) {
            blend_mode = bm;
            InitGLState(with_vao);
        }
        
        inline void ClearVertices() { ClearCommandVertices(); }
        
        template<typename V, typename T>
        void AddVertices(Texture* texture, rect<V> vert, rect<T> tex_rect, uint32_t cl = 0xffffffff) {
            TextureInfo<T, 4> tex = {{
                {tex_rect.left, tex_rect.top},
                {tex_rect.left + tex_rect.width, tex_rect.top},
                {tex_rect.left, tex_rect.top + tex_rect.height},
                {tex_rect.left + tex_rect.width, tex_rect.top + tex_rect.height}
            }};
            AddVertices(texture, vert, tex, cl);
        }
        
        template<typename V, typename T>
        void AddVertices(Texture* texture, rect<V> vert, TextureInfo<T, 4> tex, uint32_t cl = 0xffffffff) {
            auto sz = BeginPrimitive(GL_TRIANGLES, texture->GetTextureId());
            int16_t idx[6] = {0, 1, 2, 2, 1, 3};
            for(int32_t i = 0; i < 6; ++i)
                idx[i] += std::get<1>(sz);
            vt2 verts[4];
            SetVertex(verts, vert);
            SetTexcoord(verts, texture, tex);
            for(int32_t i = 0; i < 4; ++i)
                verts[i].color = cl;
            PushVertices(verts, idx, 4, 6);
            need_redraw = true;
        }
        
        inline void SetVertex(vt2* verts, recti& vert) {
            verts[0].vertex = ConvScreenCoord({vert.left, vert.top});
            verts[1].vertex = ConvScreenCoord({vert.left + vert.width, vert.top});
            verts[2].vertex = ConvScreenCoord({vert.left, vert.top + vert.height});
            verts[3].vertex = ConvScreenCoord({vert.left + vert.width, vert.top + vert.height});
        }
        
        inline void SetVertex(vt2* verts, rectf& vert) {
            verts[0].vertex = {vert.left, vert.top};
            verts[1].vertex = {vert.left + vert.width, vert.top};
            verts[2].vertex = {vert.left, vert.top + vert.height};
            verts[3].vertex = {vert.left + vert.width, vert.top + vert.height};
        }
        
        inline void SetTexcoord(vt2* verts, Texture* texture, texi4& tex) {
            verts[0].texcoord = texture->ConvTexCoord(tex.vert[0]);
            verts[1].texcoord = texture->ConvTexCoord(tex.vert[1]);
            verts[2].texcoord = texture->ConvTexCoord(tex.vert[2]);
            verts[3].texcoord = texture->ConvTexCoord(tex.vert[3]);
        }
        
        inline void SetTexcoord(vt2* verts, Texture* texture, texf4& tex) {
            verts[0].texcoord = tex.vert[0];
            verts[1].texcoord = tex.vert[1];
            verts[2].texcoord = tex.vert[2];
            verts[3].texcoord = tex.vert[3];
        }
        
        virtual bool PrepareRender() {
            if(need_redraw)
                UploadVertices();
            return need_redraw;
        }
        
    };
    
}

#endif

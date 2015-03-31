#ifndef _RENDER_UTIL_H_
#define _RENDER_UTIL_H_

namespace base
{
    
    template<typename VTYPE>
    class RenderObject;
    
    template<typename VTYPE>
    class RenderUnit {
        friend class RenderObject<VTYPE>;
    public:
        virtual ~RenderUnit() {}
        virtual void PushVertices() {}
        virtual void UpdateVertices() {}
        
        inline void SetUpdate();
        inline void SetRedraw();
        
    protected:
        int16_t vert_index = 0;
        int16_t index_index = 0;
        bool need_update = true;
        std::vector<VTYPE> vertices;
        std::vector<int16_t> indices;
        RenderObject<VTYPE>* manager = nullptr;
    };
    
    template<typename VTYPE>
    class IRenderState {
    public:
        virtual ~IRenderState() {
            if(vbo_id)
                glDeleteBuffers(&vbo_id, 1);
            if(vbo_idx)
                glDeleteBuffers(&vbo_idx, 1);
            if(vao_id)
                glDeleteVertexArrays(&vao_id, 1);
        }
        
        void InitState(bool with_vao) {
            glGenBuffers(&vbo_id, 1);
            glGenBuffers(&vbo_idx, 1);
            if(with_vao)
                vao_id = typename VTYPE::InitVao(vbo_id, vbo_idx);
        }
        
        std::vector<recti> scissor_stack;
        std::vector<float> global_alpha;
        uint32_t vbo_id = 0;
        uint32_t vbo_idx = 0;
        uint32_t vao_id = 0;
    };
    
    template<typename VTYPE>
    struct RenderCmd {
        virtual ~RenderCmd() {}
        virtual bool CheckPrimitive(int16_t pri_type, int16_t texid) { return false; }
        virtual void Execute(const IRenderState<VTYPE>& state) = 0;
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
        virtual void Execute(const IRenderState<VTYPE>& state) {
            glBindTexture(GL_TEXTURE_2D, texture_id);
            glDrawElements(primitive_type, count, GL_UNSIGNED_SHORT, (const void*)(uintptr_t)(start_index * sizeof(VTYPE)));
        }
        int16_t primitive_type;
        int16_t texture_id;
        int16_t start_index;
        int16_t count;
    };

    template<typename VTYPE>
    struct RenderCmdBeginScissor : public RenderCmd<VTYPE> {
        RenderCmdBeginScissor(recti rect) : scissor_rect(rect) {}
        virtual void Execute(const IRenderState<VTYPE>& state) {
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
        virtual void Execute(const IRenderState<VTYPE>& state) {
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
    struct RenderCmdCancelScissor  : public RenderCmd<VTYPE> {
        virtual void Execute(const IRenderState<VTYPE>& state) {
            glDisable(GL_SCISSOR_TEST);
        }
    };
    
    template<typename VTYPE>
    struct RenderCmdRestoreScissor  : public RenderCmd<VTYPE> {
        virtual void Execute(const IRenderState<VTYPE>& state) {
            if(!state.scissor_stack.empty()) {
                recti rt = state.scissor_stack.back();
                glScissor(rt.left, rt.top, rt.width, rt.height);
            }
        }
    };
    
    template<typename VTYPE>
    struct RenderCmdBeginGlobalAlpha : public RenderCmd<VTYPE> {
        RenderCmdBeginGlobalAlpha(float alpha) : global_alpha(alpha) {}
        virtual void Execute(const IRenderState<VTYPE>& state) {

        }
        float global_alpha;
    };
    
    template<typename VTYPE>
    struct RenderCmdEndGlobalAlpha  : public RenderCmd<VTYPE> {
        virtual void Execute(const IRenderState<VTYPE>& state) {

        }
    };
    
    template<typename VTYPE>
    class RenderObject : public IRenderState<VTYPE> {
    public:
        RenderObject() {
            vertex_buffer.reserve(4096);
            index_buffer.reserve(6144);
        }
        
        virtual ~RenderObject() {
            for(auto unit : all_units)
                delete unit;
            all_units.clear();
        }
        
        std::tuple<int16_t, int16_t> BeginPrimitive(int16_t pri_type, int16_t texid) {
            bool merge = false;
            if(!render_commands.empty() && render_commands.back()->CheckPrimitive(pri_type, texid))
                merge = true;
            if(!merge)
                render_commands.push_back(new RenderCmdDraw<VTYPE>(pri_type, texid, (int16_t)index_buffer.size()));
            return std::make_tuple((int16_t)vertex_buffer.size(), (int16_t)index_buffer.size());
        }
        
        void PushVertices(VTYPE* vert_data, int16_t* index_data, int16_t vcount, int16_t icount) {
            for(int32_t i = 0; i < vcount; ++i)
                vertex_buffer.push_back(vert_data[i]);
            for(int32_t i = 0; i < icount; ++i)
                index_buffer.push_back(index_data[i]);
            static_cast<RenderCmdDraw<VTYPE>*>(render_commands.back())->count += icount;
        }
        
        void BeginUpdate() {
            glBindBuffer(GL_ARRAY_BUFFER, IRenderState<VTYPE>::vbo_id);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IRenderState<VTYPE>::vbo_idx);
        }
        
        void UpdateVertices(VTYPE* vert_data, int16_t start_index, int16_t count) {
            glBufferSubData(GL_ARRAY_BUFFER, sizeof(VTYPE) * start_index, sizeof(VTYPE) * count, vert_data);
        }
        
        void Updateindices(int16_t* index_data, int16_t start_index, int16_t count) {
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int16_t) * start_index, sizeof(int16_t) * count, index_data);
        }
        
        virtual void PushVerticesAll() { Clear(); }
        
        void UploadVertices() {
            glBindBuffer(GL_ARRAY_BUFFER, IRenderState<VTYPE>::vbo_id);
            if(vertex_buffer.size() > pre_vert_size) {
                glBufferData(GL_ARRAY_BUFFER, sizeof(VTYPE) * vertex_buffer.size(), &vertex_buffer[0], GL_DYNAMIC_DRAW);
                pre_vert_size = vertex_buffer.size();
            } else
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VTYPE) * vertex_buffer.size(), &vertex_buffer[0]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IRenderState<VTYPE>::vbo_idx);
            if(index_buffer.size() > pre_index_size) {
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int16_t) * index_buffer.size(), &index_buffer[0], GL_DYNAMIC_DRAW);
                pre_index_size = index_buffer.size();
            } else
                glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(int16_t) * index_buffer.size(), &index_buffer[0]);
        }
        
        template<template<typename> class CMD_TYPE, typename... TR>
        void PushCommand(TR... tr) {
            static_assert(std::is_base_of<RenderCmd<VTYPE>, CMD_TYPE<VTYPE>>::value, "command type should be subclass of RenderCmd.");
            render_commands.push_back(new CMD_TYPE<VTYPE>(std::forward<TR>(tr)...));
        }

        void Render() {
            if(need_redraw) {
                PushVerticesAll();
                UploadVertices();
            } else
                UpdateUnits();
            typename VTYPE::DrawBegin(IRenderState<VTYPE>::vbo_id, IRenderState<VTYPE>::vbo_idx, IRenderState<VTYPE>::vao_id);
            for(auto& rc : render_commands)
                rc->Execute(*this);
            typename VTYPE::DrawEnd(IRenderState<VTYPE>::vbo_id, IRenderState<VTYPE>::vbo_idx, IRenderState<VTYPE>::vao_id);
            need_redraw = false;
            all_need_update = false;
        }
        
        void Clear() {
            vertex_buffer.clear();
            index_buffer.clear();
            pre_vert_size = 0;
            pre_index_size = 0;
            for(auto& rc : render_commands)
                delete rc;
            render_commands.clear();
            update_units.clear();
        }
        
        template<typename ALLOC_TYPE, typename... TR>
        ALLOC_TYPE* NewObject(TR... tr) {
            static_assert(std::is_base_of<RenderUnit<VTYPE>, ALLOC_TYPE>::value, "object type should be subclass of RenderUnit.");
            auto ptr = new ALLOC_TYPE(std::forward<TR>(tr)...);
            ptr->manager = this;
            return ptr;
        }
        
        template<typename ALLOC_TYPE>
        bool DeleteObject(ALLOC_TYPE* at) {
            static_assert(std::is_base_of<RenderUnit<VTYPE>, ALLOC_TYPE>::value, "object type should be subclass of RenderUnit.");
            auto iter = all_units.find(at);
            if(iter != all_units.end()) {
                update_units.erase(at);
                all_units.erase(iter);
                delete at;
                return true;
            }
            return false;
        }
        
        inline void AddUpdateUnit(RenderUnit<VTYPE>* unit) { if(!need_redraw) update_units.insert(unit); }
        inline void UpdateUnits() { for(auto& punit : update_units) punit->Update(); update_units.clear(); }
        inline void ClearUpdatingUnits() { update_units.clear(); }
        inline void SetRedraw() { need_redraw = true; }
        inline bool AllUnitNeedUpdate() { return all_need_update; }
        
    protected:
        std::vector<VTYPE> vertex_buffer;
        std::vector<int16_t> index_buffer;
        int32_t pre_vert_size = 0;
        int32_t pre_index_size = 0;
        bool need_redraw = true;
        bool all_need_update = false;
        std::vector<RenderCmd<VTYPE>*> render_commands;
        std::set<RenderUnit<VTYPE>*> all_units;     // owner
        std::set<RenderUnit<VTYPE>*> update_units;  // weak reference
    };
    
    template<typename VTYPE>
    inline void RenderUnit<VTYPE>::SetUpdate() {
        need_update = true;
        manager->AddUpdateUnit(this);
    }
    
    template<typename VTYPE>
    inline void RenderUnit<VTYPE>::SetRedraw() {
        need_update = true;
        manager->SetRedraw();
    }

    class RenderObject2DLayout : public base::RenderObject<base::v2ct> {
    public:
        inline void SetScreenSize(v2i sz) {
            if(sz == screen_size)
                return;
            screen_size = sz;
            all_need_update = true;
        }
        
        inline v2f ConvScreenCoord(v2i pos) {
            return v2f{pos.x * 2.0f / screen_size.x - 1.0f, 1.0f - pos.y * 2.0f / screen_size.y};
        }
        
    protected:
        v2i screen_size = {1, 1};
    };
    
}

#endif

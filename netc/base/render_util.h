#ifndef _RENDER_UTIL_H_
#define _RENDER_UTIL_H_

namespace base
{
    
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
    class RenderObject : public IRenderState<VTYPE> {
    public:
        RenderObject() {
            vertex_buffer.reserve(4096);
            index_buffer.reserve(6144);
        }
        
        std::tuple<int16_t, int16_t> BeginPrimitive(int16_t pri_type, int16_t texid) {
            bool merge = false;
            if(!render_commands.empty() && render_commands.back()->CheckPrimitive(pri_type, texid))
                merge = true;
            if(!merge)
                render_commands.push_back(std::make_shared<RenderCmdDraw<VTYPE>>(pri_type, texid, (int16_t)index_buffer.size()));
            return std::make_tuple((int16_t)vertex_buffer.size(), (int16_t)index_buffer.size());
        }
        
        void PushVertices(VTYPE* vert_data, int16_t* index_data, int16_t vcount, int16_t icount) {
            for(int32_t i = 0; i < vcount; ++i)
                vertex_buffer.push_back(vert_data[i]);
            for(int32_t i = 0; i < icount; ++i)
                index_buffer.push_back(index_data[i]);
            std::static_pointer_cast<RenderCmdDraw<VTYPE>>(render_commands.back())->count += icount;
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
        
        void UpdateAll() {
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
        
        void BeginScissor(recti srect) {
            render_commands.push_back(std::make_shared<RenderCmdBeginScissor<VTYPE>>(srect));
        }
        
        void EndScissor() {
            render_commands.push_back(std::make_shared<RenderCmdEndScissor<VTYPE>>());
        }
        
        void CancelScissor() {
            render_commands.push_back(std::make_shared<RenderCmdCancelScissor<VTYPE>>());
        }
        
        void RestoreScissor() {
            render_commands.push_back(std::make_shared<RenderCmdRestoreScissor<VTYPE>>());
        }
        
        void Render() {
            typename VTYPE::DrawBegin(IRenderState<VTYPE>::vbo_id, IRenderState<VTYPE>::vbo_idx, IRenderState<VTYPE>::vao_id);
            for(auto& rc : render_commands)
                rc->Execute(*this);
            typename VTYPE::DrawEnd(IRenderState<VTYPE>::vbo_id, IRenderState<VTYPE>::vbo_idx, IRenderState<VTYPE>::vao_id);
        }
        
        void Clear() {
            vertex_buffer.clear();
            index_buffer.clear();
            pre_vert_size = 0;
            pre_index_size = 0;
            render_commands.clear();
        }
        
    protected:
        std::vector<VTYPE> vertex_buffer;
        std::vector<int16_t> index_buffer;
        int32_t pre_vert_size = 0;
        int32_t pre_index_size = 0;
        std::vector<std::shared_ptr<RenderCmd<VTYPE>>> render_commands;
    };
    
    template<typename VTYPE>
    class RenderUnit {
    public:
        virtual void PushVertices(RenderObject<VTYPE>& render_obj) {}
        virtual void UpdateVertices(RenderObject<VTYPE>& render_obj) {}
        
    protected:
        bool need_update;
        int16_t vert_index = 0;
        int16_t index_index = 0;
        std::vector<VTYPE> vertices;
        std::vector<int16_t> indices;
    };
    
    class UpdateUnit {
    public:
        virtual void Update() { need_update = false; }
        inline bool NeedUpdate() { return need_update; }
        inline void SetUpdate(bool up) { need_update = up; }
        
    protected:
        bool need_update = false;
    };
    
    template<typename UUT>
    class UpdateUnitMgr {
    public:
        template<typename UT>
        void AddUpdateUnit(UT ptr) {
            auto unit = std::const_pointer_cast<UUT>(ptr);
            if(unit->NeedUpdate)
                return;
            unit->SetUpdate(true);
            update_units.push_back(unit);
        }
        
        void UpdateAll() {
            for(auto& punit : update_units) {
                auto unit = punit.lock();
                if(unit)
                    unit->Update();
            }
            update_units.clear();
        }
        
        void Clear() {
            for(auto& punit : update_units) {
                auto unit = punit.lock();
                if(unit)
                    unit->SetUpdate(false);
            }
            update_units.clear();
        }
        
    protected:
        std::vector<std::weak_ptr<UUT>> update_units;
    };
    
}

#endif

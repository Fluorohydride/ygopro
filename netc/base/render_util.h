#ifndef _RENDER_UTIL_H_
#define _RENDER_UTIL_H_

namespace ygopro
{
    
    struct RenderCommand {
        RenderCommand(int16_t pri_type, int16_t texid) {
            cmd_type = 1;
            primitive_type = pri_type;
            texture_id = texid;
            count = 0;
        }
        RenderCommand(recti rect) {
            cmd_type = 2;
            clip_rect = rect;
        }
        int16_t cmd_type;
        union {
            struct {
                int16_t primitive_type;
                int16_t texture_id;
                int16_t count;
            };
            recti clip_rect;
        };
    };
    
    template<typename VTYPE>
    class RenderObject {
    public:
        RenderObject();
        ~RenderObject();
        
        void BeginClipping(recti clip_rect) {
            render_commands.emplace_back(clip_rect);
        }
        
        std::pair<int16_t, int16_t> BeginPrimitive(int16_t pri_type, int16_t texid) {
            bool merge = false;
            if(!render_commands.empty()) {
                auto& back = render_commands.back();
                if(back.cmd_type == 1 && back.primitive_type == pri_type && back.texture_id == texid)
                    merge = true;
            }
            if(!merge)
                render_commands.emplace_back(pri_type, texid);
            return std::make_pair((int16_t)vertex_buffer.size(), (int16_t)index_buffer.size());
        }
        
        void PushVertices(VTYPE* vert_data, int16_t* index_data, int16_t vcount, int16_t icount) {
            for(int32_t i = 0; i < vcount; ++i)
                vertex_buffer.push_back(vert_data[i]);
            for(int32_t i = 0; i < icount; ++i)
                index_buffer.push_back(index_data[i]);
            render_commands.back().count += icount;
        }
        
        void UpdateVertices(VTYPE* vert_data, int16_t start_index, int16_t count) {
            
        }
        
        void Updateindices(int16_t* index_data, int16_t start_index, int16_t count) {
            
        }
        
        void Render() {
            
        }
        
        void Clear() {
            vertex_buffer.clear();
            index_buffer.clear();
            clipping_stack.clear();
            render_commands.clear();
        }
    protected:
        std::vector<VTYPE> vertex_buffer;
        std::vector<int16_t> index_buffer;
        std::vector<recti> clipping_stack;
        std::vector<RenderCommand> render_commands;
        int32_t vbo_id = 0;
        int32_t vbo_idx = 0;
        int32_t vao_id = 0;
    };
    
    template<typename VTYPE>
    class RenderUnit {
    public:
        void PushVertices(RenderObject<VTYPE>& render_obj);
        void UpdateVertices(RenderObject<VTYPE>& render_obj);
    protected:
        int16_t vert_index;
        int16_t index_index;
    };
    
}

#endif

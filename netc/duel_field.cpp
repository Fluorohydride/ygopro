#include "../common/common.h"

#include "image_mgr.h"
#include "scene_mgr.h"

#include "duel_scene.h"
#include "duel_field.h"

namespace ygopro
{
    
    void FieldBlock::Init(unsigned int idx, rectf center, ti4 ti) {
        vertex_index = idx;
        vertex.push_back({center.left - center.width * 0.5f, center.top - center.height * 0.5f, 0.0f});
        vertex.push_back({center.left + center.width * 0.5f, center.top - center.height * 0.5f, 0.0f});
        vertex.push_back({center.left - center.width * 0.5f, center.top + center.height * 0.5f, 0.0f});
        vertex.push_back({center.left + center.width * 0.5f, center.top + center.height * 0.5f, 0.0f});
        field_tex = ti;
        alpha = 1.0f;
        hl = 0.0f;
    }
    
    void FieldBlock::RefreshVertices() {
        std::array<v3hct, 4> vert;
        vert[0].vertex = vertex[0];
        vert[0].texcoord = field_tex.vert[0];
        vert[1].vertex = vertex[1];
        vert[1].texcoord = field_tex.vert[1];
        vert[2].vertex = vertex[2];
        vert[2].texcoord = field_tex.vert[2];
        vert[3].vertex = vertex[3];
        vert[3].texcoord = field_tex.vert[3];
        for(int i = 0; i < 4; ++i) {
            vert[i].color = ((int)(alpha.Get() * 255) << 24) | 0xffffff;
            vert[i].hcolor = ((int)(hl.Get() * 255) << 24) | 0xffffff;
        }
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(v3hct) * vertex_index, sizeof(v3hct), &vert[0]);
    }
    
    bool FieldBlock::UpdateVertices(double tm) {
        alpha.Update(tm);
        hl.Update(tm);
        RefreshVertices();
        return alpha.NeedUpdate() && hl.NeedUpdate();
    }
    
}

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
        texcoord.push_back(ti.vert[0]);
        texcoord.push_back(ti.vert[1]);
        texcoord.push_back(ti.vert[2]);
        texcoord.push_back(ti.vert[3]);
        alpha = 1.0f;
        hl = 0.0f;
    }
    
    bool FieldBlock::CheckInside(float px, float py) {
        bool cx = (vertex[0].x < vertex[1].x) ? (vertex[0].x <= px && vertex[1].x >= px) : (vertex[0].x >= px && vertex[1].x <= px);
        bool cy = (vertex[0].y < vertex[2].y) ? (vertex[0].y <= py && vertex[2].y >= py) : (vertex[0].y >= py && vertex[2].y <= py);
        return cx && cy;
    }
    
    void FieldBlock::RefreshVertices() {
        std::array<v3hct, 4> vert;
        vert[0].vertex = vertex[0];
        vert[0].texcoord = texcoord[0];
        vert[1].vertex = vertex[1];
        vert[1].texcoord = texcoord[1];
        vert[2].vertex = vertex[2];
        vert[2].texcoord = texcoord[2];
        vert[3].vertex = vertex[3];
        vert[3].texcoord = texcoord[3];
        for(int i = 0; i < 4; ++i) {
            vert[i].color = ((int)(alpha.Get() * 255) << 24) | 0xffffff;
            vert[i].hcolor = ((int)(hl.Get() * 255) << 24) | 0xffffff;
        }
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(v3hct) * vertex_index, sizeof(v3hct) * 4, &vert[0]);
    }
    
    bool FieldBlock::UpdateVertices(double tm) {
        alpha.Update(tm);
        hl.Update(tm);
        RefreshVertices();
        return alpha.NeedUpdate() || hl.NeedUpdate();
    }
    
    void DuelField::InitBlocks() {
        for(int i = 0 ; i < 17; ++i) {
            field_blocks[0][i] = std::make_shared<FieldBlock>();
            field_blocks[1][i] = std::make_shared<FieldBlock>();
        }
        field_blocks[0][0 ]->Init(0 , SceneMgr::Get().LayoutRectConfig("mzone1"), ImageMgr::Get().GetTexture("mzone"));
        field_blocks[0][1 ]->Init(4 , SceneMgr::Get().LayoutRectConfig("mzone2"), ImageMgr::Get().GetTexture("mzone"));
        field_blocks[0][2 ]->Init(8 , SceneMgr::Get().LayoutRectConfig("mzone3"), ImageMgr::Get().GetTexture("mzone"));
        field_blocks[0][3 ]->Init(12, SceneMgr::Get().LayoutRectConfig("mzone4"), ImageMgr::Get().GetTexture("mzone"));
        field_blocks[0][4 ]->Init(16, SceneMgr::Get().LayoutRectConfig("mzone5"), ImageMgr::Get().GetTexture("mzone"));
        field_blocks[0][5 ]->Init(20, SceneMgr::Get().LayoutRectConfig("szone1"), ImageMgr::Get().GetTexture("szone"));
        field_blocks[0][6 ]->Init(24, SceneMgr::Get().LayoutRectConfig("szone2"), ImageMgr::Get().GetTexture("szone"));
        field_blocks[0][7 ]->Init(28, SceneMgr::Get().LayoutRectConfig("szone3"), ImageMgr::Get().GetTexture("szone"));
        field_blocks[0][8 ]->Init(32, SceneMgr::Get().LayoutRectConfig("szone4"), ImageMgr::Get().GetTexture("szone"));
        field_blocks[0][9 ]->Init(36, SceneMgr::Get().LayoutRectConfig("szone5"), ImageMgr::Get().GetTexture("szone"));
        field_blocks[0][10]->Init(40, SceneMgr::Get().LayoutRectConfig("fdzone"), ImageMgr::Get().GetTexture("fdzone"));
        field_blocks[0][11]->Init(44, SceneMgr::Get().LayoutRectConfig("pzonel"), ImageMgr::Get().GetTexture("pzonel"));
        field_blocks[0][12]->Init(48, SceneMgr::Get().LayoutRectConfig("pzoner"), ImageMgr::Get().GetTexture("pzoner"));
        field_blocks[0][13]->Init(52, SceneMgr::Get().LayoutRectConfig("mdeck" ), ImageMgr::Get().GetTexture("mdeck"));
        field_blocks[0][14]->Init(56, SceneMgr::Get().LayoutRectConfig("exdeck"), ImageMgr::Get().GetTexture("exdeck"));
        field_blocks[0][15]->Init(60, SceneMgr::Get().LayoutRectConfig("grave" ), ImageMgr::Get().GetTexture("grave"));
        field_blocks[0][16]->Init(64, SceneMgr::Get().LayoutRectConfig("banish"), ImageMgr::Get().GetTexture("banish"));
        for(int i = 0; i < 17; ++i) {
            *field_blocks[1][i] = *field_blocks[0][i];
            for(auto& vert : field_blocks[1][i]->vertex)
                vert = vert * -1;
            field_blocks[1][i]->vertex_index = field_blocks[0][i]->vertex_index + 68;
        }
    }
    
    void DuelField::RefreshBlocks() {
        for(int i = 0 ; i < 17; ++i) {
            field_blocks[0][i]->RefreshVertices();
            field_blocks[1][i]->RefreshVertices();
        }
    }
    
    std::pair<int, int> DuelField::CheckHoverBlock(float px, float py) {
        for(int i = 0 ; i < 17; ++i) {
            if(field_blocks[0][i]->CheckInside(px, py))
                return std::make_pair(1, i);
            if(field_blocks[1][i]->CheckInside(px, py))
                return std::make_pair(2, i);
        }
        return std::make_pair(0, 0);
    }
    
}

#ifndef _DUEL_FIELD_H_
#define _DUEL_FIELD_H_

#include "glbase.h"

namespace ygopro
{
    
    struct FieldBlock {
        void Init(unsigned int idx, rectf center, ti4 ti);
        void RefreshVertices();
        bool UpdateVertices(double tm);
        
        unsigned int vertex_index = 0;
        glbase::Attribute<float> alpha;
        glbase::Attribute<float> hl;
        std::vector<v3f> vertex;
        ti4 field_tex;
    };
    
    struct FieldCard {
        void RefreshVertices();
        bool UpdateVertices(double tm);
        
        unsigned int vertex_index = 0;
        glbase::Attribute<v3f> pos;
        glbase::Attribute<glm::quat> rot;
        glbase::Attribute<float> alpha;
        glbase::Attribute<float> hl;
        std::vector<v3f> vertex;
        ti4 card_tex;
    };
    
    struct DuelField {
        void AddCard();
        void MoveCard(int side, int zone, int seq, int subs, int toside, int tozone, int toseq, int tosubs);
        void Update();
        void Draw();
        void UpdateIndex();
        
        unsigned int card_count = 0;
        std::array<FieldBlock, 17> field_blocks[2];
        std::vector<FieldCard> m_zone[2];
        std::vector<FieldCard> s_zone[2];
        std::vector<FieldCard> deck[2];
        std::vector<FieldCard> hand[2];
        std::vector<FieldCard> extra[2];
        std::vector<FieldCard> grave[2];
        std::vector<FieldCard> banished[2];
    };
    
}

#endif

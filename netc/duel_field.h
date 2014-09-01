#ifndef _DUEL_FIELD_H_
#define _DUEL_FIELD_H_

#include "glbase.h"

namespace ygopro
{
    
    struct FieldObject {
        virtual void RefreshVertices() = 0;
        virtual bool UpdateVertices(double tm) = 0;
        
        unsigned int vertex_index = 0;
        MutableAttribute<float> alpha;
        MutableAttribute<float> hl;
        std::vector<v3f> vertex;
        std::vector<v2f> texcoord;
        bool updating = false;
    };
    
    struct FieldBlock : public FieldObject {
        virtual void RefreshVertices();
        virtual bool UpdateVertices(double tm);
        
        void Init(unsigned int idx, rectf center, ti4 ti);
        bool CheckInside(float px, float py);
    };
    
    struct FieldCard : public FieldObject {
        MutableAttribute<v3f> pos;
        MutableAttribute<glm::quat> rot;
    };
    
    struct DuelField {
        void InitBlocks();
        void RefreshBlocks();
        void AddCard();
        void MoveCard(int side, int zone, int seq, int subs, int toside, int tozone, int toseq, int tosubs);
        void Update();
        void Draw();
        void UpdateIndex();
        std::pair<int, int> CheckHoverBlock(float px, float py);
        
        unsigned int card_count = 0;
        std::array<std::shared_ptr<FieldBlock>, 17> field_blocks[2];
        std::vector<std::shared_ptr<FieldCard>> m_zone[2];
        std::vector<std::shared_ptr<FieldCard>> s_zone[2];
        std::vector<std::shared_ptr<FieldCard>> deck[2];
        std::vector<std::shared_ptr<FieldCard>> hand[2];
        std::vector<std::shared_ptr<FieldCard>> extra[2];
        std::vector<std::shared_ptr<FieldCard>> grave[2];
        std::vector<std::shared_ptr<FieldCard>> banished[2];
    };
    
}

#endif

#ifndef _DUEL_FIELD_H_
#define _DUEL_FIELD_H_

#include "glbase.h"

namespace ygopro
{
    
    class FieldBlock {
    public:
        virtual void RefreshVertices();
        virtual void UpdateVertices();
        virtual void FadeinColor(unsigned int cl, double tm);
        virtual void FadeinHColor(unsigned int hcl, double tm);
        
    protected:
        unsigned int vertex_index = 0;
        v3f current_pos;
        bool update_color = false;
        unsigned int current_color = 0;
        unsigned int start_color = 0;
        unsigned int end_color = 0;
        double color_time = 0.0;
        bool update_hcolor = false;
        unsigned int start_hcolor = 0;
        unsigned int end_hcolor = 0;
        double hcolor_time = 0.0;
        std::vector<v3f> vertex;
        std::vector<unsigned short> index;
    };
    
    class FieldCard {
    public:
        virtual void RefreshVertices();
        virtual void UpdateVertices();
        virtual void MoveTo(v3i pos, double tm);
        virtual void FadeinColor(unsigned int cl, double tm);
        virtual void FadeinHColor(unsigned int hcl, double tm);
        
    protected:
        unsigned int vertex_index = 0;
        bool update_pos = false;
        v3f current_pos;
        v3f start_pos;
        v3f end_pos;
        double moving_time = 0.0;
        bool update_vert = false;
        glm::quat current_quat;
        glm::quat start_quat;
        glm::quat end_quat;
        double rotating_time = 0.0;
        bool update_color = false;
        unsigned int current_color = 0;
        unsigned int start_color = 0;
        unsigned int end_color = 0;
        double color_time = 0.0;
        bool update_hcolor = false;
        unsigned int start_hcolor = 0;
        unsigned int end_hcolor = 0;
        double hcolor_time = 0.0;
        std::vector<v3f> vertex;
        std::vector<unsigned short> index;
    };
    
    class DuelField {
    public:
        void InitFieldBlock();
        void InitFieldCard();
        void AddCard();
        void MoveCard(int side, int zone, int seq, int toside, int tozone, int toseq);
        
    protected:
        std::vector<FieldBlock> field_blocks[2];
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

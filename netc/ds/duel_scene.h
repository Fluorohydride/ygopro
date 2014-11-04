#ifndef _DUEL_SCENE_H_
#define _DUEL_SCENE_H_

#include "../glbase.h"
#include "../scene_mgr.h"

namespace ygopro
{
    
    struct FieldObject {
        virtual void RefreshVertices() = 0;
        virtual bool UpdateVertices(double tm) = 0;
        
        uint32_t vertex_index = 0;
        MutableAttribute<float> dy;
        MutableAttribute<float> alpha;
        MutableAttribute<float> hl;
        std::vector<v3f> vertex;
        std::vector<v2f> texcoord;
        bool updating = false;
        uint32_t hlcolor = 0xffffff;
    };
    
    struct FieldBlock : public FieldObject {
        virtual void RefreshVertices();
        virtual bool UpdateVertices(double tm);
        
        void Init(uint32_t idx, rectf center, ti4 ti);
        bool CheckInside(float px, float py);
        v3f translation;
    };
    
    struct FieldCard : public FieldObject {
        virtual void RefreshVertices();
        virtual bool UpdateVertices(double tm);
        
        void Init(uint32_t idx, uint32_t code, int32_t side);
        void SetCode(uint32_t code, bool refresh = true);
        void SetIconTex(int32_t iid, bool refresh = true);
        
        uint32_t code = 0;
        uint32_t status = 0;
		int32_t side = 0;
		int32_t loc = 0;
		int32_t seq = 0;
		int32_t pos = 0;
        bool update_rvert = true;
        std::vector<v3f> vertex_r;
        std::vector<std::shared_ptr<FieldCard>> olcards;
        MutableAttribute<v3f> translation;
        MutableAttribute<glm::quat> rotation;
    };
    
    struct ViewParam {
        float fovy = 3.1415926f * 0.25f;
        float cnear = 0.1f;
        float cfar = 100.0f;
        float angle = 3.1415926f * 0.25f;
        float radius = 8.0f;
        float ratio = 1.0f;
        float xoffset = 0.0f;
        float yoffset = 0.0f;
        float cameray = 0.0f;
        float cameraz = 0.0f;
        rectf cardrect;
        float handmin = 0.0f;
        float handmax = 0.0f;
        float handy[2];
        float scrx = 0.0f;
        float scry = 0.0f;
        rectf hand_rect[2];
        float hand_width[2];
        glm::mat4 mvp;
        glm::quat hand_quat[2];
    };
    
    class DuelScene : public Scene {
    public:
        DuelScene();
        virtual ~DuelScene();
        virtual void Activate();
        virtual bool Update();
        virtual void Draw();
        virtual void SetSceneSize(v2i sz);
        virtual recti GetScreenshotClip();
        
        void UpdateParams();
        void UpdateHandRect();
        void UpdateBackground();
        void UpdateField();
        void UpdateRegion();
        void UpdateMisc();
        void UpdateIndex();
        
        void InitField();
        void RefreshBlocks();
        void AddUpdateBlock(std::shared_ptr<FieldBlock> pblock);
        std::shared_ptr<FieldCard> AddCard(uint32_t code, int32_t side, int32_t zone, int32_t seq, int32_t subs);
        std::shared_ptr<FieldCard> GetCard(int32_t side, int32_t zone, int32_t seq, int32_t subs);
        std::shared_ptr<FieldCard> RemoveCard(int32_t side, int32_t zone, int32_t seq, int32_t subs);
        void RefreshPos(std::shared_ptr<FieldCard> pcard, bool update = true, float tm = 0.0f);
        void RefreshHand(int32_t side, bool update = true, float tm = 0.0f);
        void MoveCard(std::shared_ptr<FieldCard> pcard, int32_t toside, int32_t tozone, int32_t toseq, int32_t tosubs, bool update = true, float tm = 0.0f);
        void ChangePos(std::shared_ptr<FieldCard> pcard, int32_t pos, bool update = true, float tm = 0.0f);
        void ReleaseCard(std::shared_ptr<FieldCard> pcard);
        void AddUpdateCard(std::shared_ptr<FieldCard> pcard);
        void ClearField();
        
        v2i CheckHoverBlock(float px, float py);
        v2f GetProjectXY(float sx, float sy);
        v2i GetHoverPos(int32_t posx, int32_t posy);
        ViewParam& GetViewParams() { return vparam; }
        std::shared_ptr<FieldBlock> GetFieldBlock(int32_t x, int32_t y);
        std::shared_ptr<FieldCard> GetFieldCard(int32_t x, int32_t y);
        
    protected:
        v2i scene_size = {0, 0};
        uint32_t index_buffer = 0;
        uint32_t card_index_buffer = 0;
        uint32_t field_buffer = 0;
        uint32_t back_buffer = 0;
        uint32_t card_buffer = 0;
        uint32_t misc_buffer = 0;
        uint32_t field_vao = 0;
        uint32_t back_vao = 0;
        uint32_t card_vao = 0;
        uint32_t misc_vao = 0;        
        bool update_bg = true;
        bool update_misc = true;
        bool update_index = true;
        int32_t refresh_region = 0;
        glbase::Shader duel_shader;
        ViewParam vparam;
        
        std::array<std::shared_ptr<FieldBlock>, 17> field_blocks[2];
        std::vector<std::shared_ptr<FieldCard>> m_zone[2];
        std::vector<std::shared_ptr<FieldCard>> s_zone[2];
        std::vector<std::shared_ptr<FieldCard>> deck[2];
        std::vector<std::shared_ptr<FieldCard>> hand[2];
        std::vector<std::shared_ptr<FieldCard>> extra[2];
        std::vector<std::shared_ptr<FieldCard>> grave[2];
        std::vector<std::shared_ptr<FieldCard>> banished[2];
        std::vector<std::shared_ptr<FieldCard>> alloc_cards;
        std::list<std::weak_ptr<FieldObject>> updating_blocks;
        std::list<std::weak_ptr<FieldObject>> updating_cards;
        
    };
    
}

#endif

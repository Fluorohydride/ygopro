#ifndef _DUEL_SCENE_H_
#define _DUEL_SCENE_H_

#include "../common/tcpclient.h"

#include "glbase.h"
#include "scene_mgr.h"

namespace ygopro
{
    
    class DuelCommand;
    
    struct FieldObject {
        virtual void RefreshVertices() = 0;
        virtual bool UpdateVertices(double tm) = 0;
        
        unsigned int vertex_index = 0;
        MutableAttribute<float> dy;
        MutableAttribute<float> alpha;
        MutableAttribute<float> hl;
        std::vector<v3f> vertex;
        std::vector<v2f> texcoord;
        bool updating = false;
        unsigned int hlcolor = 0xffffff;
    };
    
    struct FieldBlock : public FieldObject {
        virtual void RefreshVertices();
        virtual bool UpdateVertices(double tm);
        
        void Init(unsigned int idx, rectf center, ti4 ti);
        bool CheckInside(float px, float py);
        v3f translation;
    };
    
    struct FieldCard : public FieldObject {
        virtual void RefreshVertices();
        virtual bool UpdateVertices(double tm);
        
        void Init(unsigned int idx, unsigned int code, int side);
        void SetCode(unsigned int code, bool refresh = true);
        void SetIconTex(int iid, bool refresh = true);
        
        unsigned int code = 0;
        unsigned int status = 0;
		int side = 0;
		int loc = 0;
		int seq = 0;
		int pos = 0;
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
    
    class DuelScene : public Scene, public TcpClientSeed {
    public:
        DuelScene();
        virtual ~DuelScene();
        virtual void Activate();
        virtual bool Update();
        virtual void Draw();
        virtual void SetSceneSize(v2i sz);
        virtual recti GetScreenshotClip();
        virtual void MouseMove(sgui::MouseMoveEvent evt);
        virtual void MouseButtonDown(sgui::MouseButtonEvent evt);
        virtual void MouseButtonUp(sgui::MouseButtonEvent evt);
        virtual void MouseWheel(sgui::MouseWheelEvent evt);
        virtual void KeyDown(sgui::KeyEvent evt);
        virtual void KeyUp(sgui::KeyEvent evt);
        
        virtual void OnConnected();
        virtual void OnConnectError();
        virtual void OnConnectTimeOut();
        virtual void OnDisconnected();
        virtual void HandlePacket(unsigned short proto, unsigned char data[], unsigned int size);
        
        void UpdateParams();
        void UpdateHandRect();
        void UpdateBackground();
        void UpdateField();
        void UpdateRegion();
        void UpdateMisc();
        void UpdateIndex();
        
        void InitField();
        void RefreshBlocks();
        std::shared_ptr<FieldCard> AddCard(unsigned int code, int side, int zone, int seq, int subs);
        std::shared_ptr<FieldCard> GetCard(int side, int zone, int seq, int subs);
        std::shared_ptr<FieldCard> RemoveCard(int side, int zone, int seq, int subs);
        void RefreshPos(std::shared_ptr<FieldCard> pcard, bool update = true, float tm = 0.0f);
        void RefreshHand(int side, bool update = true, float tm = 0.0f);
        void MoveCard(std::shared_ptr<FieldCard> pcard, int toside, int tozone, int toseq, int tosubs, bool update = true, float tm = 0.0f);
        void ChangePos(std::shared_ptr<FieldCard> pcard, int pos, bool update = true, float tm = 0.0f);
        void ReleaseCard(std::shared_ptr<FieldCard> pcard);
        void AddUpdateCard(std::shared_ptr<FieldCard> pcard);
        void ClearField();
        
        v2i CheckHoverBlock(float px, float py);
        v2f GetProjectXY(float sx, float sy);
        v2i GetHoverPos(int posx, int posy);
        
    protected:
        v2i scene_size = {0, 0};
        unsigned int index_buffer = 0;
        unsigned int card_index_buffer = 0;
        unsigned int field_buffer = 0;
        unsigned int back_buffer = 0;
        unsigned int card_buffer = 0;
        unsigned int misc_buffer = 0;
        unsigned int field_vao = 0;
        unsigned int back_vao = 0;
        unsigned int card_vao = 0;
        unsigned int misc_vao = 0;        
        bool update_bg = true;
        bool update_misc = true;
        bool update_index = true;
        int refresh_region = 0;
        CommandList<DuelCommand> duel_commands;
        glbase::Shader duel_shader;
        
        ViewParam vparam;
        bool btnDown[2] = {false};
        v2i btnPos[2];
        std::weak_ptr<FieldBlock> pre_block;
        std::weak_ptr<FieldCard> pre_card;
        v2i click_pos = {0, 0};
        
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

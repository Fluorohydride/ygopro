#ifndef _DUEL_SCENE_H_
#define _DUEL_SCENE_H_

#include "utils/action.h"

#include "../render_util.h"
#include "../scene_mgr.h"

namespace ygopro
{
    
    class FieldObject : public base::RenderUnit<vt3> {
    public:
        virtual ~FieldObject() {}
        virtual bool CheckAvailable() { return true; }
        virtual int32_t GetPrimitiveType() { return GL_TRIANGLES; }
        
        inline void SetColor(uint32_t cl) { color = cl; SetUpdate(); }
        inline void SetHL(uint32_t h) { hl = h; SetUpdate(); }
        
        uint32_t color = 0xffffffff;
        uint32_t hl = 0;
    };
    
    class FieldBlock : public FieldObject {
    public:
        virtual int32_t GetTextureId();
        virtual void RefreshVertices();
        
        inline void SetPosition(rectf pos) { translation = {pos.left, pos.top}; quad_size = {pos.width, pos.height}; SetUpdate(); }
        inline void SetTexture(texf4 tex) { block_texture = tex; SetUpdate(); }
        inline void Mirror(FieldBlock* fb) {
            translation = fb->translation * -1.0f;
            quad_size = fb->quad_size * -1.0f;
            block_texture = fb->block_texture;
            SetUpdate();
        }
        inline v2f GetCenter() { return translation; }
        
        bool CheckInside(float px, float py);
        
        v2f translation = {0.0f, 0.0f};
        v2f quad_size = {0.0f, 0.0f};
        texf4 block_texture;
    };
    
    class FieldCard : public FieldObject, public std::enable_shared_from_this<FieldCard> {
    public:
        virtual ~FieldCard();
        virtual int32_t GetTextureId();
        virtual void RefreshVertices();
        
        void SetCode(uint32_t code);
        std::pair<v3f, glm::quat> GetPositionInfo();
        void UpdatePosition(int32_t tm);
        
        void Attach(std::shared_ptr<FieldCard> target);
        void Detach();
        
        inline void SetTranslation(v3f tr) { translation = tr; update_vert = true; SetUpdate(); }
        inline void SetRotation(glm::quat rot) { rotation = rot; update_vert = true; SetUpdate(); }
        inline void SetYOffset(float yo) { yoffset = yo; update_vert = true; SetUpdate(); }
        inline void SetCardTex(texf4 ctex) { card_texture = ctex; SetUpdate(); }
        inline void SetSleeveTex(texf4 stex) { sleeve_texture = stex; SetUpdate(); }
        inline void SetIconTex(texf4 itex) { icon_texture = itex; SetUpdate(); }
        
        bool update_vert = true;
        float yoffset = 0.0f;
        v3f translation = {0.0f, 0.0f, 0.0f};
        glm::quat rotation;
        texf4 card_texture;
        texf4 sleeve_texture;
        texf4 icon_texture;
        
        uint32_t code = 0;
        uint32_t status = 0;
        int32_t card_side = 0;
        int32_t card_loc = 0;
        int32_t card_seq = 0;
        int32_t card_pos = 0;
        std::vector<std::shared_ptr<FieldCard>> attached_cards;
        std::shared_ptr<FieldCard> attaching_card;
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
        rectf iconrect;
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

    class FieldRenderer : public base::RenderObject<vt3> {
    public:
        FieldRenderer() { InitGLState(true); }
        virtual void PushVerticesAll();
    };
    
    class FieldCardRenderer : public base::RenderObject<vt3> {
    public:
        FieldCardRenderer() { InitGLState(true); }
        virtual void PushVerticesAll();
    };
    
    class DuelScene : public Scene, public ActionMgr<int64_t>, public base::RenderCompositorWithViewport {
    public:
        DuelScene(base::Shader* _2dshader, base::Shader* _3dshader);
        virtual ~DuelScene();
        virtual void Activate();
        virtual void Terminate();
        virtual bool Update();
        virtual bool Draw();
        virtual void SetSceneSize(v2i sz);
        virtual recti GetScreenshotClip();
        
        void UpdateParams();
        void UpdateHandRect();
        void UpdateViewOffset(v2f offset);
        void UpdateViewAngle(float angle);
        void UpdateViewRadius(float radius);
        
        void InitField();
        std::shared_ptr<FieldCard> AddCard(uint32_t code, int32_t side, int32_t zone, int32_t seq, int32_t subs);
        std::shared_ptr<FieldCard> GetCard(int32_t side, int32_t zone, int32_t seq, int32_t subs);
        std::shared_ptr<FieldCard> RemoveCard(int32_t side, int32_t zone, int32_t seq, int32_t subs);
        void MoveCard(std::shared_ptr<FieldCard> pcard, int32_t toside, int32_t tozone, int32_t toseq, int32_t tosubs, int32_t tm);
        void ChangePos(std::shared_ptr<FieldCard> pcard, int32_t pos, int32_t tm);
        void ClearField();
        void InitHp(int32_t local_pl, int32_t hp);
        void AddChain(uint32_t code, int32_t side, int32_t zone, int32_t seq, int32_t subs, int32_t tside, int32_t tzone, int32_t tseq);
        void DrawCard(int32_t pl, int32_t data);
        
        v2i CheckHoverBlock(float px, float py);
        v2f GetProjectXY(float sx, float sy);
        v2i GetHoverPos(int32_t posx, int32_t posy);
        std::shared_ptr<FieldBlock> GetFieldBlock(int32_t x, int32_t y);
        std::shared_ptr<FieldCard> GetFieldCard(int32_t x, int32_t y);
        
    protected:
        std::shared_ptr<base::SimpleTextureRenderer> bg_renderer;
        std::shared_ptr<FieldRenderer> field_renderer;
        std::shared_ptr<FieldCardRenderer> fieldcard_renderer;
        
    };
    
}

#endif

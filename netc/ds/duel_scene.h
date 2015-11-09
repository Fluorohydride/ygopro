#ifndef _DUEL_SCENE_H_
#define _DUEL_SCENE_H_

#include "utils/action.h"

#include "../render_util.h"
#include "../scene_mgr.h"

#include "duel_scene_handler.h"

namespace ygopro
{
    
    template<typename VTYPE>
    class DuelObject : public base::RenderUnit<VTYPE> {
    public:
        virtual ~DuelObject() {}
        virtual bool CheckAvailable() { return true; }
        virtual int32_t GetPrimitiveType() { return GL_TRIANGLES; }
        
        inline void SetColor(uint32_t cl) { color = cl; this->SetUpdate(); }
        inline void SetHL(uint32_t h) { hl = h; this->SetUpdate(); }
        
        uint32_t color = 0xffffffff;
        uint32_t hl = 0;
    };
    
    class FieldBlock : public DuelObject<vt3> {
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
    
    enum class CardPosParam {
        None = 0,
        Confirm = 1,
        Shuffle = 2,
    };
    
    enum class CardActType {
        Linear = 0,
        Asymptotic = 1,
    };
    
    class FieldCard : public DuelObject<vt3>, public std::enable_shared_from_this<FieldCard> {
    public:
        virtual ~FieldCard();
        virtual int32_t GetTextureId();
        virtual void RefreshVertices();
        
        void SetCode(uint32_t code);
        std::pair<v3f, glm::quat> GetPositionInfo(CardPosParam param = CardPosParam::None);
        void UpdateTo(int32_t tm, std::pair<v3f, glm::quat> npos, CardActType act_type = CardActType::Linear);
        void TranslateTo(int32_t tm, v3f tl, CardActType act_type = CardActType::Linear);
        void RotateTo(int32_t tm, glm::quat rot);
        void UpdatePosition(int32_t tm, CardActType act_type = CardActType::Linear);
        void UpdateTranslation(int32_t tm, CardActType act_type = CardActType::Linear);
        void UpdateRotation(int32_t tm);
        
        void Attach(std::shared_ptr<FieldCard> target);
        void Detach();
        void AddCounter() {}
        void RemoveCounter() {}
        void ClearCounter() {}
        void AddContinuousTarget() {}
        void RemoveContinuousTarget() {}
        void ClearContinuousTarget() {}
        
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
        uint32_t reason = 0;
        CardPosInfo pos_info;
        std::vector<std::shared_ptr<FieldCard>> attached_cards;
        FieldCard* attaching_card = nullptr;
    };
    
    class FloatingNumber : public DuelObject<vt2> {
    public:
        virtual int32_t GetTextureId();
        virtual void RefreshVertices();
        
        void SetValue(int32_t val);
        void SetValueStr(const std::string& val_str);
        void SetCharSize(v2i sz) { char_size = sz; SetUpdate(); }
        inline void SetPosition(v2i pos) { char_pos = pos; SetUpdate(); }
        
    protected:
        std::string val_string;
        v2i char_size = {0, 0};
        v2i char_pos = {0, 0};
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
    
    class MiscObjectRenderer : public base::RenderObject2DLayout {
    public:
        MiscObjectRenderer() { InitGLState(true); }
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
        
        v2i CheckHoverBlock(float px, float py);
        v2f GetProjectXY(float sx, float sy);
        v2i GetHoverPos(int32_t posx, int32_t posy);
        CardPosInfo GetHoverCardPos(v2i hp);
        std::shared_ptr<FieldCard> GetHoverCard(CardPosInfo pos_info);
        std::shared_ptr<FieldBlock> GetFieldBlock(int32_t x, int32_t y);
        
        void ClearAllCards();
        void RedrawAllCards() { fieldcard_renderer->RequestRedraw(); }
        inline std::shared_ptr<FieldCard> CreateCard() { return fieldcard_renderer->NewSharedObject<FieldCard>(); }
        inline void RemoveCard(std::shared_ptr<FieldCard> ptr) { fieldcard_renderer->DeleteObject(ptr.get()); }
        inline std::shared_ptr<FieldBlock> CreateFieldBlock() { return field_renderer->NewSharedObject<FieldBlock>(); }
        
        std::shared_ptr<FloatingNumber> AddFloatingNumber();
        
    protected:
        std::shared_ptr<base::SimpleTextureRenderer> bg_renderer;
        std::shared_ptr<FieldRenderer> field_renderer;
        std::shared_ptr<FieldCardRenderer> fieldcard_renderer;
        std::shared_ptr<MiscObjectRenderer> miscobject_renderer;
    };
    
}

#endif

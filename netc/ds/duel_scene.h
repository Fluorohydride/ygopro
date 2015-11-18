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
        inline v2f GetSize() { return quad_size; }
        
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
    
    class FloatingObject : public DuelObject<vt2> {
    public:
        FloatingObject(uint32_t rid) : ref_id(rid) {}
        inline uint32_t GetRefId() { return ref_id; }
        inline void SetCenter(v2i pos) { center_pos = pos; SetUpdate(); }
        inline void SetRotation(float rot) {
            rot_matrix[0] = std::cosf(rot);
            rot_matrix[1] = std::sinf(rot);
            SetUpdate();
        }
        inline v2i RotPoint(v2i point, v2i center) {
            v2i tr1 = point - center;
            v2i tr2 = {(int32_t)(tr1.x * rot_matrix[0] + tr1.y * rot_matrix[1]), (int32_t)(tr1.x * -rot_matrix[1] + tr1.y * rot_matrix[0])};
            return tr2 + center;
        };
        
    protected:
        v2i center_pos = {0, 0};
        uint32_t ref_id = 0;
        float rot_matrix[2] = {1.0f, 0.0f};
    };
    
    class FloatingNumber : public FloatingObject {
    public:
        FloatingNumber(uint32_t rid) : FloatingObject(rid) {}
        virtual int32_t GetTextureId();
        virtual void RefreshVertices();
        
        void SetValue(int32_t val);
        void SetValueStr(const std::string& val_str);
        inline void SetCharSize(v2i sz) { char_size = sz; SetUpdate(); }
        inline void SetSColor(uint32_t cl) { scolor = cl; SetUpdate(); }
        
    protected:
        std::string val_string;
        v2i char_size = {0, 0};
        uint32_t scolor = 0xff000000;
    };
    
    class FloatingSprite : public FloatingObject {
    public:
        FloatingSprite(uint32_t rid) : FloatingObject(rid) {}
        virtual int32_t GetTextureId();
        virtual void RefreshVertices();
        
        void BuildSprite(recti rct, texf4 tex);
        void BuildSprite(v2i* verts, texf4 tex, v2i center);
        inline void SetTexture(base::Texture* tex) { texture = tex; SetRedraw(false); }
        
    protected:
        base::Texture* texture = nullptr;
        std::array<v2i, 4> points;
        texf4 texcoord;
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
        virtual void PushVerticesAll() {
            base::RenderObject2DLayout::PushVerticesAll();
            for(auto& iter : objects)
                iter.second->PushVertices();
        }
        
        template<typename T>
        std::shared_ptr<T> AddFloatingObject() {
            auto ptr = NewSharedObject<T>(count_ref);
            objects[count_ref++] = ptr.get();
            RequestRedraw();
            return ptr;
        }

        template<typename T>
        void RemoveFloatingObject(T* ptr) {
            uint32_t ref_id = ptr->GetRefId();
            if(DeleteObject(ptr)) {
                objects.erase(ref_id);
                RequestRedraw();
            }
        }
        
    protected:
        std::map<uint32_t, FloatingObject*> objects;
        uint32_t count_ref = 0;
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
        
        inline std::shared_ptr<FloatingNumber> AddFloatingNumber() { return miscobject_renderer->AddFloatingObject<FloatingNumber>(); }
        inline std::shared_ptr<FloatingSprite> AddFloatingSprite() { return miscobject_renderer->AddFloatingObject<FloatingSprite>(); }
        inline void RemoveFloatingNumber(FloatingNumber* ptr) { miscobject_renderer->RemoveFloatingObject(ptr); }
        inline void RemoveFloatingSprite(FloatingSprite* ptr) { miscobject_renderer->RemoveFloatingObject(ptr); }
        
    protected:
        std::shared_ptr<base::SimpleTextureRenderer> bg_renderer;
        std::shared_ptr<FieldRenderer> field_renderer;
        std::shared_ptr<FieldCardRenderer> fieldcard_renderer;
        std::shared_ptr<MiscObjectRenderer> miscobject_renderer;
    };
    
}

#endif

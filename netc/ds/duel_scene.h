#ifndef _DUEL_SCENE_H_
#define _DUEL_SCENE_H_

#include "utils/action.h"

#include "../render_util.h"
#include "../scene_mgr.h"
#include "../card_data.h"

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
    
    class FieldSprite : public DuelObject<vt3> {
    public:
        FieldSprite(uint32_t rid) : ref_id(rid) {}
        virtual int32_t GetTextureId();
        virtual void RefreshVertices();
        
        inline void SetTexcoord(texf4 tex) { texcoord = tex; SetUpdate(); }
        inline void SetTexture(base::Texture* tex) { texture = tex; SetRedraw(false); }
        inline void SetSize(v2f sz) { sprite_size = sz; update_vert = true; SetUpdate(); }
        inline void SetTranslation(v3f trans) { translation = trans; update_vert = true; SetUpdate(); }
        inline void SetRotation(glm::quat rot) { rotation = rot; update_vert = true; SetUpdate(); }
        inline uint32_t GetRefId() { return ref_id; }
        
        base::Texture* texture = nullptr;
        texf4 texcoord;
        bool update_vert = true;
        v2f sprite_size = {0.0f, 0.0f};
        v3f translation = {0.0f, 0.0f, 0.0f};
        glm::quat rotation;
        uint32_t ref_id = 0;
    };
    
    class FieldSummonEffect : public FieldSprite {
    public:
        FieldSummonEffect(int32_t rid) : FieldSprite(rid) {}
        virtual void RefreshVertices();
        inline void SetOuterZ(float z) { outer_z = z; update_vert = true; SetUpdate(); }
        inline void SetTexParam(v2f tp) { tex_r = tp.x; tex_o = tp.y; SetUpdate(); }
        
        float outer_z = 0.0f;
        float tex_r = 0.0f;
        float tex_o = 0.0f;
    };
    
    class FieldParticles : public FieldSprite {
    public:
        FieldParticles(int32_t rid) : FieldSprite(rid) {}
    };
    
    class FieldBlock : public FieldSprite {
    public:
        FieldBlock() : FieldSprite(0) {}
        
        inline void Mirror(FieldBlock* fb) {
            texture = fb->texture;
            texcoord = fb->texcoord;
            translation = fb->translation * -1.0f;
            sprite_size = fb->sprite_size * -1.0f;
            update_vert = true;
            SetUpdate();
        }
        inline void SetPosition(rectf rct) {
            translation = {rct.left, rct.top, 0.0f};
            sprite_size = {rct.width, rct.height};
            SetUpdate();
        }
        inline v2f GetCenter() { return {translation.x, translation.y}; }
        inline bool CheckInside(float px, float py) {
            return std::abs(px - translation.x) <= std::abs(sprite_size.x / 2.0f) && std::abs(py - translation.y) <= std::abs(sprite_size.y / 2.0f);
        }
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
        void AddCounter(uint16_t counter_type, uint32_t count);
        void RemoveCounter(uint16_t counter_type, uint32_t count);
        void ClearCounter();
        void AddContinuousTarget(FieldCard* target);
        void RemoveContinuousTarget(FieldCard* target);
        void ClearContinuousTarget();
        void Equip(FieldCard* target);
        void Unequip();
        
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
        CardData* data = nullptr;
        CardPosInfo pos_info;
        std::vector<std::shared_ptr<FieldCard>> attached_cards;
        FieldCard* attaching_card = nullptr;
        FieldCard* equiping_card = nullptr;
        std::set<FieldCard*> equip_this;
        std::set<FieldCard*> targeting_cards;
        std::set<FieldCard*> target_this;
        std::map<uint16_t, uint32_t> counter_map;
    };
    
    class FloatingObject : public DuelObject<vt2> {
    public:
        FloatingObject(uint32_t rid) : ref_id(rid) {}
        inline uint32_t GetRefId() { return ref_id; }
        inline void SetCenter(v2i pos, v2f prop = {0.0f, 0.0f}) { center_pos = pos; center_prop = prop; SetUpdate(); }
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
        inline v2i PosInScreen(v2i rel, v2f prop, v2i scr) {
            return {rel.x + (int32_t)(prop.x * scr.x), rel.y + (int32_t)(prop.y * scr.y)};
        }
        inline v2i CenterPosInScreen(v2i scr) {
            return {center_pos.x + (int32_t)(center_prop.x * scr.x), center_pos.y + (int32_t)(center_prop.y * scr.y)};
        }        
        
        v2i center_pos = {0, 0};
        v2f center_prop = {0.0f, 0.0f};
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
        
        std::string val_string;
        v2i char_size = {0, 0};
        uint32_t scolor = 0xff000000;
    };
    
    class FloatingSprite : public FloatingObject {
    public:
        FloatingSprite(uint32_t rid) : FloatingObject(rid) {}
        virtual int32_t GetTextureId();
        virtual void RefreshVertices();
        
        void BuildSprite(recti rct, texf4 tex, rectf rct_prop = {0.0f, 0.0f, 0.0f, 0.0f});
        void BuildSprite(v2i* verts, texf4 tex, v2f* vert_prop = nullptr);
        inline void SetTexcoord(texf4 tex) { texcoord = tex; SetUpdate(); }
        inline void SetTexture(base::Texture* tex) { texture = tex; SetRedraw(false); }
        
        base::Texture* texture = nullptr;
        std::array<v2i, 4> points;
        std::array<v2f, 4> points_prop;
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
    
    template<typename RENDER_OBJECT_BASE, typename OBJECT_TYPE>
    class RefObjectRenderer : public RENDER_OBJECT_BASE {
    public:
        RefObjectRenderer() { RENDER_OBJECT_BASE::InitGLState(true); }
        virtual void PushVerticesAll() {
            RENDER_OBJECT_BASE::PushVerticesAll();
            for(auto& iter : objects)
                iter.second->PushVertices();
        }
        
        template<typename OBJ>
        std::shared_ptr<OBJ> AddRefObject() {
            auto ptr = RENDER_OBJECT_BASE::template NewSharedObject<OBJ>(count_ref);
            objects[count_ref++] = ptr.get();
            RENDER_OBJECT_BASE::RequestRedraw();
            return ptr;
        }
        
        template<typename OBJ>
        void RemoveRefObject(OBJ* ptr) {
            uint32_t ref_id = ptr->GetRefId();
            if(RENDER_OBJECT_BASE::DeleteObject(ptr)) {
                objects.erase(ref_id);
                RENDER_OBJECT_BASE::RequestRedraw();
            }
        }
        
    protected:
        std::map<uint32_t, OBJECT_TYPE*> objects;
        uint32_t count_ref = 0;
    };
    
    using FloatingObjectRenderer = RefObjectRenderer<base::RenderObject2DLayout, FloatingObject>;
    using FieldSpriteRenderer = RefObjectRenderer<base::RenderObject<vt3>, FieldSprite>;
    
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
        
        template<typename T = FieldSprite>
        inline std::shared_ptr<T> AddFieldSprite() { return fieldsprite_renderer->AddRefObject<T>(); }
        inline void RemoveFieldSprite(FieldSprite* ptr) { fieldsprite_renderer->RemoveRefObject(ptr); }
        inline std::shared_ptr<FloatingNumber> AddFloatingNumber() { return floatingobject_renderer->AddRefObject<FloatingNumber>(); }
        inline void RemoveFloatingNumber(FloatingNumber* ptr) { floatingobject_renderer->RemoveRefObject(ptr); }
        inline std::shared_ptr<FloatingSprite> AddFloatingSprite() { return floatingobject_renderer->AddRefObject<FloatingSprite>(); }
        inline void RemoveFloatingSprite(FloatingSprite* ptr) { floatingobject_renderer->RemoveRefObject(ptr); }
        
    protected:
        std::shared_ptr<base::SimpleTextureRenderer> bg_renderer;
        std::shared_ptr<FieldRenderer> field_renderer;
        std::shared_ptr<FieldCardRenderer> fieldcard_renderer;
        std::shared_ptr<FieldSpriteRenderer> fieldsprite_renderer;
        std::shared_ptr<FloatingObjectRenderer> floatingobject_renderer;
    };
    
}

#endif

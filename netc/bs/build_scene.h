#ifndef _BUILD_SCENE_H_
#define _BUILD_SCENE_H_

#include "utils/action.h"

#include "../render_util.h"
#include "../deck_data.h"
#include "../scene_mgr.h"

namespace ygopro
{
    
    struct FilterCondition;
    
    class BuilderCard : public base::RenderUnit<vt2> {
    public:
        virtual ~BuilderCard() {}
        virtual bool CheckAvailable() { return true; }
        virtual int32_t GetPrimitiveType() { return GL_TRIANGLES; }
        virtual int32_t GetTextureId();
        virtual void RefreshVertices();
        
        inline void SetPos(v2f p) { pos = p; SetUpdate(); }
        inline void SetSize(v2f s) { size = s; SetUpdate(); }
        inline void SetHL(uint32_t h) { hl = h; SetUpdate(); }
        inline void SetLimit(int16_t l) { limit = l; SetUpdate(); }
        
        v2f pos = {0.0f, 0.0f};
        v2f size = {0.0f, 0.0f};
        uint32_t hl = 0;
        int16_t limit = 3;
        texf4 card_tex;
    };
    
    class MiscRenderer : public base::RenderObject2DLayout {
    public:
        MiscRenderer() { InitGLState(true); }
        virtual void PushVerticesAll();
    };
    
    class CardRenderer : public base::RenderObject2DLayout {
    public:
        CardRenderer() { InitGLState(true); }
        virtual void PushVerticesAll();
    };
    
    class BuildScene : public Scene, public base::RenderCompositor {
    public:
        BuildScene();
        virtual ~BuildScene();
        virtual void Activate();
        virtual bool Update();
        virtual bool Draw();
        virtual void SetSceneSize(v2i sz);
        virtual recti GetScreenshotClip();
        void ShowSelectedInfo(uint32_t pos, uint32_t index);
        void ShowCardInfo(uint32_t code);
        void HideCardInfo();
        
        void ClearDeck();
        void SortDeck();
        void ShuffleDeck();
        void SetDeckDirty();
        bool LoadDeckFromFile(const std::wstring& file);
        bool LoadDeckFromString(const std::string& deck_str);
        bool SaveDeckToFile(const std::wstring& file);
        std::string SaveDeckToString();
        
        void UpdateAllCard();
        void RefreshParams();
        void RefreshAllCard();
        void UpdateResult();
        
        void ChangeRegulation(int32_t index, int32_t vr);
        void ViewRegulation(int32_t limit);
        void RefreshSearchResult(const std::array<CardData*, 10> new_results);
        
        void SetCurrentSelection(int32_t sel, bool show_info);
        void MoveCard(int32_t pos, int32_t index);
        void RemoveCard(int32_t pos, int32_t index);
        void InsertSearchResult(int32_t index, bool is_side);
        
        std::shared_ptr<DeckCardData> GetCard(int32_t pos, int32_t index);
        std::pair<int32_t, int32_t> GetHoverPos(int32_t posx, int32_t posy);
        
    protected:
        std::shared_ptr<base::SimpleTextureRenderer> bg_renderer;
        std::shared_ptr<MiscRenderer> misc_renderer;
        std::shared_ptr<CardRenderer> card_renderer;
        std::shared_ptr<base::RenderCompositorWithViewport> mix_renderer;
    };
    
}
#endif

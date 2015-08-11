#ifndef _BUILD_SCENE_H_
#define _BUILD_SCENE_H_

#include "utils/action.h"

#include "../render_util.h"
#include "../deck_data.h"
#include "../scene_mgr.h"

namespace ygopro
{
    
    struct FilterCondition;
    
    class BuilderCard : public DeckCardExtraData, public base::RenderUnit<base::v2ct> {
    public:
        virtual ~BuilderCard() {}
        virtual void PushVertices() {}
        virtual void UpdateVertices() {}
        
        inline void SetPos(v2f p) { pos = p; SetUpdate(); }
        inline void SetSize(v2f s) { size = s; SetUpdate(); }
        inline void SetHL(uint32_t h) { hl = h; SetUpdate(); }
        inline void SetShowLimit(bool s) { show_limit = s; SetUpdate(); }
        inline void SetShowExclusive(bool s) { show_exclusive = s; SetUpdate(); }
        
        v2f pos = {0.0f, 0.0f};
        v2f size = {0.0f, 0.0f};
        uint32_t hl = 0;
        bool show_limit = true;
        bool show_exclusive = true;
        texi4 card_tex;
    };
    
    class BuildScene : public Scene, public base::RenderObject<base::v2ct> {
    public:
        BuildScene();
        virtual ~BuildScene();
        virtual void Activate();
        virtual bool Update();
        virtual void Draw();
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
        
        void UpdateBackGround();
        void UpdateCard();
        void UpdateAllCard();
        void RefreshParams();
        void RefreshAllCard();
        void RefreshAllIndex();
        void UpdateMisc();
        void UpdateResult();
        
        void RefreshCardPos(std::shared_ptr<DeckCardData> dcd);
        void RefreshHL(std::shared_ptr<DeckCardData> dcd);
        void RefreshLimit(std::shared_ptr<DeckCardData> dcd);
        void RefreshEx(std::shared_ptr<DeckCardData> dcd);
        void ChangeExclusive(bool check);
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
        int32_t update_status = 0;
        DeckData current_deck;
        v2i scene_size = {0, 0};
        int32_t max_row_count = 0;
        int32_t main_row_count = 0;
        v2f card_size = {0.0f, 0.0f};
        v2f icon_size = {0.0f, 0.0f};
        float minx = 0.0f;
        float maxx = 0.0f;
        float main_y_spacing = 0.0f;
        float offsety[3] = {0.0f, 0.0f, 0.0f};
        float dx[3] = {0.0f, 0.0f, 0.0f};
        bool show_exclusive = true;
        std::array<texi4, 3> limit;
        std::array<texi4, 3> pool;
        texi4 hmask;
        std::array<CardData*, 10> result_data;
        std::array<texi4, 10> result_tex;
        int32_t current_sel_result = -1;
        int32_t result_show_size = 0;
    };
    
}
#endif

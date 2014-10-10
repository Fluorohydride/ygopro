#ifndef _BUILD_SCENE_H_
#define _BUILD_SCENE_H_

#include "deck_data.h"
#include "scene_mgr.h"
#include "gui_extra.h"

namespace ygopro
{
    
    struct FilterCondition;
    
    struct BuilderCard : public DeckCardExtraData {
        MutableAttribute<v2f> pos;
        MutableAttribute<v2f> size;
        MutableAttribute<float> hl;
        unsigned int buffer_index = 0;
        ti4 card_tex;
        bool show_limit = true;
        bool show_exclusive = true;
        bool updating = false;
    };
    
    class BuildScene : public Scene {
    public:
        BuildScene();
        virtual ~BuildScene();
        virtual void Activate();
        virtual bool Update();
        virtual void Draw();
        virtual void SetSceneSize(v2i sz);
        virtual recti GetScreenshotClip();
        void ShowCardInfo(unsigned int code);
        void StopViewRegulation() { view_regulation = 0; }
        
        void ClearDeck();
        void SortDeck();
        void ShuffleDeck();
        void SetDeckDirty();
        void LoadDeckFromFile(const std::wstring& file);
        void LoadDeckFromClipboard();
        void SaveDeckToFile(const std::wstring& file);
        void SaveDeckToClipboard();
        
        void OnMenuDeck(int id);
        void OnMenuTool(int id);
        void OnMenuList(int id);
        
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
        void ChangeRegulation(int index);
        void ViewRegulation(int limit);
        void UnloadSearchResult();
        void RefreshSearchResult();
        void ResultPrevPage();
        void ResultNextPage();
        void QuickSearch(const std::wstring& keystr);
        void Search(const FilterCondition& fc, int lmt);
        
        void SetCurrentSelection(int sel, bool show_info);
        void AddUpdatingCard(std::shared_ptr<DeckCardData> dcd);
        
        std::shared_ptr<DeckCardData> GetCard(int pos, int index);
        std::pair<int, int> GetHoverPos(int posx, int posy);
        
    protected:
        unsigned int index_buffer = 0;
        unsigned int deck_buffer = 0;
        unsigned int back_buffer = 0;
        unsigned int misc_buffer = 0;
        unsigned int result_buffer = 0;
        unsigned int deck_vao = 0;
        unsigned int back_vao = 0;
        unsigned int misc_vao = 0;
        unsigned int result_vao = 0;
        bool update_bg = true;
        bool update_misc = true;
        bool update_result = true;
        int update_status = 0;
        std::wstring current_file;
        int view_regulation = 0;
        DeckData current_deck;
        v2i scene_size = {0, 0};
        int max_row_count = 0;
        int main_row_count = 0;
        v2f card_size = {0.0f, 0.0f};
        v2f icon_size = {0.0f, 0.0f};
        float minx = 0.0f;
        float maxx = 0.0f;
        float main_y_spacing = 0.0f;
        float offsety[3] = {0.0f, 0.0f, 0.0f};
        float dx[3] = {0.0f, 0.0f, 0.0f};
        bool deck_edited = false;
        bool show_exclusive = true;
        std::array<ti4, 3> limit;
        std::array<ti4, 3> pool;
        ti4 hmask;
        std::list<std::weak_ptr<DeckCardData>> updating_cards;
        std::weak_ptr<sgui::SGIconLabel> deck_label;
        std::shared_ptr<FileDialog> file_dialog;
        std::shared_ptr<FilterDialog> filter_dialog;
        std::shared_ptr<InfoPanel> info_panel;
        std::vector<CardData*> search_result;
        std::array<ti4, 10> result_tex;
        int result_page = 0;
        int result_show_size = 0;
        int current_sel_result = -1;
        std::weak_ptr<sgui::SGLabel> label_result;
        std::weak_ptr<sgui::SGLabel> label_page;
        Timer<double> build_timer;
    };
    
}
#endif

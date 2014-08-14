#ifndef _BUILD_SCENE_H_
#define _BUILD_SCENE_H_

#include "deck_data.h"
#include "scene_mgr.h"
#include "gui_extra.h"

namespace ygopro
{
    
    struct FilterCondition;
    
    struct BuilderCard : public DeckCardExtraData {
        unsigned int buffer_index = 0;
        ti4 card_tex;
        double moving_time_b = 0.0;
        double moving_time_e = 0.0;
        double fading_time_b = 0.0;
        double fading_time_e = 0.0;
        v2f pos = {0.0f, 0.0f};
        v2f size = {0.0f, 0.0f};
        v2f start_pos = {0.0f, 0.0f};
        v2f start_size = {0.0f, 0.0f};
        v2f dest_pos = {0.0f, 0.0f};
        v2f dest_size = {0.0f, 0.0f};
        float hl = 0.0f;
        float start_hl = 0.0f;
        float dest_hl = 1.0f;
        bool update_pos = false;
        bool update_color = false;
        bool show_limit = true;
        bool show_exclusive = true;
        std::function<void()> update_callback;
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
        virtual void MouseMove(sgui::MouseMoveEvent evt);
        virtual void MouseButtonDown(sgui::MouseButtonEvent evt);
        virtual void MouseButtonUp(sgui::MouseButtonEvent evt);
        virtual void KeyDown(sgui::KeyEvent evt);
        virtual void KeyUp(sgui::KeyEvent evt);
        
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
        void UpdateInfo();
        
        void RefreshCardPos(std::shared_ptr<DeckCardData> dcd);
        void RefreshHL(std::shared_ptr<DeckCardData> dcd);
        void RefreshLimit(std::shared_ptr<DeckCardData> dcd);
        void RefreshEx(std::shared_ptr<DeckCardData> dcd);
        void MoveTo(std::shared_ptr<DeckCardData> dcd, float tm, v2f dst, v2f dsz);
        void ChangeHL(std::shared_ptr<DeckCardData> dcd, float tm, float desthl);
        void ChangeExclusive(bool check);
        void ChangeRegulation(int index);
        void ViewRegulation(int limit);
        void UnloadSearchResult();
        void RefreshSearchResult();
        void ResultPrevPage();
        void ResultNextPage();
        void QuickSearch(const std::wstring& keystr);
        void Search(const FilterCondition& fc, int lmt);
        
        
        std::shared_ptr<DeckCardData> GetCard(int pos, int index);
        std::tuple<int, int, int> GetHoverCard(float x, float y);
        
    protected:
        unsigned int index_buffer = 0;
        unsigned int deck_buffer = 0;
        unsigned int back_buffer = 0;
        unsigned int misc_buffer = 0;
        unsigned int result_buffer = 0;
        bool update_bg = true;
        bool update_card = true;
        bool update_misc = true;
        bool update_result = true;
        int update_status = 0;
        std::tuple<int, int, int> prev_hov;
        std::tuple<int, int, int> prev_click;
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
        bool show_info = false;
        bool show_info_begin = false;
        double show_info_time = 0.0;
        std::array<ti4, 3> limit;
        std::array<ti4, 3> pool;
        ti4 hmask;
        std::unordered_set<std::shared_ptr<DeckCardData>> updating_cards;
        std::weak_ptr<sgui::SGIconLabel> deck_label;
        std::shared_ptr<FileDialog> file_dialog;
        std::shared_ptr<FilterDialog> filter_dialog;
        std::shared_ptr<InfoPanel> info_panel;
        std::vector<CardData*> search_result;
        std::array<ti4, 10> result_tex;
        int result_page = 0;
        int current_sel_result = -1;
        int result_show_size = 0;
        std::weak_ptr<sgui::SGLabel> label_result;
        std::weak_ptr<sgui::SGLabel> label_page;
    };
    
}
#endif

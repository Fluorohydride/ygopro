#ifndef _BUILD_SCENE_HANDLER_H_
#define _BUILD_SCENE_HANDLER_H_

#include "utils/timer.h"
#include "utils/action.h"

#include "../scene_mgr.h"
#include "../gui_extra.h"

namespace ygopro
{
    
    class BuildScene;
    
    class BuildSceneHandler : public SceneHandler {
    public:
        BuildSceneHandler(BuildScene* pscene);
        ~BuildSceneHandler();
        virtual void BeginHandler();
        virtual void EndHandler();
        virtual bool UpdateHandler();
        virtual void MouseMove(int32_t x, int32_t y);
        virtual void MouseButtonDown(int32_t button, int32_t mods, int32_t x, int32_t y);
        virtual void MouseButtonUp(int32_t button, int32_t mods, int32_t x, int32_t y);
        virtual void MouseWheel(float deltax, float deltay);
        virtual void KeyDown(int32_t key, int32_t mods);
        virtual void KeyUp(int32_t key, int32_t mods);
        
        void ViewRegulation(int32_t id);
        void StopViewRegulation() { view_regulation = 0; }
        
        void SetDeckEdited();
        void OnMenuDeck(int32_t id);
        void OnMenuTool(int32_t id);
        void OnMenuList(int32_t id);
        void SetDeckLabel(const std::wstring& str, uint32_t cl);
        
        void ResultPrevPage();
        void ResultNextPage();
        void Search(const FilterCondition& fc, int32_t lmt);
        
    protected:
        BuildScene* build_scene = nullptr;
        
        std::shared_ptr<FileDialog> file_dialog;
        std::shared_ptr<FilterDialog> filter_dialog;
        std::shared_ptr<InfoPanel> info_panel;
        
        std::weak_ptr<sgui::SGLabel> deck_label;
        std::weak_ptr<sgui::SGLabel> label_result;
        std::weak_ptr<sgui::SGLabel> label_page;
        bool deck_edited = false;
        std::wstring current_file;
        int32_t view_regulation = 0;
        std::vector<CardData*> search_result;
        int32_t result_page = 0;
        int32_t page_count = 1;
        
        std::pair<CardLocation, int32_t> hover_pos;
        std::pair<CardLocation, int32_t> click_pos;
        std::weak_ptr<DeckCardData> hover_obj;
        int64_t show_info_time = 0;
        
    };
    
}

#endif

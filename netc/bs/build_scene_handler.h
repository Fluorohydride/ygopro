#ifndef _BUILD_SCENE_HANDLER_H_
#define _BUILD_SCENE_HANDLER_H_

#include "../scene_mgr.h"
#include "../gui_extra.h"

namespace ygopro
{
    
    class BuildScene;
    
    class BuildSceneHandler : public SceneHandler, public Timer<double> {
    public:
        BuildSceneHandler(std::shared_ptr<BuildScene> pscene);
        virtual bool UpdateEvent();
        virtual void BeginHandler();
        void ViewRegulation(int32_t id);
        void StopViewRegulation() { view_regulation = 0; }
        
        void SetDeckEdited();
        void OnMenuDeck(int32_t id);
        void OnMenuTool(int32_t id);
        void OnMenuList(int32_t id);
        void ShowCardInfo(uint32_t code);
        void HideCardInfo();
        void SetDeckLabel(const std::wstring& str, uint32_t cl);
        
        void ResultPrevPage();
        void ResultNextPage();
        void Search(const FilterCondition& fc, int32_t lmt);
        
    protected:
        std::weak_ptr<BuildScene> build_scene;
        
        std::weak_ptr<sgui::SGIconLabel> deck_label;
        std::shared_ptr<FileDialog> file_dialog;
        std::shared_ptr<FilterDialog> filter_dialog;
        std::shared_ptr<InfoPanel> info_panel;
        std::weak_ptr<sgui::SGLabel> label_result;
        std::weak_ptr<sgui::SGLabel> label_page;
        bool deck_edited = false;
        std::wstring current_file;
        int32_t view_regulation = 0;
        std::vector<CardData*> search_result;
        int32_t result_page = 0;
        
    };
    
}

#endif

#ifndef _GUI_EXTRA_H_
#define _GUI_EXTRA_H_

#include <string>
#include <functional>

#include "sungui.h"

namespace ygopro
{
    
    class CommonDialog {
    public:
        virtual ~CommonDialog() {
            if(!window.expired())
                window.lock()->Destroy();
        }
    protected:
        std::weak_ptr<sgui::SGWindow> window;
    };
    
    class FileDialog : public CommonDialog {
    public:
        void Show(const std::wstring& title, const std::wstring& root, const std::wstring& filter);
        void RefreshList(sgui::SGListBox* list);
        void SetOKCallback(std::function<void (const std::wstring&)> cb = nullptr) { cbOK = cb; }
        
    protected:
        std::wstring root;
        std::wstring path;
        std::wstring filter;
        std::function<void (const std::wstring&)> cbOK;
    };
    
    struct FilterCondition;
    
    class FilterDialog : public CommonDialog {
    public:
        void Show(FilterCondition& fc);
        void SetOKCallback(std::function<void (const FilterCondition&)> cb = nullptr) { cbOK = cb; }
        
    protected:
        std::function<void (const FilterCondition&)> cbOK;
    };
    
}

#endif

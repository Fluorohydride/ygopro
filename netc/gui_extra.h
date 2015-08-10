#ifndef _GUI_EXTRA_H_
#define _GUI_EXTRA_H_

#include "sgui.h"

namespace ygopro
{

    class CommonDialog {
    public:
        virtual ~CommonDialog() {
            if(!window.expired())
                window.lock()->RemoveFromParent();
        }
        
        std::weak_ptr<sgui::SGWidgetContainer> window;
    };
    
    class MessageBox {
    public:
        static void ShowOK(const std::wstring& title, const std::wstring msg, std::function<void ()> cb);
        static void ShowOKCancel(const std::wstring& title, const std::wstring msg, std::function<void ()> cb1, std::function<void ()> cb2);
        static void ShowYesNo(const std::wstring& title, const std::wstring msg, std::function<void ()> cb1, std::function<void ()> cb2);

    };
    
    class PopupMenu {
    private:
        PopupMenu() {};
    public:
        PopupMenu& AddButton(const std::wstring& text, intptr_t cval);
        void End();
        
    private:
        v2i pos;
        int32_t item_count = 0;
        int32_t item_width = 0;
        int32_t item_height = 0;
        recti margin;
        std::function<void (int32_t)> btn_cb;
        sgui::SGPanel* pnl = nullptr;
        
    public:
        static PopupMenu& Create(v2i pos, std::function<void (int32_t)> cb = nullptr);
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
        void Show(v2i pos);
        void SetOKCallback(std::function<void (const FilterCondition&, int32_t)> cb = nullptr) { cbOK = cb; }
        void BeginSearch();
        void ClearCondition();
        static std::tuple<int32_t, int32_t, int32_t> ParseValue(const std::wstring& valstr);
        static int32_t ParseInt(const wchar_t* p, int32_t size);
        
    protected:
        std::function<void (const FilterCondition&, int32_t)> cbOK;
        sgui::SGTextEdit* keyword = nullptr;
        sgui::SGComboBox* arctype = nullptr;
        sgui::SGComboBox* subtype = nullptr;
        sgui::SGComboBox* limit_type = nullptr;
        sgui::SGComboBox* pool_type = nullptr;
        sgui::SGComboBox* attribute = nullptr;
        sgui::SGComboBox* race = nullptr;
        sgui::SGTextEdit* attack = nullptr;
        sgui::SGTextEdit* defence = nullptr;
        sgui::SGTextEdit* star = nullptr;
        sgui::SGTextEdit* scale = nullptr;
        std::wstring con_text[5];
        int32_t sel[6] = {0};
    };

    class InfoPanel : public CommonDialog {
    public:
        void ShowInfo(uint32_t code, v2i pos);
        void Destroy();
        
    protected:
        uint32_t code = 0;
        sgui::SGImage* card_image = nullptr;
        sgui::SGScrollArea* scroll_area = nullptr;
        sgui::SGImageList* misc_image = nullptr;
        sgui::SGLabel* info_text = nullptr;
        sgui::SGLabel* pen_text = nullptr;
        sgui::SGLabel* desc_text = nullptr;
    };
}

#endif

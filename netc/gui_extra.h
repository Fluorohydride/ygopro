#ifndef _GUI_EXTRA_H_
#define _GUI_EXTRA_H_

#include "base/sungui.h"

namespace ygopro
{

    class CommonDialog {
    public:
        virtual ~CommonDialog() {
            if(!window.expired())
                window.lock()->Destroy();
        }
    protected:
        std::weak_ptr<sgui::SGWidgetContainer> window;
    };
    
    class MessageBox {
    public:
        static void ShowOK(const std::wstring& title, const std::wstring& text, std::function<void ()> cb = nullptr);
        static void ShowOKCancel(const std::wstring& title, const std::wstring& text, std::function<void ()> cb1 = nullptr, std::function<void ()> cb2 = nullptr);
        static void ShowYesNo(const std::wstring& title, const std::wstring& text, std::function<void ()> cb1 = nullptr, std::function<void ()> cb2 = nullptr);
    };
    
    class PopupMenu {
    private:
        PopupMenu() {};
    public:
        PopupMenu& AddButton(const std::wstring& btn, int32_t id = 0);
        void End();
    private:
        v2i pos;
        int32_t width;
        std::function<void (int32_t)> cb;
        std::vector<std::wstring> items;
        std::vector<int32_t> ids;
    public:
        static PopupMenu& Begin(v2i pos, int32_t width, std::function<void (int32_t)> cb = nullptr);
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
        std::weak_ptr<sgui::SGTextEdit> keyword;
        std::weak_ptr<sgui::SGComboBox> type1;
        std::weak_ptr<sgui::SGComboBox> type2;
        std::weak_ptr<sgui::SGComboBox> type3;
        std::weak_ptr<sgui::SGComboBox> attribute;
        std::weak_ptr<sgui::SGComboBox> race;
        std::weak_ptr<sgui::SGTextEdit> attack;
        std::weak_ptr<sgui::SGTextEdit> defence;
        std::weak_ptr<sgui::SGTextEdit> star;
        std::wstring con_text[4];
        uint32_t sel[5] = {0};
    };

    class InfoPanel : public CommonDialog {
    public:
        void ShowInfo(uint32_t code, v2i pos, v2i sz);
        void Destroy();
        
    protected:
        uint32_t code = 0;
        std::weak_ptr<sgui::SGSprite> imageBox;
        std::weak_ptr<sgui::SGSprite> misc;
        std::weak_ptr<sgui::SGLabel> cardName;
        std::weak_ptr<sgui::SGLabel> penText;
        std::weak_ptr<sgui::SGLabel> cardText;
        std::weak_ptr<sgui::SGLabel> adText;
        std::weak_ptr<sgui::SGLabel> extraText;
    };
}

#endif

#ifndef _GUI_EXTRA_H_
#define _GUI_EXTRA_H_

#include "sgui.h"

namespace ygopro
{

    sgui::SGWidgetContainer* LoadDialog(const std::string& templ);
    template<typename T>
    T* LoadDialogAs(const std::string& templ) { return dynamic_cast<T*>(LoadDialog(templ)); }
    
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
        static void ShowOK(const std::wstring& title, const std::wstring& msg, std::function<void ()> cb);
        static void ShowOKCancel(const std::wstring& title, const std::wstring& msg, std::function<void ()> cb1, std::function<void ()> cb2);
        static void ShowYesNo(const std::wstring& title, const std::wstring& msg, std::function<void ()> cb1, std::function<void ()> cb2);
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
        static PopupMenu& Load(const std::string& name, v2i pos, std::function<void (int32_t)> cb = nullptr);
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
        void ShowExtraInfo(bool show);
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
        sgui::SGTextEdit* pscale = nullptr;
        sgui::SGLabel* extra_label[6] = {nullptr};
        std::wstring con_text[5];
        int32_t sel[6] = {0};
    };

    class InfoPanel : public CommonDialog, public Singleton<InfoPanel> {
    public:
        void ShowInfo(uint32_t code);
        void Destroy();
        inline bool IsOpen() { return !window.expired(); }
        
    protected:
        uint32_t code = 0;
        bool lock_panel = false;
        sgui::SGImage* card_image = nullptr;
        sgui::SGScrollArea* scroll_area = nullptr;
        sgui::SGImageList* misc_image = nullptr;
        sgui::SGLabel* info_text = nullptr;
        sgui::SGLabel* pen_text = nullptr;
        sgui::SGLabel* desc_text = nullptr;
    };
    
    class LogPanel : CommonDialog {
    public:
        void Show(int32_t type);
        void AddLog(int32_t msg_type, const std::wstring& msg, bool newlog = true);

    protected:
        uint32_t default_item_color = 0xffffffff;
        std::list<std::wstring> logs[3];
        sgui::SGTabControl* tabs = nullptr;
        sgui::SGListBox* log_list[3] = {nullptr, nullptr, nullptr};
        sgui::SGTextEdit* chat_box = nullptr;
        
    public:
        static const int32_t msg_type_chat = 0;
        static const int32_t msg_type_duel = 1;
        static const int32_t msg_type_system = 2;
    };
    
    class FieldCard;
    
    class OperationPanel {
    public:
        static void Confirm(const std::vector<FieldCard*>& cards, std::function<void()> close_callback = nullptr);
        static void DeclearAttribute(int32_t available, int32_t count, std::function<void(uint32_t)> close_callback = nullptr);
        static void DeclearRace(int32_t available, int32_t count, std::function<void(uint32_t)> close_callback = nullptr);
    };
}

#endif

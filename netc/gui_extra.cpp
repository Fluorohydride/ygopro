#include <wx/dir.h>
#include <wx/filename.h>

#include "gui_extra.h"
#include "scene_mgr.h"

namespace ygopro
{
    
    void MessageBox::ShowOK(const std::wstring& title, const std::wstring& text, std::function<void ()> cb) {
        auto sz = sgui::SGGUIRoot::GetSingleton().GetSceneSize();
        auto wd = sgui::SGWindow::Create(nullptr, {0, 0}, {0, 0});
        wd->GetCloseButton()->SetVisible(false);
        wd->SetText(title, 0xff000000);
        auto label = sgui::SGLabel::Create(wd, {10, 30}, text);
        auto lbsz = label->GetSize();
        if(lbsz.x < 100)
            lbsz.x = 100;
        wd->SetSize({lbsz.x + 20, lbsz.y + 80});
        wd->SetPosition({sz.x / 2 - lbsz.x / 2 - 10, sz.y / 2 - lbsz.y / 2 - 40});
        auto btn = sgui::SGButton::Create(wd, {lbsz.x / 2 + 10 - 30, 45 + lbsz.y}, {60, 25});
        btn->SetText(stringCfg[L"eui_button_ok"], 0xff000000);
        auto ptr = wd.get();
        btn->eventButtonClick.Bind([ptr, cb](sgui::SGWidget& sender)->bool {
            if(cb != nullptr)
                cb();
            ptr->Destroy();
            return true;
        });
    }

    void MessageBox::ShowOKCancel(const std::wstring& title, const std::wstring& text, std::function<void ()> cb1, std::function<void ()> cb2) {
        auto sz = sgui::SGGUIRoot::GetSingleton().GetSceneSize();
        auto wd = sgui::SGWindow::Create(nullptr, {0, 0}, {0, 0});
        wd->GetCloseButton()->SetVisible(false);
        wd->SetText(title, 0xff000000);
        auto label = sgui::SGLabel::Create(wd, {10, 30}, text);
        auto lbsz = label->GetSize();
        if(lbsz.x < 160)
            lbsz.x = 160;
        wd->SetSize({lbsz.x + 20, lbsz.y + 80});
        wd->SetPosition({sz.x / 2 - lbsz.x / 2 - 10, sz.y / 2 - lbsz.y / 2 - 40});
        auto ptr = wd.get();
        auto btnOK = sgui::SGButton::Create(wd, {lbsz.x / 2 + 10 - 70, 45 + lbsz.y}, {60, 25});
        btnOK->SetText(stringCfg[L"eui_button_ok"], 0xff000000);
        btnOK->eventButtonClick.Bind([ptr, cb1](sgui::SGWidget& sender)->bool {
            if(cb1 != nullptr)
                cb1();
            ptr->Destroy();
            return true;
        });
        auto btnCancel = sgui::SGButton::Create(wd, {lbsz.x / 2 + 10 + 10, 45 + lbsz.y}, {60, 25});
        btnCancel->SetText(stringCfg[L"eui_button_cancel"], 0xff000000);
        btnCancel->eventButtonClick.Bind([ptr, cb2](sgui::SGWidget& sender)->bool {
            if(cb2 != nullptr)
                cb2();
            ptr->Destroy();
            return true;
        });
    }
    
    void MessageBox::ShowYesNo(const std::wstring& title, const std::wstring& text, std::function<void ()> cb1, std::function<void ()> cb2) {
        auto sz = sgui::SGGUIRoot::GetSingleton().GetSceneSize();
        auto wd = sgui::SGWindow::Create(nullptr, {0, 0}, {0, 0});
        wd->GetCloseButton()->SetVisible(false);
        wd->SetText(title, 0xff000000);
        auto label = sgui::SGLabel::Create(wd, {10, 30}, text);
        auto lbsz = label->GetSize();
        if(lbsz.x < 160)
            lbsz.x = 160;
        wd->SetSize({lbsz.x + 20, lbsz.y + 80});
        wd->SetPosition({sz.x / 2 - lbsz.x / 2 - 10, sz.y / 2 - lbsz.y / 2 - 40});
        auto ptr = wd.get();
        auto btnOK = sgui::SGButton::Create(wd, {lbsz.x / 2 + 10 - 70, 45 + lbsz.y}, {60, 25});
        btnOK->SetText(stringCfg[L"eui_button_yes"], 0xff000000);
        btnOK->eventButtonClick.Bind([ptr, cb1](sgui::SGWidget& sender)->bool {
            if(cb1 != nullptr)
                cb1();
            ptr->Destroy();
            return true;
        });
        auto btnCancel = sgui::SGButton::Create(wd, {lbsz.x / 2 + 10 + 10, 45 + lbsz.y}, {60, 25});
        btnCancel->SetText(stringCfg[L"eui_button_no"], 0xff000000);
        btnCancel->eventButtonClick.Bind([ptr, cb2](sgui::SGWidget& sender)->bool {
            if(cb2 != nullptr)
                cb2();
            ptr->Destroy();
            return true;
        });
    }
    
    void FileDialog::Show(const std::wstring& title, const std::wstring& root, const std::wstring& filter) {
        if(!window.expired())
            return;
        this->root = root;
        this->path = root;
        this->filter = filter;
        if(!wxDirExists(root) && !wxMkdir(root))
            return;
        auto sz = sgui::SGGUIRoot::GetSingleton().GetSceneSize();
        auto wd = sgui::SGWindow::Create(nullptr, {sz.x / 2 - 150, sz.y / 2 - 200}, {300, 330});
        wd->SetText(title, 0xff000000);
        auto fpath = sgui::SGTextEdit::Create(wd, {10, 25}, {280, 30});
        fpath->SetReadOnly(true);
        fpath->SetText(root, 0xff000000);
        auto lst = sgui::SGListBox::Create(wd, {10, 55}, {280, 200});
        auto ffile = sgui::SGTextEdit::Create(wd, {10, 255}, {280, 30});
        auto btn = sgui::SGButton::Create(wd, {190, 290}, {100, 25});
        btn->SetText(stringCfg[L"eui_button_ok"], 0xff000000);
        window = wd;
        auto ppath = fpath.get();
        auto pfile = ffile.get();
        auto plst = lst.get();
        btn->eventButtonClick.Bind([this, pfile](sgui::SGWidget& sender)->bool {
            auto file = pfile->GetText();
            if(file.length() == 0)
                return true;
            if(cbOK != nullptr)
                cbOK(path + L"/" + file);
            window.lock()->Destroy();
            return true;
        });
        lst->eventSelChange.Bind([this, pfile, plst](sgui::SGWidget& sender, int index)->bool {
            if(index < 0)
                return true;
            auto it = plst->GetItem(index);
            if(std::get<0>(it) == 141)
                pfile->SetText(std::get<1>(it), 0xff000000);
            return true;
        });
        lst->eventDoubleClick.Bind([this, pfile, plst, ppath](sgui::SGWidget& sender, int index)->bool {
            auto it = plst->GetItem(index);
            if(std::get<0>(it) == 142) {
                int pos = path.rfind(L'/');
                path = path.substr(0, pos);
                ppath->SetText(path, 0xff000000);
                RefreshList(plst);
            } else if(std::get<0>(it) == 140) {
                path.append(L"/").append(std::get<1>(it));
                ppath->SetText(path, 0xff000000);
                RefreshList(plst);
            } else {
                if(cbOK != nullptr)
                    cbOK(path + L"/" + pfile->GetText());
                window.lock()->Destroy();
            }
            return true;
        });
        RefreshList(plst);
    }
    
    void FileDialog::RefreshList(sgui::SGListBox* list) {
        wxDir dir;
        if(!dir.Open(path))
            return;
        wxArrayString dirs;
        wxArrayString files;
        wxString fn;
        wxString prefix = path + "/";
        bool res = dir.GetFirst(&fn, wxEmptyString, wxDIR_FILES | wxDIR_DIRS);
        while(res) {
            if(wxFileName::DirExists(prefix + fn))
                dirs.Add(fn);
            else {
                if(filter.size() && fn.Right(filter.size()) == filter)
                    files.Add(fn);
            }
            res = dir.GetNext(&fn);
        }
        dirs.Sort();
        files.Sort();
        list->ClearItem();
        if(path != root)
            list->AddItem(142, stringCfg[L"eui_updir"], 0xff000000);
        for(size_t i = 0; i < dirs.GetCount(); ++i)
            list->AddItem(140, dirs[i].ToStdWstring(), 0xff000000);
        for(size_t i = 0; i < files.GetCount(); ++i)
            list->AddItem(141, files[i].ToStdWstring(), 0xff000000);
    }
    
    void FilterDialog::Show() {
        if(!window.expired())
            return;
        auto sz = sgui::SGGUIRoot::GetSingleton().GetSceneSize();
        auto wd = sgui::SGWindow::Create(nullptr, {sz.x / 2 - 150, sz.y / 2 - 200}, {300, 300});
        wd->SetText(stringCfg[L"eui_filter_title"], 0xff000000);
        auto label1 = sgui::SGLabel::Create(wd, {10, 35}, stringCfg[L"eui_filter_keyword"]);
        keyword = sgui::SGTextEdit::Create(wd, {90, 30}, {200, 30});
        auto label2 = sgui::SGLabel::Create(wd, {10, 60}, stringCfg[L"eui_filter_type"]);
        auto ptype1 = sgui::SGComboBox::Create(wd, {90, 55}, {180, 30});
        type1 = ptype1;
        ptype1->AddItem(stringCfg[L"eui_filter_na"], 0xff000000, 0);
        ptype1->AddItem(dataMgr.GetTypeString2(0x1), 0xff000000, 0x1);
        ptype1->AddItem(dataMgr.GetTypeString2(0x2), 0xff000000, 0x2);
        ptype1->AddItem(dataMgr.GetTypeString2(0x4), 0xff000000, 0x4);
        ptype1->SetSelection(0);
        auto ptype2 = sgui::SGComboBox::Create(wd, {90, 80}, {180, 30});
        type2 = ptype2;
        ptype2->AddItem(stringCfg[L"eui_filter_na"], 0xff000000, 0);
        for(unsigned int i = 0x10; i != 0x2000000; i <<=1)
            if(i != 0x4000 && i != 0x8000)
                ptype2->AddItem(dataMgr.GetTypeString2(i), 0xff000000, i);
        ptype2->SetSelection(0);
        auto label3 = sgui::SGLabel::Create(wd, {10, 110}, stringCfg[L"eui_filter_limit"]);
        type3 = sgui::SGComboBox::Create(wd, {90, 105}, {180, 30});
        auto label4 = sgui::SGLabel::Create(wd, {10, 135}, stringCfg[L"eui_filter_attribute"]);
        auto pattribute = sgui::SGComboBox::Create(wd, {90, 130}, {180, 30});
        attribute = pattribute;
        pattribute->AddItem(stringCfg[L"eui_filter_na"], 0xff000000, 0);
        for(unsigned int i = 1; i != 0x80; i <<=1)
            pattribute->AddItem(dataMgr.GetAttributeString(i), 0xff000000);
        pattribute->SetSelection(0);
        auto label5 = sgui::SGLabel::Create(wd, {10, 160}, stringCfg[L"eui_filter_race"]);
        auto prace = sgui::SGComboBox::Create(wd, {90, 155}, {180, 30});
        race = prace;
        prace->AddItem(stringCfg[L"eui_filter_na"], 0xff000000, 0);
        for(unsigned int i = 1; i != 0x1000000; i <<=1)
            prace->AddItem(dataMgr.GetRaceString(i), 0xff000000);
        prace->SetSelection(0);
        auto label6 = sgui::SGLabel::Create(wd, {10, 185}, stringCfg[L"eui_filter_attack"]);
        attack = sgui::SGTextEdit::Create(wd, {90, 180}, {200, 30});
        auto label7 = sgui::SGLabel::Create(wd, {10, 210}, stringCfg[L"eui_filter_defence"]);
        defence = sgui::SGTextEdit::Create(wd, {90, 205}, {200, 30});
        auto label8 = sgui::SGLabel::Create(wd, {10, 235}, stringCfg[L"eui_filter_star"]);
        star = sgui::SGTextEdit::Create(wd, {90, 230}, {200, 30});
        auto sch = sgui::SGButton::Create(wd, {190, 260}, {100, 25});
        sch->SetText(stringCfg[L"eui_filter_search"], 0xff000000);
        sch->eventButtonClick.Bind([this](sgui::SGWidget& sender)->bool {
            BeginSearch();
            return true;
        });
    }
    
    void FilterDialog::BeginSearch() {
        FilterCondition fc;
        if(cbOK != nullptr)
            cbOK(fc);
    }
    
    std::pair<int, int> FilterDialog::ParseValue(const std::wstring& valstr) {
        return std::make_pair(0, 0);
    }
    
}

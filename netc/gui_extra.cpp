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
    
    PopupMenu& PopupMenu::AddButton(const std::wstring& btn, int id) {
        if(id == 0)
            id = ids.size();
        items.push_back(btn);
        ids.push_back(id);
        return *this;
    }
    
    void PopupMenu::End() {
        auto pnl = sgui::SGPanel::Create(nullptr, pos, {width, (int)(25 * items.size() + 9)});
        pnl->eventDestroying.Bind([this](sgui::SGWidget& sender)->bool {
            delete this;
            return true;
        });
        auto ptr = pnl.get();
        for(size_t i = 0; i < items.size(); ++i) {
            auto btn = sgui::SGButton::Create(pnl, {3, (int)(5 + 25 * i)}, {0, width});
            btn->SetSize({-10, 25}, {1.0f, 0.0f});
            btn->SetText(items[i], 0xff000000);
            btn->SetCustomValue(ids[i]);
            btn->eventButtonClick.Bind([this, ptr](sgui::SGWidget& sender)->bool {
                if(cb != nullptr)
                    cb(sender.GetCustomValue());
                ptr->Destroy();
                return true;
            });
        }
        sgui::SGGUIRoot::GetSingleton().SetPopupObject(pnl);
    }
    
    PopupMenu& PopupMenu::Begin(sgui::v2i pos, int width, std::function<void (int)> cb) {
        PopupMenu* menu = new PopupMenu();
        menu->pos = pos;
        menu->width = width;
        menu->cb = cb;
        return *menu;
    }
    
    void SearchMenu::Create(sgui::v2i pos, std::function<void (const std::wstring&)> cb1, std::function<void()> cb2) {
        auto pnl = sgui::SGPanel::Create(nullptr, pos, {200, 100});
        auto ppnl = pnl.get();
        auto keyword = sgui::SGTextEdit::Create(pnl, {5, 5}, {185, 30});
        auto pkeyword = keyword.get();
        auto quick = sgui::SGButton::Create(pnl, {35, 35}, {120, 25});
        quick->SetText(stringCfg[L"eui_search_quick"], 0xff000000);
        quick->eventButtonClick.Bind([cb1, ppnl, pkeyword](sgui::SGWidget& sender)->bool {
            if(cb1)
                cb1(pkeyword->GetText());
            ppnl->Destroy();
            return true;
        });
        auto filter = sgui::SGButton::Create(pnl, {5, 70}, {185, 25});
        filter->SetText(stringCfg[L"eui_search_filter"], 0xff000000);
        filter->eventButtonClick.Bind([cb2, ppnl](sgui::SGWidget& sender)->bool {
            if(cb2)
                cb2();
            ppnl->Destroy();
            return true;
        });
        keyword->SetFocus();
        sgui::SGGUIRoot::GetSingleton().SetPopupObject(pnl);
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
        auto ptype3 = sgui::SGComboBox::Create(wd, {90, 105}, {180, 30});
        type3 = ptype3;
        ptype3->AddItem(stringCfg[L"eui_filter_na"], 0xff000000, 0);
        ptype3->AddItem(stringCfg[L"pool_limit0"], 0xff000000, 1);
        ptype3->AddItem(stringCfg[L"pool_limit1"], 0xff000000, 2);
        ptype3->AddItem(stringCfg[L"pool_limit2"], 0xff000000, 3);
        ptype3->AddItem(stringCfg[L"pool_ocg"], 0xff000000, 0x1);
        ptype3->AddItem(stringCfg[L"pool_tcg"], 0xff000000, 0x2);
        ptype3->SetSelection(0);
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
        auto keystr = keyword.lock()->GetText();
        if(keystr.length() > 0) {
            if(keystr[0] == L'@') {
                fc.code = ParseInt(&keystr[1], keystr.length() - 1);
            } else if(keystr[0] == L'#') {
                std::wstring setstr = L"setname_";
                setstr.append(keystr.substr(1));
                if(stringCfg.Exists(setstr))
                    fc.setcode = stringCfg[setstr];
                else
                    fc.setcode = 0xffff;
            } else
                fc.keyword = keystr;
        }
        fc.type = type1.lock()->GetSelectedValue();
        fc.subtype = type2.lock()->GetSelectedValue();
        int lmt = type3.lock()->GetSelection();
        if(lmt > 3) {
            fc.pool = type3.lock()->GetSelectedValue();
            lmt = 0;
        }
        if(fc.type == 0x1) {
            fc.attribute = attribute.lock()->GetSelectedValue();
            fc.race = race.lock()->GetSelectedValue();
            auto t1 = ParseValue(attack.lock()->GetText());
            switch(std::get<0>(t1)) {
                case 0: break;
                case 1: fc.atkmin = fc.atkmax = -2; break;
                case 2: fc.atkmin = fc.atkmax = std::get<1>(t1); break;
                case 3: fc.atkmin = std::get<1>(t1); fc.atkmax = std::get<2>(t1); break;
                default: break;
            }
            auto t2 = ParseValue(defence.lock()->GetText());
            switch(std::get<0>(t2)) {
                case 0: break;
                case 1: fc.defmin = fc.defmax = -2; break;
                case 2: fc.defmin = fc.defmax = std::get<1>(t2); break;
                case 3: fc.defmin = std::get<1>(t2); fc.defmax = std::get<2>(t2); break;
                default: break;
            }
            auto t3 = ParseValue(star.lock()->GetText());
            switch(std::get<0>(t2)) {
                case 0: case 1: break;
                case 2: fc.lvmin = fc.lvmax = std::get<1>(t3); break;
                case 3: fc.lvmin = std::get<1>(t3); fc.lvmax = std::get<2>(t3); break;
                default: break;
            }
        }
        if(cbOK != nullptr)
            cbOK(fc, lmt);
    }
    
    std::tuple<int, int, int> FilterDialog::ParseValue(const std::wstring& valstr) {
        if(valstr.length() == 0)
            return std::make_tuple(0, 0, 0);
        if(valstr == L"?")
            return std::make_tuple(1, 0, 0);
        size_t pos = valstr.find(L':');
        if(pos == valstr.npos)
            return std::make_tuple(2, ParseInt(&valstr[1], valstr.length()), 0);
        else
            return std::make_tuple(3, ParseInt(&valstr[0], pos), ParseInt(&valstr[pos + 1], valstr.length() - pos - 1));
    }
    
    int FilterDialog::ParseInt(const wchar_t* p, int size) {
        int v = 0;
        for(size_t i = 0; i < size; ++i) {
            if(p[i] >= L'0' && p[i] <= L'9') {
                v = v * 10 + p[i] - L'0';
            } else
                return v;
        }
        return v;
    }
}

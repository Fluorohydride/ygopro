#include "utils/common.h"
#include "utils/filesystem.h"

#include "config.h"
#include "card_data.h"
#include "scene_mgr.h"
#include "image_mgr.h"
#include "gui_extra.h"

namespace ygopro
{
    
    sgui::SGWindow* GenMessageBox(const std::wstring& title, const std::wstring& text, int32_t min_size) {
        auto wd = sgui::SGGUIRoot::GetSingleton().NewChild<sgui::SGWindow>();
        wd->SetCloseButtonVisible(false);
        wd->GetCaption()->SetText(title, 0xff000000);
        auto label = wd->NewChild<sgui::SGLabel>();
        label->GetTextUI()->SetText(text, 0xff000000);
        auto lbsz = label->GetAbsoluteSize();
        if(lbsz.x < 100)
            lbsz.x = 100;
        wd->SetSize({lbsz.x + 20, lbsz.y + 80});
        wd->SetPosition({0, 0}, {0.5f, 0.5f}, {-0.5f, -0.5f});
        return wd;
    }
    
    sgui::SGTextButton* MessageBoxAddButton(sgui::SGWindow* wd, const std::wstring text, std::function<void ()> cb) {
        auto btn = wd->NewChild<sgui::SGTextButton>();
        btn->GetTextUI()->SetText(text, 0xff000000);
        btn->event_click += [wd, cb](sgui::SGWidget& sender)->bool {
            if(cb) cb();
            wd->RemoveFromParent();
            return true;
        };
        return btn;
    }
    
    void MessageBox::ShowOK(const std::wstring& title, const std::wstring& text, std::function<void ()> cb) {
        auto wd = GenMessageBox(title, text, 100);
        auto btn = MessageBoxAddButton(wd, To<std::wstring>(stringCfg["eui_button_ok"].to_string()), cb);
        btn->SetPositionSize({0, -10}, {100, 40}, {0.5f, 1.0f}, {0.0f, 0.0f}, {-0.5f, -1.0f});
    }

    void MessageBox::ShowOKCancel(const std::wstring& title, const std::wstring& text, std::function<void ()> cb1, std::function<void ()> cb2) {
        auto wd = GenMessageBox(title, text, 160);
        auto btnOK = MessageBoxAddButton(wd, To<std::wstring>(stringCfg["eui_button_ok"].to_string()), cb1);
        btnOK->SetPositionSize({0, -10}, {100, 40}, {0.3f, 1.0f}, {0.0f, 0.0f}, {-0.5f, -1.0f});
        auto btnCancel = MessageBoxAddButton(wd, To<std::wstring>(stringCfg["eui_button_cancel"].to_string()), cb2);
        btnCancel->SetPositionSize({0, -10}, {100, 40}, {0.7f, 1.0f}, {0.0f, 0.0f}, {-0.5f, -1.0f});
    }
    
    void MessageBox::ShowYesNo(const std::wstring& title, const std::wstring& text, std::function<void ()> cb1, std::function<void ()> cb2) {
        auto wd = GenMessageBox(title, text, 160);
        auto btnYes = MessageBoxAddButton(wd, To<std::wstring>(stringCfg["eui_button_yes"].to_string()), cb1);
        btnYes->SetPositionSize({0, -10}, {100, 40}, {0.3f, 1.0f}, {0.0f, 0.0f}, {-0.5f, -1.0f});
        auto btnNo = MessageBoxAddButton(wd, To<std::wstring>(stringCfg["eui_button_no"].to_string()), cb2);
        btnNo->SetPositionSize({0, -10}, {100, 40}, {0.7f, 1.0f}, {0.0f, 0.0f}, {-0.5f, -1.0f});
    }
    
    PopupMenu& PopupMenu::AddButton(const std::wstring& btn, int32_t id) {
        if(id == 0)
            id = (int32_t)ids.size();
        items.push_back(btn);
        ids.push_back(id);
        return *this;
    }
    
    void PopupMenu::End() {
        auto pnl = sgui::SGGUIRoot::GetSingleton().NewChild<sgui::SGPanel>();
        pnl->event_on_destroy += [this](sgui::SGWidget& sender)->bool {
            delete this;
            return true;
        };
        for(size_t i = 0; i < items.size(); ++i) {
            auto btn = pnl->NewChild<sgui::SGTextButton>();
            btn->SetPositionSize({10, 10 + (int32_t)i * 40}, {-20, 35}, {0.0f, 0.0f}, {1.0f, 0.0f});
            btn->GetTextUI()->SetText(items[i], 0xff000000);
            btn->SetCustomValue(ids[i]);
            btn->event_click += [this, pnl](sgui::SGWidget& sender)->bool {
                if(cb != nullptr)
                    cb(static_cast<int32_t>(sender.GetCustomValue()));
                pnl->RemoveFromParent();
                return true;
            };
        }
        sgui::SGGUIRoot::GetSingleton().PopupObject(pnl->shared_from_this());
    }
    
    PopupMenu& PopupMenu::Create(v2i pos, int32_t width, std::function<void (int32_t)> cb) {
        PopupMenu* menu = new PopupMenu();
        menu->pos = pos;
        menu->width = width;
        menu->cb = cb;
        return *menu;
    }
    
    void FileDialog::Show(const std::wstring& title, const std::wstring& root, const std::wstring& filter) {
        if(!window.expired())
            return;
        this->root = root;
        this->path = root;
        this->filter = filter;
        if(!FileSystem::IsDirExists(root) && !FileSystem::MakeDir(root))
            return;
        auto wnd = sgui::SGGUIRoot::GetSingleton().NewChild<sgui::SGWindow>();
        wnd->SetPositionSize({0, 0}, {300, 330}, {0.5f, 0.5f}, {0.0f, 0.0f}, {-0.5f, -0.5f});
        wnd->GetCaption()->SetText(title, 0xff000000);
        auto fpath = wnd->NewChild<sgui::SGTextEdit>();
        fpath->SetPositionSize({10, 25}, {280, 30});
        fpath->SetReadonly(true);
        fpath->GetTextUI()->SetText(root, 0xff000000);
        auto lst = wnd->NewChild<sgui::SGListBox>();
        lst->SetPositionSize({10, 55}, {280, 200});
        auto ffile = wnd->NewChild<sgui::SGTextEdit>();
        ffile->SetPositionSize({10, 255}, {280, 30});
        auto btn = wnd->NewChild<sgui::SGTextButton>();
        btn->SetPositionSize({190, 290}, {100, 25});
        btn->GetTextUI()->SetText(To<std::wstring>(stringCfg["eui_button_ok"].to_string()), 0xff000000);
        btn->event_click.Bind([this, wnd, ffile](sgui::SGWidget& sender)->bool {
            auto file = ffile->GetTextUI()->GetText();
            if(file.length() == 0)
                return true;
            if(cbOK != nullptr)
                cbOK(path + L"/" + file);
            wnd->RemoveFromParent();
            return true;
        });
        lst->event_sel_change.Bind([this, ffile, lst](sgui::SGWidget& sender, int32_t index)->bool {
            if(index < 0)
                return true;
            if(lst->GetItemCustomValue(index) == 141)
                ffile->GetTextUI()->SetText(lst->GetItemText(index), 0xff000000);
            return true;
        });
        lst->event_item_double_click += [this, ffile, lst, fpath](sgui::SGWidget& sender, int32_t index)->bool {
            if(lst->GetItemCustomValue(index) == 142) {
                size_t pos = path.rfind(L'/');
                path = path.substr(0, pos);
                fpath->GetTextUI()->SetText(path, 0xff000000);
                RefreshList(lst);
            } else if(lst->GetItemCustomValue(index) == 140) {
                path.append(L"/").append(lst->GetItemText(index));
                fpath->GetTextUI()->SetText(path, 0xff000000);
                RefreshList(lst);
            } else {
                if(cbOK != nullptr)
                    cbOK(path + L"/" + ffile->GetTextUI()->GetText());
                window.lock()->RemoveFromParent();
            }
            return true;
        };
        RefreshList(lst);
    }
    
    void FileDialog::RefreshList(sgui::SGListBox* list) {
        std::vector<std::wstring> dirs;
        std::vector<std::wstring> files;
        FileSystem::TraversalDir(path, [this, &dirs, &files](const std::wstring& name, bool isdir) {
            if(isdir) {
                if(name != L"." && name != L"..")
                    dirs.push_back(name);
            } else {
                if(name.find(filter) == (name.size() - filter.size()))
                    files.push_back(name);
            }
        });
        std::sort(dirs.begin(), dirs.end());
        std::sort(files.begin(), files.end());
        list->ClearItems();
        if(path != root)
            list->AddItem(To<std::wstring>(stringCfg["eui_updir"].to_string()), 0xff000000, 142);
        for(size_t i = 0; i < dirs.size(); ++i)
            list->AddItem(dirs[i], 0xff000000, 140);
        for(size_t i = 0; i < files.size(); ++i)
            list->AddItem(files[i], 0xff000000, 141);
    }
    
    void FilterDialog::Show(v2i pos) {
        if(!window.expired())
            return;
        auto wd = sgui::SGPanel::Create(nullptr, pos, {250, 300});
        auto label1 = sgui::SGLabel::Create(wd, {10, 15}, stringCfg["eui_filter_keyword"]);
        auto pkeyword = sgui::SGTextEdit::Create(wd, {90, 10}, {150, 30});
        keyword = pkeyword;
        auto label2 = sgui::SGLabel::Create(wd, {10, 40}, stringCfg["eui_filter_type"]);
        auto ptype1 = sgui::SGComboBox::Create(wd, {90, 35}, {130, 30});
        type1 = ptype1;
        ptype1->AddItem(stringCfg["eui_filter_na"], 0xff000000, 0);
        ptype1->AddItem(DataMgr::Get().GetTypeString2(0x1), 0xff000000, 0x1);
        ptype1->AddItem(DataMgr::Get().GetTypeString2(0x2), 0xff000000, 0x2);
        ptype1->AddItem(DataMgr::Get().GetTypeString2(0x4), 0xff000000, 0x4);
        ptype1->SetSelection(0);
        ptype1->eventSelChange.Bind([this](sgui::SGWidget& sender, int32_t index)->bool {
            auto ptr = type2.lock();
            ptr->ClearItem();
            if(index == 0) {
                ptr->AddItem(stringCfg["eui_filter_na"], 0xff000000, 0);
            } else if(index == 1) {
                ptr->AddItem(stringCfg["eui_filter_na"], 0xff000000, 0x1e003ef);
                ptr->AddItem(DataMgr::Get().GetTypeString2(0x10), 0xff000000, 0x10);
                ptr->AddItem(DataMgr::Get().GetTypeString2(0x20), 0xff000000, 0x20);
                ptr->AddItem(DataMgr::Get().GetTypeString2(0x40), 0xff000000, 0x40);
                ptr->AddItem(DataMgr::Get().GetTypeString2(0x80), 0xff000000, 0x80);
                ptr->AddItem(DataMgr::Get().GetTypeString2(0x1000), 0xff000000, 0x1000);
                ptr->AddItem(DataMgr::Get().GetTypeString2(0x2000), 0xff000000, 0x2000);
                ptr->AddItem(DataMgr::Get().GetTypeString2(0x800000), 0xff000000, 0x800000);
                ptr->AddItem(DataMgr::Get().GetTypeString2(0x1000000), 0xff000000, 0x1000000);
                ptr->AddItem(DataMgr::Get().GetTypeString2(0x200000), 0xff000000, 0x200000);
                ptr->AddItem(DataMgr::Get().GetTypeString2(0x800), 0xff000000, 0x800);
                ptr->AddItem(DataMgr::Get().GetTypeString2(0x200), 0xff000000, 0x200);
                ptr->AddItem(DataMgr::Get().GetTypeString2(0x400), 0xff000000, 0x400);
                ptr->AddItem(DataMgr::Get().GetTypeString2(0x400000), 0xff000000, 0x400000);
            } else if(index == 2) {
                ptr->AddItem(stringCfg["eui_filter_na"], 0xff000000, 0xf0082);
                ptr->AddItem(DataMgr::Get().GetTypeString2(0x10), 0xff000000, 0x2);
                ptr->AddItem(DataMgr::Get().GetTypeString2(0x80), 0xff000000, 0x80);
                ptr->AddItem(DataMgr::Get().GetTypeString2(0x10000), 0xff000000, 0x10000);
                ptr->AddItem(DataMgr::Get().GetTypeString2(0x20000), 0xff000000, 0x20000);
                ptr->AddItem(DataMgr::Get().GetTypeString2(0x40000), 0xff000000, 0x40000);
                ptr->AddItem(DataMgr::Get().GetTypeString2(0x80000), 0xff000000, 0x80000);
            } else {
                ptr->AddItem(stringCfg["eui_filter_na"], 0xff000000, 0x120004);
                ptr->AddItem(DataMgr::Get().GetTypeString2(0x10), 0xff000000, 0x4);
                ptr->AddItem(DataMgr::Get().GetTypeString2(0x20000), 0xff000000, 0x20000);
                ptr->AddItem(DataMgr::Get().GetTypeString2(0x100000), 0xff000000, 0x100000);
            }
            ptr->SetSelection(0);
            return true;
        });
        auto ptype2 = sgui::SGComboBox::Create(wd, {90, 60}, {130, 30});
        type2 = ptype2;
        ptype2->AddItem(stringCfg["eui_filter_na"], 0xff000000, 0);
        ptype2->SetSelection(0);
        auto label3 = sgui::SGLabel::Create(wd, {10, 90}, stringCfg["eui_filter_limit"]);
        auto ptype3 = sgui::SGComboBox::Create(wd, {90, 85}, {130, 30});
        type3 = ptype3;
        ptype3->AddItem(stringCfg["eui_filter_na"], 0xff000000, 0);
        ptype3->AddItem(stringCfg["pool_limit0"], 0xff000000, 1);
        ptype3->AddItem(stringCfg["pool_limit1"], 0xff000000, 2);
        ptype3->AddItem(stringCfg["pool_limit2"], 0xff000000, 3);
        ptype3->AddItem(stringCfg["pool_ocg"], 0xff000000, 0x1);
        ptype3->AddItem(stringCfg["pool_tcg"], 0xff000000, 0x2);
        ptype3->SetSelection(0);
        auto label4 = sgui::SGLabel::Create(wd, {10, 115}, stringCfg["eui_filter_attribute"]);
        auto pattribute = sgui::SGComboBox::Create(wd, {90, 110}, {130, 30});
        attribute = pattribute;
        pattribute->AddItem(stringCfg["eui_filter_na"], 0xff000000, 0);
        for(uint32_t i = 1; i != 0x80; i <<=1)
            pattribute->AddItem(DataMgr::Get().GetAttributeString(i), 0xff000000, i);
        pattribute->SetSelection(0);
        auto label5 = sgui::SGLabel::Create(wd, {10, 140}, stringCfg["eui_filter_race"]);
        auto prace = sgui::SGComboBox::Create(wd, {90, 135}, {130, 30});
        race = prace;
        prace->AddItem(stringCfg["eui_filter_na"], 0xff000000, 0);
        for(uint32_t i = 1; i != 0x1000000; i <<=1)
            prace->AddItem(DataMgr::Get().GetRaceString(i), 0xff000000, i);
        prace->SetSelection(0);
        auto label6 = sgui::SGLabel::Create(wd, {10, 165}, stringCfg["eui_filter_attack"]);
        auto pattack = sgui::SGTextEdit::Create(wd, {90, 160}, {150, 30});
        attack = pattack;
        auto label7 = sgui::SGLabel::Create(wd, {10, 190}, stringCfg["eui_filter_defence"]);
        auto pdefence = sgui::SGTextEdit::Create(wd, {90, 185}, {150, 30});
        defence = pdefence;
        auto label8 = sgui::SGLabel::Create(wd, {10, 215}, stringCfg["eui_filter_star"]);
        auto pstar = sgui::SGTextEdit::Create(wd, {90, 210}, {150, 30});
        star = pstar;
        auto sch = sgui::SGButton::Create(wd, {140, 260}, {100, 25});
        sch->SetText(stringCfg["eui_filter_search"], 0xff000000);
        sch->eventButtonClick.Bind([this](sgui::SGWidget& sender)->bool {
            BeginSearch();
            return true;
        });
        auto clr = sgui::SGButton::Create(wd, {10, 260}, {100, 25});
        clr->SetText(stringCfg["eui_filter_clear"], 0xff000000);
        clr->eventButtonClick.Bind([this](sgui::SGWidget& sender)->bool {
            ClearCondition();
            return true;
        });
        pkeyword->SetFocus();
        sgui::SGGUIRoot::GetSingleton().SetPopupObject(wd);
        pkeyword->SetText(con_text[0], 0xff000000);
        pattack->SetText(con_text[1], 0xff000000);
        pdefence->SetText(con_text[1], 0xff000000);
        pstar->SetText(con_text[1], 0xff000000);
        ptype1->SetSelection(sel[0]);
        ptype2->SetSelection(sel[1]);
        ptype3->SetSelection(sel[2]);
        pattribute->SetSelection(sel[3]);
        prace->SetSelection(sel[4]);
    }
    
    void FilterDialog::BeginSearch() {
        FilterCondition fc;
        auto keystr = keyword.lock()->GetText();
        con_text[0] = keystr;
        if(keystr.length() > 0) {
            if(keystr[0] == L'@') {
                fc.code = ParseInt(&keystr[1], keystr.length() - 1);
                if(fc.code == 0)
                    fc.code = 1;
            } else if(keystr[0] == L'#') {
                std::string setstr = "setname_";
                setstr.append(To<std::string>(keystr.substr(1)));
                if(stringCfg.Exists(setstr))
                    fc.setcode = stringCfg[setstr];
                else
                    fc.setcode = 0xffff;
            } else
                fc.keyword = keystr;
        }
        fc.type = type1.lock()->GetSelectedValue();
        fc.subtype = type2.lock()->GetSelectedValue();
        int32_t lmt = type3.lock()->GetSelection();
        sel[0] = type1.lock()->GetSelection();
        sel[1] = type2.lock()->GetSelection();
        sel[2] = lmt;
        if(lmt > 3) {
            fc.pool = type3.lock()->GetSelectedValue();
            lmt = 0;
        }
        if((fc.type == 0) || (fc.type == 0x1)) {
            fc.attribute = attribute.lock()->GetSelectedValue();
            fc.race = race.lock()->GetSelectedValue();
            sel[3] = attribute.lock()->GetSelection();
            sel[4] = race.lock()->GetSelection();
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
            switch(std::get<0>(t3)) {
                case 0: case 1: break;
                case 2: fc.lvmin = fc.lvmax = std::get<1>(t3); break;
                case 3: fc.lvmin = std::get<1>(t3); fc.lvmax = std::get<2>(t3); break;
                default: break;
            }
        }
        if(cbOK != nullptr)
            cbOK(fc, lmt);
    }
    
    void FilterDialog::ClearCondition() {
        keyword.lock()->ClearText();
        attack.lock()->ClearText();
        defence.lock()->ClearText();
        star.lock()->ClearText();
        type1.lock()->SetSelection(0);
        type2.lock()->SetSelection(0);
        type3.lock()->SetSelection(0);
        attribute.lock()->SetSelection(0);
        race.lock()->SetSelection(0);
        for(int32_t i = 0; i < 4; ++i)
            con_text[i].clear();
        for(int32_t i = 0; i < 4; ++i)
            sel[i] = 0;
    }
    
    std::tuple<int32_t, int32_t, int32_t> FilterDialog::ParseValue(const std::wstring& valstr) {
        if(valstr.length() == 0)
            return std::make_tuple(0, 0, 0);
        if(valstr == L"?")
            return std::make_tuple(1, 0, 0);
        size_t pos = valstr.find(L':');
        if(pos == std::wstring::npos)
            return std::make_tuple(2, ParseInt(&valstr[0], valstr.length()), 0);
        else
            return std::make_tuple(3, ParseInt(&valstr[0], pos), ParseInt(&valstr[pos + 1], valstr.length() - pos - 1));
    }
    
    int32_t FilterDialog::ParseInt(const wchar_t* p, int32_t size) {
        int32_t v = 0;
        for(int32_t i = 0; i < size; ++i) {
            if(p[i] >= L'0' && p[i] <= L'9') {
                v = v * 10 + p[i] - L'0';
            } else
                return v;
        }
        return v;
    }
	
    void InfoPanel::ShowInfo(uint32_t code, v2i pos, v2i sz) {
        if(this->code == code)
            return;
        std::shared_ptr<sgui::SGWidgetContainer> wd = window.lock();
        int32_t ch = sz.y - 10;
        int32_t cw = (sz.y - 10) * 20 / 29;
        int32_t mw = sz.x - 20 - cw;
        if(wd == nullptr) {
            wd = sgui::SGPanel::Create(nullptr, pos, sz);
            window = wd;
            imageBox = sgui::SGSprite::Create(wd, {5, 5}, {cw, ch});
            misc = sgui::SGSprite::Create(wd, {cw + 10, 10}, {10, 10});
            cardName = sgui::SGLabel::Create(wd, {cw + 15, 10}, L"", mw);
            penText = sgui::SGLabel::Create(wd, {cw + 42, 60}, L"", mw - 64);
            cardText = sgui::SGLabel::Create(wd, {cw + 15, 60}, L"", mw - 10);
            adText = sgui::SGLabel::Create(wd, {0, sz.y - 30}, L"");
            extraText = sgui::SGLabel::Create(wd, {0, sz.y - 30}, L"");
        }
        this->code = code;
        auto data = DataMgr::Get()[code];
        auto img = imageBox.lock();
        auto ctex = ImageMgr::Get().LoadBigCardTexture(code);
        img->SetImage(ctex, {0, 0, img->GetSize().x, img->GetSize().y});
        if(ctex)
            img->AddTexRect({0, 0, ctex->GetImgWidth(), ctex->GetImgHeight()});
        auto name = cardName.lock();
        name->ClearText();
        name->SetSpacing(1, 5);
        name->AppendText(data->name, 0xff000000);
        name->AppendText(L"\n", 0xff000000);
        name->AppendText(DataMgr::Get().GetTypeString(data->type), 0xff000000);
        if(data->type & 0x1) {
            name->AppendText(L"  ", 0xff000000);
            name->AppendText(DataMgr::Get().GetAttributeString(data->attribute), 0xff000000);
            name->AppendText(L"/", 0xff000000);
            name->AppendText(DataMgr::Get().GetRaceString(data->race), 0xff000000);
        }
        auto extra = extraText.lock();
        extra->ClearText();
        extra->SetSpacing(1, 5);
        extra->SetPosition({cw + 15, sz.y - 50});
        uint64_t setcode = data->setcode;
        if(data->alias) {
            auto aliasdata = DataMgr::Get()[data->alias];
            if(aliasdata)
                setcode = aliasdata->setcode;
        }
        if(setcode) {
            extra->AppendText(stringCfg["eui_msg_setcode"], 0xff000000);
            for(int32_t i = 0; i < 4; ++i) {
                uint16_t sd = (setcode >> (i * 16)) & 0xffff;
                if(sd) {
                    extra->AppendText(L"#", 0xff000000);
                    extra->AppendText(DataMgr::Get().GetSetCode(sd), 0xffff0000);
                    extra->AppendText(L" ", 0xff000000);
                }
            }
        }
        extra->AppendText(L"\n", 0xff000000);
        uint32_t ccode = (data->alias == 0
                              || (data->alias > data->code && data->alias - data->code > 10)
                              || (data->alias < data->code && data->code - data->alias > 10)) ? data->code : data->alias;
        extra->AppendText(L" @", 0xff000000);
        extra->AppendText(To<std::wstring>(To<std::string>("%08d", ccode)), 0xffff0000);
        auto text = cardText.lock();
        auto pent = penText.lock();
        auto ad = adText.lock();
        auto imgs = misc.lock();
        std::vector<v2i> verts;
        std::vector<v2f> coords;
        std::vector<uint32_t> colors;
        auto pushvert = [&verts, &coords, &colors](v2i pos, v2i sz, ti4& ti, uint32_t cl = 0xffffffff) {
            verts.push_back({pos.x, pos.y});
            coords.push_back(ti.vert[0]);
            verts.push_back({pos.x + sz.x, pos.y});
            coords.push_back(ti.vert[1]);
            verts.push_back({pos.x, pos.y + sz.y});
            coords.push_back(ti.vert[2]);
            verts.push_back({pos.x + sz.x, pos.y + sz.y});
            coords.push_back(ti.vert[3]);
            for(int32_t i = 0; i < 4; ++i)
                colors.push_back(cl);
        };
        auto hmask = ImageMgr::Get().GetTexture("mmask");
        auto star = ImageMgr::Get().GetTexture("mstar");
        
        pushvert({0, 0}, {mw, 40}, hmask);
        if(data->type & 0x1) {
            for(uint32_t i = 0; i < data->star; ++i)
                pushvert({(int32_t)(mw - 21 - 16 * i), 20}, {16, 16}, star);
            std::string adstr;
            if(data->attack >= 0)
                adstr.append(To<std::string>("ATK/% 4ld", data->attack));
            else
                adstr.append("ATK/  ? ");
            if(data->defence >= 0)
                adstr.append(To<std::string>(" DEF/% 4ld", data->defence));
            else
                adstr.append(" DEF/  ? ");
            ad->SetText(To<std::wstring>(adstr), 0xff000000);
            ad->SetPosition({sz.x - 15 - ad->GetSize().x, sz.y - 30});
        } else {
            ad->ClearText();
        }
        if(data->lscale || data->rscale) {
            std::wstring pdelimiter = stringCfg["pendulum_delimiter"];
            auto pd = data->texts.find(pdelimiter);
            if(pd > 0)
                pent->SetText(data->texts.substr(0, pd - 1), 0xff000000);
            else
                pent->ClearText();
            text->SetText(data->texts.substr(pd + pdelimiter.length()), 0xff000000);
            int32_t ph = pent->GetSize().y + 13;
            if(ph < 55)
                ph = 55;
            text->SetPosition({cw + 15, 63 + ph});
            pushvert({0, 45}, {mw, ph}, hmask, 0xc0ffffff);
            pushvert({0, 50 + ph}, {mw, sz.y - 70 - ph}, hmask, 0xc0ffffff);
            auto lscale = ImageMgr::Get().GetTexture("lscale");
            auto rscale = ImageMgr::Get().GetTexture("rscale");
            pushvert({0, 50}, {30, 23}, lscale);
            pushvert({mw - 30, 50}, {30, 23}, rscale);
            if(data->lscale >= 10) {
                pushvert({1, 73}, {14, 20}, ImageMgr::Get().GetCharTex(L'0' + (data->lscale / 10)), 0xff000000);
                pushvert({15, 73}, {14, 20}, ImageMgr::Get().GetCharTex(L'0' + (data->lscale % 10)), 0xff000000);
            } else
                pushvert({8, 73}, {14, 20}, ImageMgr::Get().GetCharTex(L'0' + data->lscale), 0xff000000);
            if(data->rscale >= 10) {
                pushvert({mw - 29, 73}, {14, 20}, ImageMgr::Get().GetCharTex(L'0' + (data->rscale / 10)), 0xff000000);
                pushvert({mw - 15, 73}, {14, 20}, ImageMgr::Get().GetCharTex(L'0' + (data->rscale % 10)), 0xff000000);
            } else
                pushvert({mw - 22, 73}, {14, 20}, ImageMgr::Get().GetCharTex(L'0' + data->rscale), 0xff000000);
        } else {
            pushvert({0, 45}, {mw, sz.y - 65}, hmask, 0xc0ffffff);
            pent->SetText(L"", 0xff000000);
            text->SetText(data->texts, 0xff000000);
            text->SetPosition({cw + 15, 60});
        }
        imgs->SetImage(ImageMgr::Get().GetRawMiscTexture(), verts, colors);
        imgs->AddTexcoord(coords);
    }
    
    void InfoPanel::Destroy() {
        if(!window.expired())
            window.lock()->Destroy();
        code = 0;
    }
    
}

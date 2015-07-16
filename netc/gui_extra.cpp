#include "utils/common.h"
#include "utils/filesystem.h"
#include "utils/tokenizer.h"

#include "config.h"
#include "card_data.h"
#include "scene_mgr.h"
#include "image_mgr.h"
#include "gui_extra.h"

namespace ygopro
{
    enum class WidgetType {
        Unknown = 0,
        Window = 1,
        Panel = 2,
        TabControl = 3,
        Label = 4,
        Sprite = 5,
        SimpleButton = 6,
        TextButton = 7,
        ImageButton = 8,
        TextureButton = 9,
        CheckBox = 10,
        Radio = 11,
        ListBox = 12,
        ComboBox = 13,
        TextEdit = 14,
        ScrollBar = 15,
        ScrollArea = 16,
    };
    
    void LoadItemList(sgui::SGItemListWidget* widget, jaweson::JsonNode<>& node) {
        node.for_each([widget](const std::string& name, jaweson::JsonNode<>& item_node) {
            widget->AddItem(To<std::wstring>(stringCfg[name]), (uint32_t)item_node[0].to_integer(), (int32_t)item_node[1].to_integer());
        });
    }
    
    sgui::SGWidget* LoadChild(sgui::SGWidgetContainer* parent, const std::string& child_name, jaweson::JsonNode<>& node) {
        static std::map<std::string, WidgetType> widget_type_id = {
            {"window", WidgetType::Window},
            {"panel", WidgetType::Panel},
            {"tab", WidgetType::TabControl},
            {"label", WidgetType::Label},
            {"sprite", WidgetType::Sprite},
            {"simple button", WidgetType::SimpleButton},
            {"text button", WidgetType::TextButton},
            {"image button", WidgetType::ImageButton},
            {"texture button", WidgetType::TextureButton},
            {"checkbox", WidgetType::CheckBox},
            {"radio", WidgetType::Radio},
            {"listbox", WidgetType::ListBox},
            {"combobox", WidgetType::ComboBox},
            {"textedit", WidgetType::TextEdit},
            {"scrollbar", WidgetType::ScrollBar},
            {"scrollarea", WidgetType::ScrollArea}
        };
        std::map<int32_t, sgui::SGRadio<>*> radio_groups;
        std::string widget_type = node["type"].to_string();
        switch(widget_type_id[widget_type]) {
            case WidgetType::Unknown : break;
            case WidgetType::Window: {
                auto wnd = parent->NewChild<sgui::SGWindow>();
                wnd->SetName(child_name);
                node.for_each([wnd](const std::string& name, jaweson::JsonNode<>& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, wnd, {0, 0});
                    } else if(name == "style") {
                        wnd->SetStyle(sub_node);
                    } else if(name == "caption") {
                        auto text = To<std::wstring>(stringCfg[sub_node[0].to_string()]);
                        wnd->GetCaption()->SetText(text, sgui::SGJsonUtil::ConvertRGBA(sub_node[1]));
                    } else if(name == "close button") {
                        wnd->SetCloseButtonVisible(sub_node.to_bool());
                    } else if(name == "children") {
                        LoadChild(wnd, name, sub_node);
                    }
                });
                return wnd;
            }
            case WidgetType::Panel: {
                auto ent = node["is entity"].to_bool();
                auto pnl = parent->NewChild<sgui::SGPanel>(ent);
                pnl->SetName(child_name);
                node.for_each([pnl](const std::string& name, jaweson::JsonNode<>& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, pnl, {0, 0});
                    } else if(name == "style") {
                        pnl->SetStyle(sub_node);
                    } else if(name == "children") {
                        LoadChild(pnl, name, sub_node);
                    }
                });
                return pnl;
            }
            case WidgetType::TabControl: {
                auto tab_control = parent->NewChild<sgui::SGTabControl>();
                tab_control->SetName(child_name);
                node.for_each([tab_control](const std::string& name, jaweson::JsonNode<>& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, tab_control, {0, 0});
                    } else if(name == "style") {
                        tab_control->SetStyle(sub_node);
                    } else if(name == "tabs") {
                        sub_node.for_each([&name, tab_control](const std::string& name, jaweson::JsonNode<>& tab_node) {
                            int32_t title_color = 0xff000000;
                            auto& color_node = tab_node["title color"];
                            if(!color_node.is_empty())
                                title_color = sgui::SGJsonUtil::ConvertRGBA(color_node);
                            auto tab = tab_control->AddTab(To<std::wstring>(name), title_color);
                            auto& child_node = tab_node["children"];
                            if(!child_node.is_empty())
                                LoadChild(tab, "", child_node);
                        });
                    }
                });
                return tab_control;
            }
            case WidgetType::Label: {
                auto lbl = parent->NewChild<sgui::SGLabel>();
                lbl->SetName(child_name);
                node.for_each([lbl](const std::string& name, jaweson::JsonNode<>& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, lbl, {0, 0});
                    } else if(name == "text") {
                        auto text = To<std::wstring>(stringCfg[sub_node[0].to_string()]);
                        lbl->GetTextUI()->SetText(text, sgui::SGJsonUtil::ConvertRGBA(sub_node[1]));
                    }
                });
                return lbl;
            }
            case WidgetType::Sprite: {
                auto img = parent->NewChild<sgui::SGImage>();
                img->SetName(child_name);
                node.for_each([img](const std::string& name, jaweson::JsonNode<>& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, img, {0, 0});
                    } else if(name == "image") {
                        img->GetSpriteUI()->SetTextureRect(sgui::SGJsonUtil::ConvertRect(sub_node));
                    }
                });
                return img;
            }
            case WidgetType::SimpleButton: {
                auto btn = parent->NewChild<sgui::SGSimpleButton>(node["push button"].to_bool());
                btn->SetName(child_name);
                node.for_each([btn](const std::string& name, jaweson::JsonNode<>& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, btn, {0, 0});
                    } else if(name == "style") {
                        btn->SetStyle(sub_node);
                    }
                });
                return btn;
            }
            case WidgetType::TextButton: {
                auto btn = parent->NewChild<sgui::SGTextButton>(node["push button"].to_bool());
                btn->SetName(child_name);
                node.for_each([btn](const std::string& name, jaweson::JsonNode<>& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, btn, {0, 0});
                    } else if(name == "style") {
                        btn->SetStyle(sub_node);
                    } else if(name == "text") {
                        auto text = To<std::wstring>(stringCfg[sub_node[0].to_string()]);
                        btn->GetTextUI()->SetText(text, sgui::SGJsonUtil::ConvertRGBA(sub_node[1]));
                    }
                });
                return btn;
            }
            case WidgetType::ImageButton: {
                auto btn = parent->NewChild<sgui::SGImageButton>(node["push button"].to_bool());
                btn->SetName(child_name);
                node.for_each([btn](const std::string& name, jaweson::JsonNode<>& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, btn, {0, 0});
                    } else if(name == "style") {
                        btn->SetStyle(sub_node);
                    }
                });
                return btn;
            }
            case WidgetType::TextureButton: {
                auto btn = parent->NewChild<sgui::SGTextureButton>(node["push button"].to_bool());
                btn->SetName(child_name);
                node.for_each([btn](const std::string& name, jaweson::JsonNode<>& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, btn, {0, 0});
                    } else if(name == "style") {
                        btn->SetStyle(sub_node);
                    }
                });
                return btn;
            }
            case WidgetType::CheckBox: {
                auto chk = parent->NewChild<sgui::SGCheckBox<>>();
                chk->SetName(child_name);
                node.for_each([chk](const std::string& name, jaweson::JsonNode<>& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, chk, {0, 0});
                    } else if(name == "style") {
                        chk->SetStyle(sub_node);
                    } else if(name == "checked") {
                        chk->SetChecked(sub_node.to_bool());
                    }
                });
                return chk;
            }
            case WidgetType::Radio: {
                auto rdo = parent->NewChild<sgui::SGRadio<>>();
                rdo->SetName(child_name);
                node.for_each([rdo, &radio_groups](const std::string& name, jaweson::JsonNode<>& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, rdo, {0, 0});
                    } else if(name == "style") {
                        rdo->SetStyle(sub_node);
                    } else if(name == "group") {
                        int32_t gp = (int32_t)sub_node.to_integer();
                        auto& prev_rdo = radio_groups[gp];
                        if(prev_rdo)
                            rdo->AttackGroup(prev_rdo);
                        prev_rdo = rdo;
                    } else if(name == "checked") {
                        rdo->SetChecked(sub_node.to_bool());
                    }
                });
                return rdo;
            }
            case WidgetType::ListBox: {
                auto lb = parent->NewChild<sgui::SGListBox>();
                lb->SetName(child_name);
                node.for_each([lb](const std::string& name, jaweson::JsonNode<>& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, lb, {0, 0});
                    } else if(name == "style") {
                        lb->SetStyle(sub_node);
                    } else if(name == "items") {
                        LoadItemList(lb, dialogCfg[sub_node.to_string()]);
                    } else if(name == "selection") {
                        lb->SetSelection((int32_t)sub_node.to_integer());
                    }
                });
                return lb;
            }
            case WidgetType::ComboBox: {
                auto cb = parent->NewChild<sgui::SGComboBox>();
                cb->SetName(child_name);
                node.for_each([cb](const std::string& name, jaweson::JsonNode<>& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, cb, {0, 0});
                    } else if(name == "style") {
                        cb->SetStyle(sub_node);
                    } else if(name == "items") {
                        LoadItemList(cb, dialogCfg[sub_node.to_string()]);
                    } else if(name == "selection") {
                        cb->SetSelection((int32_t)sub_node.to_integer());
                    }
                });
                return cb;
            }
            case WidgetType::TextEdit: {
                auto te = parent->NewChild<sgui::SGTextEdit>();
                te->SetName(child_name);
                node.for_each([te](const std::string& name, jaweson::JsonNode<>& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, te, {0, 0});
                    } else if(name == "style") {
                        te->SetStyle(sub_node);
                    } else if(name == "input color") {
                        te->SetDefaultTextColor(sgui::SGJsonUtil::ConvertRGBA(sub_node));
                    } else if(name == "text") {
                        auto text = To<std::wstring>(stringCfg[sub_node[0].to_string()]);
                        te->GetTextUI()->SetText(text, sgui::SGJsonUtil::ConvertRGBA(sub_node[1]));
                    } else if(name == "readonly") {
                        te->SetReadonly(sub_node.to_bool());
                    }
                });
                return te;
            }
            case WidgetType::ScrollBar: {
                auto bar = parent->NewChild<sgui::SGScrollBar<>>(node["horizontal"].to_bool());
                bar->SetName(child_name);
                node.for_each([bar](const std::string& name, jaweson::JsonNode<>& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, bar, {0, 0});
                    } else if(name == "style") {
                        bar->SetStyle(sub_node);
                    } else if(name == "value") {
                        bar->SetValue((float)sub_node.to_double());
                    }
                });
                return bar;
            }
            case WidgetType::ScrollArea: {
                auto area = parent->NewChild<sgui::SGScrollArea>();
                area->SetName(child_name);
                node.for_each([area](const std::string& name, jaweson::JsonNode<>& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, area, {0, 0});
                    } else if(name == "scroll size") {
                        area->SetScrollSize(sgui::SGJsonUtil::ConvertV2i(sub_node, 0));
                    } else if(name == "children") {
                        LoadChild(area, name, sub_node);
                    }
                });
                return area;
            }
        }
        return nullptr;
    }
    
    sgui::SGWidgetContainer* LoadDialog(const std::string& templ) {
        auto& node = dialogCfg[templ];
        if(!node.is_object())
            return nullptr;
        return static_cast<sgui::SGWidgetContainer*>(LoadChild(&sgui::SGGUIRoot::GetSingleton(), templ, node));
    }
    
    template<typename T>
    T* LoadDialogAs(const std::string& templ) { return dynamic_cast<T*>(LoadDialog(templ)); }
    
    void MessageBox::ShowOK(const std::wstring& title, const std::wstring msg, std::function<void ()> cb) {
        auto wnd = LoadDialogAs<sgui::SGWindow>("messagebox ok");
        auto color = sgui::SGJsonUtil::ConvertRGBA(dialogCfg["default text color"]);
        if(wnd)
            return;
        wnd->GetCaption()->SetText(title, color);
        auto label = wnd->FindWidgetAs<sgui::SGLabel>("msg label");
        if(label)
            label->GetTextUI()->SetText(msg, color);
        auto ok_button = wnd->FindWidgetAs<sgui::SGTextButton>("ok button");
        if(ok_button) {
            ok_button->event_click += [wnd, cb](sgui::SGWidget& sender) {
                if(cb) cb();
                wnd->RemoveFromParent();
            };
        }
    }
    
    void MessageBox::ShowOKCancel(const std::wstring& title, const std::wstring msg, std::function<void ()> cb1, std::function<void ()> cb2) {
        auto wnd = LoadDialogAs<sgui::SGWindow>("messagebox ok");
        auto color = sgui::SGJsonUtil::ConvertRGBA(dialogCfg["default text color"]);
        if(!wnd)
            return;
        wnd->GetCaption()->SetText(title, color);
        auto label = wnd->FindWidgetAs<sgui::SGLabel>("msg label");
        if(label)
            label->GetTextUI()->SetText(msg, color);
        auto ok_button = wnd->FindWidgetAs<sgui::SGTextButton>("ok button");
        if(ok_button) {
            ok_button->event_click += [wnd, cb1](sgui::SGWidget& sender) {
                if(cb1) cb1();
                wnd->RemoveFromParent();
            };
        }
        auto cancel_button = wnd->FindWidgetAs<sgui::SGTextButton>("cancel button");
        if(cancel_button) {
            cancel_button->event_click += [wnd, cb2](sgui::SGWidget& sender) {
                if(cb2) cb2();
                wnd->RemoveFromParent();
            };
        }
    }
    
    void MessageBox::ShowYesNo(const std::wstring& title, const std::wstring msg, std::function<void ()> cb1, std::function<void ()> cb2) {
        auto wnd = LoadDialogAs<sgui::SGWindow>("messagebox ok");
        auto color = sgui::SGJsonUtil::ConvertRGBA(dialogCfg["default text color"]);
        if(!wnd)
            return;
        wnd->GetCaption()->SetText(title, color);
        auto label = wnd->FindWidgetAs<sgui::SGLabel>("msg label");
        if(label)
            label->GetTextUI()->SetText(msg, color);
        auto yes_button = wnd->FindWidgetAs<sgui::SGTextButton>("yes button");
        if(yes_button) {
            yes_button->event_click += [wnd, cb1](sgui::SGWidget& sender) {
                if(cb1) cb1();
                wnd->RemoveFromParent();
            };
        }
        auto no_button = wnd->FindWidgetAs<sgui::SGTextButton>("no button");
        if(no_button) {
            no_button->event_click += [wnd, cb2](sgui::SGWidget& sender) {
                if(cb2) cb2();
                wnd->RemoveFromParent();
            };
        }
    }
    
    PopupMenu& PopupMenu::AddButton(const std::wstring& text, intptr_t cval) {
        auto btn = pnl->NewChild<sgui::SGTextButton>();
        btn->SetPositionSize({margin.left, margin.top + (item_count + margin.top) * item_height}, {item_width, item_height});
        btn->GetTextUI()->SetText(text, sgui::SGJsonUtil::ConvertRGBA(dialogCfg["default text color"]));
        btn->SetCustomValue(cval);
        btn->event_click += [this](sgui::SGWidget& sender)->bool {
            if(btn_cb != nullptr)
                btn_cb(static_cast<int32_t>(sender.GetCustomValue()));
            pnl->RemoveFromParent();
            return true;
        };
        item_count++;
        return *this;
    }
    
    void PopupMenu::End() {
        pnl->SetPositionSize(pos, {item_width + margin.left + margin.width, (item_count + margin.top) * item_height + margin.height});
        pnl->event_on_destroy += [this](sgui::SGWidget& sender)->bool {
            delete this;
            return true;
        };
        sgui::SGGUIRoot::GetSingleton().PopupObject(pnl->shared_from_this());
    }
    
    PopupMenu& PopupMenu::Create(v2i pos, std::function<void (int32_t)> cb) {
        PopupMenu* menu = new PopupMenu();
        menu->pos = pos;
        menu->item_width = (int32_t)dialogCfg["popup menu width"].to_integer();
        menu->item_height = (int32_t)dialogCfg["popup menu height"].to_integer();
        menu->margin = sgui::SGJsonUtil::ConvertRect(dialogCfg["popup menu margin"]);
        menu->pnl = sgui::SGGUIRoot::GetSingleton().NewChild<sgui::SGPanel>();
        menu->btn_cb = cb;
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
        auto wnd = LoadDialogAs<sgui::SGWindow>("file dialog");
        if(!wnd)
            return;
        wnd->GetCaption()->SetText(title, 0xff000000);
        auto fpath = wnd->FindWidgetAs<sgui::SGTextEdit>("path");
        if(fpath)
            fpath->GetTextUI()->SetText(root, 0xff000000);
        auto lst = wnd->FindWidgetAs<sgui::SGListBox>("file list");
        auto ffile = wnd->FindWidgetAs<sgui::SGTextEdit>("file");
        auto btn = wnd->FindWidgetAs<sgui::SGTextButton>("ok button");
        if(!lst || !ffile || !btn)
            return;
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
        auto wnd = LoadDialogAs<sgui::SGPanel>("filter dialog");
        if(!wnd)
            return;
        window = wnd->CastPtr<sgui::SGWidgetContainer>();
        
        auto pkeyword = wnd->FindWidgetAs<sgui::SGTextEdit>("keyword");
        if(pkeyword)
            keyword = pkeyword->CastPtr<sgui::SGTextEdit>();
        
        auto ptype1 = wnd->FindWidgetAs<sgui::SGComboBox>("arctype");
        auto ptype2 = wnd->FindWidgetAs<sgui::SGComboBox>("subtype");
        if(ptype1 && ptype2) {
            arctype = ptype1->CastPtr<sgui::SGComboBox>();
            subtype = ptype2->CastPtr<sgui::SGComboBox>();
            ptype1->event_sel_change += [ptype2](sgui::SGWidget& sender, int32_t index)->bool {
                ptype2->ClearItems();
                if(index == 0) {
                    LoadItemList(ptype2, dialogCfg["no type"]);
                } else if(index == 1) {
                    LoadItemList(ptype2, dialogCfg["monster types"]);
                } else if(index == 2) {
                    LoadItemList(ptype2, dialogCfg["spell types"]);
                } else {
                    LoadItemList(ptype2, dialogCfg["trap types"]);
                }
                ptype2->SetSelection(0);
                return true;
            };
        }
        auto plimit = wnd->FindWidgetAs<sgui::SGComboBox>("limit");
        if(plimit)
            limit_type = plimit->CastPtr<sgui::SGComboBox>();
        auto ppool = wnd->FindWidgetAs<sgui::SGComboBox>("limit");
        if(ppool)
            pool_type = ppool->CastPtr<sgui::SGComboBox>();
        auto pattribute = wnd->FindWidgetAs<sgui::SGComboBox>("attribute");
        if(pattribute)
            attribute = pattribute->CastPtr<sgui::SGComboBox>();
        auto prace = wnd->FindWidgetAs<sgui::SGComboBox>("race");
        if(prace)
            race = prace->CastPtr<sgui::SGComboBox>();
        auto pattack = wnd->FindWidgetAs<sgui::SGTextEdit>("attack");
        if(pattack)
            attack = pattack->CastPtr<sgui::SGTextEdit>();
        auto pdefence = wnd->FindWidgetAs<sgui::SGTextEdit>("defence");
        if(pdefence)
            defence = pdefence->CastPtr<sgui::SGTextEdit>();
        auto pstar = wnd->FindWidgetAs<sgui::SGTextEdit>("star");
        if(pstar)
            star = pstar->CastPtr<sgui::SGTextEdit>();
        auto pscale = wnd->FindWidgetAs<sgui::SGTextEdit>("pscale");
        if(pscale)
            scale = pscale->CastPtr<sgui::SGTextEdit>();
        
        auto sch = wnd->FindWidgetAs<sgui::SGTextButton>("search");
        auto clr = wnd->FindWidgetAs<sgui::SGTextButton>("clear");
        if(sch)
            sch->event_click += [this](sgui::SGWidget& sender)->bool { BeginSearch(); return true; };
        if(clr)
            clr->event_click += [this](sgui::SGWidget& sender)->bool { ClearCondition(); return true; };
        
        pkeyword->SetFocus();

        sgui::SGGUIRoot::GetSingleton().PopupObject(wnd->shared_from_this());
        if(pkeyword)
            pkeyword->GetTextUI()->SetText(con_text[0], 0xff000000);
        if(pattack)
            pattack->GetTextUI()->SetText(con_text[1], 0xff000000);
        if(pdefence)
            pdefence->GetTextUI()->SetText(con_text[1], 0xff000000);
        if(pstar)
            pstar->GetTextUI()->SetText(con_text[1], 0xff000000);
        if(pscale)
            pscale->GetTextUI()->SetText(con_text[1], 0xff000000);
        if(ptype1)
            ptype1->SetSelection(sel[0]);
        if(ptype2)
            ptype2->SetSelection(sel[1]);
        if(plimit)
            plimit->SetSelection(sel[2]);
        if(ppool)
            ppool->SetSelection(sel[3]);
        if(pattribute)
            pattribute->SetSelection(sel[4]);
        if(prace)
            prace->SetSelection(sel[5]);
    }
    
    void FilterDialog::BeginSearch() {
        FilterCondition fc;
        std::wstring keystr = keyword.expired() ? L"" : keyword.lock()->GetTextUI()->GetText();
        con_text[0] = keystr;
        Tokenizer<wchar_t> tokens(keystr, L" ");
        for(size_t i = 0; i < tokens.size(); ++i) {
            auto& sep_keyword = tokens[i];
            if(sep_keyword.length() > 0) {
                if(sep_keyword[0] == L'!') {
                    fc.code = ParseInt(&sep_keyword[1], (int32_t)sep_keyword.length() - 1);
                    if(fc.code == 0)
                        fc.code = 1;
                } else if(sep_keyword[0] == L'@') {
                    auto setcode = DataMgr::Get().GetSetCode(sep_keyword.substr(1));
                    if(setcode)
                        fc.setcode = setcode;
                    else
                        fc.setcode = 0xffff;
                } else if(sep_keyword[0] == L'#') {
                    fc.tags.push_back(sep_keyword.substr(1));
                } else
                    fc.keywords.push_back(sep_keyword);
            }
        }
        if(!arctype.expired()) {
            sel[0] = arctype.lock()->GetSelection();
            fc.type = arctype.lock()->GetItemCustomValue(sel[0]);
        }
        if(!subtype.expired()) {
            sel[1] = subtype.lock()->GetSelection();
            fc.subtype = subtype.lock()->GetItemCustomValue(sel[1]);
        }
        if(!limit_type.expired()) {
            sel[2] = limit_type.lock()->GetSelection();
        }
        if(!pool_type.expired()) {
            sel[3] = pool_type.lock()->GetSelection();
            fc.pool = pool_type.lock()->GetItemCustomValue(sel[3]);
        }
        if((fc.type == 0) || (fc.type == 0x1)) {
            if(!attribute.expired()) {
                sel[4] = attribute.lock()->GetSelection();
                fc.attribute = attribute.lock()->GetItemCustomValue(sel[4]);
            }
            if(!race.expired()) {
                sel[4] = race.lock()->GetSelection();
                fc.race = race.lock()->GetItemCustomValue(sel[4]);
            }
            if(!attack.expired()) {
                auto t1 = ParseValue(attack.lock()->GetTextUI()->GetText());
                switch(std::get<0>(t1)) {
                    case 0: break;
                    case 1: fc.atkmin = fc.atkmax = -2; break;
                    case 2: fc.atkmin = fc.atkmax = std::get<1>(t1); break;
                    case 3: fc.atkmin = std::get<1>(t1); fc.atkmax = std::get<2>(t1); break;
                    default: break;
                }
            }
            if(!defence.expired()) {
                auto t2 = ParseValue(defence.lock()->GetTextUI()->GetText());
                switch(std::get<0>(t2)) {
                    case 0: break;
                    case 1: fc.defmin = fc.defmax = -2; break;
                    case 2: fc.defmin = fc.defmax = std::get<1>(t2); break;
                    case 3: fc.defmin = std::get<1>(t2); fc.defmax = std::get<2>(t2); break;
                    default: break;
                }
            }
            if(!star.expired()) {
                auto t3 = ParseValue(star.lock()->GetTextUI()->GetText());
                switch(std::get<0>(t3)) {
                    case 0: case 1: break;
                    case 2: fc.lvmin = fc.lvmax = std::get<1>(t3); break;
                    case 3: fc.lvmin = std::get<1>(t3); fc.lvmax = std::get<2>(t3); break;
                    default: break;
                }
            }
            if(!scale.expired()) {
                auto t4 = ParseValue(star.lock()->GetTextUI()->GetText());
                switch(std::get<0>(t4)) {
                    case 0: case 1: break;
                    case 2: fc.scalemin = fc.scalemax = std::get<1>(t4); break;
                    case 3: fc.scalemin = std::get<1>(t4); fc.scalemax = std::get<2>(t4); break;
                    default: break;
                }
            }
        }
        if(cbOK)
            cbOK(fc, sel[2]);
    }
    
    void FilterDialog::ClearCondition() {
        keyword.lock()->GetTextUI()->Clear();
        attack.lock()->GetTextUI()->Clear();
        defence.lock()->GetTextUI()->Clear();
        star.lock()->GetTextUI()->Clear();
        arctype.lock()->SetSelection(0);
        subtype.lock()->SetSelection(0);
        limit_type.lock()->SetSelection(0);
        pool_type.lock()->SetSelection(0);
        attribute.lock()->SetSelection(0);
        race.lock()->SetSelection(0);
        for(int32_t i = 0; i < 5; ++i)
            con_text[i].clear();
        for(int32_t i = 0; i < 6; ++i)
            sel[i] = 0;
    }
    
    std::tuple<int32_t, int32_t, int32_t> FilterDialog::ParseValue(const std::wstring& valstr) {
        if(valstr.length() == 0)
            return std::make_tuple(0, 0, 0);
        if(valstr == L"?")
            return std::make_tuple(1, 0, 0);
        int32_t pos = (int32_t)valstr.find(L':');
        if(pos == std::wstring::npos)
            return std::make_tuple(2, ParseInt(&valstr[0], (int32_t)valstr.length()), 0);
        else
            return std::make_tuple(3, ParseInt(&valstr[0], pos), ParseInt(&valstr[pos + 1], (int32_t)valstr.length() - pos - 1));
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
        if(!window.expired())
            return;
        auto wnd = LoadDialogAs<sgui::SGPanel>("info dialog");
        if(!wnd)
            return;
        window = wnd->CastPtr<sgui::SGWidgetContainer>();
        
        int32_t ch = sz.y - 10;
        int32_t cw = (sz.y - 10) * 20 / 29;
        int32_t mw = sz.x - 20 - cw;
        if(wnd == nullptr) {
            wnd = sgui::SGPanel::Create(nullptr, pos, sz);
            window = wnd;
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
                    extra->AppendText(DataMgr::Get().GetSetName(sd), 0xffff0000);
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

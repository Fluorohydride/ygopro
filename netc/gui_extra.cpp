#include "utils/common.h"
#include "utils/filesystem.h"
#include "utils/tokenizer.h"

#include "config.h"
#include "card_data.h"
#include "scene_mgr.h"
#include "ds/duel_scene.h"
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
        SpriteList = 17,
    };
    
    void LoadItemList(sgui::SGItemListWidget* widget, jaweson::JsonValue& node) {
        node.for_each([widget](const std::string& name, jaweson::JsonValue& item_node) {
            widget->AddItem(To<std::wstring>(stringCfg[name].to_string()), sgui::SGJsonUtil::ConvertRGBA(item_node[0]), item_node[1].to_value<uint32_t>());
        });
    }
    
    sgui::SGWidget* LoadChild(sgui::SGWidgetContainer* parent, const std::string& child_name, jaweson::JsonValue& node) {
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
            {"scrollarea", WidgetType::ScrollArea},
            {"spritelist", WidgetType::SpriteList}
        };
        std::map<int32_t, sgui::SGRadio<>*> radio_groups;
        std::string widget_type = node["type"].to_string();
        switch(widget_type_id[widget_type]) {
            case WidgetType::Unknown : break;
            case WidgetType::Window: {
                auto wnd = parent->NewChild<sgui::SGWindow>();
                wnd->SetName(child_name);
                node.for_each([wnd](const std::string& name, jaweson::JsonValue& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, wnd, {0, 0});
                    } else if(name == "style") {
                        wnd->SetStyle(sub_node);
                    } else if(name == "caption") {
                        auto text = To<std::wstring>(stringCfg[sub_node[0].to_string()].to_string());
                        wnd->GetCaption()->PushStringWithFormat(text, sgui::SGJsonUtil::ConvertRGBA(sub_node[1]));
                    } else if(name == "close button") {
                        wnd->SetCloseButtonVisible(sub_node.to_bool());
                    } else if(name == "allow resize") {
                        wnd->SetResizable(sub_node.to_bool());
                    } else if(name == "min size") {
                        wnd->SetMinSize({sub_node[0].to_value<int32_t>(), sub_node[1].to_value<int32_t>()});
                    } else if(name == "children") {
                        sub_node.for_each([wnd](const std::string& c_name, jaweson::JsonValue& c_node) {
                            LoadChild(wnd, c_name, c_node);
                        });
                    }
                });
                return wnd;
            }
            case WidgetType::Panel: {
                auto ent = node["is entity"].to_bool();
                auto pnl = parent->NewChild<sgui::SGPanel>(ent);
                pnl->SetName(child_name);
                node.for_each([pnl](const std::string& name, jaweson::JsonValue& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, pnl, {0, 0});
                    } else if(name == "style") {
                        pnl->SetStyle(sub_node);
                    } else if(name == "children") {
                        sub_node.for_each([pnl](const std::string& c_name, jaweson::JsonValue& c_node) {
                            LoadChild(pnl, c_name, c_node);
                        });
                    }
                });
                return pnl;
            }
            case WidgetType::TabControl: {
                auto tab_control = parent->NewChild<sgui::SGTabControl>();
                tab_control->SetName(child_name);
                node.for_each([tab_control](const std::string& name, jaweson::JsonValue& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, tab_control, {0, 0});
                    } else if(name == "style") {
                        tab_control->SetStyle(sub_node);
                    } else if(name == "tabs") {
                        sub_node.for_each([tab_control](const std::string& name, jaweson::JsonValue& tab_node) {
                            int32_t title_color = 0xff000000;
                            auto& color_node = tab_node["title color"];
                            if(!color_node.is_empty())
                                title_color = sgui::SGJsonUtil::ConvertRGBA(color_node);
                            auto tab = tab_control->AddTab(To<std::wstring>(name), title_color);
                            auto& child_node = tab_node["children"];
                            child_node.for_each([tab](const std::string& c_name, jaweson::JsonValue& c_node) {
                                LoadChild(tab, c_name, c_node);
                            });
                        });
                    }
                });
                return tab_control;
            }
            case WidgetType::Label: {
                auto lbl = parent->NewChild<sgui::SGLabel>();
                lbl->SetName(child_name);
                node.for_each([lbl](const std::string& name, jaweson::JsonValue& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, lbl, {0, 0});
                    } else if(name == "allow focus") {
                        lbl->SetAllowFocus(sub_node.to_bool());
                    } else if(name == "text") {
                        auto text = To<std::wstring>(stringCfg[sub_node[0].to_string()].to_string());
                        lbl->GetTextUI()->PushStringWithFormat(text, sgui::SGJsonUtil::ConvertRGBA(sub_node[1]));
                    } else if(name == "max width") {
                        lbl->GetTextUI()->SetMaxWidth(sub_node.to_value<int32_t>());
                    }
                });
                return lbl;
            }
            case WidgetType::Sprite: {
                auto img = parent->NewChild<sgui::SGImage>();
                img->SetName(child_name);
                node.for_each([img](const std::string& name, jaweson::JsonValue& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, img, {0, 0});
                    } else if(name == "allow focus") {
                        img->SetAllowFocus(sub_node.to_bool());
                    } else if(name == "image") {
                        img->GetSpriteUI()->SetTextureRect(sgui::SGJsonUtil::ConvertRect<int32_t>(sub_node));
                    }
                });
                return img;
            }
            case WidgetType::SimpleButton: {
                auto btn = parent->NewChild<sgui::SGSimpleButton>(node["push button"].to_bool());
                btn->SetName(child_name);
                node.for_each([btn](const std::string& name, jaweson::JsonValue& sub_node) {
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
                node.for_each([btn](const std::string& name, jaweson::JsonValue& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, btn, {0, 0});
                    } else if(name == "style") {
                        btn->SetStyle(sub_node);
                    } else if(name == "text") {
                        auto text = To<std::wstring>(stringCfg[sub_node[0].to_string()].to_string());
                        btn->GetTextUI()->PushStringWithFormat(text, sgui::SGJsonUtil::ConvertRGBA(sub_node[1]));
                    }
                });
                return btn;
            }
            case WidgetType::ImageButton: {
                auto btn = parent->NewChild<sgui::SGImageButton>(node["push button"].to_bool());
                btn->SetName(child_name);
                node.for_each([btn](const std::string& name, jaweson::JsonValue& sub_node) {
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
                node.for_each([btn](const std::string& name, jaweson::JsonValue& sub_node) {
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
                node.for_each([chk](const std::string& name, jaweson::JsonValue& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, chk, {0, 0});
                    } else if(name == "style") {
                        chk->SetStyle(sub_node);
                    } else if(name == "text") {
                        auto text = To<std::wstring>(stringCfg[sub_node[0].to_string()].to_string());
                        chk->GetTextUI()->PushStringWithFormat(text, sgui::SGJsonUtil::ConvertRGBA(sub_node[1]));
                    } else if(name == "checked") {
                        chk->SetChecked(sub_node.to_bool());
                    }
                });
                return chk;
            }
            case WidgetType::Radio: {
                auto rdo = parent->NewChild<sgui::SGRadio<>>();
                rdo->SetName(child_name);
                node.for_each([rdo, &radio_groups](const std::string& name, jaweson::JsonValue& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, rdo, {0, 0});
                    } else if(name == "style") {
                        rdo->SetStyle(sub_node);
                    } else if(name == "text") {
                        auto text = To<std::wstring>(stringCfg[sub_node[0].to_string()].to_string());
                        rdo->GetTextUI()->PushStringWithFormat(text, sgui::SGJsonUtil::ConvertRGBA(sub_node[1]));
                    } else if(name == "group") {
                        int32_t gp = sub_node.to_value<int32_t>();
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
                node.for_each([lb](const std::string& name, jaweson::JsonValue& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, lb, {0, 0});
                    } else if(name == "style") {
                        lb->SetStyle(sub_node);
                    } else if(name == "items") {
                        LoadItemList(lb, dialogCfg["item lists"][sub_node.to_string()]);
                    } else if(name == "selection") {
                        lb->SetSelection(sub_node.to_value<int32_t>());
                    }
                });
                return lb;
            }
            case WidgetType::ComboBox: {
                auto cb = parent->NewChild<sgui::SGComboBox>();
                cb->SetName(child_name);
                node.for_each([cb](const std::string& name, jaweson::JsonValue& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, cb, {0, 0});
                    } else if(name == "style") {
                        cb->SetStyle(sub_node);
                    } else if(name == "items") {
                        LoadItemList(cb, dialogCfg["item lists"][sub_node.to_string()]);
                    } else if(name == "selection") {
                        cb->SetSelection(sub_node.to_value<int32_t>());
                    }
                });
                return cb;
            }
            case WidgetType::TextEdit: {
                auto te = parent->NewChild<sgui::SGTextEdit>();
                te->SetName(child_name);
                node.for_each([te](const std::string& name, jaweson::JsonValue& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, te, {0, 0});
                    } else if(name == "style") {
                        te->SetStyle(sub_node);
                    } else if(name == "input color") {
                        te->SetDefaultTextColor(sgui::SGJsonUtil::ConvertRGBA(sub_node));
                    } else if(name == "text") {
                        auto text = To<std::wstring>(stringCfg[sub_node[0].to_string()].to_string());
                        te->GetTextUI()->PushStringWithFormat(text, sgui::SGJsonUtil::ConvertRGBA(sub_node[1]));
                    } else if(name == "readonly") {
                        te->SetReadonly(sub_node.to_bool());
                    }
                });
                return te;
            }
            case WidgetType::ScrollBar: {
                auto bar = parent->NewChild<sgui::SGScrollBar<>>(node["horizontal"].to_bool());
                bar->SetName(child_name);
                node.for_each([bar](const std::string& name, jaweson::JsonValue& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, bar, {0, 0});
                    } else if(name == "style") {
                        bar->SetStyle(sub_node);
                    } else if(name == "value") {
                        bar->SetValue(sub_node.to_value<float>());
                    }
                });
                return bar;
            }
            case WidgetType::ScrollArea: {
                auto area = parent->NewChild<sgui::SGScrollArea>();
                area->SetName(child_name);
                node.for_each([area](const std::string& name, jaweson::JsonValue& sub_node) {
                    if(name == "position") {
                        sgui::SGJsonUtil::SetUIPositionSize(sub_node, area, {0, 0});
                    } else if(name == "scroll size") {
                        area->SetScrollSize(sgui::SGJsonUtil::ConvertVec2<int32_t>(sub_node, 0));
                    } else if(name == "children") {
                        sub_node.for_each([area](const std::string& c_name, jaweson::JsonValue& c_node) {
                            LoadChild(area, c_name, c_node);
                        });
                    }
                });
                return area;
            }
            case WidgetType::SpriteList: {
                auto sl = parent->NewChild<sgui::SGImageList>();
                sl->SetName(child_name);
                sl->SetAllowFocus(false);
                sl->SetPositionSize({0, 0}, {0, 0}, {0.0f, 0.0f}, {1.0f, 1.0f});
                return sl;
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
    
    void AddMessageButton(sgui::SGWindow* wnd) {}
    
    template<typename... ARGS>
    void AddMessageButton(sgui::SGWindow* wnd, const std::string& btnname, std::function<void ()> cb, ARGS... args) {
        auto btn = wnd->FindWidgetAs<sgui::SGTextButton>(btnname);
        if(btn) {
            btn->event_click += [wnd, cb](sgui::SGWidget& sender)->bool {
                if(cb) cb();
                wnd->RemoveFromParent();
                return true;
            };
        }
        AddMessageButton(wnd, std::forward<ARGS>(args)...);
    }
    
    template<typename... ARGS>
    void ShowMessageBox(const std::string& templ, const std::wstring& title, const std::wstring& msg, ARGS... args) {
        auto wnd = LoadDialogAs<sgui::SGWindow>(templ);
        auto color = sgui::SGJsonUtil::ConvertRGBA(dialogCfg["default text color"]);
        if(!wnd)
            return;
        auto wnd_sz = wnd->GetAbsoluteSize();
        wnd->GetCaption()->SetText(title, color);
        auto label = wnd->FindWidgetAs<sgui::SGLabel>("msg label");
        if(label) {
            label->GetTextUI()->SetText(msg, color);
            label->SetDragTarget(wnd->shared_from_this());
            auto lbl_sz = label->GetAbsoluteSize();
            auto width = (wnd_sz.x > lbl_sz.x + 40) ? wnd_sz.x : (lbl_sz.x + 40);
            wnd->SetSize({width, wnd_sz.y + lbl_sz.y});
        }
        AddMessageButton(wnd, std::forward<ARGS>(args)...);
    }
    
    void MessageBox::ShowOK(const std::wstring& title, const std::wstring& msg, std::function<void ()> cb) {
        ShowMessageBox("messagebox ok", title, msg, "ok button", cb);
    }
    
    void MessageBox::ShowOKCancel(const std::wstring& title, const std::wstring& msg, std::function<void ()> cb1, std::function<void ()> cb2) {
        ShowMessageBox("messagebox ok,cancel", title, msg, "ok button", cb1, "cancel button", cb2);
    }
    
    void MessageBox::ShowYesNo(const std::wstring& title, const std::wstring& msg, std::function<void ()> cb1, std::function<void ()> cb2) {
        ShowMessageBox("messagebox yes,no", title, msg, "yes button", cb1, "no button", cb2);
    }
    
    PopupMenu& PopupMenu::AddButton(const std::wstring& text, intptr_t cval) {
        auto btn = pnl->NewChild<sgui::SGTextButton>();
        btn->SetPositionSize({margin.left, margin.top + (item_height + margin.top) * item_count}, {item_width, item_height});
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
        pnl->SetPositionSize(pos, {item_width + margin.left + margin.width, (item_height + margin.top) * item_count + margin.height});
        pnl->event_on_destroy += [this](sgui::SGWidget& sender)->bool {
            delete this;
            return true;
        };
        sgui::SGGUIRoot::GetSingleton().PopupObject(pnl->shared_from_this());
    }
    
    PopupMenu& PopupMenu::Create(v2i pos, std::function<void (int32_t)> cb) {
        PopupMenu* menu = new PopupMenu();
        menu->pos = pos;
        menu->item_width = dialogCfg["popup menu width"].to_value<int32_t>();
        menu->item_height = dialogCfg["popup menu height"].to_value<int32_t>();
        menu->margin = sgui::SGJsonUtil::ConvertRect<int32_t>(dialogCfg["popup menu margin"]);
        menu->pnl = sgui::SGGUIRoot::GetSingleton().NewChild<sgui::SGPanel>();
        menu->btn_cb = cb;
        return *menu;
    }
    
    PopupMenu& PopupMenu::Load(const std::string& name, v2i pos, std::function<void (int32_t)> cb) {
        auto& menu_node = dialogCfg["menus"][name];
        auto& menu = Create(pos, cb);
        if(!menu_node.is_empty()) {
            menu_node.for_each([&menu](const std::string& key, jaweson::JsonValue& node)->void {
                menu.AddButton(To<std::wstring>(stringCfg[key].to_string()), (intptr_t)node.to_integer());
            });
        }
        menu.End();
        return menu;
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
        if(!lst || !ffile || !btn) {
            wnd->RemoveFromParent();
            return;
        }
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
            if(lst->GetItemCustomValue(index) == 2)
                ffile->GetTextUI()->SetText(lst->GetItemText(index).substr(1), 0xff000000);
            return true;
        });
        lst->event_item_double_click += [this, ffile, lst, fpath](sgui::SGWidget& sender, int32_t index)->bool {
            if(index < 0)
                return true;
            if(lst->GetItemCustomValue(index) == 0) {
                size_t pos = path.rfind(L'/');
                path = path.substr(0, pos);
                fpath->GetTextUI()->SetText(path, 0xff000000);
                RefreshList(lst);
            } else if(lst->GetItemCustomValue(index) == 1) {
                path.append(L"/").append(lst->GetItemText(index).substr(1));
                fpath->GetTextUI()->SetText(path, 0xff000000);
                RefreshList(lst);
            } else {
                if(cbOK != nullptr)
                    cbOK(path + L"/" + ffile->GetTextUI()->GetText());
                window.lock()->RemoveFromParent();
            }
            return true;
        };
        window = wnd->CastPtr<sgui::SGWindow>();
        RefreshList(lst);
    }
    
    void FileDialog::RefreshList(sgui::SGListBox* list) {
        std::vector<std::wstring> dirs;
        std::vector<std::wstring> files;
        FileSystem::TraversalDir(path, [this, &dirs, &files](const std::wstring& name, bool isdir) {
            if(isdir) {
                if(name != L"." && name != L"..")
                    dirs.push_back(std::wstring(L"\ue08b").append(name));
            } else {
                if(name.find(filter) == (name.size() - filter.size()))
                    files.push_back(std::wstring(L"\ue08c").append(name));
            }
        });
        std::sort(dirs.begin(), dirs.end());
        std::sort(files.begin(), files.end());
        list->ClearItems();
        if(path != root)
            list->AddItem(std::wstring(L"\ue08d").append(To<std::wstring>(stringCfg["eui_updir"].to_string())), 0xff000000, 0);
        for(size_t i = 0; i < dirs.size(); ++i)
            list->AddItem(dirs[i], 0xff000000, 1);
        for(size_t i = 0; i < files.size(); ++i)
            list->AddItem(files[i], 0xff000000, 2);
    }
    
    void FilterDialog::Show(v2i pos) {
        if(!window.expired())
            return;
        auto wnd = LoadDialogAs<sgui::SGPanel>("filter dialog");
        if(!wnd)
            return;
        wnd->SetPosition({pos.x - wnd->GetAbsoluteSize().x, pos.y});
        window = wnd->CastPtr<sgui::SGWidgetContainer>();
        keyword = wnd->FindWidgetAs<sgui::SGTextEdit>("keyword");
        arctype = wnd->FindWidgetAs<sgui::SGComboBox>("arctype");
        subtype = wnd->FindWidgetAs<sgui::SGComboBox>("subtype");
        limit_type = wnd->FindWidgetAs<sgui::SGComboBox>("limit");
        pool_type = wnd->FindWidgetAs<sgui::SGComboBox>("pool");
        attribute = wnd->FindWidgetAs<sgui::SGComboBox>("attribute");
        race = wnd->FindWidgetAs<sgui::SGComboBox>("race");
        attack = wnd->FindWidgetAs<sgui::SGTextEdit>("attack");
        defence = wnd->FindWidgetAs<sgui::SGTextEdit>("defence");
        star = wnd->FindWidgetAs<sgui::SGTextEdit>("star");
        pscale = wnd->FindWidgetAs<sgui::SGTextEdit>("pscale");
        extra_label[0] = wnd->FindWidgetAs<sgui::SGLabel>("attrib label");
        extra_label[1] = wnd->FindWidgetAs<sgui::SGLabel>("race label");
        extra_label[2] = wnd->FindWidgetAs<sgui::SGLabel>("attack label");
        extra_label[3] = wnd->FindWidgetAs<sgui::SGLabel>("defence label");
        extra_label[4] = wnd->FindWidgetAs<sgui::SGLabel>("star label");
        extra_label[5] = wnd->FindWidgetAs<sgui::SGLabel>("pscale label");
        auto sch = wnd->FindWidgetAs<sgui::SGTextButton>("search");
        auto clr = wnd->FindWidgetAs<sgui::SGTextButton>("clear");
        if(sch)
            sch->event_click += [this](sgui::SGWidget& sender)->bool { BeginSearch(); return true; };
        if(clr)
            clr->event_click += [this](sgui::SGWidget& sender)->bool { ClearCondition(); return true; };

        sgui::SGGUIRoot::GetSingleton().PopupObject(wnd->shared_from_this());
        if(keyword) {
            keyword->SetFocus();
            keyword->GetTextUI()->SetText(con_text[0], 0xff000000);
        }
        if(attack)
            attack->GetTextUI()->SetText(con_text[1], 0xff000000);
        if(defence)
            defence->GetTextUI()->SetText(con_text[2], 0xff000000);
        if(star)
            star->GetTextUI()->SetText(con_text[3], 0xff000000);
        if(pscale)
            pscale->GetTextUI()->SetText(con_text[4], 0xff000000);
        if(arctype && subtype) {
            arctype->event_sel_change += [this, wnd](sgui::SGWidget& sender, int32_t index)->bool {
                subtype->ClearItems();
                if(index == 0) {
                    LoadItemList(subtype, dialogCfg["item lists"]["no type"]);
                    ShowExtraInfo(false);
                } else if(index == 1) {
                    LoadItemList(subtype, dialogCfg["item lists"]["monster types"]);
                    ShowExtraInfo(true);
                } else if(index == 2) {
                    LoadItemList(subtype, dialogCfg["item lists"]["spell types"]);
                    ShowExtraInfo(false);
                } else {
                    LoadItemList(subtype, dialogCfg["item lists"]["trap types"]);
                    ShowExtraInfo(false);
                }
                subtype->SetSelection(0);
                return true;
            };
        }
        if(arctype)
            arctype->SetSelection(sel[0]);
        if(subtype)
            subtype->SetSelection(sel[1]);
        if(limit_type)
            limit_type->SetSelection(sel[2]);
        if(pool_type)
            pool_type->SetSelection(sel[3]);
        if(attribute)
            attribute->SetSelection(sel[4]);
        if(race)
            race->SetSelection(sel[5]);
    }
    
    void FilterDialog::ShowExtraInfo(bool show) {
        auto wnd = window.lock();
        if(!wnd)
            return;
        auto sz = wnd->GetAbsoluteSize();
        wnd->SetSize({sz.x, dialogCfg["filter dialog"]["dialog height"][show ? 1 : 0].to_value<int32_t>()});
        for(int32_t i = 0; i < 6; ++i)
            if(extra_label[i])
                extra_label[i]->SetVisible(show);
        if(attribute)
            attribute->SetVisible(show);
        if(race)
            race->SetVisible(show);
        if(attack)
            attack->SetVisible(show);
        if(defence)
            defence->SetVisible(show);
        if(star)
            star->SetVisible(show);
        if(pscale)
            pscale->SetVisible(show);
    }
    
    void FilterDialog::BeginSearch() {
        FilterCondition fc;
        std::wstring keystr = keyword ? keyword->GetTextUI()->GetText() : L"";
        con_text[0] = keystr;
        Tokenizer<wchar_t> tokens(keystr, L" ");
        for(size_t i = 0; i < tokens.size(); ++i) {
            auto& sep_keyword = tokens[i];
            if(sep_keyword.length() > 0) {
                if(sep_keyword[0] == L'=') {
                    fc.code = ParseInt(&sep_keyword[1], (int32_t)sep_keyword.length() - 1);
                    if(fc.code == 0)
                        fc.code = 1;
                } else if(sep_keyword[0] == L'@') {
                    if(sep_keyword[1] == L'@') {
                        fc.setcode = To<int32_t>(sep_keyword.substr(2));
                    } else {
                        auto setcode = DataMgr::Get().GetSetCode(sep_keyword.substr(1));
                        if(setcode)
                            fc.setcode = setcode;
                        else
                            fc.setcode = 0xffff;
                    }
                } else if(sep_keyword[0] == L'#') {
                    fc.tags.push_back(sep_keyword.substr(1));
                } else
                    fc.keywords.push_back(sep_keyword);
            }
        }
        if(arctype) {
            sel[0] = arctype->GetSelection();
            fc.type = arctype->GetItemCustomValue(sel[0]);
        }
        if(subtype) {
            sel[1] = subtype->GetSelection();
            fc.subtype = subtype->GetItemCustomValue(sel[1]);
        }
        if(limit_type) {
            sel[2] = limit_type->GetSelection();
        }
        if(pool_type) {
            sel[3] = pool_type->GetSelection();
            fc.pool = pool_type->GetItemCustomValue(sel[3]);
        }
        if((fc.type == 0) || (fc.type == 0x1)) {
            if(attribute) {
                sel[4] = attribute->GetSelection();
                fc.attribute = attribute->GetItemCustomValue(sel[4]);
            }
            if(race) {
                sel[5] = race->GetSelection();
                fc.race = race->GetItemCustomValue(sel[5]);
            }
            if(attack) {
                auto t1 = ParseValue(attack->GetTextUI()->GetText());
                switch(std::get<0>(t1)) {
                    case 0: break;
                    case 1: fc.atkmin = fc.atkmax = -2; break;
                    case 2: fc.atkmin = fc.atkmax = std::get<1>(t1); break;
                    case 3: fc.atkmin = std::get<1>(t1); fc.atkmax = std::get<2>(t1); break;
                    default: break;
                }
            }
            if(defence) {
                auto t2 = ParseValue(defence->GetTextUI()->GetText());
                switch(std::get<0>(t2)) {
                    case 0: break;
                    case 1: fc.defmin = fc.defmax = -2; break;
                    case 2: fc.defmin = fc.defmax = std::get<1>(t2); break;
                    case 3: fc.defmin = std::get<1>(t2); fc.defmax = std::get<2>(t2); break;
                    default: break;
                }
            }
            if(star) {
                auto t3 = ParseValue(star->GetTextUI()->GetText());
                switch(std::get<0>(t3)) {
                    case 0: case 1: break;
                    case 2: fc.lvmin = fc.lvmax = std::get<1>(t3); break;
                    case 3: fc.lvmin = std::get<1>(t3); fc.lvmax = std::get<2>(t3); break;
                    default: break;
                }
            }
            if(pscale) {
                auto t4 = ParseValue(pscale->GetTextUI()->GetText());
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
        if(keyword)
            keyword->GetTextUI()->Clear();
        if(attack)
            attack->GetTextUI()->Clear();
        if(defence)
            defence->GetTextUI()->Clear();
        if(star)
            star->GetTextUI()->Clear();
        if(pscale)
            pscale->GetTextUI()->Clear();
        if(arctype)
            arctype->SetSelection(0);
        if(subtype)
            subtype->SetSelection(0);
        if(limit_type)
            limit_type->SetSelection(0);
        if(pool_type)
            pool_type->SetSelection(0);
        if(attribute)
            attribute->SetSelection(0);
        if(race)
            race->SetSelection(0);
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
        int32_t pos = (int32_t)valstr.find(L'-');
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
	
    void InfoPanel::ShowInfo(uint32_t code) {
        sgui::SGPanel* wnd = nullptr;
        auto data = DataMgr::Get()[code];
        if(data == nullptr)
            return;
        if(window.expired()) {
            wnd = LoadDialogAs<sgui::SGPanel>("info dialog");
            if(!wnd)
                return;
            wnd->SetDragable(true);
            card_image = wnd->FindWidgetAs<sgui::SGImage>("card image");
            scroll_area = wnd->FindWidgetAs<sgui::SGScrollArea>("scroll area");
            misc_image = wnd->FindWidgetAs<sgui::SGImageList>("misc image");
            info_text = wnd->FindWidgetAs<sgui::SGLabel>("info text");
            pen_text = wnd->FindWidgetAs<sgui::SGLabel>("pendulum text");
            desc_text = wnd->FindWidgetAs<sgui::SGLabel>("card text");
            window = wnd->CastPtr<sgui::SGWidgetContainer>();
            auto pin_panel = wnd->FindWidgetAs<sgui::SGCheckBox<>>("pin panel");
            if(pin_panel) {
                pin_panel->event_check_change += [wnd, this](sgui::SGWidget& sender, bool chk) {
                    if(chk)
                        sgui::SGGUIRoot::GetSingleton().PopupCancel(wnd->shared_from_this());
                    else
                        sgui::SGGUIRoot::GetSingleton().PopupObject(wnd->shared_from_this());
                    return true;
                };
                if(lock_panel)
                    pin_panel->SetChecked(lock_panel);
                else
                    sgui::SGGUIRoot::GetSingleton().PopupObject(wnd->shared_from_this());
            }
            if(scroll_area) {
                scroll_area->event_mouse_wheel += [this](sgui::SGWidget& sender, float dx, float dy)->bool {
                    auto vbar = scroll_area->GetScrollBar(false);
                    if(scroll_area->CurrentHovingWidget() != vbar)
                        vbar->OnMouseWheel(dx, dy);
                    return true;
                };
                scroll_area->SetDragTarget(wnd->shared_from_this());
            }
        } else {
            if(this->code == code)
                return;
            if(scroll_area)
                scroll_area->ChangeViewOffset({0, 0});
        }
        auto& dlg_node = dialogCfg["info dialog"];
        int32_t info_margin = dlg_node["info margin"].to_value<int32_t>();
        recti star_offset = sgui::SGJsonUtil::ConvertRect<int32_t>(dlg_node["star offset"]);
        int32_t slider_width = dlg_node["slider width"].to_value<int32_t>();
        int32_t min_pen_height = dlg_node["min pendilum text height"].to_value<int32_t>();
        int32_t scale_width = dlg_node["scale blank"].to_value<int32_t>();
        v2i scale_icon = sgui::SGJsonUtil::ConvertVec2<int32_t>(dlg_node["scale icon"], 0);
        v2i scale_size = sgui::SGJsonUtil::ConvertVec2<int32_t>(dlg_node["scale size"], 0);
        uint32_t info_backcolor = sgui::SGJsonUtil::ConvertRGBA(dlg_node["info backcolor"]);
        
        auto hmask = ImageMgr::Get().GetTexture("mmask");
        auto star = ImageMgr::Get().GetTexture("mstar");
        this->code = code;
        uint64_t setcode = data->setcode;
        if(data->alias) {
            auto aliasdata = DataMgr::Get()[data->alias];
            if(aliasdata)
                setcode = aliasdata->setcode;
        }
        
        if(card_image) {
            auto ctex = ImageMgr::Get().LoadBigCardTexture(code);
            if(ctex) {
                card_image->GetSpriteUI()->SetTexture(ctex);
                card_image->GetSpriteUI()->SetTextureRect({0, 0, ctex->GetImgWidth(), ctex->GetImgHeight()});
            }
        }
        if(misc_image) {
            misc_image->GetSpriteUI()->Clear();
            misc_image->GetSpriteUI()->SetTexture(ImageMgr::Get().GetRawMiscTexture());
        }
        sgui::UIVertexArray<4> v;
        int32_t info_text_height = 0;
        if(info_text) {
            info_text->GetTextUI()->Clear();
            info_text->SetPosition({info_margin, info_margin});
            info_text->GetTextUI()->SetLinespacing(2);
            //card name
            info_text->GetTextUI()->AppendText(data->name, 0xff000000);
            
            uint32_t ccode = (data->alias == 0
                              || (data->alias > data->code && data->alias - data->code > 10)
                              || (data->alias < data->code && data->code - data->alias > 10)) ? data->code : data->alias;
            info_text->GetTextUI()->AppendText(L" [", 0xff000000);
            info_text->GetTextUI()->AppendText(To<std::wstring>(L"%08d", ccode), 0xffff0000);
            
            info_text->GetTextUI()->AppendText(L"]\n", 0xff000000);
            // types
            info_text->GetTextUI()->AppendText(DataMgr::Get().GetTypeString(data->type), 0xff000000);
            if(data->type & 0x1) {
                info_text->GetTextUI()->AppendText(L"\n", 0xff000000);
                info_text->GetTextUI()->AppendText(DataMgr::Get().GetAttributeString(data->attribute), 0xff000000);
                info_text->GetTextUI()->AppendText(L"/", 0xff000000);
                info_text->GetTextUI()->AppendText(DataMgr::Get().GetRaceString(data->race), 0xff000000);
                info_text->GetTextUI()->AppendText(L"    ", 0xff000000);
                std::wstring adstr;
                if(data->attack >= 0)
                    adstr.append(To<std::wstring>(L"ATK/% 4ld", data->attack));
                else
                    adstr.append(L"ATK/  ? ");
                if(data->defence >= 0)
                    adstr.append(To<std::wstring>(L"  DEF/% 4ld", data->defence));
                else
                    adstr.append(L" DEF/  ? ");
                info_text->GetTextUI()->AppendText(adstr, 0xff000000);
            }
            if(setcode) {
                info_text->GetTextUI()->AppendText(L"\n", 0xff000000);
                info_text->GetTextUI()->AppendText(To<std::wstring>(stringCfg["eui_msg_setcode"].to_string()), 0xff000000);
                for(int32_t i = 0; i < 4; ++i) {
                    uint16_t sd = (setcode >> (i * 16)) & 0xffff;
                    if(sd) {
                        info_text->GetTextUI()->AppendText(L"  @", 0xff000000);
                        info_text->GetTextUI()->AppendText(DataMgr::Get().GetSetName(sd), 0xffff0000);
                    }
                }
            }
            info_text_height = info_text->GetAbsoluteSize().y;
            if(misc_image) {
                v.BuildSprite({0, 0, -slider_width, info_text_height + info_margin * 2}, {0.0f, 0.0f, 1.0f, 0.0f}, hmask, info_backcolor);
                misc_image->GetSpriteUI()->AddSprite(v.Ptr());
                if(data->type & 0x1) {
                    for(uint32_t i = 0; i < data->star; ++i) {
                        int32_t starx = -slider_width - star_offset.left - star_offset.width * (i + 1);
                        v.BuildSprite({starx, star_offset.top, star_offset.width, star_offset.height}, {1.0f, 0.0f, 0.0f, 0.0f}, star, 0xffffffff);
                        misc_image->GetSpriteUI()->AddSprite(v.Ptr());
                    }
                }
            }
            info_text_height += info_margin * 3;
        }
        
        int32_t pen_height = 0;
        std::wstring pdelimiter = To<std::wstring>(stringCfg["pendulum_delimiter"].to_string());
        auto pd = data->texts.find(pdelimiter);
        if(data->lscale || data->rscale) {
            int32_t ph = 0;
            if(pen_text) {
                pen_text->GetTextUI()->Clear();
                pen_text->SetPosition({info_margin + scale_width, info_text_height + info_margin});
                pen_text->GetTextUI()->SetLinespacing(2);
                pen_text->GetTextUI()->SetMaxWidth(scroll_area->GetViewSize().x - slider_width - scale_width * 2);
                if(pd > 0)
                    pen_text->GetTextUI()->SetText(data->texts.substr(0, pd - 1), 0xff000000);
                ph = pen_text->GetAbsoluteSize().y;
            }
            if(ph < min_pen_height)
                ph = min_pen_height;
            v.BuildSprite({0, info_text_height, -slider_width, ph + info_margin * 2}, {0.0f, 0.0f, 1.0f, 0.0f}, hmask, info_backcolor);
            misc_image->GetSpriteUI()->AddSprite(v.Ptr());
            pen_height = ph + info_margin * 3;
            
            auto lscale = ImageMgr::Get().GetTexture("lscale");
            auto rscale = ImageMgr::Get().GetTexture("rscale");
            auto push_num = [&v, &scale_size, this](v2i pos, float px, int32_t i, uint32_t cl) {
                v.BuildSprite({pos.x, pos.y, scale_size.x, scale_size.y}, {px, 0.0f, 0.0f, 0.0f}, ImageMgr::Get().GetCharTex(L'0' + i), cl);
                misc_image->GetSpriteUI()->AddSprite(v.Ptr());
            };
            int32_t scale_y = info_text_height + info_margin + scale_icon.y;
            int32_t left_midx = scale_width / 2;
            v.BuildSprite({left_midx - scale_icon.x / 2, info_text_height + info_margin, scale_icon.x, scale_icon.y}, {0.0f, 0.0f, 0.0f, 0.0f}, lscale, 0xffffffff);
            misc_image->GetSpriteUI()->AddSprite(v.Ptr());
            if(data->lscale >= 10) {
                push_num({left_midx - scale_size.x, scale_y}, 0.0f, data->lscale / 10, 0xff000000);
                push_num({left_midx, scale_y}, 0.0f, data->lscale % 10, 0xff000000);
            } else {
                push_num({left_midx - scale_size.x / 2, scale_y}, 0.0f, data->lscale, 0xff000000);
            }
            int32_t right_midx = -slider_width - scale_width / 2;
            v.BuildSprite({right_midx - scale_icon.x / 2, info_text_height + info_margin, scale_icon.x, scale_icon.y}, {1.0f, 0.0f, 0.0f, 0.0f}, rscale, 0xffffffff);
            misc_image->GetSpriteUI()->AddSprite(v.Ptr());
            if(data->rscale >= 10) {
                push_num({right_midx - scale_size.x, scale_y}, 1.0f, data->rscale / 10, 0xff000000);
                push_num({right_midx, scale_y}, 1.0f, data->rscale % 10, 0xff000000);
            } else {
                push_num({right_midx - scale_size.x / 2, scale_y}, 1.0f, data->rscale, 0xff000000);
            }
        } else {
            if(pen_text)
                pen_text->GetTextUI()->Clear();
        }
        
        int32_t desc_height = 0;
        if(desc_text) {
            desc_text->GetTextUI()->Clear();
            desc_text->SetPosition({info_margin, info_text_height + pen_height + info_margin});
            desc_text->GetTextUI()->SetLinespacing(2);
            desc_text->GetTextUI()->SetMaxWidth(scroll_area->GetViewSize().x - slider_width - info_margin * 2);
            if(pd != std::wstring::npos)
                desc_text->GetTextUI()->SetText(data->texts.substr(pd + pdelimiter.length()), 0xff000000);
            else
                desc_text->GetTextUI()->SetText(data->texts, 0xff000000);
            desc_height = desc_text->GetAbsoluteSize().y + info_margin * 2;
            if(info_text_height + pen_height + desc_height < scroll_area->GetViewSize().y)
                desc_height = scroll_area->GetViewSize().y - info_text_height - pen_height;
        }
        if(misc_image) {
            v.BuildSprite({0, info_text_height + pen_height, -slider_width, desc_height}, {0.0f, 0.0f, 1.0f, 0.0f}, hmask, info_backcolor);
            misc_image->GetSpriteUI()->AddSprite(v.Ptr());
        }
        scroll_area->SetScrollSize({scroll_area->GetViewSize().x, info_text_height + pen_height + desc_height});
    }
    
    void InfoPanel::Destroy() {
        if(!window.expired())
            window.lock()->RemoveFromParent();
        code = 0;
    }
    
    void LogPanel::Show(int32_t type) {
        sgui::SGPanel* wnd = nullptr;
        if(window.expired()) {
            wnd = LoadDialogAs<sgui::SGPanel>("log dialog");
            if(!wnd)
                return;
            window = wnd->CastPtr<sgui::SGWidgetContainer>();
            tabs = wnd->FindWidgetAs<sgui::SGTabControl>("log tab");
            if(tabs)
                tabs->SetDragTarget(wnd->shared_from_this());
            log_list[0] = wnd->FindWidgetAs<sgui::SGListBox>("chat list");
            log_list[1] = wnd->FindWidgetAs<sgui::SGListBox>("duel list");
            log_list[2] = wnd->FindWidgetAs<sgui::SGListBox>("sys list");
            chat_box = wnd->FindWidgetAs<sgui::SGTextEdit>("chat box");
            auto& back_color_node = dialogCfg["log dialog"]["item back color"];
            default_item_color = sgui::SGJsonUtil::ConvertRGBA(dialogCfg["log dialog"]["item color"]);
            uint32_t bcolor[3] = {0xc0000000, 0xc0000000, 0xc0404040};
            for(int32_t i = 0; i < 3; ++i)
                bcolor[i] = sgui::SGJsonUtil::ConvertRGBA(back_color_node[i]);
            for(int32_t i = 0; i < 3; ++i) {
                if(log_list[i]) {
                    log_list[i]->SetItemBackColor(bcolor[0], bcolor[1], bcolor[2]);
                    for(auto& iter : logs[i])
                        AddLog(i, iter, false);
                }
            }
            sgui::SGWidget* button = wnd->FindWidgetAs<sgui::SGWidget>("close button");
            if(button) {
                button->event_click += [wnd](sgui::SGWidget& sender)->bool {
                    wnd->RemoveFromParent();
                    return true;
                };
            }
            auto send_callback = [this](sgui::SGWidget& sender)->bool {
                if(chat_box->GetTextUI()->GetText().size() > 0) {
                    AddLog(0, chat_box->GetTextUI()->GetText(), true);
                    chat_box->Clear();
                }
                return true;
            };
            if(chat_box)
                chat_box->event_entered += send_callback;
            sgui::SGWidget* send_button = wnd->FindWidgetAs<sgui::SGWidget>("send");
            if(chat_box && send_button)
                send_button->event_click += send_callback;
        }
        if(tabs) {
            tabs->SetCurrentTab(type);
            if(type == 0 && chat_box)
                chat_box->SetFocus();
        }
    }
    
    void LogPanel::AddLog(int32_t msg_type, const std::wstring& msg, bool newlog) {
        if(msg_type >= 3)
            return;
        if(log_list[msg_type]) {
            log_list[msg_type]->AddItem(L"", default_item_color);
            auto count = log_list[msg_type]->GetItemCount();
            auto ui = log_list[msg_type]->GetItemUI(count - 1);
            ui->PushStringWithFormat(msg, default_item_color, nullptr);
            log_list[msg_type]->SetBeginItem(count);
        }
        if(newlog)
            logs[msg_type].push_back(msg);
    }
    
    void OperationPanel::Confirm(const std::vector<FieldCard*>& cards, std::function<void()> close_callback) {
        if(cards.empty())
            return;
        auto item_size = sgui::SGJsonUtil::ConvertVec2<int32_t>(dialogCfg["confirm dialog"]["item size"], 0);
        auto item_margin = dialogCfg["confirm dialog"]["item margin"].to_value<int32_t>();
        v2i area_size = {(int32_t)(item_size.x * cards.size() + item_margin * (cards.size() - 1)), item_size.y};
        auto wnd = LoadDialogAs<sgui::SGWidgetContainer>("confirm dialog");
        if(!wnd)
            return;
        wnd->event_on_destroy += [close_callback](sgui::SGWidget& sender)->bool { if(close_callback) close_callback(); return true; };
        auto card_area = wnd->FindWidgetAs<sgui::SGScrollArea>("scroll area");
        if(card_area) {
            card_area->SetScrollSize(area_size);
            card_area->ModifyScrollBarPosSize(true)->BatchModifyPosSize().PosRX(0).PosPX(0.0f).SizeRX(0).SizePX(1.0f);
            auto vs = card_area->GetViewSize();
            bool dragable = true;
            if(area_size.x < vs.x) {
                dragable = false;
                card_area->BatchModifyPosSize().PosRX(0).PosPX(0.5f).SizeRX(area_size.x).SizePX(0.0f).AlignX(-0.5f);
            } else {
                card_area->SetDragTarget(card_area->CastPtr<sgui::SGClickableObject>());
                card_area->event_drag_update += [card_area](sgui::SGClickableObject& sender, int32_t sx, int32_t sy, int32_t dx, int32_t dy)->bool {
                    card_area->ChangeViewOffset(card_area->GetViewOffset() + v2i{sx - dx, 0});
                    return true;
                };
            }
            for(int32_t i = 0; i < (int32_t)cards.size(); ++i) {
                uint32_t code = cards[i]->code;
                auto img = card_area->NewChild<sgui::SGImage>(true);
                img->SetPositionSize({i * (item_size.x + item_margin), 0}, item_size);
                img->GetSpriteUI()->SetTexture(ImageMgr::Get().GetRawCardTexture());
                std::weak_ptr<sgui::SGImage> ref_image = img->CastPtr<sgui::SGImage>();
                texf4 card_tex = ImageMgr::Get().GetCardTexture(code, [i, item_size, item_margin, ref_image](texf4 tex) {
                    auto ptr = ref_image.lock();
                    if(!ptr)
                        return;
                    ptr->GetSpriteUI()->SetTexcoords(tex);
                });
                img->GetSpriteUI()->SetDirectCoord(true);
                img->GetSpriteUI()->SetTexcoords(card_tex);
                img->event_mouse_enter += [img, code, ref_image](sgui::SGWidget& sender)->bool {
                    img->SetHColor(0x80ffffff);
                    if(InfoPanel::Get().IsOpen()) {
                        auto wait = std::make_shared<ActionWait<int64_t>>(300);
                        auto show = std::make_shared<ActionCallback<int64_t>>([code, ref_image]() {
                            if(!ref_image.expired())
                                InfoPanel::Get().ShowInfo(code);
                        });
                        SceneMgr::Get().PushAction(std::make_shared<ActionSequence<int64_t>>(wait, show), img, 0);
                    }
                    return true;
                };
                img->event_mouse_leave += [img](sgui::SGWidget& sender)->bool {
                    img->SetHColor(0);
                    SceneMgr::Get().RemoveAction(img);
                    return true;
                };
                if(dragable)
                    img->SetDragTarget(card_area->CastPtr<sgui::SGClickableObject>());
            }
            for(int32_t i = 0; i < (int32_t)cards.size(); ++i) {
                auto lbl = card_area->NewChild<sgui::SGLabel>();
                lbl->GetTextUI()->PushStringWithFormat(cards[i]->pos_info.ToString(), 0xffffffff, nullptr);
                lbl->SetPosition({i * (item_size.x + item_margin) + item_size.x / 2, item_size.y + 10}, {0.0f, 0.0f}, {-0.5f, 0.0f});
            }
        }
        auto ok_button = wnd->FindWidgetAs<sgui::SGTextButton>("ok button");
        if(ok_button)
            ok_button->event_click += [wnd](sgui::SGWidget& sender)->bool { wnd->RemoveFromParent(); return true; };
    }
    
    void OperationPanel::DeclearAttribute(int32_t available, int32_t count, std::function<void(uint32_t)> close_callback) {
        static std::string att_names[] = {"earth", "water", "fire", "wind", "light", "dark", "divine"};
        uint32_t icolor = sgui::SGJsonUtil::ConvertRGBA(dialogCfg["declear attribute"]["item color"]);
        uint32_t scolor = sgui::SGJsonUtil::ConvertRGBA(dialogCfg["declear attribute"]["selected color"]);
        auto wnd = LoadDialogAs<sgui::SGWidgetContainer>("declear attribute");
        if(!wnd)
            return;
        uint32_t* preturn_value = new uint32_t(0);
        uint32_t* pcount = new uint32_t(0);
        wnd->event_on_destroy += [preturn_value, pcount, close_callback](sgui::SGWidget& sender)->bool {
            if(close_callback)
                close_callback(*preturn_value);
            delete preturn_value;
            delete pcount;
            return true;
        };
        for(int32_t i = 0; i < 7; ++i) {
            auto att_button = wnd->FindWidgetAs<sgui::SGTextButton>(att_names[i]);
            if(!att_button)
                continue;
            att_button->SetColor(icolor);
            uint32_t att_value = 1 << i;
            if((available & att_value) == 0) {
                att_button->SetVisible(false);
            } else {
                att_button->event_click += [wnd, icolor, scolor, att_value, preturn_value, pcount, count](sgui::SGWidget& sender) {
                    sgui::SGTextButton& btn = static_cast<sgui::SGTextButton&>(sender);
                    if(btn.IsPushed()) {
                        *preturn_value |= att_value;
                        (*pcount)++;
                        btn.SetColor(scolor);
                        if(*pcount >= count) {
                            wnd->RemoveFromParent();
                        }
                    } else {
                        *preturn_value &= ~att_value;
                        (*pcount)--;
                        btn.SetColor(icolor);
                    }
                    return true;
                };
            }
        }
    }
    
    void OperationPanel::DeclearRace(int32_t available, int32_t count, std::function<void(uint32_t)> close_callback) {
        static std::string rac_names[] = {
            "warrior", "spellcaster", "fairy", "fiend",
            "zombie", "machine", "aqua", "pyro",
            "rock", "windbeast", "plant", "insect",
            "thunder", "dragon", "beast", "beastwarrior",
            "dinosaur", "fish", "seaserpent", "reptile",
            "psychic", "divine", "creatorgod", "phantomdragon"
        };
        uint32_t icolor = sgui::SGJsonUtil::ConvertRGBA(dialogCfg["declear race"]["item color"]);
        uint32_t scolor = sgui::SGJsonUtil::ConvertRGBA(dialogCfg["declear race"]["selected color"]);
        auto wnd = LoadDialogAs<sgui::SGWidgetContainer>("declear race");
        if(!wnd)
            return;
        uint32_t* preturn_value = new uint32_t(0);
        uint32_t* pcount = new uint32_t(0);
        wnd->event_on_destroy += [preturn_value, pcount, close_callback](sgui::SGWidget& sender)->bool {
            if(close_callback)
                close_callback(*preturn_value);
            delete preturn_value;
            delete pcount;
            return true;
        };
        for(int32_t i = 0; i < 28; ++i) {
            auto rac_button = wnd->FindWidgetAs<sgui::SGTextButton>(rac_names[i]);
            if(!rac_button)
                continue;
            rac_button->SetColor(icolor);
            uint32_t rac_value = 1 << i;
            if((available & rac_value) == 0) {
                rac_button->SetVisible(false);
            } else {
                rac_button->event_click += [wnd, icolor, scolor, rac_value, preturn_value, pcount, count](sgui::SGWidget& sender) {
                    sgui::SGTextButton& btn = static_cast<sgui::SGTextButton&>(sender);
                    if(btn.IsPushed()) {
                        *preturn_value |= rac_value;
                        (*pcount)++;
                        btn.SetColor(scolor);
                        if(*pcount >= count) {
                            wnd->RemoveFromParent();
                        }
                    } else {
                        *preturn_value &= ~rac_value;
                        (*pcount)--;
                        btn.SetColor(icolor);
                    }
                    return true;
                };
            }
        }
    }
}

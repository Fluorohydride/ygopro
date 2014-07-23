#include <wx/dir.h>
#include <wx/filename.h>

#include "gui_extra.h"
#include "scene_mgr.h"
#include "card_data.h"

namespace ygopro
{
    
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
        auto btn = sgui::SGButton::Create(wd, {190, 285}, {100, 30});
        btn->SetText(stringCfg[L"eui_button_ok"], 0xff000000);
        window = wd;
        auto ppath = fpath.get();
        auto pfile = ffile.get();
        auto plst = lst.get();
        btn->eventButtonClick.Bind([this, pfile](sgui::SGWidget& sender)->bool{
            if(cbOK != nullptr)
                cbOK(path + L"/" + pfile->GetText());
            window.lock()->Destroy();
            return true;
        });
        lst->eventSelChange.Bind([this, pfile, plst](sgui::SGWidget& sender, int index)->bool{
            if(index < 0)
                return true;
            auto it = plst->GetItem(index);
            if(std::get<0>(it) == 141)
                pfile->SetText(std::get<1>(it), 0xff000000);
            return true;
        });
        lst->eventDoubleClick.Bind([this, plst, ppath](sgui::SGWidget& sender, int index)->bool{
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
        for(int i = 0; i < dirs.GetCount(); ++i)
            list->AddItem(140, dirs[i].ToStdWstring(), 0xff000000);
        for(int i = 0; i < files.GetCount(); ++i)
            list->AddItem(141, files[i].ToStdWstring(), 0xff000000);
    }
    
    void FilterDialog::Show(FilterCondition& fc) {
        
    }
    
}

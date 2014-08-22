#include "../common/common.h"

#include <wx/wfstream.h>
#include <wx/zipstrm.h>

#include "zip_archive.h"

namespace ygopro
{
    
    ZipArchive::~ZipArchive() {
        
    }
    
    bool ZipArchive::Load(const std::wstring& file) {
        wxFFileInputStream in(file);
        if(!in.IsOk())
            return false;
        wxZipInputStream zip(in);
        if(!zip.IsOk())
            return false;
        wxZipEntry* entry;
        while((entry = zip.GetNextEntry()) != nullptr) {
            std::string file = entry->GetInternalName().ToUTF8().data();
            auto& current = entries[file];
            current.reset(entry);
        }
        current_file = file;
        return true;
    }
    
    int ZipArchive::GetFileLength(const std::string& filename) {
        auto iter = entries.find(filename);
        if(iter == entries.end())
            return 0;
        auto& ptr = iter->second;
        if(ptr == nullptr)
            return 0;
        return ptr->GetSize();
    }
    
    int ZipArchive::ReadFile(const std::string& filename, unsigned char* buffer) {
        auto iter = entries.find(filename);
        if(iter == entries.end())
            return 0;
        auto& ptr = iter->second;
        if(ptr == nullptr)
            return 0;
        wxFFileInputStream in(current_file);
        if(!in.IsOk())
            return 0;
        wxZipInputStream zip(in);
        if(!zip.IsOk())
            return 0;
        int sz = ptr->GetSize();
        zip.OpenEntry(*iter->second);
        zip.Read(buffer, sz);
        zip.CloseEntry();
        return sz;
    }
    
}

#include "../common/common.h"

#include <zlib.h>

#include "zip_archive.h"

namespace ygopro
{
    
    ZipArchive::~ZipArchive() {
        
    }
    
    bool ZipArchive::Load(const std::wstring& file) {
        std::ifstream zipfile(To<std::string>(file), std::ios::in | std::ios::binary);
        if(!zipfile)
            return false;
        current_file = file;
        return true;
    }
    
    int ZipArchive::GetFileLength(const std::string& filename) {
        auto iter = entries.find(filename);
        if(iter == entries.end())
            return 0;
        return 0;
    }
    
    int ZipArchive::ReadFile(const std::string& filename, unsigned char* buffer) {
        auto iter = entries.find(filename);
        if(iter == entries.end())
            return 0;
        return 0;
    }
    
}

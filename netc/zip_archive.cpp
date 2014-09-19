#include "../common/common.h"

#include <zlib.h>

#include "zip_archive.h"

namespace ygopro
{
    
    ZipArchive::~ZipArchive() {
        if(!zip_file)
			zip_file.close();
    }
    
    bool ZipArchive::Load(const std::wstring& file) {
        zip_file.open(To<std::string>(file), std::ios::in | std::ios::binary);
        if(!zip_file)
            return false;
		unsigned int header = 0;
		char* pheader = (char*)&header;
		ZipHeader zip_header;
		char buffer[1024];
		while(!zip_file.eof()) {
			while(pheader[0] != 0x50 && !zip_file.eof())
				zip_file.read(pheader, 1);
			if(pheader[0] == 0x50) {
				zip_file.read(&pheader[1], 3);
				if(header != 0x04034b50)
					continue;
				zip_file.read((char*)&zip_header, ZIP_HEADER_SIZE);
				zip_file.read(buffer, zip_header.name_size);
				buffer[zip_header.name_size] = 0;
				std::string name = buffer;
				ZipFileInfo& finfo = entries[name];
				finfo.compressed = (zip_header.comp_fun == 0x8);
				finfo.comp_size = zip_header.comp_size;
				finfo.file_size = zip_header.file_size;
				finfo.data_offset = (long)zip_file.tellg() + (long)zip_header.ex_size;
				if(zip_header.global_sig & 0x4)
					zip_file.seekg(zip_header.ex_size + zip_header.comp_size + ZIP_FILEDESC_SIZE, zip_file.cur);
				else
					zip_file.seekg(zip_header.ex_size + zip_header.comp_size, zip_file.cur);
			}
		}
        return true;
    }
    
    int ZipArchive::GetFileLength(const std::string& filename) {
		if(!zip_file)
			return 0;
        auto iter = entries.find(filename);
        if(iter == entries.end())
            return 0;
        return iter->second.file_size;
    }
    
    int ZipArchive::ReadFile(const std::string& filename, unsigned char* buffer) {
		if(!zip_file)
			return 0;
        auto iter = entries.find(filename);
        if(iter == entries.end())
            return 0;
        return 0;
    }
    
}

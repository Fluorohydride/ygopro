#include "../common/common.h"

#include <zlib.h>

#include "zip_archive.h"

namespace ygopro
{
    
    ZipArchive::~ZipArchive() {
    }
    
    void ZipArchive::Load(const std::vector<std::wstring>& files) {
        is_loading = true;
        loading_files = files;
        std::thread(std::bind(&ZipArchive::_load_inner, this)).detach();
    }
    
    int ZipArchive::GetFileLength(const std::string& filename) {
        while(is_loading)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        auto iter = entries.find(filename);
        if(iter == entries.end())
            return 0;
        return iter->second.file_size;
    }
    
    int ZipArchive::ReadFile(const std::string& filename, unsigned char* buffer) {
        while(is_loading)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        auto iter = entries.find(filename);
        if(iter == entries.end())
            return 0;
        std::ifstream zip_file(iter->second.src_file, std::ios::in | std::ios::binary);
        if(!zip_file)
            return 0;
        if(iter->second.compressed) {
            unsigned char* raw = new unsigned char[iter->second.comp_size];
            zip_file.seekg(iter->second.data_offset, zip_file.beg);
            zip_file.read((char*)raw, iter->second.comp_size);
            unsigned long decom_size = iter->second.file_size;
            z_stream strm;
            strm.zalloc = Z_NULL;
            strm.zfree = Z_NULL;
            strm.opaque = Z_NULL;
            inflateInit2(&strm, -15);
            strm.avail_in = iter->second.comp_size;
            strm.next_in = raw;
            strm.avail_out = iter->second.file_size;
            strm.next_out = buffer;
            int ret = inflate(&strm, Z_FINISH);
            inflateEnd(&strm);
            delete []raw;
            if(ret != Z_STREAM_END)
                return 0;
            return decom_size;
        } else {
            zip_file.seekg(iter->second.data_offset, zip_file.beg);
            zip_file.read((char*)buffer, iter->second.file_size);
            return iter->second.file_size;
        }
    }
    
    void ZipArchive::_load_inner() {
        for(auto& file : loading_files) {
            std::string fname = To<std::string>(file);
            std::ifstream zip_file(fname, std::ios::in | std::ios::binary);
            if(!zip_file)
                continue;
            unsigned int header = 0;
            char* pheader = (char*)&header;
            ZipHeader zip_header;
            char buffer[1024];
            while(!zip_file.eof()) {
                header = 0;
                while(pheader[0] != 0x50 && !zip_file.eof())
                    zip_file.read(pheader, 1);
                if(pheader[0] == 0x50) {
                    zip_file.read(&pheader[1], 3);
                    if(header == 0x02014b50)
                        break;
                    if(header != 0x04034b50) {
                        zip_file.seekg(-3, zip_file.cur);
                        continue;
                    }
                    zip_file.read((char*)&zip_header, sizeof(ZipHeader));
                    zip_file.read(buffer, zip_header.name_size);
                    buffer[zip_header.name_size] = 0;
                    std::string name = buffer;
                    ZipFileInfo& finfo = entries[name];
                    finfo.src_file = fname;
                    finfo.compressed = (zip_header.comp_fun == 0x8);
                    finfo.comp_size = zip_header.comp_size;
                    finfo.file_size = zip_header.file_size;
                    finfo.data_offset = (long)zip_file.tellg() + (long)zip_header.ex_size;
                    if(zip_header.global_sig & 0x8)
                        zip_file.seekg(zip_header.ex_size + zip_header.comp_size + ZIP_FILEDESC_SIZE, zip_file.cur);
                    else
                        zip_file.seekg(zip_header.ex_size + zip_header.comp_size, zip_file.cur);
                }
            }
        }
        loading_files.clear();
        is_loading = false;
    }
}

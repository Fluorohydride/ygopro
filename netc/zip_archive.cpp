#include "../common/common.h"

#include <zlib.h>

#include "zip_archive.h"

namespace ygopro
{
    
    ZipArchive::~ZipArchive() {
    }
    
    void ZipArchive::Load(const std::vector<std::wstring>& files) {
        char name_buffer[1024];
        ZipEndBlock end_block;
        for(auto& file : files) {
            std::string fname = To<std::string>(file);
            std::ifstream zip_file(fname, std::ios::in | std::ios::binary);
            if(!zip_file)
                continue;
            zip_file.seekg(0, zip_file.end);
            auto file_size = zip_file.tellg();
            if(file_size == 0)
                continue;
            zip_file.seekg(-ZIP_END_BLOCK_SIZE, zip_file.end);
            zip_file.read((char*)&end_block, ZIP_END_BLOCK_SIZE);
            if(end_block.block_header != 0x06054b50) {
                int32_t end_buffer_size = (file_size >= 0xffff + ZIP_END_BLOCK_SIZE) ? (0xffff + ZIP_END_BLOCK_SIZE) : (int32_t)file_size;
                char* end_buffer = new char[end_buffer_size];
                zip_file.seekg(-end_buffer_size, zip_file.end);
                zip_file.read(end_buffer, end_buffer_size);
                int32_t end_block_pos = -1;
                for(int32_t i = end_buffer_size - 4; i >= 0; --i) {
                    if(end_buffer[i] == 0x50) {
                        if(end_buffer[i + 1] == 0x4b && end_buffer[i + 2] == 0x05 && end_buffer[i + 3] == 0x06) {
                            end_block_pos = i;
                            break;
                        }
                    }
                }
                delete[] end_buffer;
                if(end_block_pos == -1)
                    continue;
                zip_file.seekg(-(end_buffer_size - end_block_pos), zip_file.end);
            }
            zip_file.seekg(end_block.directory_offset, zip_file.beg);
            ZipDirectoryHeader* dir_header = nullptr;
            char* buffer = new char[end_block.directory_size];
            zip_file.read(buffer, end_block.directory_size);
            auto pos = 0;
            while(pos + ZIP_DIRECTORY_SIZE < end_block.directory_size) {
                while(buffer[pos] != 0x50 && pos + ZIP_DIRECTORY_SIZE < end_block.directory_size)
                    ++pos;
                if(buffer[pos] == 0x50) {
                    dir_header = reinterpret_cast<ZipDirectoryHeader*>(&buffer[pos]);
                    if(dir_header->block_header != 0x02014b50)
                        continue;
                    memcpy(name_buffer, &buffer[pos + ZIP_DIRECTORY_SIZE], dir_header->name_size);
                    name_buffer[dir_header->name_size] = 0;
                    std::string name = name_buffer;
                    ZipFileInfo& finfo = entries[name];
                    finfo.src_file = fname;
                    finfo.compressed = (dir_header->comp_fun == 0x8);
                    finfo.comp_size = dir_header->comp_size;
                    finfo.file_size = dir_header->file_size;
                    finfo.data_offset = dir_header->data_offset;
                    pos += ZIP_DIRECTORY_SIZE + dir_header->name_size + dir_header->ex_size + dir_header->cmt_size;
                }
            }
            delete[] buffer;
        }
    }
    
    size_t ZipArchive::GetFileLength(const std::string& filename) {
        auto iter = entries.find(filename);
        if(iter == entries.end())
            return 0;
        return iter->second.file_size;
    }
    
    size_t ZipArchive::ReadFile(const std::string& filename, uint8_t* buffer) {
        auto iter = entries.find(filename);
        if(iter == entries.end())
            return 0;
        std::ifstream zip_file(iter->second.src_file, std::ios::in | std::ios::binary);
        if(!zip_file)
            return 0;
        ZipFileHeader file_header;
        zip_file.seekg(iter->second.data_offset, zip_file.beg);
        zip_file.read((char*)&file_header, ZIP_FILE_SIZE);
        if(file_header.block_header != 0x04034b50)
            return 0;
        zip_file.seekg(file_header.name_size + file_header.ex_size, zip_file.cur);
        if(iter->second.compressed) {
            uint8_t* raw = new uint8_t[iter->second.comp_size];
            zip_file.read((char*)raw, iter->second.comp_size);
            size_t decom_size = iter->second.file_size;
            z_stream strm;
            strm.zalloc = Z_NULL;
            strm.zfree = Z_NULL;
            strm.opaque = Z_NULL;
            inflateInit2(&strm, -15);
            strm.avail_in = iter->second.comp_size;
            strm.next_in = raw;
            strm.avail_out = iter->second.file_size;
            strm.next_out = buffer;
            int32_t ret = inflate(&strm, Z_FINISH);
            inflateEnd(&strm);
            delete []raw;
            if(ret != Z_STREAM_END)
                return 0;
            return decom_size;
        } else {
            zip_file.read((char*)buffer, iter->second.file_size);
            return iter->second.file_size;
        }
    }
}

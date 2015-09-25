#ifndef _ZIP_READER_H_
#define _ZIP_READER_H_

const int32_t ZIP_FILE_SIZE = 30;
const int32_t ZIP_DIRECTORY_SIZE = 46;
const int32_t ZIP_END_BLOCK_SIZE = 22;

#ifdef _WIN32
#pragma pack(push, 2)
#endif
// header = 0x02014b50
struct ZipDirectoryHeader {
    int32_t block_header;
    int16_t ver_compress;
    int16_t ver_decompress;
    int16_t global_sig;
    int16_t comp_fun;  // only support 00-no compression 08-deflate
    int16_t mod_time;
    int16_t mod_date;
    int32_t crc32;
    int32_t comp_size;
    int32_t file_size;
    int16_t name_size;
    int16_t ex_size;
    int16_t cmt_size;
    int16_t start_disk; // generally 0
    int16_t inter_att;
    int32_t exter_att;
    int32_t data_offset;
#ifdef _WIN32
};
#pragma pack(pop)
#else
} __attribute__((packed));
#endif

#ifdef _WIN32
#pragma pack(push, 2)
#endif
// header = 0x04034b50
struct ZipFileHeader {
    int32_t block_header;
    int16_t ver_req;
    int16_t global_sig;
    int16_t comp_fun; // only support 00-no compression 08-deflate
    int16_t mod_time;
    int16_t mod_date;
    int32_t crc32;
    int32_t comp_size;
    int32_t file_size;
    int16_t name_size;
    int16_t ex_size;
#ifdef _WIN32
};
#pragma pack(pop)
#else
} __attribute__((packed));
#endif

#ifdef _WIN32
#pragma pack(push, 2)
#endif
// header = 0x06054b50
struct ZipEndBlock {
    int32_t block_header;
    int16_t disk_number; // generally 0
    int16_t directory_disk; // generally 0
    int16_t directory_count_disk;
    int16_t directory_count; // generally same as directory_count_disk
    int32_t directory_size;
    int32_t directory_offset;
    int16_t comment_size;
#ifdef _WIN32
};
#pragma pack(pop)
#else
} __attribute__((packed));
#endif

#include "utils/stb_image.h"

struct ZipFileInfo {
    std::string src_file;
    bool compressed = false;
    size_t data_offset = 0;
    size_t comp_size = 0;
    size_t file_size = 0;
    uint8_t* datas = nullptr;
};

class ZipReader {
public:
    ~ZipReader() {
        for(auto& iter : entries)
            if(iter.second.datas != nullptr)
                delete[] iter.second.datas;
    }
    
    void Load(const std::vector<std::string>& files) {
        char name_buffer[1024];
        ZipEndBlock end_block;
        for(auto& file : files) {
            std::ifstream zip_file(file, std::ios::in | std::ios::binary);
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
                    finfo.src_file = file;
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
    
    size_t GetFileLength(const std::string& filename) {
        auto iter = entries.find(filename);
        if(iter == entries.end())
            return 0;
        return iter->second.file_size;
    }
    
    std::pair<uint8_t*, size_t> ReadFile(const std::string& filename) {
        auto iter = entries.find(filename);
        if(iter == entries.end())
            return std::make_pair(nullptr, 0);
        if(iter->second.datas != nullptr)
            return std::make_pair(iter->second.datas, iter->second.file_size);
        std::ifstream zip_file(iter->second.src_file, std::ios::in | std::ios::binary);
        if(!zip_file)
            return std::make_pair(nullptr, 0);
        ZipFileHeader file_header;
        zip_file.seekg(iter->second.data_offset, zip_file.beg);
        zip_file.read((char*)&file_header, ZIP_FILE_SIZE);
        if(file_header.block_header != 0x04034b50)
            return std::make_pair(nullptr, 0);
        zip_file.seekg(file_header.name_size + file_header.ex_size, zip_file.cur);
        iter->second.datas = new uint8_t[iter->second.file_size];
        if(iter->second.compressed) {
            uint8_t* raw = new uint8_t[iter->second.comp_size];
            zip_file.read((char*)raw, iter->second.comp_size);
            int32_t ret = stbi_zlib_decode_noheader_buffer((char*)iter->second.datas, (int)iter->second.file_size, (char const*)raw, (int)iter->second.comp_size);
            delete []raw;
            if(ret == -1) {
                delete[] iter->second.datas;
                iter->second.datas = nullptr;
                return std::make_pair(nullptr, 0);
            }
            return std::make_pair(iter->second.datas, iter->second.file_size);
        } else {
            zip_file.read((char*)iter->second.datas, iter->second.file_size);
            return std::make_pair(iter->second.datas, iter->second.file_size);
        }
    }
    
protected:
    std::unordered_map<std::string, ZipFileInfo> entries;
};

#endif /* _ZIP_READER_H_ */

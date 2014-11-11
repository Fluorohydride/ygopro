#ifndef _ZIP_READER_H_
#define _ZIP_READER_H_

// currently the game only support reading files from .zip format with deflate algorithm

namespace ygopro
{
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
    
    struct ZipFileInfo {
        std::string src_file;
        bool compressed;
        size_t data_offset;
        size_t comp_size;
        size_t file_size;
    };
    
    class ZipArchive {
    public:
        ~ZipArchive();
        void Load(const std::vector<std::wstring>& files);
        size_t GetFileLength(const std::string& filename);
        size_t ReadFile(const std::string& filename, uint8_t* buffer);
    
    protected:
        std::unordered_map<std::string, ZipFileInfo> entries;
    };
    
}

#endif /* _ZIP_READER_H_ */

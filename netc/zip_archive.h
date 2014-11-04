#ifndef _ZIP_READER_H_
#define _ZIP_READER_H_

// currently the game only support reading files from .zip format with deflate algorithm

namespace ygopro
{
    const int32_t ZIP_HEADER_SIZE = 30;
    const int32_t ZIP_FILEDESC_SIZE = 12;
    
#ifdef _WIN32
    #pragma pack(push, 2)
#endif
	// header = 0x04034b50
    struct ZipHeader {
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
        char name_ptr[1];
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
        void _load_inner();
        
        bool is_loading = false;
        std::vector<std::wstring> loading_files;
        std::unordered_map<std::string, ZipFileInfo> entries;
    };
    
}

#endif /* _ZIP_READER_H_ */

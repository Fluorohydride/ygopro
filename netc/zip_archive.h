#ifndef _ZIP_READER_H_
#define _ZIP_READER_H_

// currently the game only support reading files from .zip format with deflate algorithm

namespace ygopro
{
    const int ZIP_FILEDESC_SIZE = 12;
    
#ifdef _WIN32
    #pragma pack(push, 2)
#endif
	// header = 0x04034b50
    struct ZipHeader {
        short ver_req;
        short global_sig;
        short comp_fun; // only support 00-no compression 08-deflate
        short mod_time;
        short mod_date;
        int crc32;
        int comp_size;
        int file_size;
        short name_size;
        short ex_size;
#ifdef _WIN32
    };
    #pragma pack(pop)
#else
    } __attribute__((packed));
#endif
    
    struct ZipFileInfo {
        std::string src_file;
        bool compressed;
        long data_offset;
        long comp_size;
        long file_size;
    };
    
    class ZipArchive {
    public:
        ~ZipArchive();
        void Load(const std::vector<std::wstring>& files);
        int GetFileLength(const std::string& filename);
        int ReadFile(const std::string& filename, unsigned char* buffer);
    
    protected:
        void _load_inner();
        
        bool is_loading = false;
        std::vector<std::wstring> loading_files;
        std::unordered_map<std::string, ZipFileInfo> entries;
    };
    
}

#endif /* _ZIP_READER_H_ */

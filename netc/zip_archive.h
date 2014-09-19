#ifndef _ZIP_READER_H_
#define _ZIP_READER_H_

class wxZipEntry;

namespace ygopro
{
    const int ZIP_HEADER_SIZE = 30;
    const int ZIP_FILEDESC_SIZE = 12;
    
    struct ZipHeader {
        int header; // 0x04034b50
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
    };
    
    struct ZipFileInfo {
        bool compressed;
        long data_offset;
        long comp_size;
        long file_size;
    };
    
    class ZipArchive {
    public:
        ~ZipArchive();
        bool Load(const std::wstring& file);
        int GetFileLength(const std::string& filename);
        int ReadFile(const std::string& filename, unsigned char* buffer);
        
    protected:
        std::wstring current_file;
        std::unordered_map<std::string, ZipFileInfo> entries;
    };
    
}

#endif /* _ZIP_READER_H_ */

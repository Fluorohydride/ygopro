#ifndef _ZIP_READER_H_
#define _ZIP_READER_H_

namespace ygopro
{
    const int ZIP_HEADER_SIZE = 26;
    const int ZIP_FILEDESC_SIZE = 12;
    
	// header = 0x04034b50
    struct ZipHeader {
		char data[26];
		
        inline short ver_req() { return *(short*)&data[0]; }
        inline short global_sig()  { return *(short*)&data[2]; }
        inline short comp_fun() { return *(short*)&data[4]; } // only support 00-no compression 08-deflate
        inline short mod_time() { return *(short*)&data[6]; }
        inline short mod_date() { return *(short*)&data[8]; }
        inline int crc32() { return *(int*)&data[10]; }
        inline int comp_size() { return *(int*)&data[14]; }
        inline int file_size() { return *(int*)&data[18]; }
        inline short name_size() { return *(short*)&data[22]; }
        inline short ex_size() { return *(short*)&data[24]; }
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
		std::ifstream zip_file;
        std::unordered_map<std::string, ZipFileInfo> entries;
    };
    
}

#endif /* _ZIP_READER_H_ */

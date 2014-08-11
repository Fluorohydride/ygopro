#ifndef _ZIP_READER_H_
#define _ZIP_READER_H_

#include <string>
#include <memory>
#include <unordered_map>

class wxZipEntry;

namespace ygopro
{
    
    class ZipArchive {
    public:
        ~ZipArchive();
        bool Load(const std::wstring& file);
        int GetFileLength(const std::string& filename);
        int ReadFile(const std::string& filename, unsigned char* buffer);
        
    protected:
        std::wstring current_file;
        std::unordered_map<std::string, std::shared_ptr<wxZipEntry>> entries;
    };
    
}

#endif /* _ZIP_READER_H_ */

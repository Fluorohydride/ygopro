#ifndef _TEXTFILE_H_
#define _TEXTFILE_H_

#include "common.h"

class TextFile {
public:
    TextFile() {
        file_size = 0;
        datas.push_back(0);
    }
    
    TextFile(const std::string& filename) {
        Load(filename);
    }
    
    void Load(const std::string& filename) {
        datas.clear();
        std::ifstream txtfile(filename, std::ios::binary);
        if(txtfile) {
            txtfile.seekg(0, txtfile.end);
            file_size = txtfile.tellg();
            txtfile.seekg(0, txtfile.beg);
            datas.resize(file_size + 1);
            txtfile.read(datas.data(), file_size);
            datas[file_size] = 0;
        } else {
            file_size = 0;
            datas.push_back(0);
        }
    }
    
    inline char* Data() {
        return datas.data();
    }
    
    inline size_t Length() { return file_size; }
    
private:
    size_t file_size = 0;
    std::vector<char> datas;
};

#endif

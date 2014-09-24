#ifndef _TEXTFILE_H_
#define _TEXTFILE_H_
    
class TextFile {
public:
    TextFile(const std::string& filename) {
        std::ifstream txtfile(filename, std::ios::binary);
        if(txtfile) {
            txtfile.seekg(0, txtfile.end);
            file_size = txtfile.tellg();
            txtfile.seekg(0, txtfile.beg);
            datas.resize(file_size + 1);
            txtfile.read(&datas[0], file_size);
            datas[file_size] = 0;
        } else {
            file_size = 0;
            datas.push_back(0);
        }
    }
    
    inline char* Data() {
        return &datas[0];
    }
    
private:
    size_t file_size = 0;
    std::vector<char> datas;
};

#endif

#ifndef _BUFFERUTIL_H_
#define _BUFFERUTIL_H_

class BufferUtil {
public:
    BufferUtil(unsigned char* base) {
        ptr = base;
    }
    
    template<typename T>
    inline T Read() {
        T ret = *(T*)ptr;
        ptr += sizeof(T);
        return ret;
    }
    
    template<typename T>
    inline void Write(T val) {
        (*(T*)ptr) = val;
        ptr += sizeof(T);
    }
    
    inline std::string Read() {
        std::string str = (char*)ptr;
        return std::move(str);
    }
    
    inline std::string Read(unsigned int sz) {
        if(sz == 0)
            return "";
        char buf[1024];
        if(sz > 1024)
            sz = 1024;
        memcpy(buf, ptr, sz);
        buf[sz - 1] = 0;
        std::string str = buf;
        return std::move(str);
    }
    
    inline void Read(void* buf, unsigned int sz) {
        memcpy(buf, ptr, sz);
        ptr += sz;
    }
    
    inline void Write(const std::string& str) {
        size_t length = str.length();
        memcpy(ptr, str.c_str(), length);
        ptr += length;
        *ptr = 0;
        ptr++;
    }
    
    inline void Skip(unsigned int offset) {
        ptr += offset;
    }

private:
    unsigned char* ptr;

};

#endif //BUFFERIO_H

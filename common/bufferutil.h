#ifndef _BUFFERUTIL_H_
#define _BUFFERUTIL_H_

class BufferUtil {
public:
    BufferUtil(void* base, size_t sz) {
        ptr = static_cast<uint8_t*>(base);
        end = ptr + sz;
    }
    
    template<typename T>
    inline T Read() {
        T ret = *(T*)ptr;
        ptr += sizeof(T);
        return ret;
    }
    
    template<typename T>
    inline void Read(T* val) {
        memcpy(val, ptr, sizeof(T));
        ptr += sizeof(T);
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
    
    inline std::string Read(size_t sz) {
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
    
    inline void Read(void* buf, size_t sz) {
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
    
    inline void Skip(size_t offset) {
        ptr += offset;
    }

    inline void SetEnd() {
        ptr = end;
    }
    
    inline bool IsEnd() {
        return ptr >= end;
    }
    
private:
    uint8_t* ptr;
    uint8_t* end;

};

#endif //BUFFERIO_H

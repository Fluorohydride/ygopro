#ifndef _BUFFERUTIL_H_
#define _BUFFERUTIL_H_

#include "common.h"

class BufferReader {
public:
    BufferReader(void* base, size_t sz) {
        ptr = reinterpret_cast<uint8_t*>(base);
        end = ptr + sz;
    }
    
    template<typename T>
    inline T Read() {
        if(ptr + sizeof(T) > end) {
            ptr = end;
            return T();
        }
        T ret;
        memcpy(&ret, ptr, sizeof(T));
        ptr += sizeof(T);
        return ret;
    }
    
    template<typename T>
    inline void Read(T* val) {
        if(ptr + sizeof(T) > end) {
            ptr = end;
            return;
        }
        memcpy(val, ptr, sizeof(T));
        ptr += sizeof(T);
    }
    
    inline std::string Read(size_t sz) {
        auto endptr = ptr;
        while(endptr < end && *endptr != 0 && (sz && endptr < ptr + sz))
            endptr++;
        std::string str;
        str.insert(str.end(), ptr, endptr);
        ptr = endptr;
        return str;
    }
    
    inline void Read(void* buf, size_t sz) {
        if(ptr + sz > end)
            sz = end - ptr;
        memcpy(buf, ptr, sz);
        ptr += sz;
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
    
    inline uint8_t* DataPtr() { return ptr; }
    
private:
    uint8_t* ptr;
    uint8_t* end;
};

class BufferWriter {
public:
    BufferWriter(std::vector<uint8_t>& buffer) {
        buffer_ptr = &buffer;
    }
    
    template<typename T>
    inline void Write(const T& val) {
        const uint8_t* data_ptr = reinterpret_cast<const uint8_t*>(&val);
        buffer_ptr->insert(buffer_ptr->end(), data_ptr, data_ptr + sizeof(T));
    }
    
    inline void Write(const std::string& str) {
        buffer_ptr->insert(buffer_ptr->end(), &str.front(), &str.back());
    }
    
    inline void WriteRepeat(uint8_t val, size_t count) {
        buffer_ptr->insert(buffer_ptr->end(), count, val);
    }
    
    inline void WriteBuffer(uint8_t* ptr, size_t count) {
        buffer_ptr->insert(buffer_ptr->end(), ptr, ptr + count);
    }
    
protected:
    std::vector<uint8_t>* buffer_ptr = nullptr;
};

#endif //BUFFERUTIL_H

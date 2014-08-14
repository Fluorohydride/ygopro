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
    
    inline void Skip(unsigned int offset) {
        ptr += offset;
    }

private:
    unsigned char* ptr;

};

#endif //BUFFERIO_H

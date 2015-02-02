#ifndef _SINGLETON_H_
#define _SINGLETON_H_

template<typename T>
class Singleton {
protected:
    inline Singleton() = default;
    
public:
    static inline T &Get() {
        static T obj;
        return obj;
    }
    
    static inline T *GetPtr() {
        return &Get();
    }
    
};


#endif
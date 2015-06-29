#ifndef _CLOCK_H_
#define _CLOCK_H_

#include "common.h"

class SysClock {
public:
    int64_t GetSysClock() { return pause_time ? pause_now : now; }
    void InitSysClock() { start_time = EpochTime(); }
    void UpdateSysClock() {
        if(pause_time)
            return;
        int64_t now_long = EpochTime();
        now = now_long - start_time;
    }
    void PauseSysClock() {
        if(pause_time)
            return;
        pause_time = EpochTime();
        pause_now = pause_time - start_time;
    }
    void ResumeSysClock() {
        if(!pause_time)
            return;
        uint64_t now_long = EpochTime();
        start_time += now_long - pause_time;
        now = pause_time;
        pause_time = 0;
    }
    
protected:
    inline int64_t EpochTime() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
    
    int64_t start_time = 0;
    int64_t pause_time = 0;
    int64_t now = 0;
    int64_t pause_now = 0;
};

#endif

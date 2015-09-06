#ifndef _TIMER_H_
#define _TIMER_H_

#include "common.h"

template<typename TIME_TYPE>
struct TimerEvent {
    TIME_TYPE end_time;
    std::function<TIME_TYPE()> call_back;
    
    static bool Comp(const TimerEvent<TIME_TYPE>& a, const TimerEvent<TIME_TYPE>& b) {
        return a.end_time > b.end_time;
    }
};

template<typename TIME_TYPE>
class Timer {
public:
    void InitTimer(TIME_TYPE t) {
        cur_time = t;
    }
    
    void UpdateTimer(TIME_TYPE t) {
        cur_time = t;
        while(!event_list.empty()) {
            if(t < event_list.front().end_time)
                break;
            auto evt = event_list.front();
            TIME_TYPE next_interval = evt.call_back();
            std::pop_heap(event_list.begin(), event_list.end(), TimerEvent<TIME_TYPE>::Comp);
            event_list.pop_back();
            if(next_interval > TIME_TYPE()) {
                evt.end_time = cur_time + next_interval;
                event_list.push_back(evt);
                std::push_heap(event_list.begin(), event_list.end(), TimerEvent<TIME_TYPE>::Comp);
            }
        }
    }
    
    void RegisterTimerEvent(std::function<TIME_TYPE()> cb, TIME_TYPE first) {
        if(cb == nullptr)
            return;
        TimerEvent<TIME_TYPE> te;
        te.end_time = cur_time + first;
        te.call_back = cb;
        event_list.push_back(te);
        std::push_heap(event_list.begin(), event_list.end(), TimerEvent<TIME_TYPE>::Comp);
    }
    
protected:
    TIME_TYPE cur_time;
    std::vector<TimerEvent<TIME_TYPE>> event_list;
};

#endif

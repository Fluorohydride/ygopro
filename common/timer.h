#ifndef _TIMER_H_
#define _TIMER_H_

template<typename TIME_TYPE>
struct TimerEvent {
    TIME_TYPE end_time;
    TIME_TYPE interval;
    bool repeat;
    std::function<void()> call_back;
    
    static bool Comp(const TimerEvent<TIME_TYPE>& a, const TimerEvent<TIME_TYPE>& b) {
        return a.end_time < b.end_time;
    }
    
    typedef decltype(&Comp) comp_type;
};

template<typename TIME_TYPE>
class Timer {
public:
    Timer() : event_set(TimerEvent<TIME_TYPE>::Comp) {}
    void Init(TIME_TYPE t) {
        cur_time = t;
    }
    
    void UpdateTime(TIME_TYPE t) {
        cur_time = t;
        auto iter = event_set.begin();
        while(iter != event_set.end() && t >= (*iter).end_time) {
            auto evt = *iter;
            event_set.erase(iter);
            evt.call_back();
            if(evt.repeat) {
                evt.end_time = cur_time + evt.interval;
                event_set.insert(evt);
            }
            iter = event_set.begin();
        }
    }
    
    void RegisterEvent(std::function<void()> cb, TIME_TYPE first, TIME_TYPE inv, bool rep) {
        if(cb == nullptr)
            return;
        TimerEvent<TIME_TYPE> te;
        te.end_time = cur_time + first;
        te.interval = inv;
        te.repeat = rep;
        te.call_back = cb;
        event_set.insert(te);
    }
    
protected:
    TIME_TYPE cur_time;
    std::multiset<TimerEvent<TIME_TYPE>, typename TimerEvent<TIME_TYPE>::comp_type> event_set;
};

#endif

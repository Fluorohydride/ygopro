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
    void InitTimer(TIME_TYPE t) {
        cur_time = t;
    }
    
    void UpdateTime(TIME_TYPE t) {
        cur_time = t;
        auto iter = event_list.begin();
        while(!event_list.empty() && (t >= (*iter).end_time)) {
            auto evt = *iter;
            evt.call_back();
            std::pop_heap(event_list.begin(), event_list.end(), TimerEvent<TIME_TYPE>::Comp);
            event_list.pop_back();
            if(evt.repeat) {
                evt.end_time = cur_time + evt.interval;
                event_list.push_back(evt);
                std::push_heap(event_list.begin(), event_list.end(), TimerEvent<TIME_TYPE>::Comp);
            }
            iter = event_list.begin();
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
        event_list.push_back(te);
        std::push_heap(event_list.begin(), event_list.end(), TimerEvent<TIME_TYPE>::Comp);
    }
    
protected:
    TIME_TYPE cur_time;
    std::vector<TimerEvent<TIME_TYPE>> event_list;
};

#endif

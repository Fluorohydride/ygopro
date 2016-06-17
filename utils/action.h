#ifndef _ACTION_H_
#define _ACTION_H_

#include "common.h"

template<typename TIME_TYPE>
class TGen {
public:
	virtual double GetT(TIME_TYPE start_time, TIME_TYPE cur_time) = 0;
};

template<typename TIME_TYPE>
class TGenLinear : public TGen<TIME_TYPE> {
public:
    TGenLinear(TIME_TYPE tm) : last_time(tm) {}
    virtual double GetT(TIME_TYPE start_time, TIME_TYPE cur_time) {
        if(cur_time >= start_time + last_time)
            return 1.0;
        return (double)(cur_time - start_time) / last_time;
    }
protected:
    TIME_TYPE last_time;
};

template<typename TIME_TYPE>
class TGenMove  : public TGen<TIME_TYPE> {
public:
    TGenMove(TIME_TYPE tm, double k) {
        last_time = tm;
        k_coff = k;
        v0 = k / (1.0 - pow(2.718281828, -k * tm));
    }
    virtual double GetT(TIME_TYPE start_time, TIME_TYPE cur_time) {
        if(cur_time >= start_time + last_time)
            return 1.0;
        TIME_TYPE tm = (cur_time - start_time);
        return v0 / k_coff * (1.0 - pow(2.718281828, -k_coff * tm));
    }
protected:
    TIME_TYPE last_time;
    double k_coff;
    double v0;
};

template<typename TIME_TYPE>
class TGenPeriodic : public TGen<TIME_TYPE> {
public:
    TGenPeriodic(TIME_TYPE freq) : freq_tm(freq) {}
    virtual double GetT(TIME_TYPE start_time, TIME_TYPE cur_time) {
        double period = (double)(cur_time - start_time) / freq_tm;
        return period - (int32_t)period;
    }
protected:
    TIME_TYPE freq_tm;
};

template<typename TIME_TYPE>
class TGenPeriodicRet : public TGen<TIME_TYPE> {
public:
    TGenPeriodicRet(TIME_TYPE freq) : freq_tm(freq) {}
    virtual double GetT(TIME_TYPE start_time, TIME_TYPE cur_time) {
        double period = (double)(cur_time - start_time) / freq_tm;
        period = period - (int32_t)period;
        if(period <= 0.5)
            return period * 2.0;
        else
            return 2.0 - period * 2.0;
    }
protected:
    TIME_TYPE freq_tm;
};

template<typename TIME_TYPE>
class TGenHarmonic : public TGen<TIME_TYPE> {
public:
    TGenHarmonic(TIME_TYPE freq) : freq_tm(freq) {}
    virtual double GetT(TIME_TYPE start_time, TIME_TYPE cur_time) {
        double period = (double)(cur_time - start_time) / freq_tm;
        return sin(period * 3.1415926 * 2.0 - 3.1415926 * 0.5) * 0.5 + 0.5;
    }
protected:
    TIME_TYPE freq_tm;
};

template<typename TIME_TYPE>
class Action {
public:
    virtual void InitStartTime(TIME_TYPE tm) {}
    virtual bool Perform(TIME_TYPE cur_time) = 0;
};

template<typename TIME_TYPE>
class ActionSequence : public Action<TIME_TYPE> {
public:
    ActionSequence() {}
    
    template<typename... ARGS>
    ActionSequence(ARGS... args) { Push(std::forward<ARGS>(args)...); }
    
    virtual void InitStartTime(TIME_TYPE tm) {
        if(!sequence.empty())
            sequence.front()->InitStartTime(tm);
        perform_index = 0;
    }
    virtual bool Perform(TIME_TYPE cur_time) {
        while(perform_index < sequence.size()) {
            auto res = sequence[perform_index]->Perform(cur_time);
            if(res)
                return true;
            perform_index++;
            if(perform_index < sequence.size())
                sequence[perform_index]->InitStartTime(cur_time);
        }
        return false;
    }
    
    template<typename... ARGS>
    void Push(std::shared_ptr<Action<TIME_TYPE>> ptr, ARGS... args) { sequence.push_back(ptr); Push(std::forward<ARGS>(args)...); }
    void Push(std::shared_ptr<Action<TIME_TYPE>> ptr) { sequence.push_back(ptr); }
    
protected:
    int32_t perform_index = 0;
    std::vector<std::shared_ptr<Action<TIME_TYPE>>> sequence;
};

template<typename TIME_TYPE>
class ActionRepeat : public Action<TIME_TYPE> {
    using repcb_type = std::function<bool(bool)>;
public:
    ActionRepeat(std::shared_ptr<Action<TIME_TYPE>> ac, repcb_type cb = nullptr) : rep_action(ac), callback(cb) {}
    ActionRepeat(std::shared_ptr<Action<TIME_TYPE>> ac, int32_t count) : rep_action(ac) {
        int32_t cur_count = 0;
        callback = [cur_count, count](bool init) mutable ->bool {
            if(init) { cur_count = 0; return true; }
            return ++cur_count <= count;
        };
    }
    
    virtual void InitStartTime(TIME_TYPE tm) { if(rep_action) rep_action->InitStartTime(tm); if(callback) callback(true); }
    virtual bool Perform(TIME_TYPE cur_time) {
        if(!rep_action)
            return false;
        if(!rep_action->Perform(cur_time)) {
            if(callback && !callback(false))
                return false;
            rep_action->InitStartTime(cur_time);
            return true;
        }
        return true;
    }

protected:
    std::shared_ptr<Action<TIME_TYPE>> rep_action;
    repcb_type callback;
};

template<typename TIME_TYPE>
class ActionWait : public Action<TIME_TYPE> {
public:
    ActionWait(TIME_TYPE tm) : last_time(tm) {}
    virtual void InitStartTime(TIME_TYPE tm) { start_time = tm; }
    virtual bool Perform(TIME_TYPE cur_time) { return cur_time < start_time + last_time; }
    
protected:
    TIME_TYPE start_time;
    TIME_TYPE last_time;
};

template<typename TIME_TYPE>
class ActionCallback : public Action<TIME_TYPE> {
    using cb_type = std::function<void()>;
public:
    ActionCallback(cb_type cb) : callback(cb) {}
    virtual bool Perform(TIME_TYPE cur_time) {
        if(callback)
            callback();
        return false;
    }
protected:
    cb_type callback;
};

template<typename TIME_TYPE, typename OBJ_TYPE = void>
class LerpAnimator : public Action<TIME_TYPE> {
    using lerp_cb = std::function<bool(OBJ_TYPE*, double)>; // return false to exit
public:
    LerpAnimator(TIME_TYPE tm, std::shared_ptr<OBJ_TYPE> obj, lerp_cb cb, std::shared_ptr<TGen<TIME_TYPE>> gobj)
        : last_time(tm), ani_obj(obj), callback(cb), tgen_obj(gobj) {}
    virtual void InitStartTime(TIME_TYPE tm) { start_time = tm; }
    virtual bool Perform(TIME_TYPE cur_time) {
        auto ptr = ani_obj.lock();
        if(ptr == nullptr || callback == nullptr || tgen_obj == nullptr)
            return false;
        bool result = callback(ptr.get(), tgen_obj->GetT(start_time, cur_time));
        bool end = last_time > TIME_TYPE() && cur_time >= start_time + last_time;
        return result && !end;
    }
    
protected:
    TIME_TYPE start_time;
    TIME_TYPE last_time;
    std::weak_ptr<OBJ_TYPE> ani_obj;
    lerp_cb callback;
    std::shared_ptr<TGen<TIME_TYPE>> tgen_obj;
};

template<typename TIME_TYPE>
class LerpAnimator<TIME_TYPE, void> : public Action<TIME_TYPE> {
    using lerp_cb = std::function<bool(double)>; // return false to exit
public:
    LerpAnimator(TIME_TYPE tm, lerp_cb cb, std::shared_ptr<TGen<TIME_TYPE>> gobj)
        : last_time(tm), callback(cb), tgen_obj(gobj) {}
    virtual void InitStartTime(TIME_TYPE tm) { start_time = tm; }
    virtual bool Perform(TIME_TYPE cur_time) {
        if(callback == nullptr || tgen_obj == nullptr)
            return false;
        bool result = callback(tgen_obj->GetT(start_time, cur_time));
        bool end = last_time > TIME_TYPE() && cur_time >= start_time + last_time;
        return result && !end;
    }
    
protected:
    TIME_TYPE start_time;
    TIME_TYPE last_time;
    lerp_cb callback;
    std::shared_ptr<TGen<TIME_TYPE>> tgen_obj;
};

template<typename TIME_TYPE>
class ActionMgr {
protected:
    struct ActionStatus {
        std::shared_ptr<Action<TIME_TYPE>> act;
        void* object;
        int32_t acttype;
        bool pending_remove;
    };
public:
    inline void InitActionTime(TIME_TYPE tm) { cur_time = tm; }
    
    ActionMgr& PushAction(std::shared_ptr<Action<TIME_TYPE>> ani, void* obj = nullptr, int32_t acttype = 0) {
        ani->InitStartTime(cur_time);
        actions.push_back(ActionStatus{ani, obj, acttype, false});
        return *this;
    }
    
    ActionMgr& operator << (std::shared_ptr<Action<TIME_TYPE>> ani) {
        ani->InitStartTime(cur_time);
        actions.push_back(ActionStatus{ani, nullptr, 0, false});
        return *this;
    }
    
    void UpdateActionTime(TIME_TYPE tm) {
        auto iter = actions.begin();
        while(iter != actions.end()) {
            auto cur = iter++;
            if(cur->pending_remove || !cur->act->Perform(tm))
                actions.erase(cur);
        }
        cur_time = tm;
    }
    
    void ClearAllActions() { actions.clear(); }
    void RemoveAction(void* obj) {
        auto iter = actions.begin();
        while(iter != actions.end()) {
            auto cur = iter++;
            if(cur->object == obj)
                cur->pending_remove = true;
        }
    }
    void RemoveAction(void* obj, int32_t acttype) {
        auto iter = actions.begin();
        while(iter != actions.end()) {
            auto cur = iter++;
            if(cur->object == obj && cur->acttype == acttype)
                cur->pending_remove = true;
        }
    }
    
protected:
    TIME_TYPE cur_time = TIME_TYPE();
    std::list<ActionStatus> actions;
};

#endif

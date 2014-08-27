#ifndef _ANIMATOR_H_
#define _ANIMATOR_H_

template<typename T>
class Animator {
public:
    inline virtual T GetCurrent(double cur_time) = 0;
    inline virtual bool IsEnd(double cur_time) = 0;
};

template<typename T>
class InterpolaterAnimator : public Animator<T> {
public:
    InterpolaterAnimator(T sv, T ev) : start_val(sv), end_val(ev) {}
    inline virtual float GetRate(double cur_time) = 0;
    virtual T GetCurrent(double cur_time) {
        if(this->IsEnd(cur_time))
            return this->end_val;
        float rate = this->GetRate(cur_time);
        return this->start_val + (this->end_val - this->start_val) * rate;
    }
protected:
    T start_val;
    T end_val;
};

template<typename T>
class LinearInterpolater : public InterpolaterAnimator<T> {
public:
    LinearInterpolater(T sv, T ev, double stm, double tm) : InterpolaterAnimator<T>(sv, ev) {
        start_time = stm;
        end_time = stm + tm;
    }
    inline virtual bool IsEnd(double cur_time) {
        return cur_time >= end_time;
    }
    inline virtual float GetRate(double cur_time) {
        if(cur_time >= end_time)
            return 1.0f;
        return (cur_time - start_time) / (end_time - start_time);
    }
protected:
    double start_time;
    double end_time;
};

template<typename T>
class MoveInterpolater : public InterpolaterAnimator<T> {
public:
    MoveInterpolater(T sv, T ev, double stm, double tm) : InterpolaterAnimator<T>(sv, ev) {
        start_time = stm;
        end_time = stm + tm;
    }
    inline virtual bool IsEnd(double cur_time) {
        return cur_time >= end_time;
    }
    inline virtual float GetRate(double cur_time) {
        if(cur_time >= end_time)
            return 1.0f;
        float rate = (cur_time - start_time) / (end_time - start_time);
        return rate * 2.0 - rate * rate;
    }
protected:
    double start_time;
    double end_time;
};

template<typename T>
class PeriodicInterpolater : public InterpolaterAnimator<T> {
public:
    PeriodicInterpolater(T sv, T ev, double stm, double freq) : InterpolaterAnimator<T>(sv, ev) {
        start_time = stm;
        freq_tm = freq;
    }
    inline virtual bool IsEnd(double cur_time) {
        return false;
    }
    inline virtual float GetRate(double cur_time) {
        float period = (cur_time - this->start_time) / freq_tm;
        return period - (int)period;
    }
    
    double start_time;
    double freq_tm;
};

template<typename T>
class MutableAttribute {
public:
    template<typename PTR>
    inline void SetAnimator(PTR p) {
        int_ptr = std::static_pointer_cast<Animator<T>>(p);
    }
    
    inline T Update(double tm) {
        if(int_ptr != nullptr) {
            val = int_ptr->GetCurrent(tm);
            if(int_ptr->IsEnd(tm))
                int_ptr.reset();
        }
        return val;
    }
    
    inline void Reset() {
        int_ptr.reset();
    }
    
    inline void Reset(T new_val) {
        val = new_val;
        int_ptr.reset();
    }
    
    inline const T& Get() { return val; }
    inline bool NeedUpdate() { return int_ptr != nullptr; }
    
    inline void operator = (const T& new_val) { val = new_val; }
    inline operator const T&() { return val; }
    
protected:
    T val = T();
    std::shared_ptr<Animator<T>> int_ptr;
};

#endif

#ifndef _ANIMATOR_H_
#define _ANIMATOR_H_

template<typename T>
class Animator {
public:
	virtual T GetCurrent(double cur_time) = 0;
	virtual bool IsEnd(double cur_time) = 0;
};

class TGen {
public:
	virtual bool IsGenEnd(double cur_time) = 0;
	virtual float GetT(double cur_time) = 0;
};

class TGenLinear : public TGen {
public:
    TGenLinear(double stm, double tm) {
        start_time = stm;
        end_time = stm + tm;
    }
    inline virtual bool IsGenEnd(double cur_time) {
        return cur_time >= end_time;
    }
    inline virtual float GetT(double cur_time) {
        if(cur_time >= end_time)
            return 1.0f;
        return (cur_time - start_time) / (end_time - start_time);
    }
protected:
    double start_time;
    double end_time;
};

class TGenMove  : public TGen {
public:
    TGenMove(double stm, double tm, double k) {
        start_time = stm;
        end_time = stm + tm;
        k_coff = k;
        v0 = k / (1.0 - pow(2.718281828, -k * tm));
    }
    inline virtual bool IsGenEnd(double cur_time) {
        return cur_time >= end_time;
    }
    inline virtual float GetT(double cur_time) {
        if(cur_time >= end_time)
            return 1.0f;
        double tm = (cur_time - start_time);
        return v0 / k_coff * (1.0 - pow(2.718281828, -k_coff * tm));
    }
protected:
    double start_time;
    double end_time;
    double k_coff;
    double v0;
};

class TGenPeriodic : public TGen {
public:
    TGenPeriodic(double stm, double freq) {
        start_time = stm;
        freq_tm = freq;
    }
    inline virtual bool IsGenEnd(double cur_time) {
        return false;
    }
    inline virtual float GetT(double cur_time) {
        float period = (cur_time - start_time) / freq_tm;
        return period - (int)period;
    }
protected:
    double start_time;
    double freq_tm;
};

class TGenPeriodicRet : public TGen {
public:
    TGenPeriodicRet(double stm, double freq) {
        start_time = stm;
        freq_tm = freq;
    }
    inline virtual bool IsGenEnd(double cur_time) {
        return false;
    }
    inline virtual float GetT(double cur_time) {
        float period = (cur_time - start_time) / freq_tm;
        period = period - (int)period;
        if(period <= 0.5f)
            return period * 2.0f;
        else
            return 2.0f - period * 2.0f;
    }
    
protected:
    double start_time;
    double freq_tm;
};

class TGenHarmonic : public TGen {
public:
    TGenHarmonic(double stm, double freq) {
        start_time = stm;
        freq_tm = freq;
    }
    inline virtual bool IsGenEnd(double cur_time) {
        return false;
    }
    inline virtual float GetT(double cur_time) {
        float period = (cur_time - start_time) / freq_tm;
        return sinf(period * 3.1415926 * 2.0 - 3.1415926 * 0.5) * 0.5f + 0.5f;
    }
protected:
    double start_time;
    double freq_tm;
};

template<typename T, typename TGENTYPE>
class LerpAnimator : public Animator<T>, public TGENTYPE {
public:
    template<typename... ARGS>
    LerpAnimator(T sv, T ev, ARGS... args) : TGENTYPE(args...), start_val(sv), end_val(ev) {}
    inline virtual bool IsEnd(double cur_time) {
        return this->IsGenEnd(cur_time);
    }
    inline virtual T GetCurrent(double cur_time) {
        float t = this->GetT(cur_time);
        return this->start_val + (this->end_val - this->start_val) * t;
    }
protected:
    T start_val;
    T end_val;
};

template<typename T, typename TGENTYPE>
class LerpAnimatorCB : public Animator<T>, public TGENTYPE {
public:
    typedef std::function<T (const T& st, const T& ed, float t)> callback_type;
    template<typename... ARGS>
    LerpAnimatorCB(callback_type cb, T sv, T ev, ARGS... args) : TGENTYPE(args...), call_back(cb), start_val(sv), end_val(ev) {}
    inline virtual bool IsEnd(double cur_time) {
        return this->IsGenEnd(cur_time);
    }
    inline virtual T GetCurrent(double cur_time) {
        float t = this->GetT(cur_time);
        return call_back(start_val, end_val, t);
    }
protected:
    callback_type call_back;
    T start_val;
    T end_val;    
};

template<typename T>
class MutableAttribute {
public:
    MutableAttribute() = default;
    
    MutableAttribute(const MutableAttribute<T>& v) {
        val = v.val;
        int_ptr.reset();
    }
    
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

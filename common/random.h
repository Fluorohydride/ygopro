#ifndef _RANDOM_H_
#define _RANDOM_H_

#include <random>

class Random {
private:
    std::mt19937 gen;
    
public:
    void RandomInit(unsigned int seed) {
        gen.seed(seed);
    }
    
    // random value between [0, 0xffffffff]
    inline unsigned int GetRandomUInt32() {
        return std::uniform_int_distribution<>()(gen);
    }
    
    // random value between [0, max]
    inline unsigned int GetRandomUInt32(unsigned int max) {
        return std::uniform_int_distribution<>(0, max)(gen);
    }
    
    // random value between [min, max]
    inline unsigned int GetRandomUInt32(unsigned int min, unsigned int max) {
        return std::uniform_int_distribution<>(min, max)(gen);
    }
    
    // random value between [0.0, 1.0)
    inline double GetRandomReal() {
        return std::uniform_real_distribution<>()(gen);
    }
    
    // random value between [0.0, max)
    inline double GetRandomReal(double max) {
        return std::uniform_real_distribution<>(0.0, max)(gen);
    }
    
    // random value between [min, max)
    inline double GetRandomReal(double min, double max) {
        return std::uniform_real_distribution<>(min, max)(gen);
    }
    
};

#endif

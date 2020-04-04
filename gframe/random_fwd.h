#ifndef RANDOM_FWD_H
#define RANDOM_FWD_H

#include <random>
#include <cstdint>

using randengine = std::mersenne_twister_engine<uint32_t, 32, 624, 397, 31, 0x9908b0df, 11, 0xffffffff, 7, 0x9d2c5680, 15, 0xefc60000, 18, 1812433253>;


#endif // RANDOM_FWD_H

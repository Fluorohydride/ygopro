#ifndef MT19937_H
#define MT19937_H

#include <random>
#include <cstdint>

namespace RNG {
using mt19937 = std::mersenne_twister_engine<unsigned int, 32, 624, 397, 31, 0x9908b0df,
	11, 0xffffffff, 7, 0x9d2c5680, 15, 0xefc60000, 18, 1812433253>;
}

#endif // MT19937_H

#ifndef _RANDOM_H_
#define _RANDOM_H_

#include <random>

class Random {
private:
	std::mt19937 gen;

public:

	void Initialise(unsigned int seed) {
		gen.seed(seed);
	}

	// random value between [0, 0xffffffff)
	inline unsigned int GetUInt32() {
		std::uniform_int_distribution<> d;
		return d(gen);
	}

	// random value between [0, max)
	inline unsigned int GetUInt32(unsigned int max) {
		std::uniform_int_distribution<> d(0, max);
		return d(gen);
	}

	// random value between [min, max]
	inline unsigned int GetUInt32(unsigned int min, unsigned int max) {
		std::uniform_int_distribution<> d(min, max);
		return d(gen);
	}

};

extern Random globalRandom;

#endif

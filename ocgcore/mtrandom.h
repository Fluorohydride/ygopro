/*
 * mtrandom.h
 *
 *  Created on: 2009-10-18
 *      Author: Argon.Sun
 */

#ifndef MTRANDOM_H_
#define MTRANDOM_H_

//Mersenne Twister
class mtrandom {
public:
	mtrandom() :
		left(1) {
		init();
	}
	explicit mtrandom(unsigned int seed) :
		left(1) {
		init(seed);
	}
	mtrandom(unsigned int* init_key, int key_length) :
		left(1) {
		int i = 1, j = 0;
		int k = N > key_length ? N : key_length;
		init();
		for (; k; --k) {
			state[i] = (state[i] ^ ((state[i - 1] ^ (state[i - 1] >> 30))
					* 1664525UL)) + init_key[j] + j; // non linear
			state[i] &= 4294967295UL;
			++i;
			++j;
			if (i >= N) {
				state[0] = state[N - 1];
				i = 1;
			}
			if (j >= key_length)
				j = 0;
		}
		for (k = N - 1; k; --k) {
			state[i] = (state[i] ^ ((state[i - 1] ^ (state[i - 1] >> 30))
					* 1566083941UL)) - i;
			state[i] &= 4294967295UL;
			++i;
			if (i >= N) {
				state[0] = state[N - 1];
				i = 1;
			}
		}
		state[0] = 2147483648UL;
	}
	void reset(unsigned int rs) {
		init(rs);
		next_state();
	}
	unsigned int rand() {
		unsigned int y;
		if (0 == --left)
			next_state();
		y = *next++;
		y ^= (y >> 11);
		y ^= (y << 7) & 0x9d2c5680UL;
		y ^= (y << 15) & 0xefc60000UL;
		y ^= (y >> 18);
		return y;
	}
	double real() {
		return (double) rand() / 0xffffffffUL;
	}
	double res53() {
		unsigned int a = rand() >> 5, b = rand() >> 6;
		return (a * 67108864.0 + b) / 9007199254740992.0;
	}
	void init(unsigned int seed = 19650218UL) {
		state[0] = seed & 4294967295UL;
		for (int j = 1; j < N; ++j) {
			state[j] = (1812433253UL * (state[j - 1] ^ (state[j - 1] >> 30))
					+ j);
			state[j] &= 4294967295UL; // for >32 bit machines
		}
	}
private:
	void next_state() {
		unsigned int* p = state;
		int i;
		for (i = N - M + 1; --i; ++p)
			*p = (p[M] ^ twist(p[0], p[1]));
		for (i = M; --i; ++p)
			*p = (p[M - N] ^ twist(p[0], p[1]));
		*p = p[M - N] ^ twist(p[0], state[0]);
		left = N;
		next = state;
	}
	unsigned int mixbits(unsigned int u, unsigned int v) const {
		return (u & 2147483648UL) | (v & 2147483647UL);
	}
	unsigned int twist(unsigned int u, unsigned int v) const {
		return ((mixbits(u, v) >> 1) ^ (v & 1UL ? 2567483615UL : 0UL));
	}
	static const int N = 624, M = 397;
	unsigned int state[N];
	unsigned int left;
	unsigned int* next;
};

#endif /* MTRANDOM_H_ */

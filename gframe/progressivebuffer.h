/*
 * progressivebuffer.h
 *
 *  Created on: 16/3/2019
 *      Author: edo9300
 */

#ifndef PROGRESSIVEBUFFER_H_
#define PROGRESSIVEBUFFER_H_

#include <cmath>
#include <vector>


class ProgressiveBuffer {
public:
	std::vector<uint8_t> data;
	ProgressiveBuffer() {};
	void clear() {
		data.clear();
	}
	template<class T>
	T& at(const size_t _Pos) {
		auto valsize = sizeof(T);
		size_t size = (_Pos + 1) * valsize;
		if(data.size() < size)
			data.resize(size);
		return *(T*)(data.data() + _Pos * valsize);
	}
	bool bitGet(const size_t _Pos) {
		int pos = (int)std::floor(_Pos / 8);
		int index = _Pos % 8;
		size_t size = pos + 1;
		if(data.size() < size) {
			data.resize(size);
			return false;
		}
		return !!(data[pos] & (1 << index));
	}
	void bitSet(const size_t _Pos, bool set = true) {
		int pos = (int)std::floor(_Pos / 8);
		int index = _Pos % 8;
		size_t size = pos + 1;
		if(data.size() < size) {
			data.resize(size);
		}
		if(set)
			data[pos] |= (1 << index);
		else {
			data[pos] &= ~(1 << index);
		}

	}
};

#endif /* PROGRESSIVEBUFFER_H_ */

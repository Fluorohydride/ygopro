/*
 * progressivebuffer.h
 *
 *  Created on: 16/3/2019
 *      Author: edo9300
 */

#ifndef PROGRESSIVEBUFFER_H_
#define PROGRESSIVEBUFFER_H_

#include <vector>
#include <cstdint>

class ProgressiveBuffer {
public:
	std::vector<uint8_t> data;
	ProgressiveBuffer() {};
	void clear() {
		data.clear();
	}
	template<class T>
	T& at(const size_t _Pos) {
		static constexpr auto valsize = sizeof(T);
		size_t size = (_Pos + 1) * valsize;
		if(data.size() < size)
			data.resize(size);
		return *(T*)(data.data() + _Pos * valsize);
	}
	bool bitGet(const size_t _Pos) {
		size_t pos = _Pos / 8u;
		uint32_t index = _Pos % 8u;
		size_t size = pos + 1;
		if(data.size() < size) {
			data.resize(size);
			return false;
		}
		return !!(data[pos] & (1 << index));
	}
	void bitSet(const size_t _Pos, bool set = true) {
		size_t pos = _Pos / 8u;
		uint32_t index = _Pos % 8u;
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

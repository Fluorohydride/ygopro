/*
 * effectset.h
 *
 *  Created on: 2011-10-8
 *      Author: Argon
 */

#ifndef EFFECTSET_H_
#define EFFECTSET_H_

#include <stdlib.h>
#include <array>
#include <algorithm>
#include "effect.h"

class effect;

bool effect_sort_id(const effect* e1, const effect* e2);

struct effect_set {
	effect_set(): count(0) {}
	void add_item(effect* peffect) {
		if(count >= 64) return;
		container[count++] = peffect;
	}
	void remove_item(int index) {
		if(index >= count)
			return;
		if(index == count - 1) {
			count--;
			return;
		}
		for(int i = index; i < count - 1; ++i)
			container[i] = container[i + 1];
		count--;
	}
	void clear() {
		count = 0;
	}
	void sort() {
		if(count < 2)
			return;
		std::sort(container.begin(), container.begin() + count, effect_sort_id);
	}
	effect*& get_last() {
		return container[count - 1];
	}
	effect*& operator[] (int index) {
		return container[index];
	}
	effect*& at(int index) {
		return container[index];
	}
	std::array<effect*, 64> container;
	int count;
};

#endif //EFFECTSET_H_

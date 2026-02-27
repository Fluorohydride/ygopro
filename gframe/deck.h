#ifndef DECK_H
#define DECK_H

#include <vector>
#include <cstdint>
#include "data_manager.h"

namespace ygo {

struct Deck {
	std::vector<code_pointer> main;
	std::vector<code_pointer> extra;
	std::vector<code_pointer> side;
	void clear() {
		main.clear();
		extra.clear();
		side.clear();
	}
};

struct DeckArray {
	std::vector<uint32_t> main;
	std::vector<uint32_t> extra;
	std::vector<uint32_t> side;
};

}

#endif // DECK_H

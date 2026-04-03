#ifndef DECK_H
#define DECK_H

#include <vector>
#include <cstdint>
#include "data_manager.h"

namespace ygo {

struct Deck {
	std::vector<const CardDataC*> main;
	std::vector<const CardDataC*> extra;
	std::vector<const CardDataC*> side;
	void clear() {
		main.clear();
		extra.clear();
		side.clear();
	}
	size_t size() const noexcept {
		return main.size() + extra.size() + side.size();
	}
};

struct DeckArray {
	std::vector<uint32_t> main;
	std::vector<uint32_t> extra;
	std::vector<uint32_t> side;
};

}

#endif // DECK_H

#ifndef DECK_H
#define DECK_H
#include <vector>

namespace ygo {

struct CardDataC;
struct Deck {
	using Vector = std::vector<const CardDataC*>;
	Vector main;
	Vector extra;
	Vector side;
	void clear() {
		main.clear();
		extra.clear();
		side.clear();
	}
};
}

#endif //DECK_H
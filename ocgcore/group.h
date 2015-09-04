/*
 * group.h
 *
 *  Created on: 2010-5-6
 *      Author: Argon
 */

#ifndef GROUP_H_
#define GROUP_H_

#include "common.h"
#include <set>
#include <list>

class card;
class duel;

class group {
public:
	typedef std::set<card*, card_sort> card_set;
	int32 scrtype;
	int32 ref_handle;
	duel* pduel;
	card_set container;
	card_set::iterator it;
	uint32 is_readonly;
	
	inline bool has_card(card* c) {
		return container.find(c) != container.end();
	}
	
	explicit group(duel* pd);
	group(duel* pd, card* pcard);
	group(duel* pd, const card_set& cset);
	~group();
};

#endif /* GROUP_H_ */

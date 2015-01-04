/*
 * group.cpp
 *
 *  Created on: 2010-8-3
 *      Author: Argon
 */

#include "group.h"
#include "card.h"
#include "duel.h"

group::group(duel* pd) {
	scrtype = 2;
	ref_handle = 0;
	pduel = pd;
	is_readonly = FALSE;
}
group::group(duel* pd, card* pcard) {
	container.insert(pcard);
	scrtype = 2;
	ref_handle = 0;
	pduel = pd;
	is_readonly = FALSE;
}
group::group(duel* pd, const card_set& cset): container(cset) {
	scrtype = 2;
	ref_handle = 0;
	pduel = pd;
	is_readonly = FALSE;
}
group::~group() {

}

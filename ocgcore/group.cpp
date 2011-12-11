/*
 * group.cpp
 *
 *  Created on: 2010-8-3
 *      Author: Argon
 */

#include "group.h"
#include "card.h"
#include "duel.h"

group::group() {
	scrtype = 2;
	ref_handle = 0;
	pduel = 0;
	is_readonly = FALSE;
	it = container.begin();
}
group::~group() {

}

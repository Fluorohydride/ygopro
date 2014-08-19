/*
 * card.h
 *
 *  Created on: 2010-4-8
 *      Author: Argon
 */

#ifndef CARD_H_
#define CARD_H_

#include "common.h"
#include "effectset.h"
#include <set>
#include <map>

class card;
class duel;
class effect;
class group;

struct card_state {
	uint32 code;
	uint32 type;
	uint32 level;
	uint32 rank;
	uint32 lscale;
	uint32 rscale;
	uint32 attribute;
	uint32 race;
	int32 attack;
	int32 defence;
	int32 base_attack;
	int32 base_defence;
	uint8 controler;
	uint8 location;
	uint8 sequence;
	uint8 position;
	uint32 reason;
	card* reason_card;
	uint8 reason_player;
	effect* reason_effect;
};

struct query_cache {
	uint32 code;
	uint32 alias;
	uint32 type;
	uint32 level;
	uint32 rank;
    uint32 lscale;
	uint32 rscale;
	uint32 attribute;
	uint32 race;
	int32 attack;
	int32 defence;
	int32 base_attack;
	int32 base_defence;
	uint32 reason;
	uint32 owner;
	uint32 status;
};

class card {
public:
	typedef std::vector<card*> card_vector;
	typedef std::multimap<uint32, effect*> effect_container;
	typedef std::set<card*, card_sort> card_set;
	typedef std::map<effect*, effect_container::iterator> effect_indexer;
	typedef std::map<effect*, uint32> effect_relation;
	typedef std::map<card*, uint32> relation_map;
	typedef std::map<uint16, uint16> counter_map;
	typedef std::map<uint16, card*> attacker_map;
	int32 scrtype;
	int32 ref_handle;
	duel* pduel;
	card_data data;
	card_state previous;
	card_state temp;
	card_state current;
	query_cache q_cache;
	uint8 owner;
	uint8 summon_player;
	uint32 summon_type;
	uint32 status;
	uint32 operation_param;
	uint8 announce_count;
	uint8 attacked_count;
	uint8 attack_all_target;
	uint16 cardid;
	uint32 fieldid;
	uint32 fieldid_r;
	uint16 turnid;
	uint16 turn_counter;
	uint8 unique_pos[2];
	uint16 unique_uid;
	uint32 unique_code;
	uint8 assume_type;
	uint32 assume_value;
	effect* unique_effect;
	card* equiping_target;
	card* pre_equip_target;
	card* overlay_target;
	relation_map relations;
	counter_map counters;
	attacker_map announced_cards;
	attacker_map attacked_cards;
	attacker_map battled_cards;
	card_set equiping_cards;
	card_set material_cards;
	card_set effect_target_owner;
	card_set effect_target_cards;
	card_vector xyz_materials;
	effect_container single_effect;
	effect_container field_effect;
	effect_container equip_effect;
	effect_indexer indexer;
	effect_relation relate_effect;
	effect_set_v immune_effect;

	card();
	~card();
	static bool card_operation_sort(card* c1, card* c2);

	void update_infos(int32 query_flag);
	void update_infos_nocache(int32 query_flag);
	uint32 get_info_location();
	uint32 get_code();
	uint32 get_another_code();
	int32 is_set_card(uint32 set_code);
	uint32 get_type();
	int32 get_base_attack(uint8 swap = FALSE);
	int32 get_attack(uint8 swap = FALSE);
	int32 get_base_defence(uint8 swap = FALSE);
	int32 get_defence(uint8 swap = FALSE);
	uint32 get_level();
	uint32 get_rank();
	uint32 get_synchro_level(card* pcard);
	uint32 get_ritual_level(card* pcard);
	uint32 is_xyz_level(card* pcard, uint32 lv);
	uint32 get_attribute();
	uint32 get_race();
	uint32 get_lscale();
	uint32 get_rscale();
	int32 is_position(int32 pos);
	void set_status(uint32 status, int32 enabled);
	int32 get_status(uint32 status);
	int32 is_status(uint32 status);

	void equip(card *target, uint32 send_msg = TRUE);
	void unequip();
	int32 get_union_count();
	void xyz_overlay(card_set* materials);
	void xyz_add(card* mat, card_set* des);
	void xyz_remove(card* mat);
	void apply_field_effect();
	void cancel_field_effect();
	void enable_field_effect(int32 enabled);
	int32 add_effect(effect* peffect);
	void remove_effect(effect* peffect);
	void remove_effect(effect* peffect, effect_container::iterator it);
	int32 copy_effect(uint32 code, uint32 reset, uint32 count);
	void reset(uint32 id, uint32 reset_type);
	void reset_effect_count();
	int32 refresh_disable_status();
	uint8 refresh_control_status();

	void count_turn(uint16 ct);
	void create_relation(card* target, uint32 reset);
	void create_relation(effect* peffect);
	int32 is_has_relation(card* target);
	int32 is_has_relation(effect* peffect);
	void release_relation(card* target);
	void release_relation(effect* peffect);
	int32 leave_field_redirect(uint32 reason);
	int32 destination_redirect(uint8 destination, uint32 reason);
	int32 add_counter(uint8 playerid, uint16 countertype, uint16 count);
	int32 remove_counter(uint16 countertype, uint16 count);
	int32 is_can_add_counter(uint8 playerid, uint16 countertype, uint16 count);
	int32 get_counter(uint16 countertype);
	void set_material(card_set* materials);
	void add_card_target(card* pcard);
	void cancel_card_target(card* pcard);
	
	void filter_effect(int32 code, effect_set* eset, uint8 sort = TRUE);
	void filter_single_continuous_effect(int32 code, effect_set* eset, uint8 sort = TRUE);
	void filter_immune_effect();
	void filter_disable_related_cards();
	int32 filter_summon_procedure(uint8 playerid, effect_set* eset, uint8 ignore_count);
	int32 filter_set_procedure(uint8 playerid, effect_set* eset, uint8 ignore_count);
	void filter_spsummon_procedure(uint8 playerid, effect_set* eset);
	void filter_spsummon_procedure_g(uint8 playerid, effect_set* eset);
	effect* is_affected_by_effect(int32 code);
	effect* is_affected_by_effect(int32 code, card* target);
	effect* check_equip_control_effect();
	int32 fusion_check(group* fusion_m, card* cg, int32 chkf);
	void fusion_select(uint8 playerid, group* fusion_m, card* cg, int32 chkf);
	
	int32 is_equipable(card* pcard);
	int32 is_summonable();
	int32 is_summonable(effect* peffect);
	int32 is_can_be_summoned(uint8 playerid, uint8 ingore_count, effect* peffect);
	int32 get_summon_tribute_count(uint8 ignore_count = 0);
	int32 get_set_tribute_count();
	int32 is_can_be_flip_summoned(uint8 playerid);
	int32 is_special_summonable(uint8 playerid);
	int32 is_can_be_special_summoned(effect* reason_effect, uint32 sumtype, uint8 sumpos, uint8 sumplayer, uint8 toplayer, uint8 nocheck, uint8 nolimit);
	int32 is_setable_mzone(uint8 playerid, uint8 ignore_count, effect* peffect);
	int32 is_setable_szone(uint8 playerid, uint8 ignore_fd = 0);
	int32 is_affect_by_effect(effect* peffect);
	int32 is_destructable();
	int32 is_destructable_by_battle(card* pcard);
	int32 is_destructable_by_effect(effect* peffect, uint8 playerid);
	int32 is_removeable(uint8 playerid);
	int32 is_removeable_as_cost(uint8 playerid);
	int32 is_releasable_by_summon(uint8 playerid, card* pcard);
	int32 is_releasable_by_nonsummon(uint8 playerid);
	int32 is_releasable_by_effect(uint8 playerid, effect* peffect);
	int32 is_capable_send_to_grave(uint8 playerid);
	int32 is_capable_send_to_hand(uint8 playerid);
	int32 is_capable_send_to_deck(uint8 playerid);
	int32 is_capable_send_to_extra(uint8 playerid);
	int32 is_capable_cost_to_grave(uint8 playerid);
	int32 is_capable_cost_to_hand(uint8 playerid);
	int32 is_capable_cost_to_deck(uint8 playerid);
	int32 is_capable_cost_to_extra(uint8 playerid);
	int32 is_capable_attack();
	int32 is_capable_attack_announce(uint8 playerid);
	int32 is_capable_change_position(uint8 playerid);
	int32 is_capable_turn_set(uint8 playerid);
	int32 is_capable_change_control();
	int32 is_control_can_be_changed();
	int32 is_capable_be_battle_target(card* pcard);
	int32 is_capable_be_effect_target(effect* peffect, uint8 playerid);
	int32 is_can_be_fusion_material(uint8 ignore_mon = FALSE);
	int32 is_can_be_synchro_material(card* scard, card* tuner = 0);
	int32 is_can_be_xyz_material(card* scard);
};

#define ASSUME_CODE			1
#define ASSUME_TYPE			2
#define ASSUME_LEVEL		3
#define ASSUME_RANK			4
#define ASSUME_ATTRIBUTE	5
#define ASSUME_RACE			6
#define ASSUME_ATTACK		7
#define ASSUME_DEFENCE		8

#endif /* CARD_H_ */

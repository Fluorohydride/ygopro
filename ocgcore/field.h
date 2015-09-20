/*
 * field.h
 *
 *  Created on: 2010-5-8
 *      Author: Argon
 */

#ifndef FIELD_H_
#define FIELD_H_

#include "memory.h"
#include "common.h"
#include "effectset.h"
#include <vector>
#include <set>
#include <map>
#include <list>
#include <array>
#include <functional>
#include <unordered_map>
#include <unordered_set>

class card;
struct card_data;
class duel;
class group;
class effect;

struct tevent {
	card* trigger_card;
	group* event_cards;
	effect* reason_effect;
	uint32 event_code;
	uint32 event_value;
	uint32 reason;
	uint8 event_player;
	uint8 reason_player;
	bool operator< (const tevent& v) const;
};
struct optarget {
	group* op_cards;
	uint8 op_count;
	uint8 op_player;
	int32 op_param;
};
struct chain {
	typedef std::map<uint32, optarget > opmap;
	uint16 chain_id;
	uint8 chain_count;
	uint8 triggering_player;
	uint8 triggering_controler;
	uint8 triggering_location;
	uint8 triggering_sequence;
	effect* triggering_effect;
	group* target_cards;
	int32 replace_op;
	uint8 target_player;
	int32 target_param;
	effect* disable_reason;
	uint8 disable_player;
	tevent evt;
	opmap opinfos;
	uint32 flag;
	static bool chain_operation_sort(const chain& c1, const chain& c2);
};

struct player_info {
	typedef std::vector<card*> card_vector;
	int32 lp;
	int32 start_count;
	int32 draw_count;
	uint32 used_location;
	uint32 disabled_location;
	uint32 extra_p_count;
	card_vector list_mzone;
	card_vector list_szone;
	card_vector list_main;
	card_vector list_grave;
	card_vector list_hand;
	card_vector list_remove;
	card_vector list_extra;
	card_vector tag_list_main;
	card_vector tag_list_hand;
	card_vector tag_list_extra;
};
struct field_effect {
	typedef std::multimap<uint32, effect*> effect_container;
	typedef std::map<effect*, effect_container::iterator> effect_indexer;
	typedef std::map<effect*, effect*> oath_effects;
	typedef std::set<effect*> effect_collection;

	effect_container aura_effect;
	effect_container ignition_effect;
	effect_container activate_effect;
	effect_container trigger_o_effect;
	effect_container trigger_f_effect;
	effect_container quick_o_effect;
	effect_container quick_f_effect;
	effect_container continuous_effect;
	effect_indexer indexer;
	oath_effects oath;
	effect_collection pheff;
	effect_collection cheff;
	effect_collection rechargeable;
	effect_collection spsummon_count_eff;
	
	std::list<card*> disable_check_list;
	std::set<card*, card_sort> disable_check_set;
};
struct field_info {
	int32 field_id;
	int16 copy_id;
	int16 turn_id;
	int16 card_id;
	uint8 phase;
	uint8 turn_player;
	uint8 priorities[2];
	uint8 can_shuffle;
};
struct lpcost {
	int32 count;
	int32 amount;
	int32 lpstack[8];
};
struct processor_unit {
	uint16 type;
	uint16 step;
	effect* peffect;
	group* ptarget;
	ptr arg1;
	ptr arg2;
};
union return_value {
	int8 bvalue[64];
	int16 svalue[32];
	int32 ivalue[16];
	int64 lvalue[8];
};
struct processor {
	typedef std::vector<effect*> effect_vector;
	typedef std::vector<card*> card_vector;
	typedef std::vector<uint32> option_vector;
	typedef std::list<card*> card_list;
	typedef std::list<tevent> event_list;
	typedef std::list<chain> chain_list;
	typedef std::map<effect*, chain> instant_f_list;
	typedef std::vector<chain> chain_array;
	typedef std::list<processor_unit> processor_list;
	typedef std::set<card*, card_sort> card_set;
	typedef std::set<effect*> effect_collection;
	typedef std::set<std::pair<effect*, tevent> > delayed_effect_collection;

	processor_list units;
	processor_list subunits;
	processor_unit reserved;
	card_vector select_cards;
	card_vector summonable_cards;
	card_vector spsummonable_cards;
	card_vector repositionable_cards;
	card_vector msetable_cards;
	card_vector ssetable_cards;
	card_vector attackable_cards;
	effect_vector select_effects;
	option_vector select_options;
	event_list point_event;
	event_list instant_event;
	event_list queue_event;
	event_list used_event;
	event_list single_event;
	event_list solving_event;
	event_list sub_solving_event;
	chain_array select_chains;
	chain_array current_chain;
	chain_list tpchain;
	chain_list ntpchain;
	chain_list continuous_chain;
	chain_list desrep_chain;
	chain_list new_fchain;
	chain_list new_fchain_s;
	chain_list new_ochain;
	chain_list new_ochain_s;
	chain_list new_fchain_b;
	chain_list new_ochain_b;
	chain_list new_ochain_h;
	chain_list new_chains;
	chain_list tmp_chain;
	delayed_effect_collection delayed_quick_tmp;
	delayed_effect_collection delayed_quick_break;
	delayed_effect_collection delayed_quick;
	instant_f_list quick_f_chain;
	card_set leave_confirmed;
	card_set special_summoning;
	card_set equiping_cards;
	card_set control_adjust_set[2];
	card_set self_destroy_set;
	card_set self_tograve_set;
	card_set release_cards;
	card_set release_cards_ex;
	card_set release_cards_ex_sum;
	card_set destroy_set;
	card_set battle_destroy_rep;
	card_set fusion_materials;
	card_set synchro_materials;
	card_set operated_set;
	card_set discarded_set;
	card_set destroy_canceled;
	card_set delayed_enable_set;
	effect_set_v disfield_effects;
	effect_set_v extraz_effects;
	effect_set_v extraz_effects_e;
	std::set<effect*> reseted_effects;
	std::list<effect*> delayed_tp;
	std::list<effect*> delayed_ntp;
	event_list delayed_tev;
	event_list delayed_ntev;
	std::unordered_map<card*, uint32> readjust_map;
	std::unordered_set<card*> unique_cards[2];
	std::unordered_map<uint32, uint32> effect_count_code;
	std::unordered_map<uint32, uint32> effect_count_code_duel;
	std::unordered_map<uint32, uint32> spsummon_once_map[2];
	std::unordered_map<uint32, uint32> spsummon_once_map_rst[2];
	std::multimap<int32, card*, std::greater<int32> > xmaterial_lst;
	ptr temp_var[4];
	uint32 global_flag;
	uint16 pre_field[2];
	uint16 opp_mzone[5];
	int32 chain_limit;
	uint8 chain_limp;
	int32 chain_limit_p;
	uint8 chain_limp_p;
	uint8 chain_solving;
	uint8 conti_solving;
	uint8 win_player;
	uint8 win_reason;
	uint8 re_adjust;
	effect* reason_effect;
	uint8 reason_player;
	card* summoning_card;
	uint8 summon_depth;
	uint8 summon_cancelable;
	card* attacker;
	card* sub_attacker;
	card* attack_target;
	card* sub_attack_target;
	card* limit_tuner;
	group* limit_xyz;
	group* limit_syn;
	uint8 attack_cancelable;
	uint8 attack_rollback;
	uint8 effect_damage_step;
	int32 battle_damage[2];
	int32 summon_count[2];
	uint8 extra_summon[2];
	int32 spe_effect[2];
	int32 duel_options;
	uint32 copy_reset;
	uint8 copy_reset_count;
	uint8 dice_result[5];
	uint8 coin_result[5];
	uint8 to_bp;
	uint8 to_m2;
	uint8 to_ep;
	uint8 skip_m2;
	uint8 chain_attack;
	card* chain_attack_target;
	uint8 selfdes_disabled;
	uint8 overdraw[2];
	int32 check_level;
	uint8 shuffle_check_disabled;
	uint8 shuffle_hand_check[2];
	uint8 shuffle_deck_check[2];
	uint8 deck_reversed;
	uint8 remove_brainwashing;
	uint8 flip_delayed;
	uint8 damage_calculated;
	uint8 hand_adjusted;
	uint8 summon_state_count[2];
	uint8 normalsummon_state_count[2];
	uint8 flipsummon_state_count[2];
	uint8 spsummon_state_count[2];
	uint8 spsummon_state_count_rst[2];
	uint8 spsummon_state_count_tmp[2];
	bool spsummon_rst;
	uint8 attack_state_count[2];
	uint8 battle_phase_count[2];
	uint8 phase_action;
	uint8 battle_phase_action;
	uint32 hint_timing[2];
	uint8 current_player;
	uint8 conti_player;
	std::unordered_map<uint32, std::pair<uint32, uint32> > summon_counter;
	std::unordered_map<uint32, std::pair<uint32, uint32> > normalsummon_counter;
	std::unordered_map<uint32, std::pair<uint32, uint32> > spsummon_counter;
	std::unordered_map<uint32, std::pair<uint32, uint32> > flipsummon_counter;
	std::unordered_map<uint32, std::pair<uint32, uint32> > attack_counter;
	std::unordered_map<uint32, std::pair<uint32, uint32> > chain_counter;
};
class field {
public:
	typedef std::multimap<uint32, effect*> effect_container;
	typedef std::map<effect*, effect_container::iterator> effect_indexer;
	typedef std::set<card*, card_sort> card_set;
	typedef std::vector<effect*> effect_vector;
	typedef std::vector<card*> card_vector;
	typedef std::list<card*> card_list;
	typedef std::list<tevent> event_list;
	typedef std::list<chain> chain_list;
	typedef std::map<effect*, chain> instant_f_list;
	typedef std::vector<chain> chain_array;
	typedef std::list<processor_unit> processor_list;
	typedef std::map<effect*, effect*> oath_effects;

	duel* pduel;
	player_info player[2];
	card* temp_card;
	field_info infos;
	lpcost cost[2];
	field_effect effects;
	processor core;
	return_value returns;
	tevent nil_event;

	static int32 field_used_count[32];
	explicit field(duel* pduel);
	~field();
	void reload_field_info();
	
	void add_card(uint8 playerid, card* pcard, uint8 location, uint8 sequence);
	void remove_card(card* pcard);
	void move_card(uint8 playerid, card* pcard, uint8 location, uint8 sequence);
	void set_control(card* pcard, uint8 playerid, uint16 reset_phase, uint8 reset_count);
	card* get_field_card(uint8 playerid, uint8 location, uint8 sequence);
	int32 is_location_useable(uint8 playerid, uint8 location, uint8 sequence);
	int32 get_useable_count(uint8 playerid, uint8 location, uint8 uplayer, uint32 reason, uint32* list = 0);
	void shuffle(uint8 playerid, uint8 location);
	void reset_sequence(uint8 playerid, uint8 location);
	void swap_deck_and_grave(uint8 playerid);
	void reverse_deck(uint8 playerid);
	void tag_swap(uint8 playerid);
	
	void add_effect(effect* peffect, uint8 owner_player = 2);
	void remove_effect(effect* peffect);
	void remove_oath_effect(effect* reason_effect);
	void reset_effect(uint32 id, uint32 reset_type);
	void reset_phase(uint32 phase);
	void reset_chain();
	void add_effect_code(uint32 code, uint32 playerid);
	uint32 get_effect_code(uint32 code, uint32 playerid);
	void dec_effect_code(uint32 code, uint32 playerid);
	
	void filter_field_effect(uint32 code, effect_set* eset, uint8 sort = TRUE);
	void filter_affected_cards(effect* peffect, card_set* cset);
	void filter_player_effect(uint8 playerid, uint32 code, effect_set* eset, uint8 sort = TRUE);
	int32 filter_matching_card(int32 findex, uint8 self, uint32 location1, uint32 location2, group* pgroup, card* pexception, uint32 extraargs, card** pret = 0, int32 fcount = 0, int32 is_target = FALSE);
	int32 filter_field_card(uint8 self, uint32 location, uint32 location2, group* pgroup);
	effect* is_player_affected_by_effect(uint8 playerid, uint32 code);

	int32 get_release_list(uint8 playerid, card_set* release_list, card_set* ex_list, int32 use_con, int32 use_hand, int32 fun, int32 exarg, card* exp);
	int32 check_release_list(uint8 playerid, int32 count, int32 use_con, int32 use_hand, int32 fun, int32 exarg, card* exp);
	int32 get_summon_release_list(card* target, card_set* release_list, card_set* ex_list, card_set* ex_list_sum, group* mg = 0, uint32 ex = 0);
	int32 get_summon_count_limit(uint8 playerid);
	int32 get_draw_count(uint8 playerid);
	void get_ritual_material(uint8 playerid, effect* peffect, card_set* material);
	void ritual_release(card_set* material);
	void get_xyz_material(card* scard, int32 findex, uint32 lv, int32 maxc);
	void get_overlay_group(uint8 self, uint8 s, uint8 o, card_set* pset);
	int32 get_overlay_count(uint8 self, uint8 s, uint8 o);
	void update_disable_check_list(effect* peffect);
	void add_to_disable_check_list(card* pcard);
	void adjust_disable_check_list();
	void adjust_self_destroy_set();
	void add_unique_card(card* pcard);
	void remove_unique_card(card* pcard);
	effect* check_unique_onfield(card* pcard, uint8 controler);
	int32 check_spsummon_once(card* pcard, uint8 playerid);
	void check_card_counter(card* pcard, int32 counter_type, int32 playerid);
	void check_chain_counter(effect* peffect, int32 playerid, int32 chainid, bool cancel = false);
	void set_spsummon_counter(uint8 playerid, bool add = true, bool chain = false);
	int32 check_spsummon_counter(uint8 playerid, uint8 ct = 1);
	
	int32 check_lp_cost(uint8 playerid, uint32 cost);
	void save_lp_cost();
	void restore_lp_cost();
	int32 pay_lp_cost(uint32 step, uint8 playerid, uint32 cost);

	uint32 get_field_counter(uint8 self, uint8 s, uint8 o, uint16 countertype);
	int32 effect_replace_check(uint32 code, const tevent& e);
	int32 get_attack_target(card* pcard, card_vector* v, uint8 chain_attack = FALSE);
	void attack_all_target_check();
	int32 check_synchro_material(card* pcard, int32 findex1, int32 findex2, int32 min, int32 max, card* smat, group* mg);
	int32 check_tuner_material(card* pcard, card* tuner, int32 findex1, int32 findex2, int32 min, int32 max, card* smat, group* mg);
	int32 check_with_sum_limit(card_vector* mats, int32 acc, int32 index, int32 count, int32 min, int32 max);
	int32 check_xyz_material(card* pcard, int32 findex, int32 lv, int32 min, int32 max, group* mg);
	
	int32 is_player_can_draw(uint8 playerid);
	int32 is_player_can_discard_deck(uint8 playerid, int32 count);
	int32 is_player_can_discard_deck_as_cost(uint8 playerid, int32 count);
	int32 is_player_can_discard_hand(uint8 playerid, card* pcard, effect* peffect, uint32 reason);
	int32 is_player_can_summon(uint8 playerid);
	int32 is_player_can_summon(uint32 sumtype, uint8 playerid, card* pcard);
	int32 is_player_can_mset(uint32 sumtype, uint8 playerid, card* pcard);
	int32 is_player_can_sset(uint8 playerid, card* pcard);
	int32 is_player_can_spsummon(uint8 playerid);
	int32 is_player_can_spsummon(effect* peffect, uint32 sumtype, uint8 sumpos, uint8 playerid, uint8 toplayer, card* pcard);
	int32 is_player_can_flipsummon(uint8 playerid, card* pcard);
	int32 is_player_can_spsummon_monster(uint8 playerid, uint8 toplayer, uint8 sumpos, card_data* pdata);
	int32 is_player_can_spsummon_count(uint8 playerid, uint32 count);
	int32 is_player_can_release(uint8 playerid, card* pcard);
	int32 is_player_can_place_counter(uint8 playerid, card* pcard, uint16 countertype, uint16 count);
	int32 is_player_can_remove_counter(uint8 playerid, card* pcard, uint8 s, uint8 o, uint16 countertype, uint16 count, uint32 reason);
	int32 is_player_can_remove_overlay_card(uint8 playerid, card* pcard, uint8 s, uint8 o, uint16 count, uint32 reason);
	int32 is_player_can_send_to_grave(uint8 playerid, card* pcard);
	int32 is_player_can_send_to_hand(uint8 playerid, card* pcard);
	int32 is_player_can_send_to_deck(uint8 playerid, card* pcard);
	int32 is_player_can_remove(uint8 playerid, card* pcard);
	int32 is_chain_negatable(uint8 chaincount, uint8 naga_check = FALSE);
	int32 is_chain_disablable(uint8 chaincount, uint8 naga_check = FALSE);
	int32 check_chain_target(uint8 chaincount, card* pcard);
	int32 is_able_to_enter_bp();

	void add_process(uint16 type, uint16 step, effect* peffect, group* target, ptr arg1, ptr arg2);
	int32 process();
	int32 execute_cost(uint16 step, effect* peffect, uint8 triggering_player);
	int32 execute_operation(uint16 step, effect* peffect, uint8 triggering_player);
	int32 execute_target(uint16 step, effect* peffect, uint8 triggering_player);
	void raise_event(card* event_card, uint32 event_code, effect* reason_effect, uint32 reason, uint8 reason_player, uint8 event_player, uint32 event_value);
	void raise_event(card_set* event_cards, uint32 event_code, effect* reason_effect, uint32 reason, uint8 reason_player, uint8 event_player, uint32 event_value);
	void raise_single_event(card* trigger_card, card_set* event_cards, uint32 event_code, effect* reason_effect, uint32 reason, uint8 reason_player, uint8 event_player, uint32 event_value);
	int32 check_event(uint32 code, tevent* pe = 0);
	int32 check_event_c(effect* peffect, uint8 playerid, int32 neglect_con, int32 neglect_cost, int32 copy_info, tevent* pe = 0);
	int32 check_hint_timing(effect* peffect);
	int32 process_phase_event(int16 step, int32 phase_event);
	int32 process_point_event(int16 step, int32 skip_trigger, int32 skip_freechain, int32 skip_new);
	int32 process_quick_effect(int16 step, int32 skip_freechain, uint8 priority);
	int32 process_instant_event();
	int32 process_single_event();
	int32 process_idle_command(uint16 step);
	int32 process_battle_command(uint16 step);
	int32 process_damage_step(uint16 step);
	void calculate_battle_damage(effect** pdamchange, card** preason_card, uint8* battle_destroyed);
	int32 process_turn(uint16 step, uint8 turn_player);

	int32 add_chain(uint16 step);
	int32 sort_chain(uint16 step, uint8 tp);
	int32 solve_continuous(uint16 step, effect* peffect, uint8 triggering_player);
	int32 solve_chain(uint16 step, uint32 chainend_arg1, uint32 chainend_arg2);
	int32 break_effect();
	void adjust_instant();
	void adjust_all();
	void refresh_location_info_instant();
	int32 refresh_location_info(uint16 step);
	int32 adjust_step(uint16 step);

	//operations
	int32 negate_chain(uint8 chaincount);
	int32 disable_chain(uint8 chaincount);
	void change_chain_effect(uint8 chaincount, int32 replace_op);
	void change_target(uint8 chaincount, group* targets);
	void change_target_player(uint8 chaincount, uint8 playerid);
	void change_target_param(uint8 chaincount, int32 param);
	void remove_counter(uint32 reason, card* pcard, uint32 rplayer, uint32 s, uint32 o, uint32 countertype, uint32 count);
	void remove_overlay_card(uint32 reason, card* pcard, uint32 rplayer, uint32 s, uint32 o, uint16 min, uint16 max);
	void get_control(effect* reason_effect, uint32 reason_player, card* pcard, uint32 playerid, uint32 reset_phase, uint32 reset_count);
	void swap_control(effect* reason_effect, uint32 reason_player, card* pcard1, card* pcard2, uint32 reset_phase, uint32 reset_count);
	void equip(uint32 equip_player, card* equip_card, card* target, uint32 up, uint32 is_step);
	void draw(effect* reason_effect, uint32 reason, uint32 reason_player, uint32 playerid, uint32 count);
	void damage(effect* reason_effect, uint32 reason, uint32 reason_player, card* pcard, uint32 playerid, uint32 amount);
	void recover(effect* reason_effect, uint32 reason, uint32 reason_player, uint32 playerid, uint32 amount);
	void summon(uint32 sumplayer, card* target, effect* proc, uint32 ignore_count, uint32 min_tribute);
	void special_summon_rule(uint32 sumplayer, card* target, uint32 summon_type);
	void special_summon(card_set* target, uint32 sumtype, uint32 sumplayer, uint32 playerid, uint32 nocheck, uint32 nolimit, uint32 positions);
	void special_summon_step(card* target, uint32 sumtype, uint32 sumplayer, uint32 playerid, uint32 nocheck, uint32 nolimit, uint32 positions);
	void special_summon_complete(effect* reason_effect, uint8 reason_player);
	void destroy(card_set* targets, effect* reason_effect, uint32 reason, uint32 reason_player, uint32 playerid = 2, uint32 destination = 0, uint32 sequence = 0);
	void destroy(card* target, effect* reason_effect, uint32 reason, uint32 reason_player, uint32 playerid = 2, uint32 destination = 0, uint32 sequence = 0);
	void release(card_set* targets, effect* reason_effect, uint32 reason, uint32 reason_player);
	void release(card* target, effect* reason_effect, uint32 reason, uint32 reason_player);
	void send_to(card_set* targets, effect* reason_effect, uint32 reason, uint32 reason_player, uint32 playerid, uint32 destination, uint32 sequence, uint32 position);
	void send_to(card* target, effect* reason_effect, uint32 reason, uint32 reason_player, uint32 playerid, uint32 destination, uint32 sequence, uint32 position);
	void move_to_field(card* target, uint32 move_player, uint32 playerid, uint32 destination, uint32 positions, uint32 enable = FALSE, uint32 ret = FALSE, uint32 is_equip = FALSE);
	void change_position(card_set* targets, effect* reason_effect, uint32 reason_player, uint32 au, uint32 ad, uint32 du, uint32 dd, uint32 noflip, uint32 enable = FALSE);
	void change_position(card* target, effect* reason_effect, uint32 reason_player, uint32 npos, uint32 noflip, uint32 enable = FALSE);

	int32 remove_counter(uint16 step, uint32 reason, card* pcard, uint8 rplayer, uint8 s, uint8 o, uint16 countertype, uint16 count);
	int32 remove_overlay_card(uint16 step, uint32 reason, card* pcard, uint8 rplayer, uint8 s, uint8 o, uint16 min, uint16 max);
	int32 get_control(uint16 step, effect* reason_effect, uint8 reason_player, card* pcard, uint8 playerid, uint16 reset_phase, uint8 reset_count);
	int32 swap_control(uint16 step, effect* reason_effect, uint8 reason_player, card* pcard1, card* pcard2, uint16 reset_phase, uint8 reset_count);
	int32 control_adjust(uint16 step);
	int32 self_destroy(uint16 step);
	int32 equip(uint16 step, uint8 equip_player, card* equip_card, card* target, uint32 up, uint32 is_step);
	int32 draw(uint16 step, effect* reason_effect, uint32 reason, uint8 reason_player, uint8 playerid, uint32 count);
	int32 damage(uint16 step, effect* reason_effect, uint32 reason, uint8 reason_player, card* pcard, uint8 playerid, uint32 amount);
	int32 recover(uint16 step, effect* reason_effect, uint32 reason, uint8 reason_player, uint8 playerid, uint32 amount);
	int32 summon(uint16 step, uint8 sumplayer, card* target, effect* proc, uint8 ignore_count, uint8 min_tribute);
	int32 flip_summon(uint16 step, uint8 sumplayer, card* target);
	int32 mset(uint16 step, uint8 setplayer, card* ptarget, effect* proc, uint8 ignore_count, uint8 min_tribute);
	int32 sset(uint16 step, uint8 setplayer, uint8 toplayer, card* ptarget);
	int32 sset_g(uint16 step, uint8 setplayer, uint8 toplayer, group* ptarget);
	int32 special_summon_rule(uint16 step, uint8 sumplayer, card* target, uint32 summon_type);
	int32 special_summon_step(uint16 step, group* targets, card* target);
	int32 special_summon(uint16 step, effect* reason_effect, uint8 reason_player, group* targets);
	int32 destroy(uint16 step, group* targets, card* target, uint8 battle);
	int32 destroy(uint16 step, group* targets, effect* reason_effect, uint32 reason, uint8 reason_player);
	int32 release(uint16 step, group* targets, card* target);
	int32 release(uint16 step, group* targets, effect* reason_effect, uint32 reason, uint8 reason_player);
	int32 send_to(uint16 step, group* targets, card* target);
	int32 send_to(uint16 step, group* targets, effect* reason_effect, uint32 reason, uint8 reason_player);
	int32 discard_deck(uint16 step, uint8 playerid, uint8 count, uint32 reason);
	int32 move_to_field(uint16 step, card* target, uint32 enable, uint32 ret, uint32 is_equip);
	int32 change_position(uint16 step, group* targets, effect* reason_effect, uint8 reason_player, uint32 enable);
	int32 operation_replace(uint16 step, effect* replace_effect, group* targets, ptr arg, ptr replace_type);
	int32 select_synchro_material(int16 step, uint8 playerid, card* pcard, int32 min, int32 max, card* smat, group* mg);
	int32 select_xyz_material(int16 step, uint8 playerid, uint32 lv, card* pcard, int32 min, int32 max);
	int32 select_release_cards(int16 step, uint8 playerid, uint8 check_field, uint8 cancelable, int32 min, int32 max);
	int32 select_tribute_cards(int16 step, uint8 playerid, uint8 cancelable, int32 min, int32 max);
	int32 toss_coin(uint16 step, effect* reason_effect, uint8 reason_player, uint8 playerid, uint8 count);
	int32 toss_dice(uint16 step, effect* reason_effect, uint8 reason_player, uint8 playerid, uint8 count1, uint8 count2);

	int32 select_battle_command(uint16 step, uint8 playerid);
	int32 select_idle_command(uint16 step, uint8 playerid);
	int32 select_effect_yes_no(uint16 step, uint8 playerid, card* pcard);
	int32 select_yes_no(uint16 step, uint8 playerid, uint32 description);
	int32 select_option(uint16 step, uint8 playerid);
	int32 select_card(uint16 step, uint8 playerid, uint8 cancelable, uint8 min, uint8 max);
	int32 select_chain(uint16 step, uint8 playerid, uint8 spe_count, uint8 forced);
	int32 select_place(uint16 step, uint8 playerid, uint32 flag, uint8 count);
	int32 select_position(uint16 step, uint8 playerid, uint32 code, uint8 positions);
	int32 select_tribute(uint16 step, uint8 playerid, uint8 cancelable, uint8 min, uint8 max);
	int32 select_counter(uint16 step, uint8 playerid, uint16 countertype, uint16 count);
	int32 select_with_sum_limit(int16 step, uint8 playerid, int32 acc, int32 min, int max);
	int32 sort_card(int16 step, uint8 playerid, uint8 is_chain);
	int32 announce_race(int16 step, uint8 playerid, int32 count, int32 available);
	int32 announce_attribute(int16 step, uint8 playerid, int32 count, int32 available);
	int32 announce_card(int16 step, uint8 playerid);
	int32 announce_number(int16 step, uint8 playerid);
};

//Location Use Reason
#define LOCATION_REASON_TOFIELD	0x1
#define LOCATION_REASON_CONTROL	0x2
//Chain Info
#define CHAIN_DISABLE_ACTIVATE	0x01
#define CHAIN_DISABLE_EFFECT	0x02
#define CHAIN_HAND_EFFECT		0x04
#define CHAININFO_CHAIN_COUNT			0x01
#define CHAININFO_TRIGGERING_EFFECT		0x02
#define CHAININFO_TRIGGERING_PLAYER		0x04
#define CHAININFO_TRIGGERING_CONTROLER	0x08
#define CHAININFO_TRIGGERING_LOCATION	0x10
#define CHAININFO_TRIGGERING_SEQUENCE	0x20
#define CHAININFO_TARGET_CARDS			0x40
#define CHAININFO_TARGET_PLAYER			0x80
#define CHAININFO_TARGET_PARAM			0x100
#define CHAININFO_DISABLE_REASON		0x200
#define CHAININFO_DISABLE_PLAYER		0x400
#define CHAININFO_CHAIN_ID				0x800
#define CHAININFO_TYPE					0x1000
#define CHAININFO_EXTTYPE				0x2000
//Timing
#define TIMING_DRAW_PHASE			0x1
#define TIMING_STANDBY_PHASE		0x2
#define TIMING_MAIN_END				0x4
#define TIMING_BATTLE_START			0x8
#define TIMING_BATTLE_END			0x10
#define TIMING_END_PHASE			0x20
#define TIMING_SUMMON				0x40
#define TIMING_SPSUMMON				0x80
#define TIMING_FLIPSUMMON			0x100
#define TIMING_MSET					0x200
#define TIMING_SSET					0x400
#define TIMING_POS_CHANGE			0x800
#define TIMING_ATTACK				0x1000
#define TIMING_DAMAGE_STEP			0x2000
#define TIMING_DAMAGE_CAL			0x4000
#define TIMING_CHAIN_END			0x8000
#define TIMING_DRAW					0x10000
#define TIMING_DAMAGE				0x20000
#define TIMING_RECOVER				0x40000
#define TIMING_DESTROY				0x80000
#define TIMING_REMOVE				0x100000
#define TIMING_TOHAND				0x200000
#define TIMING_TODECK				0x400000
#define TIMING_TOGRAVE				0x800000
#define TIMING_BATTLE_PHASE			0x1000000
#define TIMING_EQUIP				0x2000000

#define GLOBALFLAG_DECK_REVERSE_CHECK	0x1
#define GLOBALFLAG_BRAINWASHING_CHECK	0x2
#define GLOBALFLAG_SCRAP_CHIMERA		0x4
#define GLOBALFLAG_DELAYED_QUICKEFFECT	0x8
#define GLOBALFLAG_DETACH_EVENT			0x10
#define GLOBALFLAG_MUST_BE_SMATERIAL	0x20
#define GLOBALFLAG_SPSUMMON_COUNT		0x40
#define GLOBALFLAG_XMAT_COUNT_LIMIT		0x80
#define GLOBALFLAG_SELF_TOGRAVE			0x100
#define GLOBALFLAG_SPSUMMON_ONCE		0x200
//
#define PROCESSOR_NONE		0
#define PROCESSOR_WAITING	0x10000
#define PROCESSOR_END		0x20000

#define PROCESSOR_ADJUST			1
#define PROCESSOR_HINT				2
#define PROCESSOR_TURN				3
#define PROCESSOR_WAIT				4
#define PROCESSOR_REFRESH_LOC		5
#define PROCESSOR_SELECT_IDLECMD	10
#define PROCESSOR_SELECT_EFFECTYN	11
#define PROCESSOR_SELECT_BATTLECMD	12
#define PROCESSOR_SELECT_YESNO		13
#define PROCESSOR_SELECT_OPTION		14
#define PROCESSOR_SELECT_CARD		15
#define PROCESSOR_SELECT_CHAIN		16
#define PROCESSOR_SELECT_PLACE		18
#define PROCESSOR_SELECT_POSITION	19
#define PROCESSOR_SELECT_TRIBUTE_P	20
#define PROCESSOR_SORT_CHAIN		21
#define PROCESSOR_SELECT_COUNTER	22
#define PROCESSOR_SELECT_SUM		23
#define PROCESSOR_SELECT_DISFIELD	24
#define PROCESSOR_SORT_CARD			25
#define PROCESSOR_SELECT_RELEASE	26
#define PROCESSOR_SELECT_TRIBUTE	27
#define PROCESSOR_POINT_EVENT		30
#define PROCESSOR_QUICK_EFFECT		31
#define PROCESSOR_IDLE_COMMAND		32
#define PROCESSOR_PHASE_EVENT		33
#define PROCESSOR_BATTLE_COMMAND	34
#define PROCESSOR_DAMAGE_STEP		35
#define PROCESSOR_ADD_CHAIN			40
#define PROCESSOR_SOLVE_CHAIN		42
#define PROCESSOR_SOLVE_CONTINUOUS	43
#define PROCESSOR_EXECUTE_COST		44
#define PROCESSOR_EXECUTE_OPERATION	45
#define PROCESSOR_EXECUTE_TARGET	46
#define PROCESSOR_DESTROY			50
#define PROCESSOR_RELEASE			51
#define PROCESSOR_SENDTO			52
#define PROCESSOR_MOVETOFIELD		53
#define PROCESSOR_CHANGEPOS			54
#define PROCESSOR_OPERATION_REPLACE	55
#define PROCESSOR_DESTROY_STEP		56
#define PROCESSOR_RELEASE_STEP		57
#define PROCESSOR_SENDTO_STEP		58
#define PROCESSOR_SUMMON_RULE		60
#define PROCESSOR_SPSUMMON_RULE		61
#define PROCESSOR_SPSUMMON			62
#define PROCESSOR_FLIP_SUMMON		63
#define PROCESSOR_MSET				64
#define PROCESSOR_SSET				65
#define PROCESSOR_SPSUMMON_STEP		66
#define PROCESSOR_SSET_G			67
#define PROCESSOR_DRAW				70
#define PROCESSOR_DAMAGE			71
#define PROCESSOR_RECOVER			72
#define PROCESSOR_EQUIP				73
#define PROCESSOR_GET_CONTROL		74
#define PROCESSOR_SWAP_CONTROL		75
#define PROCESSOR_CONTROL_ADJUST	76
#define PROCESSOR_SELF_DESTROY		77
#define PROCESSOR_PAY_LPCOST		80
#define PROCESSOR_REMOVE_COUNTER	81
#define PROCESSOR_ATTACK_DISABLE	82

#define PROCESSOR_DESTROY_S			100
#define PROCESSOR_RELEASE_S			101
#define PROCESSOR_SENDTO_S			102
#define PROCESSOR_CHANGEPOS_S		103
#define PROCESSOR_ANNOUNCE_RACE		110
#define PROCESSOR_ANNOUNCE_ATTRIB	111
#define PROCESSOR_ANNOUNCE_LEVEL	112
#define PROCESSOR_ANNOUNCE_CARD		113
#define PROCESSOR_ANNOUNCE_TYPE		114
#define PROCESSOR_ANNOUNCE_NUMBER	115
#define PROCESSOR_ANNOUNCE_COIN		116
#define PROCESSOR_TOSS_DICE			117
#define PROCESSOR_TOSS_COIN			118
#define PROCESSOR_SELECT_YESNO_S	120
#define PROCESSOR_SELECT_OPTION_S	121
#define PROCESSOR_SELECT_CARD_S		122
#define PROCESSOR_SELECT_EFFECTYN_S	123
#define PROCESSOR_SELECT_PLACE_S	125
#define PROCESSOR_SELECT_POSITION_S	126
#define PROCESSOR_SELECT_TRIBUTE_S	127
#define PROCESSOR_SORT_CARDS_S		128
#define PROCESSOR_SELECT_RELEASE_S	129
#define PROCESSOR_SELECT_TARGET		130
#define PROCESSOR_SELECT_FUSION		131
#define PROCESSOR_SELECT_SYNCHRO	132
#define PROCESSOR_SELECT_SUM_S		133
#define PROCESSOR_SELECT_DISFIELD_S	134
#define PROCESSOR_SPSUMMON_S		135
#define PROCESSOR_SPSUMMON_STEP_S	136
#define PROCESSOR_SPSUMMON_COMP_S	137
#define PROCESSOR_RANDOM_SELECT_S	138
#define PROCESSOR_SELECT_XMATERIAL	139
#define PROCESSOR_DRAW_S			140
#define PROCESSOR_DAMAGE_S			141
#define PROCESSOR_RECOVER_S			142
#define PROCESSOR_EQUIP_S			143
#define PROCESSOR_GET_CONTROL_S		144
#define PROCESSOR_SWAP_CONTROL_S	145
#define PROCESSOR_DISCARD_HAND_S	150
#define PROCESSOR_DISCARD_DECK_S	151
#define PROCESSOR_SORT_DECK_S		152
#define PROCESSOR_REMOVEOL_S		160
#define PROCESSOR_MOVETOFIELD_S		161

//Hints
#define HINT_EVENT				1
#define HINT_MESSAGE			2
#define HINT_SELECTMSG			3
#define HINT_OPSELECTED			4
#define HINT_EFFECT				5
#define HINT_RACE				6
#define HINT_ATTRIB				7
#define HINT_CODE				8
#define HINT_NUMBER				9
#define HINT_CARD				10
//
#define CHINT_TURN				1
#define CHINT_CARD				2
#define CHINT_RACE				3
#define CHINT_ATTRIBUTE			4
#define CHINT_NUMBER			5
#define CHINT_DESC_ADD			6
#define CHINT_DESC_REMOVE		7
//Messages
#define MSG_RETRY				1
#define MSG_HINT				2
#define MSG_WAITING				3
#define MSG_START				4
#define MSG_WIN					5
#define MSG_UPDATE_DATA			6
#define MSG_UPDATE_CARD			7
#define MSG_REQUEST_DECK		8
#define MSG_SELECT_BATTLECMD	10
#define MSG_SELECT_IDLECMD		11
#define MSG_SELECT_EFFECTYN		12
#define MSG_SELECT_YESNO		13
#define MSG_SELECT_OPTION		14
#define MSG_SELECT_CARD			15
#define MSG_SELECT_CHAIN		16
#define MSG_SELECT_PLACE		18
#define MSG_SELECT_POSITION		19
#define MSG_SELECT_TRIBUTE		20
#define MSG_SORT_CHAIN			21
#define MSG_SELECT_COUNTER		22
#define MSG_SELECT_SUM			23
#define MSG_SELECT_DISFIELD		24
#define MSG_SORT_CARD			25
#define MSG_CONFIRM_DECKTOP		30
#define MSG_CONFIRM_CARDS		31
#define MSG_SHUFFLE_DECK		32
#define MSG_SHUFFLE_HAND		33
#define MSG_REFRESH_DECK		34
#define MSG_SWAP_GRAVE_DECK		35
#define MSG_SHUFFLE_SET_CARD	36
#define MSG_REVERSE_DECK		37
#define MSG_DECK_TOP			38
#define MSG_NEW_TURN			40
#define MSG_NEW_PHASE			41
#define MSG_MOVE				50
#define MSG_POS_CHANGE			53
#define MSG_SET					54
#define MSG_SWAP				55
#define MSG_FIELD_DISABLED		56
#define MSG_SUMMONING			60
#define MSG_SUMMONED			61
#define MSG_SPSUMMONING			62
#define MSG_SPSUMMONED			63
#define MSG_FLIPSUMMONING		64
#define MSG_FLIPSUMMONED		65
#define MSG_CHAINING			70
#define MSG_CHAINED				71
#define MSG_CHAIN_SOLVING		72
#define MSG_CHAIN_SOLVED		73
#define MSG_CHAIN_END			74
#define MSG_CHAIN_NEGATED		75
#define MSG_CHAIN_DISABLED		76
#define MSG_CARD_SELECTED		80
#define MSG_RANDOM_SELECTED		81
#define MSG_BECOME_TARGET		83
#define MSG_DRAW				90
#define MSG_DAMAGE				91
#define MSG_RECOVER				92
#define MSG_EQUIP				93
#define MSG_LPUPDATE			94
#define MSG_UNEQUIP				95
#define MSG_CARD_TARGET			96
#define MSG_CANCEL_TARGET		97
#define MSG_PAY_LPCOST			100
#define MSG_ADD_COUNTER			101
#define MSG_REMOVE_COUNTER		102
#define MSG_ATTACK				110
#define MSG_BATTLE				111
#define MSG_ATTACK_DISABLED		112
#define MSG_DAMAGE_STEP_START	113
#define MSG_DAMAGE_STEP_END		114
#define MSG_MISSED_EFFECT		120
#define MSG_BE_CHAIN_TARGET		121
#define MSG_CREATE_RELATION		122
#define MSG_RELEASE_RELATION	123
#define MSG_TOSS_COIN			130
#define MSG_TOSS_DICE			131
#define MSG_ANNOUNCE_RACE		140
#define MSG_ANNOUNCE_ATTRIB		141
#define MSG_ANNOUNCE_CARD		142
#define MSG_ANNOUNCE_NUMBER		143
#define MSG_CARD_HINT			160
#define MSG_TAG_SWAP			161
#define MSG_RELOAD_FIELD		162
#define MSG_AI_NAME				163
#define MSG_SHOW_HINT			164
#define MSG_MATCH_KILL			170
#define MSG_CUSTOM_MSG			180
#endif /* FIELD_H_ */

#pragma once

#include "..\..\includes.hpp"
#include "..\..\structs.hpp"
#include "..\lagcompensation\lagcompensation.h"

class aimbot : public singleton< aimbot > {
private:
	struct target_t {
		float best_distance = 1000000.0f;
		bool backtrack = false;
		int tid = -1;
		vec3_t aimspot;
		float simtime = 0.f;
		tickrecord_t best_record;

		void reset() {
			best_distance = INT_MAX;
			tid = -1;
			simtime = 0.f;
			aimspot.Init();

			backtrack = false;
			best_record;
		}
	};

	struct aimbotconfig_t {
		bool autoshoot;
		bool autowall;
		bool autoscope;
		bool hitchance;

		int minimum_damage;
		int hitchance_amount;
		int multipointscale;
		std::vector< MultiDropdownItem_t > hitscan;
		std::vector< MultiDropdownItem_t > hitscan_history;
		std::vector< MultiDropdownItem_t > multipoint;
		std::vector< MultiDropdownItem_t > baim;

		float pointscale;
		int baim_health;
		int baimaftershoots;
		bool quickstop;
	};

	weapon_t * local_weapon;
	vec3_t engine_angles;
	float recoil_scale;
public:
	aimbotconfig_t config;
	target_t target_data;
public:
	bool huyrevolver();
	bool CockRevolver();
	void create_move();
	void iterate_players();

	void zeus_run();

	bool can_zeus(Vector & bestpoint);

	void aim();
	//float GetDamageVec(const Vector &vecPoint, player_t *player, int hitbox);
	float BestHitPoint(player_t *player, int prioritized, float minDmg, mstudiohitboxset_t *hitset, matrix3x4_t matrix[], Vector &vecOut);

	vec3_t best_point(player_t * e, matrix3x4_t bones[128]);

	struct bt_point_return_t {
		tickrecord_t record;
		vec3_t point;
	};

	aimbot::bt_point_return_t best_point_backtrack(player_t * e, player_record_t & player);
	void getpoints(player_t * e, int hitbox_id, matrix3x4_t bones[128], std::vector< vec3_t > & points);

	void quickstop();

	void quickstop2();

	bool hitchance(vec3_t angles, player_t * m_entity, float chance);

	void get_target(player_t * e, float fov, vec3_t location, float simulation_time, bool backtrack, tickrecord_t lag_record);

	bool head_check(player_t * e, int m_fFlags, vec3_t m_vecVelocity);
	bool is_slow_walking(player_t * entity);

	std::vector< int > hitboxes_from_vector(player_t * e, int m_fFlags, vec3_t m_vecVelocity, std::vector< MultiDropdownItem_t > arr);

	void update_config();

	int revolver_cocks = -1;
};
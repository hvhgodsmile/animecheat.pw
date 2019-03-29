#pragma once

#include "..\..\includes.hpp"
#include "..\..\structs.hpp"
#include <deque>
struct ResolveInfo
{
	ResolveInfo()
	{
		m_bActive = false;

		m_flVelocity = 0.f;
		m_vecVelocity = Vector(0, 0, 0);
		m_angEyeAngles = QAngle(0, 0, 0);
		m_flSimulationTime = 0.f;
		m_flLowerBodyYawTarget = 0.f;

		m_flStandingTime = 0.f;
		m_flMovingLBY = 0.f;
		m_flLbyDelta = 180.f;
		m_bIsMoving = false;

		m_angDirectionFirstMoving = QAngle(0, 0, 0);
		m_nCorrectedFakewalkIdx = 0;
	}

	bool operator==(const ResolveInfo &other)
	{
		return other.m_flSimulationTime == m_flSimulationTime;
	}

	bool m_bActive;

	float_t m_flVelocity;
	Vector m_vecVelocity;
	QAngle m_angEyeAngles;
	float_t m_flSimulationTime;
	float_t m_flLowerBodyYawTarget;

	int32_t m_iLayerCount = 0;
	AnimationLayer animationLayer[15];

	float_t m_flStandingTime;
	float_t m_flMovingLBY;
	float_t m_flLbyDelta;
	bool m_bIsMoving;

	QAngle m_angDirectionFirstMoving;
	int32_t m_nCorrectedFakewalkIdx;

	int32_t m_nShotsMissed = 0;
};
enum resolver_modes : int {
	none,
	brute,
	override,
	lby,
	lby_update,
	anti_freestand,
	angle_correction,
	desync_yaw
};

class c_player_resolver {
public:
	int m_mode;
	bool is_fake;
	struct resolver_info_t {
		// data
		vec3_t m_velocity;
		vec3_t m_origin;
		int shots_missed_moving_lby, shots_missed_moving_lby_delta;
		bool is_dormant;
		bool is_last_moving_lby_valid;
		bool is_fakewalking;
		bool is_last_moving_lby_delta_valid;
		bool is_balance_adjust_triggered, is_balance_adjust_playing;
		bool m_did_lby_flick, did_predicted_lby_flick;
		bool has_fake;
		float m_lowerbody;
		int m_flags;
		float m_pitch;
		float m_at_target;
		float at_target_yaw = 0.0f;
		float m_next_predicted_lby_update;
		float m_last_moving_lby_delta;
		// last moving lby
		float m_last_move_lby;
		bool m_last_move_lby_valid;
		float m_last_move_time;

		// ghetto pitch fix
		float last_pitchdown;

		// stuff for fakewalk
		AnimationLayer m_anim_layers[15];

		bool m_balance_adjust_triggered, m_balance_adjust_playing;
		float m_last_balance_adjust_time;

		bool m_fakewalking;
		bool m_predicted_flick;
		bool m_flick;

		// angle data
		vec3_t back;
		vec3_t left;
		vec3_t right;
		bool m_dormant;
		// brute data
		float m_predicted_yaw_brute;
	};

	struct anti_freestand_info_t {
		int right_damage = 0, left_damage = 0;
		float right_fraction = 0.f, left_fraction = 0.f;
	};

	player_t * m_e;

	resolver_info_t m_current;
	resolver_info_t m_previous;
	anti_freestand_info_t m_antifreestand;

	bool m_has_previous;

	void create_move(vec3_t latency_based_eye_pos);
	void lby_prediction();
	void run();
	void update();
	int Delta(float f1, float f2);
	void jitter_average(std::deque<int> deltas, int delta_max, int delta_min, int delta_avg);
	void update_fakewalk_data(resolver_info_t & current, player_t * e);
	bool is_slow_walking(player_t * e);
	bool is_fakewalk(player_t * e);
	void triggers_979(player_t * e);
	bool IsAdjustingBalance(player_t *player, ResolveInfo &record, AnimationLayer *layer);
	void resolve();
	bool can_extrapolate(bool can_autowall, player_t* entity, int extension, int accuracy_boost); // extrapolates the local position for a 
	bool can_autowall(player_t * entity, vec3_t src, int accuracy);
	void fakelagfix();
	void pitch_resolve();
};
struct tickrecord_t {
	vec3_t m_vec_origin;
	vec3_t m_eye_angles;
	vec3_t m_abs_angles;
	vec3_t m_render_angles;
	vec3_t m_bbmin;
	vec3_t m_bbmax;
	vec3_t m_velocity;

	vec3_t m_previous_origin;
	float m_previous_simulation_time;
	float m_previous_curtime;

	float m_simulation_time;
	float m_duck_amount;
	float m_duck_speed;
	float m_lowerbody;
	float m_curtime;
	int m_flags;

	AnimationLayer m_anim_layers[15];
	matrix3x4_t m_bone_matrix[128];
	float m_pose_params[24];

	float m_anim_time;
	c_baseplayeranimationstate * m_anim_state;

	c_player_resolver m_resolver;

	void save(player_t * e, c_player_resolver resolver) {
		m_resolver = resolver;

		m_vec_origin = e->m_vecOrigin();
		m_abs_angles = e->abs_angles();
		m_eye_angles = e->m_angEyeAngles();
		m_render_angles = e->GetRenderAngles();
		m_velocity = e->m_vecVelocity();
		m_simulation_time = e->m_flSimulationTime();
		m_duck_amount = e->m_flDuckAmount();
		m_duck_speed = e->m_flDuckSpeed();
		m_lowerbody = e->m_flLowerBodyYawTarget();
		m_flags = e->m_fFlags();
		m_curtime = g_csgo.m_globals()->m_curtime;

		for (int i = 0; i < 15; i++)
			m_anim_layers[i] = e->get_animlayer(i);

		memcpy(m_pose_params, e->m_flPoseParameter(), sizeof(float) * 24);

		e->SetupBones(m_bone_matrix, 128, BONE_USED_BY_ANYTHING, 0.f);

		auto collideable = e->GetCollideable();

		if (!collideable)
			return;

		m_bbmin = collideable->OBBMins();
		m_bbmax = collideable->OBBMaxs();
	}
};

struct player_record_t {
	player_t * m_e;

	c_player_resolver * m_resolver = new c_player_resolver();

	void log();

	std::deque< tickrecord_t > get_valid_track();

	std::deque< tickrecord_t > m_track;
};

struct incoming_sequence_t {
	incoming_sequence_t::incoming_sequence_t(int instate, int outstate, int seqnr, float time) {
		m_in_reliable_state = instate;
		m_out_reliable_state = outstate;
		m_sequence_nr = seqnr;
		m_current_time = time;
	}

	int m_in_reliable_state;
	int m_out_reliable_state;
	int m_sequence_nr;
	float m_current_time;
};

enum {
	disable_interpolation,
	enable_interpolation
};

class lagcompensation : public singleton< lagcompensation > {
public:
	void fsn(ClientFrameStage_t stage);
	void create_move();
	void AnimationFix(player_t* pEnt);
	void HandleHits(player_t* pEnt);
	bool continue_loop(int id, player_t * e, ClientFrameStage_t stage);
	void AnimFix();

	void apply_interpolation_flags(player_t * e, int flag);

	void animation_fix(player_t * e);
	void DoAnimationFix(player_t * e);
	void NotReallyUsefull(player_t * e);
	player_record_t players[65];
	std::deque< vec3_t > last_eye_positions;
public:
	std::deque< incoming_sequence_t > sequences;
	int m_last_incoming_sequence_number;

	void update_sequence();
	void clear_sequence();
	void add_latency(INetChannel * net_channel);
};

static bool complete_restore = false;
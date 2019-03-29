#pragma once

#include "..\includes.hpp"

class player_t;
class CUserCmd;

class ctx_t {
public:
	struct Globals {
		float w2s_matrix[4][4];
		float hitalpha;
		vec3_t real_angles;
		vec3_t fake_angles;
		vec3_t angles;
		bool aimbotting;
		bool isFakeing;
		bool hitgroundanimation;
		Vector unpredictedVelocity;
		bool NewRound;	
		Vector unpredicitedVelocity_v2;
		bool fakelagging;
		bool fakewalking;
		bool should_choke_packets;
		bool ShotFired[65];
		int shots[65];
		int missed_shots[65];
		float pitchHit[65];
		bool  Hit[65];
		float next_update[65];
		float add_time[65];
		int PlayersChockedPackets[65] = { 0 };
		Vector FakelagUnfixedPos[65] = { Vector(0, 0, 0) };
		bool FakelagFixed[65] = { false };
		void add_shot();
		void remove_shot(IGameEvent * event);
	}m_globals;

	bool available();

	player_t * m_local;

	matrix3x4_t backtack_matrix[64][128];

	vec3_t unpred_velocity;

	bool send_packet;
	bool backtrack_chams;
	Vector RealAngle;
	void set_command(CUserCmd * cmd) { m_pcmd = cmd; }
	CUserCmd * get_command();
private:
	CUserCmd * m_pcmd;
};

extern ctx_t g_ctx;
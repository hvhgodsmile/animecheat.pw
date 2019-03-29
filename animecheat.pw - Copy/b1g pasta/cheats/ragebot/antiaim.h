#pragma once

#include "..\..\includes.hpp"
#include "..\..\structs.hpp"

class CCSGOPlayerAnimState;

class antiaim : public singleton< antiaim > {
public:
	bool nade_out(CUserCmd * pUserCmd);
	void create_move();
	void jittertime();
	void update_lowerbody_breaker();
	float at_target();
	bool DesyncRotate();
	float DesyncAnimations();
	float GetDesyncDelta();
	float get_max_desync_delta();
	float freestanding();
	float desynctest();
	float freestanding1();
	float get_pitch();
	float get_fake();
	float get_real();
	bool freeze_check = false;
	void FreeStanding();
	void get_unpredicted_velocity();
	player_t * m_e;
	vec3_t unpredicted_velocity;

	bool m_bBreakLowerBody = false;
	float m_flLastLby = 0.0f;
	float m_flNextBodyUpdate = 0.f;
	bool m_bBreakBalance = false;
	float m_flLastMoveTime;
};
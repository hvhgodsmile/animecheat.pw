#include "antiaim.h"
#include "manual_aa.h"
#include "..\sdk\animation_state.h"
#include "..\autowall\autowall.h"



bool antiaim::nade_out(CUserCmd *pUserCmd)
{
	auto m_weapon = g_ctx.m_local->m_hActiveWeapon().Get();

	if (m_weapon->is_grenade())
	{

		if (!m_weapon->m_bPinPulled() || g_ctx.get_command()->m_buttons & IN_ATTACK || g_ctx.get_command()->m_buttons & IN_ATTACK2)
		{

			float fThrowTime = m_weapon->m_fThrowTime();

			if ((fThrowTime > 0) && (fThrowTime < g_csgo.m_globals()->m_curtime))
				return true;
		}

		return false;
	}
	else
		return (g_ctx.get_command()->m_buttons & IN_ATTACK && m_weapon->can_fire());
}

float GetMaxFakeDelta()
{
	auto animstate = g_ctx.m_local->get_animation_state();
	float speedfactor = clamp<float>(animstate->m_flSpeedFraction(), 0.0f, 1.0f);
	float unk1 = ((animstate->m_flLandingRatio() * -0.3f) - 0.2f) * speedfactor;
	float unk2 = unk1 + 1.0f;
	if (animstate->m_fDuckAmount > 0.0f) {
		float max_velocity = clamp<float>(animstate->m_flMaxWeaponVelocity(), 0.0f, 1.0f);
		float duck_speed = animstate->m_fDuckAmount * max_velocity;
		unk2 += (duck_speed * (0.5f - unk2));
	}

	return animstate->yaw_desync_adjustment() * unk2;
}
void antiaim::create_move() {
	manual_aa::get().update();
	auto local_player = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(g_csgo.m_engine()->GetLocalPlayer()));
	auto m_weapon = g_ctx.m_local->m_hActiveWeapon().Get();

	
	if (!m_weapon)
		return;

	if (g_ctx.get_command()->m_buttons & IN_USE)
		return;

	if (g_ctx.m_local->m_bGunGameImmunity() || g_ctx.m_local->m_fFlags() & FL_FROZEN || freeze_check)
		return;

	if (m_weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER) {
		if (g_ctx.get_command()->m_buttons & IN_ATTACK2)
			return;

		if (m_weapon->CanFirePostPone() && (g_ctx.get_command()->m_buttons & IN_ATTACK))
			return;
	}
	else if (m_weapon->is_grenade()) {
		if (m_weapon->is_grenade() && m_weapon->m_fThrowTime() > 0.f)
			return;
	}
	else {
		if (m_weapon->get_csweapon_info()->type == WEAPONTYPE_KNIFE && ((g_ctx.get_command()->m_buttons & IN_ATTACK) || (g_ctx.get_command()->m_buttons & IN_ATTACK2))) {
			return;
		}
		else if ((g_ctx.get_command()->m_buttons & IN_ATTACK) && (m_weapon->m_iItemDefinitionIndex() != WEAPON_C4)) {
			if (m_weapon->can_fire())
				return;
		}
	}

	if (g_ctx.m_local->get_move_type() == MOVETYPE_NOCLIP || g_ctx.m_local->get_move_type() == MOVETYPE_LADDER)
		return;

	if (!g_ctx.m_globals.fakelagging)
		g_ctx.send_packet = g_ctx.get_command()->m_command_number % 2;

	g_ctx.get_command()->m_viewangles.x = get_pitch();

    if (!manual_aa::get().run())
        get_real();
	static bool desync_flip = false;

	static auto get_max_desync_delta_aimware = [&]() {
		auto animstate = g_ctx.m_local->get_animation_state();
		float feet_yaw = animstate->m_flAbsRotation();
		static bool b_switch = false;
		if (g_ctx.send_packet)
			b_switch = !b_switch;

		float feet_delta = math::normalize_yaw(g_ctx.m_globals.real_angles.y - feet_yaw);
		float fake_yaw;
		float desync_delta = GetMaxFakeDelta();

		float delta = clamp<float>(math::normalize_yaw(desync_delta - feet_delta), -desync_delta, desync_delta);
		float negative_delta = clamp<float>(math::normalize_yaw(desync_delta + feet_delta), -desync_delta, desync_delta);

		bool c_switch = fabs(negative_delta) > fabs(delta);

		if (fabs(feet_delta) > 20.f)
			b_switch = !c_switch;

		if (b_switch)
			fake_yaw = g_ctx.m_globals.real_angles.y - negative_delta;
		else
			fake_yaw = g_ctx.m_globals.real_angles.y + delta;

		if (g_ctx.send_packet)
			return get_real();
		else
			return fake_yaw;
	};


	if (g_ctx.send_packet && (g_csgo.m_inputsys()->IsButtonDown(g_cfg.antiaim.DESYNCANTIAIM))) {
		g_ctx.m_globals.real_angles.y = get_real() + 180.0f + 2.0f * (desync_flip ? -get_max_desync_delta_aimware() : get_max_desync_delta_aimware());
		g_ctx.get_command()->m_viewangles.y += 180.0f + (desync_flip ? -get_max_desync_delta_aimware() : get_max_desync_delta_aimware());
		desync_flip = !desync_flip;
	}
	if (!g_ctx.send_packet && g_cfg.antiaim.lby_breaker) {
		if (m_bBreakBalance && g_cfg.antiaim.break_adjustment) {
			bool negative = g_cfg.antiaim.lby_delta < 0;

			if (abs(g_cfg.antiaim.lby_delta) > 120) {
				float new_delta = g_cfg.antiaim.lby_delta / 2;

				g_ctx.get_command()->m_viewangles.y += negative ? -new_delta : new_delta;
			}
			else {
				float new_delta = g_cfg.antiaim.lby_delta - 180;

				g_ctx.get_command()->m_viewangles.y += negative ? -new_delta : new_delta;
			}
		}



		if (m_bBreakLowerBody) {
			g_ctx.get_command()->m_viewangles.y += g_cfg.antiaim.lby_delta;

			if (g_cfg.antiaim.pitch_flick) g_ctx.get_command()->m_viewangles.x = -89;
		}
	}
}

float antiaim::get_max_desync_delta() {

	auto local_player = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(g_csgo.m_engine()->GetLocalPlayer()));
	auto animstate = uintptr_t(local_player->get_animation_state());
	Vector yawfeetdelta = local_player->GetBaseEntity()->GetAbsAngles();
	yawfeetdelta.y = local_player->get_animation_state()->m_flGoalFeetYaw - local_player->get_animation_state()->m_flEyeYaw;
	math::normalize_angles(yawfeetdelta);

	Vector an = local_player->GetAbsAngles();
	an.y = local_player->get_animation_state()->m_flCurrentFeetYaw;
	local_player->set_abs_angles(an);
	float flYaw = an.y;

	float v47 = local_player->get_animation_state()->m_flCurrentFeetYaw;
	float pes = 360; float v146 = 360;
	if (v47 >= -360.0)
	{
		pes = fminf(v47, 360);
		v146 = pes;
	}

	float v155 = fmod(yawfeetdelta.y, 360.0);
	if (flYaw <= v146)
	{
		if (v155 <= -180.0)
			yawfeetdelta.y = v155 + 360, 0;
	}
	else if (v155 >= 180.0)
	{
		yawfeetdelta.y = v155 - 360, 0;
	}


	// g_LocalPlayer->GetPlayerAnimState()->m_flCurrentFeetYaw;


	float rate = 180;
	float duckammount = *(float *)(animstate + 0xA4);
	float speedfraction = max(0, min(*reinterpret_cast<float*>(animstate + 0xF8), 1));
	float speedfactor;
	if (speedfraction >= 0.0)
		speedfactor = max(0, min(1, *reinterpret_cast<float*> (animstate + 0xFC)));
	else
		speedfactor = 0.0f;

	float unk1 = ((*reinterpret_cast<float*> (animstate + 0x11C) * -0.30000001) - 0.19999999) * speedfraction;
	float unk2 = unk1 + 1.f;
	if (duckammount > 0) {

		unk2 += ((duckammount * speedfactor) * (0.5f - unk2));

	}


	int maxrot = local_player->get_animation_state()->m_flGoalFeetYaw  * unk2;
	int invrot = local_player->get_animation_state()->m_flCurrentFeetYaw  * unk2;

	DWORD xmmword_10BF8550;

	if (yawfeetdelta.y <= maxrot)
	{
		if (invrot > yawfeetdelta.y)
			*(float *)(animstate + 0x80) = + flYaw;
	}
	else
	{
		*(float *)(animstate + 0x80) = flYaw;// prevent these
	}
	Vector gfeet2 = local_player->GetBaseEntity()->GetAbsAngles();
	gfeet2.y = fmod(*(float *)(animstate + 0x80), 360);
	Vector gfeet = local_player->GetBaseEntity()->GetAbsAngles();
	gfeet.y = fmod(*(float *)(animstate + 0x80), 360);
	math::clamp_angles(gfeet);

	if (gfeet2.y > 180, 0)
		gfeet.y = gfeet2.y - 360;
	if (gfeet.y < -180.0)
		gfeet.y = gfeet.y + 360;
	//v60 = *(float *)(v3 + 220);
	float feetg = gfeet.y;
	//g_LocalPlayer->GetPlayerAnimState()->m_flCurrentFeetYaw() = gfeet.yaw;


	return feetg;
}


void antiaim::update_lowerbody_breaker()
{
	static float last_real;
	bool no_active = true;
	float bestrotation = 0.f;
	static bool IsInGame = true;

	if (!g_csgo.m_engine()->IsInGame())
	{
		IsInGame = true;
		if (!g_csgo.m_clientstate()->m_nChokedCommands)
			antiaim::get().update_lowerbody_breaker();
	}


	if (!g_ctx.m_local->m_iHealth())
		return;

	float
		server_time = util::server_time();




	m_bBreakLowerBody = false;
	m_bBreakBalance = false;

	if (m_flNextBodyUpdate < server_time)
	{
		m_flNextBodyUpdate = server_time + 1.1;
		m_bBreakLowerBody = true;
	}

	if (m_flNextBodyUpdate - g_csgo.m_globals()->m_interval_per_tick < server_time)
		m_bBreakBalance = true;
	else
	{
		if (!no_active)
			g_ctx.get_command()->m_viewangles.y = RAD2DEG(bestrotation) - 180;

		last_real = g_ctx.get_command()->m_viewangles.y;
	}


	const bool moving_on_ground = g_ctx.m_local->m_vecVelocity().Length2D() > 0.1;

	if (moving_on_ground)
	{
		m_bBreakLowerBody = false;
		m_bBreakBalance = false;
	}

	if (!(g_ctx.m_local->m_fFlags() & FL_ONGROUND))
	{
		m_bBreakLowerBody = false;
		m_bBreakBalance = false;
	}
}

float antiaim::at_target() {
	int cur_tar = 0;
	float last_dist = 999999999999.0f;

	for (int i = 0; i < g_csgo.m_globals()->m_maxclients; i++) {
		auto m_entity = static_cast<entity_t*>(g_csgo.m_entitylist()->GetClientEntity(i));

		if (!m_entity || m_entity == g_ctx.m_local)
			continue;

		if (!m_entity->is_player())
			continue;

		auto m_player = (player_t*)m_entity;
		if (!m_player->IsDormant() && m_player->is_alive() && m_player->m_iTeamNum() != g_ctx.m_local->m_iTeamNum()) {
			float cur_dist = (m_entity->m_vecOrigin() - g_ctx.m_local->m_vecOrigin()).Length();

			if (!cur_tar || cur_dist < last_dist) {
				cur_tar = i;
				last_dist = cur_dist;
			}
		}
	}

	if (cur_tar) {
		auto m_entity = static_cast<player_t*>(g_csgo.m_entitylist()->GetClientEntity(cur_tar));
		if (!m_entity) {
			return g_ctx.get_command()->m_viewangles.y;
		}

		Vector target_angle = math::calculate_angle(g_ctx.m_local->m_vecOrigin(), m_entity->m_vecOrigin());
		return target_angle.y;
	}

	return g_ctx.get_command()->m_viewangles.y;
}


static auto perform_rotation = [=](Vector o_angle, float offset, float rot_deg, float rot_speed, bool center_rotation) {
	Vector ang = o_angle;
	ang += offset - (center_rotation ? rot_deg / 2.0f : rot_deg) + std::fmodf(g_csgo.m_globals()->m_curtime * 222.49223595f * rot_speed, rot_deg);
	return  ang.y;
};
float antiaim::get_pitch() {
	int	index;

	if (!(g_ctx.m_local->m_fFlags() & FL_ONGROUND)) {
		index = g_cfg.antiaim.pitch[1];
	}
	else {
		index = g_cfg.antiaim.pitch[1];

		if (g_ctx.m_local->m_vecVelocity().Length() > 0.1f)
			index = g_cfg.antiaim.pitch[1];
	}

	float pitch = g_ctx.get_command()->m_viewangles.x;

	switch (index) {
	case 1:
		pitch = 89.0f;
		break;
	case 2:
		pitch = -89.0f;
		break;
	case 3: {
		pitch = 89.0f;
		break;
	}
	case 4:
		pitch = 1080.0f;
		break;
	case 5:
		pitch = -179.990005f;
		break;
	case 6:
		pitch = 971.0f;
		break;
	}

	return pitch;
}


float antiaim::get_real() {
	int
		index,
		base_angle,
		offset;

	if (!(g_ctx.m_local->m_fFlags() & FL_ONGROUND)) {
		index = g_cfg.antiaim.yaw[2];
		base_angle = g_cfg.antiaim.base_angle[2];
		offset = g_cfg.antiaim.yaw_offset[2];
	}
	else {
		index = g_cfg.antiaim.yaw[0];
		base_angle = g_cfg.antiaim.base_angle[0];
		offset = g_cfg.antiaim.yaw_offset[0];

		if (g_ctx.m_local->m_vecVelocity().Length() > 0.1f) {
			index = g_cfg.antiaim.yaw[1];
			base_angle = g_cfg.antiaim.base_angle[1];
			offset = g_cfg.antiaim.yaw_offset[1];
		}
	}

	float yaw = g_ctx.get_command()->m_viewangles.y;

	if (index) {
		yaw = 0.0f;

		if (base_angle == 1) {
			vec3_t angles; g_csgo.m_engine()->GetViewAngles(angles);
			yaw = angles.y;
		}

		if (base_angle == 2)
			yaw = at_target();



		switch (index) {
		case 1:
			g_ctx.get_command()->m_viewangles.y += 90.0f;
			break;
		case 2:
			g_ctx.get_command()->m_viewangles.y += 180;
			break;
		case 3:
			g_ctx.get_command()->m_viewangles.y = perform_rotation(Vector(0.0f, yaw, 0.0f), 0, 85.0f, 0.4f, true);
		case 4:
			g_ctx.get_command()->m_viewangles.y += 180 + (math::random_float(0, 50) - 25);
			break;
		case 5: {
			vec3_t angles; g_csgo.m_engine()->GetViewAngles(angles);
			g_ctx.get_command()->m_viewangles.y = angles.y;
			break;
		}
		case 6:
			g_ctx.get_command()->m_viewangles.y = g_ctx.m_local->m_flLowerBodyYawTarget();
			break;
		case 7:
			g_ctx.get_command()->m_viewangles.y = freestanding();
			g_ctx.get_command()->m_viewangles.y = perform_rotation(Vector(0.0f, yaw, 0.0f), 0, 85.0f, 0.4f, true);
			break;
		case 8:
			auto weapon = g_ctx.m_local->m_hActiveWeapon().Get();

			if (g_ctx.get_command()->m_buttons & IN_ATTACK && weapon->can_fire())
				g_ctx.get_command()->m_viewangles.y = freestanding1() + (math::random_float(0, 50) - 25);
			else
				g_ctx.get_command()->m_viewangles.y = freestanding1();
			break;
		}
		g_ctx.get_command()->m_viewangles.y += offset;
	}
	return yaw;
}

void antiaim::get_unpredicted_velocity() {
	if (!g_ctx.m_local)
		return;

	unpredicted_velocity = g_ctx.m_local->m_vecVelocity();
}

float antiaim::freestanding() {
	enum {
		back,
		right,
		left,
		forward,
		jitter,
		right2,
		left2,
	};


	vec3_t view_angles;
	g_csgo.m_engine()->GetViewAngles(view_angles);

	static constexpr int damage_tolerance = 30;

	std::vector< player_t * > enemies;

	auto get_target = [&]() -> player_t * {
		player_t * target = nullptr;
		float best_fov = 360;

		for (int id = 1; id <= g_csgo.m_globals()->m_maxclients; id++) {
			auto e = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(id));

			if (!e->valid(true))
				continue;

			float fov = math::get_fov(view_angles, math::calculate_angle(g_ctx.m_local->get_eye_pos(), e->m_vecOrigin()));

			if (fov < best_fov) {
				target = e;
				best_fov = fov;
			}

			enemies.push_back(e);
		}

		return target;
	};

	player_t * e = get_target();

	if (!e)
		return view_angles.y + 180;

	auto calculate_damage = [&](vec3_t point) -> int {
		int damage = 0;
		for (auto& enemy : enemies)
			damage += autowall::get().calculate_return_info(enemy->get_eye_pos(), point, enemy, g_ctx.m_local, 1).m_damage;

		return damage;
	};

	auto rotate_and_extend = [](vec3_t position, float yaw, float distance) -> vec3_t {
		vec3_t direction;
		math::angle_vectors(vec3_t(0, yaw, 0), direction);

		return position + (direction * distance);
	};

	vec3_t
		head_position = g_ctx.m_local->get_eye_pos(),
		at_target = math::calculate_angle(g_ctx.m_local->m_vecOrigin(), e->m_vecOrigin());

	float angles[8] = {
		at_target.y + 180.f,
		at_target.y - 70.f,
		at_target.y + 70.f,
		at_target.y,
		at_target.y + 180 + (math::random_float(0, 50) - 25),
		at_target.y - 135.f,
		at_target.y + 135.f,
	};

	vec3_t head_positions[7] = {
		rotate_and_extend(head_position, at_target.y + 180.f, 17.f),
		rotate_and_extend(head_position, at_target.y - 70.f, 17.f),
		rotate_and_extend(head_position, at_target.y + 70.f, 17.f),
		rotate_and_extend(head_position, at_target.y, 17.f),
		rotate_and_extend(head_position, at_target.y + 180 + math::random_float(0, 50), 25),
		rotate_and_extend(head_position, at_target.y - 135.f, 17.f),
		rotate_and_extend(head_position, at_target.y + 135.f, 17.f)
	};

	int damages[7] = {
		calculate_damage(head_positions[back]),
		calculate_damage(head_positions[right]),
		calculate_damage(head_positions[left]),
		calculate_damage(head_positions[forward]),
		calculate_damage(head_positions[jitter]),
		calculate_damage(head_positions[right2]),
		calculate_damage(head_positions[left2]),
	};

	if (damages[right] > damage_tolerance && damages[left] > damage_tolerance)
		return angles[back];

	if (at_target.x > 15.f)
		return angles[back];

	if (damages[right] == damages[left]) {
		auto trace_to_end = [](vec3_t start, vec3_t end) -> vec3_t {
			trace_t trace;
			CTraceFilterWorldOnly filter;
			Ray_t ray;

			ray.Init(start, end);
			g_csgo.m_trace()->TraceRay(ray, MASK_ALL, &filter, &trace);

			return trace.endpos;
		};

		vec3_t
			trace_right_endpos = trace_to_end(head_position, head_positions[right]),
			trace_left_endpos = trace_to_end(head_position, head_positions[left]);

		Ray_t ray;
		trace_t trace;
		CTraceFilterWorldOnly filter;

		ray.Init(trace_right_endpos, e->get_eye_pos());
		g_csgo.m_trace()->TraceRay(ray, MASK_ALL, &filter, &trace);
		float distance_1 = (trace.startpos - trace.endpos).Length();

		ray.Init(trace_left_endpos, e->get_eye_pos());
		g_csgo.m_trace()->TraceRay(ray, MASK_ALL, &filter, &trace);
		float distance_2 = (trace.startpos - trace.endpos).Length();

		if (fabs(distance_1 - distance_2) > 15.f)
			return (distance_1 < distance_2) ? angles[right] : angles[left];
		else
			return angles[back];
	}
	else
		return (damages[right] < damages[left]) ? angles[right] : angles[left];
}




float antiaim::freestanding1() {
	enum {
		back,
		right,
		left
	};

	vec3_t view_angles;
	g_csgo.m_engine()->GetViewAngles(view_angles);

	static constexpr int damage_tolerance = 10;

	std::vector< player_t * > enemies;

	auto get_target = [&]() -> player_t * {
		player_t * target = nullptr;
		float best_fov = FLT_MAX;

		for (int id = 1; id <= g_csgo.m_globals()->m_maxclients; id++) {
			auto e = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(id));

			if (!e->valid(true))
				continue;

			float fov = math::get_fov(view_angles, math::calculate_angle(g_ctx.m_local->get_eye_pos(), e->m_vecOrigin()));

			if (fov < best_fov) {
				target = e;
				best_fov = fov;
			}

			enemies.push_back(e);
		}

		return target;
	};

	player_t * e = get_target();

	if (!e)
		return view_angles.y + 180;

	auto calculate_damage = [&](vec3_t point) -> int {
		int damage = 0;
		for (auto& enemy : enemies)
			damage += autowall::get().calculate_return_info(enemy->get_eye_pos(), point, enemy, g_ctx.m_local, 1).m_damage;

		return damage;
	};

	auto rotate_and_extend = [](vec3_t position, float yaw, float distance) -> vec3_t {
		vec3_t direction;
		math::angle_vectors(vec3_t(0, yaw, 0), direction);

		return position + (direction * distance);
	};

	vec3_t
		head_position = g_ctx.m_local->get_eye_pos(),
		at_target = math::calculate_angle(g_ctx.m_local->m_vecOrigin(), e->m_vecOrigin());

	float angles[3] = {
			at_target.y + 180.f + (math::random_float(0, 50) - 25),
			at_target.y - 90.f,
			at_target.y + 90.f
	};

	vec3_t head_positions[3] = {
		rotate_and_extend(head_position, at_target.y + 180.f, 35.f + (math::random_float(0, 50) - 25)),
		rotate_and_extend(head_position, at_target.y - 75.f, 35.f),
		rotate_and_extend(head_position, at_target.y + 75.f, 35.f)
	};

	int damages[3] = {
		calculate_damage(head_positions[back]),
		calculate_damage(head_positions[right]),
		calculate_damage(head_positions[left])
	};

	if (damages[right] > damage_tolerance && damages[left] > damage_tolerance)
		return angles[back];

	if (at_target.x > 15.0f)
		return angles[back];

	if (damages[right] == damages[left]) {
		auto trace_to_end = [](vec3_t start, vec3_t end) -> vec3_t {
			trace_t trace;
			CTraceFilterWorldOnly filter;
			Ray_t ray;

			ray.Init(start, end);
			g_csgo.m_trace()->TraceRay(ray, MASK_ALL, &filter, &trace);

			return trace.endpos;
		};

		vec3_t
			trace_right_endpos = trace_to_end(head_position, head_positions[right]),
			trace_left_endpos = trace_to_end(head_position, head_positions[left]);

		Ray_t ray;
		trace_t trace;
		CTraceFilterWorldOnly filter;

		ray.Init(trace_right_endpos, e->get_eye_pos());
		g_csgo.m_trace()->TraceRay(ray, MASK_ALL, &filter, &trace);
		float distance_1 = (trace.startpos - trace.endpos).Length();

		ray.Init(trace_left_endpos, e->get_eye_pos());
		g_csgo.m_trace()->TraceRay(ray, MASK_ALL, &filter, &trace);
		float distance_2 = (trace.startpos - trace.endpos).Length();

		if (fabs(distance_1 - distance_2) > 15.0f)
			return (distance_1 < distance_2) ? angles[right] : angles[left];
		else
			return angles[back];
	}
	else
		return (damages[right] < damages[left]) ? angles[right] : angles[left];
}







#include "aimbot.h"
#include "..\autowall\autowall.h"
#include "..\sdk\animation_state.h"

#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))
bool aimbot::CockRevolver()
{
	// 0.234375f to cock and shoot, 15 ticks in 64 servers, 30(31?) in 128 

	// THIS DOESNT WORK, WILL WORK ON LATER AGAIN WHEN I FEEL LIKE KILLING MYSELF 

	// DONT USE TIME_TO_TICKS as these values aren't good for it. it's supposed to be 0.2f but that's also wrong 
	constexpr float REVOLVER_COCK_TIME = 0.2421875f;
	const int count_needed = floor(REVOLVER_COCK_TIME / g_csgo.m_globals()->m_interval_per_tick);
	static int cocks_done = 0;

	if (!local_weapon ||
		local_weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER ||
		g_ctx.m_local->m_flNextAttack() > g_csgo.m_globals()->m_curtime ||
		local_weapon->IsReloading())
	{
		if (local_weapon && local_weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER)
			g_ctx.get_command()->m_buttons &= ~IN_ATTACK;
		cocks_done = 0;
		return true;
	}

	if (cocks_done < count_needed)
	{
		g_ctx.get_command()->m_buttons |= IN_ATTACK;
		++cocks_done;
		return false;
	}
	else
	{
		g_ctx.get_command()->m_buttons &= ~IN_ATTACK;
		cocks_done = 0;
		return true;
	}

	// 0.0078125 - 128ticks - 31 - 0.2421875 
	// 0.015625 - 64 ticks - 16 - 0.234375f 

	g_ctx.get_command()->m_buttons |= IN_ATTACK;

	/*
	3 steps:
	1. Come, not time for update, cock and return false;
	2. Come, completely outdated, cock and set time, return false;
	3. Come, time is up, cock and return true;
	Notes:
	Will I not have to account for high ping when I shouldn't send another update?
	Lower framerate than ticks = riperino? gotta check if lower then account by sending earlier | frametime memes
	*/

	float curtime = TICKS_TO_TIME(g_ctx.m_local->m_nTickBase());
	static float next_shoot_time = 0.f;

	bool ret = false;

	if (next_shoot_time - curtime < -0.5)
		next_shoot_time = curtime + 0.2f - g_csgo.m_globals()->m_interval_per_tick; // -1 because we already cocked THIS tick ??? 

	if (next_shoot_time - curtime - g_csgo.m_globals()->m_interval_per_tick <= 0.f) {
		next_shoot_time = curtime + 0.2f;
		ret = true;

		// should still go for one more tick but if we do, we're gonna shoot sooo idk how2do rn, its late 
		// the aimbot should decide whether to shoot or not yeh 
	}

	return ret;
}
void aimbot::create_move() {
	g_csgo.m_engine()->GetViewAngles(this->engine_angles);
	this->local_weapon = g_ctx.m_local->m_hActiveWeapon().Get();

	auto weapon_recoil_scale = g_csgo.m_cvar()->FindVar("weapon_recoil_scale");
	this->recoil_scale = weapon_recoil_scale->GetFloat();

	if (!local_weapon)
		return;

	update_config();

	if ((g_ctx.m_local->m_flNextAttack() > util::server_time()))
		return;

	if (local_weapon->is_non_aim() || local_weapon->m_iClip1() < 1)
		return;
	if (g_cfg.ragebot.auto_revolver)
	{
		if (!this->CockRevolver())
			return;
	}


	if (!local_weapon->can_fire())
		return;

	iterate_players();
	aim();
}

void aimbot::iterate_players() {
	target_data.reset();
	static weapon_t * old_weapon;
	if (local_weapon != old_weapon) {
		old_weapon = local_weapon;
		g_ctx.get_command()->m_buttons &= ~IN_ATTACK; return;
	}
	for (int i = 1; i <= g_csgo.m_globals()->m_maxclients; i++) {
		auto e = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(i));
		if (!e->valid(true)) continue;
		matrix3x4_t bones[128];
		if (!e->SetupBones(bones, 128, 0x100, 0.f)) continue;
		float simulation_time = e->m_flSimulationTime();
		vec3_t location = best_point(e, bones);
		bool backtrack = false;
		tickrecord_t lag_record;
		if (location == vec3_t(0, 0, 0) && g_cfg.ragebot.lagcomp) {
			auto & player = lagcompensation::get().players[i];
			if (player.m_e && !player.get_valid_track().empty()) {
				//				CBacktracking::get().ShotBackTrackAimbotStart(e);//these 3 lines maybe fuck up the toes
				//				CBacktracking::get().RestoreTemporaryRecord(e);
				//				CBacktracking::get().ShotBackTrackedTick(e);
				bt_point_return_t return_info = best_point_backtrack(e, player);
				location = return_info.point;
				lag_record = return_info.record;
				simulation_time = lag_record.m_simulation_time;
				backtrack = true;
			}
		}
		if (location == vec3_t(0, 0, 0)) continue;
		float fov = math::get_fov(engine_angles, math::calculate_angle(g_ctx.m_local->get_eye_pos(), location));
		if (fov > g_cfg.ragebot.field_of_view) continue;
		get_target(e, fov, location, simulation_time, backtrack, lag_record);
	}
}
void aimbot::zeus_run()
{
	Vector aim_pos;
	auto e = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(target_data.tid));
	if (!g_ctx.m_local->m_hActiveWeapon()->m_iItemDefinitionIndex() == (short)ItemDefinitionIndex::WEAPON_TASER)
		return;;

	Vector point = e->hitbox_position(2);
	if (util::visible(g_ctx.m_local->get_eye_pos(), point, e, g_ctx.m_local)) {
		if ((g_ctx.m_local->m_vecOrigin() - e->m_vecOrigin()).Length() <= 180) {
			aim_pos = math::calculate_angle(g_ctx.m_local->get_eye_pos(), point);
			aim_pos.Clamp();
			g_ctx.get_command()->m_buttons |= IN_ATTACK;
		}
	}
	if (g_ctx.get_command()->m_buttons & IN_ATTACK)
		g_ctx.get_command()->m_viewangles = aim_pos;
}

bool aimbot::can_zeus(Vector &bestpoint)
{
	auto e = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(target_data.tid));
	Vector forward;
	math::angle_vectors(bestpoint, forward);
	forward *= 183;

	Vector eye_pos = g_ctx.m_local->get_eye_pos();

	Ray_t ray;
	trace_t trace;
	ray.Init(eye_pos, eye_pos + forward);

	CTraceFilter filter;
	filter.pSkip = g_ctx.m_local;

	g_csgo.m_trace()->TraceRay(ray, MASK_SHOT, &filter, &trace);

	return trace.hit_entity == e;
}

void aimbot::aim() {
	if (target_data.tid == -1)
		return;

	auto e = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(target_data.tid));

	if (e->m_bGunGameImmunity())
		return;

	vec3_t next_angle = math::calculate_angle(g_ctx.m_local->get_eye_pos(), target_data.aimspot);

	bool hitchanced = false;
	bool hitchanced2 = false;

	if (config.hitchance)
	{
		hitchanced = hitchance(next_angle, e, config.hitchance_amount);
		hitchanced2 = hitchance(next_angle, e, 62);
	}
	else
	{
		hitchanced = true;
		hitchanced2 = true;
	}

	if (hitchanced2) {
		if (can_zeus(next_angle))
			zeus_run();
	}

	if (hitchanced) {

		g_ctx.get_command()->m_viewangles = next_angle;

		if (g_cfg.ragebot.anti_recoil)
			g_ctx.get_command()->m_viewangles -= g_ctx.m_local->m_aimPunchAngle() * recoil_scale;

		if (!g_cfg.ragebot.silent_aim)
			g_csgo.m_engine()->SetViewAngles(next_angle);

		if (config.autoshoot) {
			if (hitchanced)
			{
				if ((local_weapon->m_iItemDefinitionIndex() == WEAPON_SCAR20 ||
					local_weapon->m_iItemDefinitionIndex() == WEAPON_SSG08 ||
					local_weapon->m_iItemDefinitionIndex() == WEAPON_G3SG1 ||
					local_weapon->m_iItemDefinitionIndex() == WEAPON_AUG ||
					local_weapon->m_iItemDefinitionIndex() == WEAPON_AWP) &&
					!g_ctx.m_local->m_bIsScoped() && g_cfg.ragebot.autoscope)
					g_ctx.get_command()->m_buttons |= IN_ATTACK2;

				g_ctx.get_command()->m_buttons |= IN_ATTACK;
				g_ctx.m_globals.ShotFired[target_data.tid] = true;
				g_ctx.m_globals.shots[target_data.tid] += 1;
			}
			if (g_cfg.misc.anti_untrusted)
				g_ctx.send_packet = false;
		}

		if (g_ctx.get_command()->m_buttons & IN_ATTACK)
			g_ctx.get_command()->m_tickcount = TIME_TO_TICKS(target_data.simtime + util::lerp_time());

		g_ctx.m_globals.aimbotting = true;
	}

	if (config.quickstop && g_ctx.m_local->m_fFlags() & FL_ONGROUND)
		quickstop();



	if (config.quickstop && g_ctx.m_local->m_vecVelocity().Length2D() >= (g_ctx.m_local->m_hActiveWeapon()->get_csweapon_info()->max_speed_alt * .34f) - 5 && !GetAsyncKeyState(VK_SPACE))
		quickstop2();

	if (config.autoscope && local_weapon->m_zoomLevel() == 0) {
		if (g_ctx.get_command()->m_buttons & IN_ATTACK)
			g_ctx.get_command()->m_buttons &= ~IN_ATTACK;

		g_ctx.get_command()->m_buttons |= IN_ATTACK2;
	}
}

vec3_t aimbot::best_point(player_t * e, matrix3x4_t bones[128]) {
	vec3_t output = vec3_t(0, 0, 0);

	std::vector< int >
		hitboxes = hitboxes_from_vector(e, e->m_fFlags(), e->m_vecVelocity(), config.hitscan),
		multipoints = hitboxes_from_vector(e, e->m_fFlags(), e->m_vecVelocity(), config.multipoint);

	float best_damage = 0.f;

	for (auto current : hitboxes) {
		vec3_t location = e->hitbox_position(current, bones);

		if (config.autowall) {
			float damage = autowall::get().calculate_return_info(g_ctx.m_local->get_eye_pos(), location, g_ctx.m_local, e).m_damage;

			if ((damage > best_damage) && (damage > config.minimum_damage)) {
				best_damage = damage;
				output = location;
			}
		}
		else if (!config.autowall && g_ctx.m_local->point_visible(location))
			return output;
	}

	for (auto current : multipoints) {
		std::vector< vec3_t > points;  getpoints(e, current, bones, points);

		for (auto point : points) {
			if (config.autowall) {
				float damage = autowall::get().calculate_return_info(g_ctx.m_local->get_eye_pos(), point, g_ctx.m_local, e).m_damage;

				if ((damage > best_damage) && (damage > config.minimum_damage)) {
					best_damage = damage;
					output = point;
				}
			}
			else if (!config.autowall && g_ctx.m_local->point_visible(point))
				return point;
		}
	}

	return output;
}

aimbot::bt_point_return_t aimbot::best_point_backtrack(player_t * e, player_record_t & player) {
	auto hitgroup_from_hitbox = [](int hitbox) -> int {
		switch (hitbox) {
		case HITBOX_HEAD:
		case HITBOX_NECK:
			return HITGROUP_HEAD;
			break;
		case HITBOX_PELVIS:
		case HITBOX_STOMACH:
			return HITGROUP_STOMACH;
			break;
		case HITBOX_LOWER_CHEST:
		case HITBOX_CHEST:
		case HITBOX_UPPER_CHEST:
			return HITGROUP_CHEST;
			break;
		case HITBOX_RIGHT_THIGH:
		case HITBOX_RIGHT_CALF:
		case HITBOX_RIGHT_FOOT:
			return HITGROUP_RIGHTLEG;
			break;
		case HITBOX_LEFT_THIGH:
		case HITBOX_LEFT_CALF:
		case HITBOX_LEFT_FOOT:
			return HITGROUP_LEFTLEG;
			break;
		case HITBOX_RIGHT_HAND:
		case HITBOX_RIGHT_UPPER_ARM:
		case HITBOX_RIGHT_FOREARM:
			return HITGROUP_RIGHTARM;
			break;
		case HITBOX_LEFT_UPPER_ARM:
		case HITBOX_LEFT_FOREARM:
		case HITBOX_LEFT_HAND:
			return HITGROUP_LEFTARM;
			break;
		}
	};

	std::deque< tickrecord_t > track = player.get_valid_track();

	tickrecord_t best_record;
	float best_damage = 1;
	vec3_t best_position;

	for (int i = 0; i < track.size(); i++) {
		tickrecord_t record = track[i];

		std::vector< int > hitboxes = hitboxes_from_vector(e, record.m_flags, record.m_velocity, config.hitscan_history);

		for (auto hitbox : hitboxes) {
			vec3_t location = e->hitbox_position(hitbox, record.m_bone_matrix);

			if (config.autowall) {
				float damage = autowall::get().calculate_return_info(g_ctx.m_local->get_eye_pos(), location, g_ctx.m_local, e, hitgroup_from_hitbox(hitbox)).m_damage;

				if (damage > best_damage) {
					best_position = location;
					best_damage = damage;
					best_record = record;
				}
			}
			else if (!config.autowall && g_ctx.m_local->point_visible(location)) {
				float damage = 50; autowall::get().scale_damage(e, local_weapon->get_csweapon_info(), hitgroup_from_hitbox(hitbox), damage);

				if (damage > best_damage) {
					best_position = location;
					best_damage = damage;
					best_record = record;
				}
			}
		}
	}

	return bt_point_return_t{ best_record, best_position };
}
void aimbot::getpoints(player_t * e, int hitbox_id, matrix3x4_t bones[128], std::vector< vec3_t > & points) {
	studiohdr_t * studio_model = g_csgo.m_modelinfo()->GetStudiomodel(e->GetModel());
	mstudiohitboxset_t * set = studio_model->pHitboxSet(e->m_nHitboxSet());

	mstudiobbox_t * hitbox = set->pHitbox(hitbox_id);
	if (!hitbox)
		return;

	vec3_t min, max;
	math::vector_transform(hitbox->bbmin, bones[hitbox->bone], min);
	math::vector_transform(hitbox->bbmax, bones[hitbox->bone], max);

	vec3_t
		center = (min + max) * 0.5f,
		angle = math::calculate_angle(center, g_ctx.m_local->get_eye_pos());

	vec3_t forward;
	math::angle_vectors(angle, forward);

	vec3_t
		right = forward.Cross(vec3_t(0, 0, 1)),
		left = vec3_t(-right.x, -right.y, right.z),
		top = vec3_t(0, 0, 1),
		bot = vec3_t(0, 0, -1);

	if (hitbox_id == HITBOX_HEAD) {
		points.push_back(center + (top * (hitbox->m_flRadius * config.pointscale)));
		points.push_back(center + (right * (hitbox->m_flRadius * config.pointscale)));
		points.push_back(center + (left * (hitbox->m_flRadius * config.pointscale)));
	}
	else {
		points.push_back(center + (right * (hitbox->m_flRadius * config.pointscale)));
		points.push_back(center + (left * (hitbox->m_flRadius * config.pointscale)));
	}
}
void aimbot::quickstop() {
	if (!g_cfg.ragebot.quickstoptype == 0)
		return;

	g_ctx.get_command()->m_sidemove = 0;
	g_ctx.get_command()->m_forwardmove = g_ctx.m_local->m_vecVelocity().Length2D() > 20.f ? 450.f : 0.f;

	math::rotate_movement(math::calculate_angle(vec3_t(0, 0, 0), g_ctx.m_local->m_vecVelocity()).y + 180.f);
}
void AngleVectors(const Vector &angles, Vector *forward)
{
	Assert(s_bMathlibInitialized);
	Assert(forward);

	float	sp, sy, cp, cy;

	sy = sin(DEG2RAD(angles[1]));
	cy = cos(DEG2RAD(angles[1]));

	sp = sin(DEG2RAD(angles[0]));
	cp = cos(DEG2RAD(angles[0]));

	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
}
void VectorAngles(const Vector &vecForward, Vector &vecAngles)
{
	Vector vecView;
	if (vecForward[1] == 0.f && vecForward[0] == 0.f)
	{
		vecView[0] = 0.f;
		vecView[1] = 0.f;
	}
	else
	{
		vecView[1] = atan2(vecForward[1], vecForward[0]) * 180.f / 3.14159265358979323846f;

		if (vecView[1] < 0.f)
			vecView[1] += 360.f;

		vecView[2] = sqrt(vecForward[0] * vecForward[0] + vecForward[1] * vecForward[1]);

		vecView[0] = atan2(vecForward[2], vecView[2]) * 180.f / 3.14159265358979323846f;
	}

	vecAngles[0] = -vecView[0];
	vecAngles[1] = vecView[1];
	vecAngles[2] = 0.f;
}
void aimbot::quickstop2() {
	if (!g_cfg.ragebot.quickstoptype == 1)
		return;

	vec3_t Velocity = g_ctx.m_local->m_vecVelocity();
	static float Speed = 450.f;

	vec3_t Direction;
	vec3_t RealView;
	VectorAngles(Velocity, Direction);
	g_csgo.m_engine()->GetViewAngles(RealView);
	Direction.y = RealView.y - Direction.y;

	vec3_t Forward;
	AngleVectors(Direction, &Forward);
	vec3_t NegativeDirection = Forward * -Speed;

	g_ctx.get_command()->m_forwardmove = NegativeDirection.x;
	g_ctx.get_command()->m_sidemove = NegativeDirection.y;
}

bool aimbot::hitchance(vec3_t angles, player_t * e, float chance) {
	angles -= g_ctx.m_local->m_aimPunchAngle() * recoil_scale;
	vec3_t
		forward,
		right,
		up,
		src = g_ctx.m_local->get_eye_pos();
	math::angle_vectors(angles, &forward, &right, &up);
	int
		hits = 0,
		needed_hits = static_cast<int>(256.f * (chance / 100.f));
	float
		weap_spread = local_weapon->get_spread(),
		weap_inaccuracy = local_weapon->get_innacuracy();
	for (int i = 0; i < 256; i++) {
		float
			a = math::random_float(0.f, 1.f),
			b = math::random_float(0.f, 2.f * DirectX::XM_PI),
			c = math::random_float(0.f, 1.f),
			d = math::random_float(0.f, 2.f * DirectX::XM_PI),
			inaccuracy = a * weap_inaccuracy,
			spread = c * weap_spread;
		if (local_weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER) {
			if (g_ctx.get_command()->m_buttons & IN_ATTACK2) {
				a = 1.f - a * a;
				c = 1.f - c * c;
			}
		}
		vec3_t
			spread_view((cos(b) * inaccuracy) + (cos(d) * spread), (sin(b) * inaccuracy) + (sin(d) * spread), 0),
			direction;
		direction.x = forward.x + (spread_view.x * right.x) + (spread_view.y * up.x);
		direction.y = forward.y + (spread_view.x * right.y) + (spread_view.y * up.y);
		direction.z = forward.z + (spread_view.x * right.z) + (spread_view.y * up.z);
		direction.Normalized();
		vec3_t
			viewangles_spread,
			view_forward;
		math::vector_angles(direction, up, viewangles_spread);
		math::normalize_angles(viewangles_spread);
		math::angle_vectors(viewangles_spread, view_forward);
		view_forward.NormalizeInPlace();
		view_forward = src + (view_forward * local_weapon->get_csweapon_info()->range);
		trace_t tr;
		Ray_t ray;
		ray.Init(src, view_forward);
		g_csgo.m_trace()->ClipRayToEntity(ray, MASK_SHOT | CONTENTS_GRATE, e, &tr);
		if (tr.hit_entity == e) hits++;
		if (static_cast<int>((static_cast<float>(hits) / 256.f) * 100.f) >= chance) return true;
		if ((256 - i + hits) < needed_hits) return false;
	}
	return false;
}

void aimbot::get_target(player_t * e, float fov, vec3_t location, float simulation_time, bool backtrack, tickrecord_t lag_record) {
	switch (g_cfg.ragebot.selection_type) {
	case 0:
		if (target_data.best_distance < fov) return;
		target_data.best_distance = fov;
		target_data.tid = e->EntIndex();
		target_data.aimspot = location;
		target_data.simtime = simulation_time;
		target_data.backtrack = backtrack;
		target_data.best_record = lag_record;
		break;
	case 1:
		if (target_data.best_distance < e->m_iHealth()) return;
		target_data.best_distance = e->m_iHealth();
		target_data.tid = e->EntIndex();
		target_data.aimspot = location;
		target_data.simtime = simulation_time;
		target_data.backtrack = backtrack;
		target_data.best_record = lag_record;
		break;
	case 2:
		float distance = sqrt(
			pow(double(location.x - g_ctx.m_local->m_vecOrigin().x), 2) +
			pow(double(location.y - g_ctx.m_local->m_vecOrigin().y), 2) +
			pow(double(location.z - g_ctx.m_local->m_vecOrigin().z), 2));
		if (target_data.best_distance < distance) return;
		target_data.best_distance = distance;
		target_data.tid = e->EntIndex();
		target_data.aimspot = location;
		target_data.simtime = simulation_time;
		target_data.backtrack = backtrack;
		target_data.best_record = lag_record;
		break;
	}
}
bool IsMovingOnInAir(player_t* pEntity) {
	//Check if player has a velocity greater than 0 (moving) and if they are onground.
	return !(pEntity->m_fFlags() & FL_ONGROUND);
}
bool aimbot::is_slow_walking(player_t* entity) {
	/*
	*Measures:
	*Slowest -> 65
	*Highest -> 100
	*im an autist and like to walk around in cyrkle
	*/
	if (IsMovingOnInAir(entity))
		return false;
	float large = 0;
	float velocity_2D[64], old_velocity_2D[64];
	if (entity->m_vecVelocity().Length2D() != velocity_2D[entity->EntIndex()] && entity->m_vecVelocity().Length2D() != NULL) {
		old_velocity_2D[entity->EntIndex()] = velocity_2D[entity->EntIndex()];
		velocity_2D[entity->EntIndex()] = entity->m_vecVelocity().Length2D();
	}
	if (large == 0)return false;
	Vector velocity = entity->m_vecVelocity();
	Vector direction = entity->m_angEyeAngles();

	float speed = velocity.Length();
	direction.y = entity->m_angEyeAngles().y - direction.y;
	//method 1
	if (velocity_2D[entity->EntIndex()] > 1) {
		int tick_counter[64];
		if (velocity_2D[entity->EntIndex()] == old_velocity_2D[entity->EntIndex()])
			tick_counter[entity->EntIndex()] += 1;
		else
			tick_counter[entity->EntIndex()] = 0;

		while (tick_counter[entity->EntIndex()] > (1 / g_csgo.m_globals()->m_interval_per_tick) * fabsf(0.1f))//should give use 100ms in ticks if their speed stays the same for that long they are definetely up to something..
			return true;
	}
	//method 2
	//removed
	//method 3  / resource heavy but does most of the work
	//removed
	//method 4
	if (velocity.Length2D() == entity->m_flMaxspeed() / 3)//ima keep that in because its basically public
		return true;
	//method 5
	if (speed < entity->m_flMaxspeed() / 3 && velocity.Length2D() != 0)//same here
		return true;

	return false;
}
bool aimbot::head_check(player_t * e, int m_fFlags, vec3_t m_vecVelocity) {
	bool use = true;
	Vector local_position = g_ctx.m_local->m_vecOrigin() + g_ctx.m_local->m_vecViewOffset();
	if (config.baim[BAIM_INAIR].enabled && !(m_fFlags & FL_ONGROUND))
		use = false;
	if (g_cfg.ragebot.antiaim_correction || config.baim[BAIM_NOTRESOLVED].enabled) {
		auto & player = lagcompensation::get().players[e->EntIndex()];

		if (!player.m_e || !player.m_resolver)
			return use;

		if (player.m_resolver->m_mode == anti_freestand)
			use = false;
	}
	if (config.baim[BAIM_CANKILL].enabled)
	{
		float damage = g_ctx.m_local->m_hActiveWeapon()->get_csweapon_info()->damage / 1.2;//just to be safe
		if (damage > e->m_iHealth())
			use = false;
	}
	if (g_ctx.m_globals.isFakeing) use = false;
	if (config.baim[BAIM_SLOWWALK].enabled && is_slow_walking(e))
	{
		use = false;
	}

	if (g_csgo.m_inputsys()->IsButtonDown(g_cfg.ragebot.baim_key))
		use = false;
	if (config.baim[BAIM_ALWAYS].enabled)
	{
		use = false;
	}
	if (g_ctx.m_local->m_hActiveWeapon()->m_iItemDefinitionIndex() == (short)ItemDefinitionIndex::WEAPON_TASER) use = false;
	return use;
}

std::vector< int > aimbot::hitboxes_from_vector(player_t * e, int m_fFlags, vec3_t m_vecVelocity, std::vector< MultiDropdownItem_t > arr) {
	std::vector< int > hitboxes;
	bool head_valid = head_check(e, m_fFlags, m_vecVelocity);
	//õèò áîêñû êàê â ôàòàëèòè by -muff ûûûûûûûûûûûûûûûûûûûûûûûûûûûû
	if (arr[0].enabled && head_valid)
	{
		hitboxes.push_back(HITBOX_HEAD);
	}
	if (arr[1].enabled)
	{
		hitboxes.push_back(HITBOX_NECK);
	}
	if (arr[2].enabled)
	{
		hitboxes.push_back(HITBOX_UPPER_CHEST);
	}
	if (arr[3].enabled)
	{
		hitboxes.push_back(HITBOX_CHEST);
	}
	if (arr[4].enabled)
	{
		hitboxes.push_back(HITBOX_PELVIS);
	}
	if (arr[5].enabled)
	{
		hitboxes.push_back(HITBOX_STOMACH);
	}
	if (arr[6].enabled)
	{
		hitboxes.push_back(HITBOX_RIGHT_UPPER_ARM);
		hitboxes.push_back(HITBOX_LEFT_UPPER_ARM);
	}
	if (arr[7].enabled)
	{
		hitboxes.push_back(HITBOX_RIGHT_FOREARM);
		hitboxes.push_back(HITBOX_LEFT_FOREARM);
	}
	if (arr[8].enabled)
	{
		hitboxes.push_back(HITBOX_RIGHT_HAND);
		hitboxes.push_back(HITBOX_LEFT_HAND);
	}
	if (arr[9].enabled)
	{
		hitboxes.push_back(HITBOX_RIGHT_THIGH);
		hitboxes.push_back(HITBOX_LEFT_THIGH);
	}
	if (arr[10].enabled)
	{
		hitboxes.push_back(HITBOX_RIGHT_CALF);
		hitboxes.push_back(HITBOX_LEFT_CALF);
		hitboxes.push_back(HITBOX_RIGHT_FOOT);
		hitboxes.push_back(HITBOX_LEFT_FOOT);
	}
	return hitboxes;
}

void aimbot::update_config() {
	auto id = local_weapon->m_iItemDefinitionIndex();

	int element = -1;

	if (local_weapon->is_pistol() && id != WEAPON_DEAGLE)
		element = 0;

	if (local_weapon->m_iItemDefinitionIndex() == WEAPON_DEAGLE)
		element = 1;

	if (local_weapon->is_smg())
		element = 3;

	if (id == WEAPON_AWP || id == WEAPON_SSG08)
		element = 4;

	if (id == WEAPON_SCAR20 || id == WEAPON_G3SG1)
		element = 5;

	if (local_weapon->is_shotgun())
		element = 6;

	if (element == -1)
		element = 2;

	config.autoshoot = g_cfg.ragebot.autoshoot[element];
	config.autowall = g_cfg.ragebot.autowall[element];
	config.minimum_damage = g_cfg.ragebot.minimum_damage[element];
	config.hitscan = g_cfg.ragebot.hitscan[element];
	config.hitscan_history = g_cfg.ragebot.hitscan_history[element];
	config.multipoint = g_cfg.ragebot.multipoint_hitboxes[element]; // fps issue
	config.hitchance = g_cfg.ragebot.hitchance[element];
	config.hitchance_amount = g_cfg.ragebot.hitchance_amount[element];
	config.baim = g_cfg.ragebot.baim_settings[element];
	config.quickstop = g_cfg.ragebot.quickstop[element];
	config.baim_health = g_cfg.ragebot.baim_health[element];
	config.autoscope = false;

	if (element == 4 || element == 5) {
		config.autoscope = g_cfg.ragebot.autoscope[element - 3];
	}
	else if (element == 2 && (id == WEAPON_AUG || id == WEAPON_SG556)) {
		config.autoscope = g_cfg.ragebot.autoscope[0];
	}
}










































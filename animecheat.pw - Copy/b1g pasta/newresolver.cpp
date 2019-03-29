#include "../recode scatecheat3/cheats/autowall/autowall.h"
template<class T, class U>
T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}
#include "../recode scatecheat3/resolver.h"
bool has_been_initialized = false;

void c_resolver::execute() {
	if (!g_cfg.ragebot.antiaim_correction || !g_ctx.m_local->is_alive())	return;
	static bool first_execute[65];
	for (int i = 1; i <= 64; i++) {
		player_t* entity = reinterpret_cast<player_t*>(g_csgo.m_entitylist()->GetClientEntity(i));

		if (entity == g_ctx.m_local) {
			animation_fix(entity);
			continue;
		}

		if (!entity->valid(true, true))
			continue;

		if (!has_been_initialized) {
			if (i = 64)
				has_been_initialized = true;
			first_execute[i] = true;
		}
		update_record(current_record[i], entity);

		if (is_slow_walking(current_record[i], last_record[i].back(), (g_csgo.m_globals()->m_curtime - entity->m_flSimulationTime()) + g_csgo.m_globals()->m_interval_per_tick))
			compensate_for_slow_walk(entity, current_record[i].velocity);

		find_viable_record(current_record[i], last_record[i].back());
		animation_fix(entity);

		if ((current_record[i].simtime - last_record[i].back().simtime) == g_csgo.m_globals()->m_interval_per_tick)
			return;

		resolve(current_record[i], last_record[i].back(), entity);
	}
}
void c_resolver::find_viable_record(c_resolver_info record1, c_resolver_info record2) {
	record1 = record1;
}
void c_resolver::compensate_for_slow_walk(player_t* player, vec3_t velocity) {
	vec3_t origin = player->m_vecOrigin();
	player->invalidate_bone_cache();
	player->m_vecOrigin() = player->m_vecOrigin() + (player->get_choked_ticks() * (velocity * g_csgo.m_globals()->m_interval_per_tick));
	animation_fix(player);
	player->invalidate_bone_cache();
	player->m_vecOrigin() = player->m_vecOrigin();
}
void c_resolver::animation_fix(player_t* entity) {
	c_baseplayeranimationstate* state = entity->get_animation_state();

	float curtime = g_csgo.m_globals()->m_curtime;
	float frametime = g_csgo.m_globals()->m_frametime;
	float tick_interval = g_csgo.m_globals()->m_interval_per_tick;
	float host_timescale = g_csgo.m_cvar()->FindVar("host_timescale")->GetFloat();

	g_csgo.m_globals()->m_curtime = entity->m_flSimulationTime();
	g_csgo.m_globals()->m_frametime = host_timescale * tick_interval;

	AnimationLayer backup_layers[15];
	std::memcpy(backup_layers, entity->get_animlayers(), sizeof(AnimationLayer) * entity->animlayer_count());

	if (state)
		state->m_iLastClientSideAnimationUpdateFramecount = g_csgo.m_globals()->m_framecount - 1;

	entity->m_bClientSideAnimation() = true;
	entity->update_clientside_animation();
	entity->m_bClientSideAnimation() = false;

	g_csgo.m_globals()->m_curtime = curtime;
	g_csgo.m_globals()->m_frametime = frametime;

	std::memcpy(entity->get_animlayers(), backup_layers, sizeof(AnimationLayer) * entity->animlayer_count());

	matrix3x4_t matrix[128];
	auto backup = *(byte*)(uintptr_t(state) + ptrdiff_t(0x270));

	*(byte*)(uintptr_t(state) + ptrdiff_t(0x270)) = 0;
	entity->SetupBones(matrix, 128, 0x7FF00, curtime);
	*(byte*)(uintptr_t(state) + ptrdiff_t(0x270)) = backup;
}
void c_resolver::update_record(c_resolver_info record, player_t* entity) {
	record.simtime = entity->m_flSimulationTime();

	record.eye_positions.insert(record.eye_positions.begin(), entity->get_eye_pos());
	if (record.eye_positions.size() > 128)
		record.eye_positions.pop_back(); // max the vector size so it doesn't store too many records.
	memcpy(record.poses, entity->m_flPoseParameter(), sizeof(float) * 24);
	record.m_angles = entity->m_angEyeAngles();
	record.velocity = entity->m_vecVelocity();

	record.velocity_direction = math::calculate_angle(entity->get_eye_pos(), entity->get_eye_pos() + (record.velocity * g_csgo.m_globals()->m_interval_per_tick));
	record.source = local_pos;

	for (int j = 0; j < entity->animlayer_count(); j++) {
		AnimationLayer animlayer = entity->get_animlayers()[1];
		if (entity->sequence_activity(animlayer.m_nSequence) == ACT_CSGO_IDLE_TURN_BALANCEADJUST) {
			record.balance_adjust = true;
			record.last_balance_adjust_time = g_csgo.m_globals()->m_curtime;
		}
		if (record.last_balance_adjust_time - g_csgo.m_globals()->m_curtime < 0.5)
			record.balance_adjust_playing = true;
		else
			record.balance_adjust_playing = false;
	}

	last_record[entity->EntIndex()].insert(last_record[entity->EntIndex()].begin(), record);
}
void c_resolver::resolve(c_resolver_info record1, c_resolver_info record2, player_t* player) {
	if (can_autowall(player, record1.source, 65)) {
		if (can_extrapolate(can_autowall(player, record1.source, 65), player, 10, 65)) {
			vec3_t src = extrapolate_direction(record1.source, record1.eye_positions.back(), player);
			vec3_t dst = record1.eye_positions.back();

			float radius = fabs(vec3_t(player->bone_pos(8) - dst).Length2D());
			float highest = 0.f, current = 0.f, yaw = 0.f;

			for (int i = 0; i <= 360; i++) {

				Ray_t ray;
				trace_t tr;
				CTraceFilterSkipTwoEntities filter(g_ctx.m_local, player);

				vec3_t view;
				view.x = radius * cos((M_PI / 180) * i) + dst.x;
				view.y = radius * sin((M_PI / 180) * i) + dst.y;
				view.z = dst.z;

				ray.Init(src, view);
				g_csgo.m_trace()->TraceRay(ray, MASK_SHOT, &filter, &tr);

				ray.Init(src - (src * src.Dot(tr.plane.normal)), view);
				g_csgo.m_trace()->TraceRay(ray, MASK_SHOT, &filter, &tr);

				vec3_t end = tr.endpos;

				vec3_t end1, end2;

				ray.Init(view, end);
				g_csgo.m_trace()->TraceRay(ray, MASK_SHOT, &filter, &tr);
				end1 = tr.endpos;

				ray.Init(end, view);
				g_csgo.m_trace()->TraceRay(ray, MASK_SHOT, &filter, &tr);
				end2 = tr.endpos;

				current = end1.DistTo(end2);
				if (current > highest) {
					highest = current;
					yaw = i;
				}
			}

			record1.m_angles.y = yaw;
			float pitch = record1.m_angles.x;

			if (pitch >= 90) {
				pitch -= (pitch - 90);
			}
			else if (pitch <= -90) {
				pitch += (pitch + 90);
			}

			record1.m_angles.x = pitch;
			player->m_angEyeAngles() = record1.m_angles;
		}
	}
	else
	{
		float body_yaw, lean_yaw;
		body_yaw = player->m_flPoseParameter1();
		lean_yaw = player->m_flPoseParameter1();

		remap_value(body_yaw, 0, 1, -60, 60);
		remap_value(lean_yaw, 0, 1, -180, 180);

		float pitch = record1.m_angles.x;

		if (pitch >= 90) {
			pitch -= (pitch - 90);
		}
		else if (pitch <= -90) {
			pitch += (pitch + 90);
		}

		record1.m_angles.x = pitch;
		record1.m_angles.y = clamp(lean_yaw + body_yaw, -180, 180);
	}
}
vec3_t c_resolver::extrapolate_direction(vec3_t src, vec3_t dst, player_t* entity) {

	player_t* local = g_ctx.m_local;
	float radius = fabs(vec3_t(local->bone_pos(8) - src).Length2D());

	if (!can_extrapolate(can_autowall(entity, src, 65), entity, 10, 65))
		return 0;

	float highest = 0.f;
	float current = 0.f;

	vec3_t direction;
	direction.Init();

	for (int i = 0; i <= 360; i++) {
		vec3_t view;
		view.x = radius * cos((M_PI / 180) * i) + src.x;
		view.y = radius * sin((M_PI / 180) * i) + src.y;
		view.z = src.z;

		Ray_t ray;
		trace_t tr;
		CTraceFilterSkipTwoEntities filter(local, entity);

		ray.Init(view, src);
		g_csgo.m_trace()->TraceRay(ray, MASK_SHOT, &filter, &tr);

		if (!tr.DidHit())
		{
			ray.Init(view - (view * view.Dot(tr.plane.normal)), src);
			g_csgo.m_trace()->TraceRay(ray, MASK_SHOT, &filter, &tr);
		}

		vec3_t end = tr.endpos;

		ray.Init(src, end);
		g_csgo.m_trace()->TraceRay(ray, MASK_SHOT, &filter, &tr);
		vec3_t end1 = tr.endpos;

		ray.Init(end, src);
		g_csgo.m_trace()->TraceRay(ray, MASK_SHOT, &filter, &tr);
		vec3_t end2 = tr.endpos;

		current = end1.DistTo(end2);
		if (current > highest) {
			highest = current;
			direction = view;
			direction.z = 0;
		}
	}
	return direction;
}
bool c_resolver::can_autowall(player_t* entity, vec3_t src, int accuracy) {
	vec3_t view;
	vec3_t dst = entity->get_eye_pos();
	float radius = fabs(vec3_t(entity->bone_pos(8) - dst).Length2D());

	for (int i = 0; i <= 360; i += (360 / accuracy)) {

		view.x = radius * cos((M_PI / 180) * i) + dst.x;
		view.y = radius * sin((M_PI / 180) * i) + dst.y;
		view.z = dst.z;

		if (autowall::get().calculate_return_info(src, view, g_ctx.m_local, entity, HITBOX_HEAD).m_damage > 5)
			return true;
		else
			return false;
	}
}
void c_resolver::create_move() {
	local_pos = g_ctx.m_local->get_eye_pos();
}
bool c_resolver::is_slow_walking(c_resolver_info record1, c_resolver_info record2, float accuracy) {
	vec3_t prev_vel;
	static float prev_time = 0.f;
	if (prev_time < accuracy) {
		prev_vel = record1.velocity;
		prev_time = record2.simtime + 0.75;
	}
	else {
		if (record1.velocity != prev_vel)
			return false;
		else
			return true;
	}
}
bool c_resolver::should_update_record(c_resolver_info record1, c_resolver_info record2) {
	if (record1.simtime != record2.simtime)
		return true;
	else
		return false;
}
bool c_resolver::can_extrapolate(bool can_autowall, player_t* entity, int extension, int accuracy_boost) {
	vec3_t src, dst;
	src = g_ctx.m_local->get_eye_pos();
	dst = entity->get_eye_pos();

	float radius = fabs((entity->bone_pos(8) - dst).Length2D());

	if (!can_autowall) {

		vec3_t view;
		view.x = radius * cos((M_PI / 180) * src.x) + dst.x;
		view.y = radius * sin((M_PI / 180) * src.y) + dst.y;
		view.z = dst.z;

		bool penetrate = autowall::get().calculate_return_info(src, view, g_ctx.m_local, entity, HITBOX_HEAD).m_did_penetrate_wall;
		float dmg = autowall::get().calculate_return_info(src, view, g_ctx.m_local, entity, HITBOX_HEAD).m_damage;
		if (penetrate || dmg > 10) {
			can_autowall = true;
			return true;
		}
		else
			return false;
	}
	else {

		vec3_t view;
		view.x = radius * cos((M_PI / 180) * src.x) + dst.x;
		view.y = radius * sin((M_PI / 180) * src.y) + dst.y;
		view.z = dst.z;

		bool penetrate = autowall::get().calculate_return_info(src, view, g_ctx.m_local, entity, HITBOX_HEAD).m_did_penetrate_wall;
		float dmg = autowall::get().calculate_return_info(src, view, g_ctx.m_local, entity, HITBOX_HEAD).m_damage;
		if (!penetrate || dmg > 90) {
			return true;
		}
		else {
			vec3_t forward;
			math::angle_vectors(view, forward);
			forward *= extension;

			penetrate = autowall::get().calculate_return_info(src, forward, g_ctx.m_local, entity, HITBOX_HEAD).m_did_penetrate_wall;
			dmg = autowall::get().calculate_return_info(src, forward, g_ctx.m_local, entity, HITBOX_HEAD).m_damage;

			if (!penetrate || dmg > 90) {
				return true;
			}
			else {
				for (int i = 0; i < accuracy_boost; i++) {
					forward *= extension;

					penetrate = autowall::get().calculate_return_info(src, view, g_ctx.m_local, entity, HITBOX_HEAD).m_did_penetrate_wall;
					dmg = autowall::get().calculate_return_info(src, view, g_ctx.m_local, entity, HITBOX_HEAD).m_damage;
					if (!penetrate || dmg > 90) {
						return true;
					}
				}
			}
		}
	}
}
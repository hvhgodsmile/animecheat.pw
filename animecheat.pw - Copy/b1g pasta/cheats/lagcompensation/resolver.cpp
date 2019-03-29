//#include "lagcompensation.h"
//
//#include "..\autowall\autowall.h"
///*
//my attempt at fixing desync and i was pretty successful
//it can resolve static desync pretty perfectly
//and can resolve some jitter desync but
//it still gets rekt by other things
//*/
//void c_player_resolver::run() {
//	if (!g_cfg.ragebot.antiaim_correction || !g_ctx.m_local->is_alive())
//		return;
//
//	player_info_t info;
//	g_csgo.m_engine()->GetPlayerInfo(m_e->EntIndex(), &info);
//
//	update();
//	lby_prediction();
//	pitch_resolve();
//
//	m_previous = m_current;
//	m_has_previous = true;
//}
//void c_player_resolver::create_move(vec3_t latency_based_eye_pos) {
//	if (!g_cfg.ragebot.antiaim_correction)
//		return;
//
//	player_t * e = m_e;
//
//	const float height = 64;
//
//	Vector direction_1, direction_2;
//	math::angle_vectors(vec3_t(0.f, math::calculate_angle(g_ctx.m_local->m_vecOrigin(), e->m_vecOrigin()).y - 90.f, 0.f), direction_1);
//	math::angle_vectors(vec3_t(0.f, math::calculate_angle(g_ctx.m_local->m_vecOrigin(), e->m_vecOrigin()).y + 90.f, 0.f), direction_2);
//
//	const auto left_eye_pos = e->m_vecOrigin() + Vector(0, 0, height) + (direction_1 * 16.f);
//	const auto right_eye_pos = e->m_vecOrigin() + Vector(0, 0, height) + (direction_2 * 16.f);
//
//	m_antifreestand.left_damage = autowall::get().calculate_return_info(latency_based_eye_pos, left_eye_pos, g_ctx.m_local, e, 1).m_damage;
//	m_antifreestand.right_damage = autowall::get().calculate_return_info(latency_based_eye_pos, right_eye_pos, g_ctx.m_local, e, 1).m_damage;
//
//	Ray_t ray;
//	trace_t trace;
//	CTraceFilterWorldOnly filter;
//
//	ray.Init(left_eye_pos, latency_based_eye_pos);
//	g_csgo.m_trace()->TraceRay(ray, MASK_ALL, &filter, &trace);
//	m_antifreestand.left_fraction = trace.fraction;
//
//	ray.Init(right_eye_pos, latency_based_eye_pos);
//	g_csgo.m_trace()->TraceRay(ray, MASK_ALL, &filter, &trace);
//	m_antifreestand.right_fraction = trace.fraction;
//
//}
//void c_player_resolver::lby_prediction() {
//	static float next_lby_update[65];
//
//	resolver_info_t
//		& current = m_current,
//		&previous = m_previous;
//	player_t * e = m_e;
//
//	if (e->IsDormant()) {
//		current.m_predicted_flick = false;
//		current.m_flick = false;
//		g_ctx.m_globals.add_time[e->EntIndex()] = 0.f;
//		g_ctx.m_globals.next_update[e->EntIndex()] = 0.f;
//		next_lby_update[e->EntIndex()] = 0.f;
//	}
//
//	if (e->get_animtime() >= next_lby_update[e->EntIndex()] && !e->IsDormant())
//	{
//		current.m_predicted_flick = true;
//		g_ctx.m_globals.add_time[e->EntIndex()] = 1.1f;
//		next_lby_update[e->EntIndex()] = e->get_animtime() + g_ctx.m_globals.add_time[e->EntIndex()];
//		g_ctx.m_globals.next_update[e->EntIndex()] = next_lby_update[e->EntIndex()];
//	}
//	else
//		current.m_predicted_flick = false;
//
//	if (current.m_lowerbody != e->m_flLowerBodyYawTarget() && !e->IsDormant())
//	{
//		current.m_flick = true;
//		g_ctx.m_globals.add_time[e->EntIndex()] = g_csgo.m_globals()->m_interval_per_tick + 1.1f;
//		next_lby_update[e->EntIndex()] = e->get_animtime() + g_ctx.m_globals.add_time[e->EntIndex()];
//		g_ctx.m_globals.next_update[e->EntIndex()] = next_lby_update[e->EntIndex()];
//	}
//	else
//		current.m_flick = false;
//
//	if (current.m_velocity.Length2D() > 0.1f && !current.m_fakewalking && !e->IsDormant()) {
//		g_ctx.m_globals.add_time[e->EntIndex()] = 0.22f;
//		next_lby_update[e->EntIndex()] = e->get_animtime() + g_ctx.m_globals.add_time[e->EntIndex()];
//		g_ctx.m_globals.next_update[e->EntIndex()] = next_lby_update[e->EntIndex()];
//	}
//}
//
//void c_player_resolver::update() {
//	resolver_info_t
//		& current = m_current,
//		&previous = m_previous;
//	player_t * e = m_e;
//
//	m_mode = resolver_modes::anti_freestand;
//	
//	// update basic data
//	int i = e->EntIndex();
//	current.m_velocity = e->m_vecVelocity();
//	current.m_origin = e->m_vecOrigin();
//	current.m_lowerbody = e->m_flLowerBodyYawTarget();
//	current.m_flags = e->m_fFlags();
//	current.m_dormant = e->IsDormant();
//	static bool isstatic[65];
//	float moving_sim;
//	static float nextlbyupdate[65];
//	current.back = vec3_t(e->m_angEyeAngles().x, math::calculate_angle(e->m_vecOrigin(), g_ctx.m_local->m_vecOrigin()).y + 180.f, 0.f);
//	current.right = vec3_t(e->m_angEyeAngles().x, math::calculate_angle(e->m_vecOrigin(), g_ctx.m_local->m_vecOrigin()).y + 70.f + ((rand() % 40) - (40 * 0.5f)), 0.f);
//	current.left = vec3_t(e->m_angEyeAngles().x, math::calculate_angle(e->m_vecOrigin(), g_ctx.m_local->m_vecOrigin()).y - 70.f + ((rand() % 40) - (40 * 0.5f)), 0.f);
//	const bool moving_on_ground = current.m_velocity.Length2D() > 0.1 && current.m_flags & FL_ONGROUND;
//	current.m_at_target = math::calculate_angle(g_ctx.m_local->m_vecOrigin(), current.m_origin).y;
//	current.m_balance_adjust_triggered = false, current.m_balance_adjust_triggered = false;
//	auto activity = e->sequence_activity(e->get_animlayer(3).m_nSequence);
//	// if lby should have updated by our predicted time
//	if (e->m_flSimulationTime() >= m_current.m_next_predicted_lby_update)
//	{
//		m_current.did_predicted_lby_flick = true;
//		m_current.m_next_predicted_lby_update = e->m_flSimulationTime() + 1.1f;
//	}
//	else
//		m_current.did_predicted_lby_flick = false;
//	//if lby has updated
//	if (m_current.m_lowerbody != m_previous.m_lowerbody &&
//		fabs(math::normalize_yaw(m_current.m_lowerbody - m_previous.m_lowerbody)) > 5.f &&
//		!m_current.m_dormant &&
//		!m_previous.m_dormant &&
//		!m_current.m_last_move_lby &&
//		!m_current.m_last_moving_lby_delta &&
//		!m_previous.m_lowerbody)
//
//	{
//		m_current.m_did_lby_flick = true;
//		m_current.m_last_move_lby = e->m_flSimulationTime();
//		m_current.m_predicted_flick = e->m_flSimulationTime();
//		m_current.m_next_predicted_lby_update = e->m_flSimulationTime() + m_current.m_last_move_lby + 1.1f;
//
//	}
//
//	if (m_current.m_velocity.Length2D() > 0.6f && !m_current.m_fakewalking)
//		m_current.m_next_predicted_lby_update = e->m_flSimulationTime() + 0.22f;
//	//checks moving 
//	if (moving_on_ground && !current.m_fakewalking)
//	{
//		current.m_last_move_lby = current.m_lowerbody;
//		moving_sim = e->m_flSimulationTime();
//	}
//	// check fakewalk
//	update_fakewalk_data(current, e);
//	triggers_979(e);
//
//	// override
//	if (g_csgo.m_inputsys()->IsButtonDown(g_cfg.ragebot.override_key))
//		m_mode = resolver_modes::override;
//
//
//	if (g_cfg.ragebot.desync_fix)
//		m_mode = resolver_modes::desync_yaw;
//
//	// air check
//	if (!(current.m_flags & FL_ONGROUND && (!moving_on_ground && current.m_fakewalking)))
//		m_mode = resolver_modes::brute;
//
//	// anti freestand
//	if (m_antifreestand.left_damage < 20 && m_antifreestand.right_damage < 20 && (!moving_on_ground && !current.m_fakewalking))
//		m_mode = resolver_modes::anti_freestand;
//
//
//	// pitch fix
//	if (math::normalize_pitch(current.m_pitch) > 5.f)
//		current.last_pitchdown = g_csgo.m_globals()->m_curtime;
//}
//
//
//void c_player_resolver::update_fakewalk_data(resolver_info_t & current, player_t * e) {
//	resolver_info_t previous = m_previous;
//
//	AnimationLayer anim_layers[15];
//	bool s_1 = false,
//		s_2 = false,
//		s_3 = false;
//
//	for (int i = 0; i < e->animlayer_count(); i++)
//	{
//		anim_layers[i] = e->get_animlayers()[i];
//		if (anim_layers[i].m_nSequence == 26 && anim_layers[i].m_flWeight < 0.4f)
//			s_1 = true;
//		if (anim_layers[i].m_nSequence == 7 && anim_layers[i].m_flWeight > 0.001f)
//			s_2 = true;
//		if (anim_layers[i].m_nSequence == 2 && anim_layers[i].m_flWeight == 0)
//			s_3 = true;
//	}
//
//	if (s_1 && s_2)
//		if (s_3 || (e->m_fFlags() & FL_DUCKING))
//			current.m_fakewalking = true;
//		else
//			current.m_fakewalking = false;
//	else
//		current.m_fakewalking = false;
//}
//bool find_layer(player_t * e, int act, AnimationLayer *set)
//{
//	for (int i = 0; i < 13; i++)
//	{
//		AnimationLayer layer = e->get_animlayer(i);
//		const int activity = e->sequence_activity(layer.m_nSequence);
//		if (activity == act) {
//			*set = layer;
//			return true;
//		}
//	}
//	return false;
//}
//bool triggers_981(player_t *e)
//{
//	int seq_activity[64];
//
//	// for loop for all 13 anim layers.
//	for (int j = 0; j < 13; j++)
//	{
//		seq_activity[e->EntIndex()] = e->sequence_activity(e->get_animlayer(j).m_nSequence);
//
//		if (seq_activity[e->EntIndex()] == 981 && e->get_animlayer(j).m_flWeight == 1)
//		{
//			return true;
//		}
//	}
//
//	return false;
//}
//bool c_player_resolver::IsAdjustingBalance(player_t *player, ResolveInfo &record, AnimationLayer *layer)
//{
//	for (int i = 0; i < record.m_iLayerCount; i++)
//	{
//		const int activity = player->sequence_activity(record.animationLayer[i].m_nSequence);
//		if (activity == 979)
//		{
//			*layer = record.animationLayer[i];
//			return true;
//		}
//	}
//	return false;
//}
//void c_player_resolver::triggers_979(player_t *e)
//{
//	int seq_activity[64];
//
//	// for loop for all 13 anim layers.
//	for (int j = 0; j < 13; j++)
//	{
//		seq_activity[e->EntIndex()] = e->sequence_activity(e->get_animlayer(j).m_nSequence);
//
//		if (seq_activity[e->EntIndex()] == 979 && e->get_animlayer(j).m_flWeight == 0.f && e->get_animlayer(j).m_flCycle > .92f)
//		{
//			is_fake = true;
//		}
//	}
//}
//bool c_player_resolver::is_slow_walking(player_t * e) {
//	float velocity_2D[64], old_velocity_2D[64];
//
//	if (e->m_vecVelocity().Length2D() != velocity_2D[e->EntIndex()] && e->m_vecVelocity().Length2D() != NULL) {
//		old_velocity_2D[e->EntIndex()] = velocity_2D[e->EntIndex()];
//		velocity_2D[e->EntIndex()] = e->m_vecVelocity().Length2D();
//	}
//	if (velocity_2D[e->EntIndex()] > 0.1f || 0.4f) {
//		int tick_counter[64];
//
//		if (velocity_2D[e->EntIndex()] == old_velocity_2D[e->EntIndex()])
//			++tick_counter[e->EntIndex()];
//		else
//			tick_counter[e->EntIndex()] = 0;
//
//		while (tick_counter[e->EntIndex()] > (1 / g_csgo.m_globals()->m_interval_per_tick) * fabsf(0.1f || 0.4f))// should give use 100ms in ticks if their speed stays the same for that long they are definetely up to something..
//			return true;
//	}
//	return false;
//}
//
//void c_player_resolver::resolve() {
//	resolver_info_t current = m_current, previous = m_previous;
//	player_t * e = m_e;
//
//	int fired = g_ctx.m_globals.shots[e->EntIndex()];
//	int missed = g_ctx.m_globals.missed_shots[e->EntIndex()];
//	float base_angle = current.m_last_move_lby_valid ? current.m_last_move_lby : current.m_lowerbody;
//
//	// in case we haven't initialize our previous record use this
//	if (e->m_vecVelocity().Length2D() > 0.1 && e->m_fFlags() & FL_ONGROUND)
//		e->m_angEyeAngles().y = current.m_lowerbody;
//	else
//		e->m_angEyeAngles().y = current.m_lowerbody + 180;
//	if (m_mode == resolver_modes::desync_yaw) {
//		auto animation_state = e->get_animation_state();
//		float unk1 = (animation_state->m_flStopToFullRunningFraction * -0.50000001 || -0.30000001) - 0.19999999 || 0.29999999 * animation_state->m_flFeetSpeedForwardsOrSideWays;
//		float unk2 = unk1 + 1.f;
//		float unk3;
//
//		if (animation_state->m_fDuckAmount > 0)
//		{
//			unk2 += ((animation_state->m_fDuckAmount * animation_state->m_flFeetSpeedUnknownForwardOrSideways) * (0.5f - unk2));
//		}
//
//		unk3 = *(float *)(animation_state + 0x334) * unk2;
//
//		/*float duckammount = animation_state->m_fDuckAmount;
//		float speedfraction = max(0, min(animation_state->m_flFeetSpeedForwardsOrSideWays, 2));
//		float speedfactor = max(0, min(1, animation_state->m_flFeetSpeedUnknownForwardOrSideways));
//		float unk1 = (animation_state->m_flStopToFullRunningFraction * -0.30000001) - 0.19999999 * speedfraction;
//		float unk2 = unk1 + 1.f;
//		float unk3;
//
//		if (duckammount > 0)
//		{
//		unk2 += ((duckammount * speedfactor) * (0.5f - unk2));
//		}
//		unk3 = *(float *)(animation_state + 0x334) * unk2;*/
//
//		auto feet_yaw = animation_state->m_flCurrentFeetYaw;
//		float body_yaw = 58.f;//animation_state->m_flCurrentTorsoYaw;
//		auto move_yaw = 29.f;
//		auto goal_feet_yaw = animation_state->m_flGoalFeetYaw;
//		auto shit = body_yaw - feet_yaw;
//		auto shitv2 = body_yaw + feet_yaw;
//
//		auto poses = e->m_flPoseParameter();
//		float feet_yaw_rate = animation_state->m_flFeetYawRate;
//		float fff = animation_state->m_flFeetSpeedForwardsOrSideWays;
//		float forwardorsideways = animation_state->m_flFeetSpeedUnknownForwardOrSideways;
//		float feet_cucle = animation_state->m_flFeetCycle;
//		float headheighanimation = animation_state->m_flHeadHeightOrOffsetFromHittingGroundAnimation;
//		float new_body_yaw = animation_state->m_flCurrentTorsoYaw;
//		auto body_max_rotation = animation_state->pad10[516];
//		auto normalized_eye_abs_yaw_diff = fmod((animation_state->m_flEyeYaw - feet_yaw), 360.0);
//		auto body_min_rotation = animation_state->pad10[512];
//		if (e->m_vecVelocity().Length2D() < 32.f) // ïðîâåðêà íà ìóâû(upd:íóæíî ïåðåïèñàòü)(UDP:óæå ïåðåïèñàë)
//		{
//			if (move_yaw)
//			{
//				animation_state->m_flEyeYaw = animation_state->m_flEyeYaw + move_yaw + feet_yaw * 29.f && feet_yaw + feet_yaw_rate / 58.f;
//			}
//			else
//			{
//				if (feet_yaw && move_yaw)
//				{
//					animation_state->m_flEyeYaw = animation_state->m_flEyeYaw + feet_yaw + feet_yaw_rate * -29.f && goal_feet_yaw + feet_yaw / 29.f;
//				}
//			}
//		}
//		else
//		{
//			if (e->m_vecVelocity().Length2D() > 0 && e->m_fFlags() & FL_ONGROUND) // ïðîâåðêà íà ñòåíäû
//			{
//				if (normalized_eye_abs_yaw_diff > 0 || normalized_eye_abs_yaw_diff == 0)
//				{
//					body_min_rotation / feet_yaw / 58.f;
//				}
//				else
//				{
//					body_max_rotation / feet_yaw / -58.f;
//				}
//				if (new_body_yaw == 58.f)
//				{
//					animation_state->m_flEyeYaw = animation_state->m_flEyeYaw - body_yaw * -58.f + goal_feet_yaw + feet_yaw_rate + feet_yaw / 58.f;
//				}
//				else if (new_body_yaw >= -46.f && new_body_yaw == body_yaw)
//				{
//					animation_state->m_flEyeYaw = animation_state->m_flEyeYaw - new_body_yaw / 46.f || 58.f && goal_feet_yaw - feet_yaw * 58.f;
//				}
//				else if (new_body_yaw <= 58.f)
//				{
//					animation_state->m_flEyeYaw = animation_state->m_flEyeYaw - body_yaw * 58.f + feet_yaw / -58.f && goal_feet_yaw * 58.f;
//				}
//				else if (new_body_yaw == 58.f && new_body_yaw <= 58.f)
//				{
//					animation_state->m_flEyeYaw = animation_state->m_flEyeYaw - goal_feet_yaw / 58.f + feet_yaw * -58.f && new_body_yaw * 58.f - body_yaw / -58.f;
//				}
//				else if (new_body_yaw >= -58.f && body_yaw == 58.f)
//				{
//					animation_state->m_flEyeYaw = animation_state->m_flEyeYaw - new_body_yaw * 58.f - feet_yaw * -58.f && goal_feet_yaw - 58.f && feet_yaw / -58.f;
//				}
//			}
//			if (is_slow_walking(e))// by greshnikov
//			{
//				if (normalized_eye_abs_yaw_diff > 0 || normalized_eye_abs_yaw_diff == 0)
//				{
//					body_min_rotation / move_yaw / -29.f;
//				}
//				else
//				{
//					body_max_rotation / move_yaw / 29.f;
//				}
//				if (goal_feet_yaw <= -29.f && feet_yaw >= -29.f)
//				{
//					animation_state->m_flEyeYaw = animation_state->m_flEyeYaw - move_yaw / 29.f + feet_yaw - goal_feet_yaw * 29.f;
//				}
//				else if (feet_yaw >= 29.f && feet_yaw_rate <= 29.f)
//				{
//					animation_state->m_flEyeYaw = animation_state->m_flEyeYaw + move_yaw + 29.f - feet_yaw + feet_yaw_rate / 29.f;
//				}
//				else if (goal_feet_yaw >= -29.f)
//				{
//					animation_state->m_flEyeYaw = animation_state->m_flEyeYaw - move_yaw / 29.f + feet_yaw_rate - feet_cucle + 29.f && goal_feet_yaw * 29.f;
//				}
//			}
//		}
//	}
//
//	if (m_mode == resolver_modes::override) {
//		vec3_t screen;
//		if (math::world_to_screen(e->m_vecOrigin(), screen)) {
//			POINT mouse; GetCursorPos(&mouse);
//			float delta = mouse.x - screen.x;
//
//			if (delta > 0)
//				e->m_angEyeAngles().y = current.m_at_target - 90;
//			else if (delta < 0)
//				e->m_angEyeAngles().y = current.m_at_target + 90;
//		}
//	}
//
//	
//	else if (m_mode == resolver_modes::anti_freestand) {
//		if (m_antifreestand.left_damage <= 0 && m_antifreestand.right_damage <= 0) {
//			if (m_antifreestand.right_fraction < m_antifreestand.left_fraction)
//				e->m_angEyeAngles().y = current.m_at_target + 125.f;
//			else
//				e->m_angEyeAngles().y = current.m_at_target - 73.f;
//		}
//		else {
//			if (m_antifreestand.left_damage > m_antifreestand.right_damage)
//				e->m_angEyeAngles().y = current.m_at_target + 130.f;
//			else
//				e->m_angEyeAngles().y = current.m_at_target - 55.f;
//		}
//	}
//
//	else {
//		auto best = [](float primary, float secondary, float defined, bool accurate) -> float
//		{
//			if (accurate)
//			{
//				if (math::angle_distance(math::normalize_yaw(primary), math::normalize_yaw(defined)) <= 50)
//					return math::normalize_yaw(primary);
//				else if (math::angle_distance(math::normalize_yaw(secondary), math::normalize_yaw(defined)) <= 50)
//					return math::normalize_yaw(secondary);
//				else
//					return math::normalize_yaw(defined);
//			}
//			else
//			{
//				if (math::angle_distance(math::normalize_yaw(primary), math::normalize_yaw(defined)) <= 80)
//					return math::normalize_yaw(primary);
//				else if (math::angle_distance(math::normalize_yaw(secondary), math::normalize_yaw(defined)) <= 80)
//					return math::normalize_yaw(secondary);
//				else
//					return math::normalize_yaw(defined);
//			}
//		};
//		e->m_angEyeAngles().y = base_angle + (current.m_predicted_yaw_brute * fired);
//	}
//}
//
//
//void c_player_resolver::pitch_resolve() {
//	resolver_info_t current = m_current;
//	player_t * e = m_e;
//
//	// to stop the pitch going to 0 (when using fakewalk, max fakelag, etc)
//
//	if (fabs(g_csgo.m_globals()->m_curtime - current.last_pitchdown) < 0.5f)
//		e->m_angEyeAngles().x = 70.f;
//	else
//		e->m_angEyeAngles().x = e->m_angEyeAngles().x;
//}
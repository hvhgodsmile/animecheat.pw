#include "lagcompensation.h"
#include "../../resolver.h"
void lagcompensation::fsn(ClientFrameStage_t stage) {
	auto local_player = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(g_csgo.m_engine()->GetLocalPlayer()));
	for (int i = 1; i <= g_csgo.m_globals()->m_maxclients; i++) {
		auto e = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(i));
		auto & player = players[i];

		player.m_e = e;

		if (!continue_loop(i, e, stage))
			continue;

		switch (stage) {
		case FRAME_NET_UPDATE_POSTDATAUPDATE_START:
			e->set_abs_origin(e->m_vecOrigin());
			player.m_resolver->m_e = e;
			break;
		case FRAME_NET_UPDATE_POSTDATAUPDATE_END:
			e->update_clientside_animation();

			player.log();

			break;
		case FRAME_NET_UPDATE_END:
			apply_interpolation_flags(e, disable_interpolation);

			break;
		case FRAME_RENDER_START:
			if (GetAsyncKeyState(VK_SHIFT))
			{
				AnimFix();
				g_ctx.m_local->set_abs_angles(Vector(0, g_ctx.m_local->get_animation_state()->m_flGoalFeetYaw, 0));
			}
			if (!g_cfg.ragebot.desync_fix)
			{
				HandleHits(e);
				AnimationFix(e);
			}
			*(int*)((uintptr_t)e + 0xA30) = g_csgo.m_globals()->m_framecount;
			*(int*)((uintptr_t)e + 0xA28) = 0;
		}
	}
}
void lagcompensation::HandleHits(player_t* pEnt)
{
	auto NetChannel = g_csgo.m_engine()->GetNetChannelInfo();

	if (!NetChannel)
		return;

	static float predTime[65];
	static bool init[65];

	if (g_ctx.m_globals.shots[pEnt->EntIndex()])
	{
		if (init[pEnt->EntIndex()])
		{
			g_ctx.m_globals.pitchHit[pEnt->EntIndex()] = pEnt->m_angEyeAngles().x;
			predTime[pEnt->EntIndex()] = g_csgo.m_globals()->m_curtime + NetChannel->GetAvgLatency(FLOW_INCOMING) + NetChannel->GetAvgLatency(FLOW_OUTGOING) + TICKS_TO_TIME(1) + TICKS_TO_TIME(g_csgo.m_engine()->GetNetChannel()->m_nChokedPackets);
			init[pEnt->EntIndex()] = false;
		}

		if (g_csgo.m_globals()->m_curtime > predTime[pEnt->EntIndex()] && !g_ctx.m_globals.Hit[pEnt->EntIndex()])
		{
			g_ctx.m_globals.missed_shots[pEnt->EntIndex()] += 1;
			g_ctx.m_globals.shots[pEnt->EntIndex()] = false;
		}
		else if (g_csgo.m_globals()->m_curtime <= predTime[pEnt->EntIndex()] && g_ctx.m_globals.Hit[pEnt->EntIndex()])
			g_ctx.m_globals.shots[pEnt->EntIndex()] = false;

	}
	else
		init[pEnt->EntIndex()] = true;

	g_ctx.m_globals.Hit[pEnt->EntIndex()] = false;
}
void lagcompensation::AnimationFix(player_t* pEnt)
{
	//who needs structs or classes not me lol
	static float oldSimtime[65];
	static float storedSimtime[65];
	static float ShotTime[65];
	static float SideTime[65][3];
	static int LastDesyncSide[65];
	bool UseFreestandAngle[65];
	static bool Delaying[65];
	static AnimationLayer StoredLayers[64][15];
	static c_baseplayeranimationstate * StoredAnimState[65];
	static float StoredPosParams[65][24];
	static Vector oldEyeAngles[65];
	static float oldGoalfeetYaw[65];
	float FreestandAngle[65];
	float* PosParams = (float*)((uintptr_t)pEnt + 0x2774);
	bool update = false;
	bool shot = false;
	static bool jittering[65];

	auto* AnimState = pEnt->get_animation_state();
	if (!AnimState || !pEnt->get_animlayers() || !PosParams)
		return;

	auto RemapVal = [](float val, float A, float B, float C, float D) -> float
	{
		if (A == B)
			return val >= B ? D : C;
		return C + (D - C) * (val - A) / (B - A);
	};

	if (storedSimtime[pEnt->EntIndex()] != pEnt->m_flSimulationTime())
	{
		jittering[pEnt->EntIndex()] = false;
		pEnt->m_bClientSideAnimation() = true;
		pEnt->update_clientside_animation();

		memcpy(StoredPosParams[pEnt->EntIndex()], PosParams, sizeof(float) * 24);
		memcpy(StoredLayers[pEnt->EntIndex()], pEnt->get_animlayers(), (sizeof(AnimationLayer) * pEnt->NumOverlays()));

		oldGoalfeetYaw[pEnt->EntIndex()] = AnimState->m_flGoalFeetYaw;

		if (pEnt->m_hActiveWeapon())
		{
			if (ShotTime[pEnt->EntIndex()] != pEnt->m_hActiveWeapon()->m_fLastShotTime())
			{
				shot = true;
				ShotTime[pEnt->EntIndex()] = pEnt->m_hActiveWeapon()->m_fLastShotTime();
			}
			else
				shot = false;
		}
		else
		{
			shot = false;
			ShotTime[pEnt->EntIndex()] = 0.f;
		}

		float angToLocal = math::normalize_yaw(math::calculate_angle(g_ctx.m_local->m_vecOrigin(), pEnt->m_vecOrigin()).y);
		float Back = math::normalize_yaw(angToLocal);
		float DesyncFix = 0;
		float Resim = math::normalize_yaw((0.24f / (pEnt->m_flSimulationTime() - oldSimtime[pEnt->EntIndex()]))*(oldEyeAngles[pEnt->EntIndex()].y - pEnt->m_angEyeAngles().y));

		if (Resim > 58.f)
			Resim = 58.f;
		if (Resim < -58.f)
			Resim = -58.f;

		if (pEnt->m_vecVelocity().Length2D() > 0.5f && !shot)
		{
			float Delta = math::normalize_yaw(math::normalize_yaw(math::calculate_angle(Vector(0, 0, 0), pEnt->m_vecVelocity()).y) - math::normalize_yaw(math::normalize_yaw(AnimState->m_flGoalFeetYaw + RemapVal(PosParams[11], 0, 1, -60, 60)) + Resim));

			int CurrentSide = 0;

			if (Delta < 0)
			{
				CurrentSide = 1;
				SideTime[pEnt->EntIndex()][1] = g_csgo.m_globals()->m_curtime;
			}
			else if (Delta > 0)
			{
				CurrentSide = 2;
				SideTime[pEnt->EntIndex()][2] = g_csgo.m_globals()->m_curtime;
			}

			if (LastDesyncSide[pEnt->EntIndex()] == 1)
			{
				Resim += (58.f - Resim);
				DesyncFix += (58.f - Resim);
			}
			if (LastDesyncSide[pEnt->EntIndex()] == 2)
			{
				Resim += (-58.f - Resim);
				DesyncFix += (-58.f - Resim);
			}

			if (LastDesyncSide[pEnt->EntIndex()] != CurrentSide)
			{
				Delaying[pEnt->EntIndex()] = true;

				if (.5f < (g_csgo.m_globals()->m_curtime - SideTime[pEnt->EntIndex()][LastDesyncSide[pEnt->EntIndex()]]))
				{
					LastDesyncSide[pEnt->EntIndex()] = CurrentSide;
					Delaying[pEnt->EntIndex()] = false;
				}
			}

			if (!Delaying[pEnt->EntIndex()])
				LastDesyncSide[pEnt->EntIndex()] = CurrentSide;
		}
		else if (!shot)
		{
			float Brute = UseFreestandAngle[pEnt->EntIndex()] ? math::normalize_yaw(Back + FreestandAngle[pEnt->EntIndex()]) : pEnt->m_flLowerBodyYawTarget();

			float Delta = math::normalize_yaw(math::normalize_yaw(Brute - math::normalize_yaw(math::normalize_yaw(AnimState->m_flGoalFeetYaw + RemapVal(PosParams[11], 0, 1, -60, 60))) + Resim));

			if (Delta > 58.f)
				Delta = 58.f;
			if (Delta < -58.f)
				Delta = -58.f;

			Resim += Delta;
			DesyncFix += Delta;

			if (Resim > 58.f)
				Resim = 58.f;
			if (Resim < -58.f)
				Resim = -58.f;
		}

		float Equalized = math::normalize_yaw(math::normalize_yaw(AnimState->m_flGoalFeetYaw + RemapVal(PosParams[11], 0, 1, -60, 60)) + Resim);

		float JitterDelta = fabs(math::normalize_yaw(oldEyeAngles[pEnt->EntIndex()].y - pEnt->m_angEyeAngles().y));

		if (JitterDelta >= 70.f && !shot)
			jittering[pEnt->EntIndex()] = true;

		if (pEnt != g_ctx.m_local && pEnt->m_iTeamNum() != g_ctx.m_local->m_iTeamNum() && (pEnt->m_fFlags() & FL_ONGROUND) && g_cfg.ragebot.desync_fix)
		{
			if (jittering[pEnt->EntIndex()])
				AnimState->m_flGoalFeetYaw = math::normalize_yaw(pEnt->m_angEyeAngles().y + DesyncFix);
			else
				AnimState->m_flGoalFeetYaw = Equalized;

			pEnt->m_flLowerBodyYawTarget1(), AnimState->m_flGoalFeetYaw;
		}

		StoredAnimState[pEnt->EntIndex()] = AnimState;

		oldEyeAngles[pEnt->EntIndex()] = pEnt->m_angEyeAngles();

		oldSimtime[pEnt->EntIndex()] = storedSimtime[pEnt->EntIndex()];

		storedSimtime[pEnt->EntIndex()] = pEnt->m_flSimulationTime();

		update = true;
	}

	pEnt->m_bClientSideAnimation() = false;

	if (pEnt != g_ctx.m_local && pEnt->m_iTeamNum() != g_ctx.m_local->m_iTeamNum() && (pEnt->m_fFlags() & FL_ONGROUND) && g_cfg.ragebot.desync_fix)
		pEnt->m_flLowerBodyYawTarget1(), AnimState->m_flGoalFeetYaw;

	AnimState = StoredAnimState[pEnt->EntIndex()];

	memcpy((void*)PosParams, &StoredPosParams[pEnt->EntIndex()], (sizeof(float) * 24));
	memcpy(pEnt->get_animlayers(), StoredLayers[pEnt->EntIndex()], (sizeof(AnimationLayer) * pEnt->NumOverlays()));

	if (pEnt != g_ctx.m_local && pEnt->m_iTeamNum() != g_ctx.m_local->m_iTeamNum() && (pEnt->m_fFlags() & FL_ONGROUND) && g_cfg.ragebot.desync_fix && jittering[pEnt->EntIndex()])
		pEnt->set_abs_angles(Vector(0, pEnt->m_angEyeAngles().y, 0));
	else
		pEnt->set_abs_angles(Vector(0, oldGoalfeetYaw[pEnt->EntIndex()], 0));

	*reinterpret_cast<int*>(uintptr_t(pEnt) + 0xA30) = g_csgo.m_globals()->m_framecount;
	*reinterpret_cast<int*>(uintptr_t(pEnt) + 0xA28) = 0;
}
void lagcompensation::create_move() {
	last_eye_positions.push_front(g_ctx.m_local->get_eye_pos());
	if (last_eye_positions.size() > 128)
		last_eye_positions.pop_back();

	auto nci = g_csgo.m_engine()->GetNetChannelInfo();
	if (!nci)
		return;

	const int latency_ticks = TIME_TO_TICKS(nci->GetLatency(FLOW_OUTGOING));
	const auto latency_based_eye_pos = last_eye_positions.size() <= latency_ticks ? last_eye_positions.back() : last_eye_positions[latency_ticks];

	for (int i = 1; i <= g_csgo.m_globals()->m_maxclients; i++) {
		auto e = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(i));
		auto & player = players[i];

		player.m_e = e;

		if (!e) { player.m_e = nullptr; continue; }

		if (!e->valid(true))
			continue;

		player.m_resolver->m_e = e;


	}
}



void player_record_t::log() {
	if (!g_cfg.ragebot.lagcomp) {
		m_track.clear();

		return;
	}

	player_t * e = m_e;

	if (!m_track.size()) {
		tickrecord_t record;
		record.save(e, *m_resolver);

		m_track.push_front(record);

		return;
	}

	if (m_track.front().m_simulation_time != e->m_flSimulationTime()) {
		tickrecord_t record;
		record.save(e, *m_resolver);

		record.m_previous_origin = m_track.front().m_vec_origin;
		record.m_previous_simulation_time = m_track.front().m_simulation_time;
		record.m_previous_curtime = m_track.front().m_curtime;

		m_track.push_front(record);
	}

	if (g_csgo.m_globals()->m_curtime - m_track.back().m_curtime > 1.f)
		m_track.pop_back();
}

std::deque< tickrecord_t > player_record_t::get_valid_track() {
	auto delta_time = [&](float simulation_time) -> float {
		auto nci = g_csgo.m_engine()->GetNetChannelInfo();

		if (!nci)
			return FLT_MAX;

		float correct = 0.f;
		correct += nci->GetLatency(FLOW_OUTGOING);
		correct += nci->GetLatency(FLOW_INCOMING);
		correct += util::lerp_time();
		correct = math::clamp< float >(correct, 0.f, 1.f);

		return fabs(correct - (g_csgo.m_globals()->m_curtime - simulation_time));
	};

	std::deque< tickrecord_t > track;

	for (const auto & record : m_track) {
		if (delta_time(record.m_simulation_time) < 0.2f)
			track.push_back(record);
	}

	return track;
}
void lagcompensation::AnimFix()
{
	if (g_csgo.m_engine()->IsConnected())
	{
		if (g_csgo.m_engine()->IsInGame())
		{
			auto entity = g_ctx.m_local;
			if (entity->is_alive())
			{
				if (entity->get_animation_state())
				{
					//start
					auto OldCurtime = g_csgo.m_globals()->m_curtime;
					auto OldFrametime = g_csgo.m_globals()->m_frametime;
					auto OldFraction = entity->get_animation_state()->m_flUnknownFraction = 0;

					g_csgo.m_globals()->m_curtime = entity->get_simulation_time();
					g_csgo.m_globals()->m_frametime =
						g_csgo.m_globals()->m_interval_per_tick *
						g_csgo.m_cvar()->FindVar("host_timescale")->GetFloat();

					AnimationLayer Layers[15];
					std::memcpy(Layers, entity->get_animlayers(), (sizeof(AnimationLayer) * entity->NumOverlays()));

					if (entity->get_animation_state())
						entity->get_animation_state()->m_iLastClientSideAnimationUpdateFramecount = g_csgo.m_globals()->m_frametime - 1;

					entity->m_bClientSideAnimation() = true;
					entity->update_clientside_animation();
					entity->m_bClientSideAnimation() = false;

					//end
					std::memcpy(entity->get_animlayers(), Layers, (sizeof(AnimationLayer) * entity->NumOverlays()));

					g_csgo.m_globals()->m_curtime = OldCurtime;
					g_csgo.m_globals()->m_frametime = OldFrametime;

					entity->get_animation_state()->m_flUnknownFraction = OldFraction;
				}
			}
		}
	}
}
bool lagcompensation::continue_loop(int id, player_t * e, ClientFrameStage_t stage) {
	auto local_player = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(g_csgo.m_engine()->GetLocalPlayer()));
	auto & player = players[id];
	if (!e) { player.m_e = nullptr; return false; }

	switch (stage) {
	case FRAME_RENDER_START:
			if (local_player && local_player->get_animation_state()) {
				AnimationLayer o_anim_overlays[13];
				if (g_csgo.m_globals()->m_curtime = local_player->m_flSimulationTime()) {
					g_csgo.m_globals()->m_frametime =
					local_player->m_bClientSideAnimation() = true;
					local_player->update_clientside_animation();
					local_player->m_bClientSideAnimation() = false;
				}

				local_player->set_abs_angles(Vector(0.0f, local_player->get_animation_state()->m_flGoalFeetYaw, 0.0f));
			}
		if (!e->valid(true))
			return false;
		break;
	case FRAME_NET_UPDATE_POSTDATAUPDATE_START:
		if (!e->valid(true)) {
			delete player.m_resolver;
			player.m_resolver = new c_player_resolver();

			return false;
		}

		break;
	case FRAME_NET_UPDATE_POSTDATAUPDATE_END:
		if (e->m_iHealth() <= 0 || e->m_iTeamNum() == g_ctx.m_local->m_iTeamNum()) {
			player.m_track.clear();

			return false;
		}

		if (e->IsDormant() || e->m_bGunGameImmunity())
			return false;

		break;
	case FRAME_NET_UPDATE_END:
		if (!e->valid(true))
			return false;

		break;
	}

	return true;
}

void lagcompensation::apply_interpolation_flags(player_t * e, int flag) {
	const auto var_map = reinterpret_cast<uintptr_t>(e) + 36;

	for (auto index = 0; index < *reinterpret_cast<int*>(var_map + 20); index++)
		*reinterpret_cast<uintptr_t*>(*reinterpret_cast<uintptr_t*>(var_map) + index * 12) = flag;
}

void lagcompensation::NotReallyUsefull(player_t * e)
{
	if (!g_csgo.m_engine()->IsInGame() || !g_csgo.m_engine()->IsConnected())
	{
		auto local_player = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(g_csgo.m_engine()->GetLocalPlayer()));
		c_baseplayeranimationstate * animation_state = e->get_animation_state();

		if (!animation_state)
			return;
		if (!local_player)
			return;

		e->m_bClientSideAnimation() = true;

		auto old_curtime = g_csgo.m_globals()->m_curtime;
		auto old_frametime = g_csgo.m_globals()->m_frametime;
		auto old_ragpos = local_player->get_ragdoll_pos();
		g_csgo.m_globals()->m_curtime = e->m_flSimulationTime();
		g_csgo.m_globals()->m_frametime = g_csgo.m_globals()->m_interval_per_tick;
		auto player_animation_state = reinterpret_cast<DWORD*>(local_player + 0x3900);
		auto player_model_time = reinterpret_cast<int*>(player_animation_state + 112);
		if (player_animation_state != nullptr && player_model_time != nullptr)
			if (*player_model_time == g_csgo.m_globals()->m_framecount)
				*player_model_time = g_csgo.m_globals()->m_framecount - 1;
		AnimationLayer backup_layers[15];

		local_player->get_ragdoll_pos() = old_ragpos;
		e->update_clientside_animation();

		float flPoseParameter[24];
		float* pose = (float*)((uintptr_t)g_ctx.m_local->m_flPoseParameter());
		std::memcpy(&flPoseParameter, pose, sizeof(float) * 24);


		g_csgo.m_globals()->m_curtime = old_curtime;
		g_csgo.m_globals()->m_frametime = old_frametime;
		e->set_abs_angles(vec3_t(0, animation_state->m_flGoalFeetYaw, 0.f));//if u not doin dis it f*cks up the model lol

		e->m_bClientSideAnimation() = false;
	}
}


void lagcompensation::animation_fix(player_t * e) {
	c_baseplayeranimationstate * animation_state = e->get_animation_state();

	if (!animation_state)
		return;

	auto player = players[e->EntIndex()];
	e->m_bClientSideAnimation() = false;
	if (player.m_track.size()) {
		tickrecord_t * record = &player.m_track.front();

		if (e->m_flSimulationTime() != record->m_anim_time) {
			e->m_bClientSideAnimation() = true;

			e->update_clientside_animation();

			record->m_anim_time = e->m_flSimulationTime();
			record->m_anim_state = e->get_animation_state();
		}
		else {
			e->m_bClientSideAnimation() = false;

			e->set_animation_state(record->m_anim_state);
		}
		e->set_abs_origin(e->abs_origin());
		e->set_abs_angles(vec3_t(0, animation_state->m_flGoalFeetYaw, 0));
		animation_state->m_flUnknownFraction = 0.f;
		animation_state->m_bOnGround = false;
	}
}
void lagcompensation::DoAnimationFix(player_t * e)
{
	c_baseplayeranimationstate * animation_state = e->get_animation_state();

	if (!animation_state)
		return;
	auto old_curtime = g_csgo.m_globals()->m_curtime;
	auto old_frametime = g_csgo.m_globals()->m_frametime;

	g_csgo.m_globals()->m_curtime = e->m_flSimulationTime();
	g_csgo.m_globals()->m_frametime = g_csgo.m_globals()->m_interval_per_tick;

	AnimationLayer backup_layers[15];
	std::memcpy(backup_layers, g_ctx.m_local->get_animlayers(), (sizeof::AnimationLayer) * 15);

	if (e->get_animation_state())
		(e->get_animation_state()->m_iLastClientSideAnimationUpdateFramecount = g_csgo.m_globals()->m_framecount - 1);


	e->m_bClientSideAnimation() = true;
	e->update_clientside_animation();
	e->m_bClientSideAnimation() = false;

	std::memcpy(g_ctx.m_local->get_animlayers(), backup_layers, (sizeof::AnimationLayer) * 15);

	g_csgo.m_globals()->m_curtime = old_curtime;
	g_csgo.m_globals()->m_frametime = old_frametime;
	animation_state->m_bOnGround = false;
	animation_state->m_flUnknownFraction = 0.f;
	e->set_abs_angles(vec3_t(0, animation_state->m_flUnknownFraction, 0.f));//if u not doin dis it f*cks up the model lol
	e->set_abs_angles(vec3_t(0, animation_state->m_flGoalFeetYaw, 0.f));//if u not doin dis it f*cks up the model lol
}


void lagcompensation::update_sequence() {
	if (!g_csgo.m_clientstate())
		return;

	auto net_channel = g_csgo.m_clientstate()->m_NetChannel;

	if (net_channel) {
		if (net_channel->m_nInSequenceNr > m_last_incoming_sequence_number) {
			m_last_incoming_sequence_number = net_channel->m_nInSequenceNr;
			sequences.push_front(incoming_sequence_t(net_channel->m_nInReliableState, net_channel->m_nOutReliableState, net_channel->m_nInSequenceNr, g_csgo.m_globals()->m_realtime));
		}

		if (sequences.size() > 2048)
			sequences.pop_back();
	}
}

void lagcompensation::clear_sequence() {
	m_last_incoming_sequence_number = 0;

	sequences.clear();
}

void lagcompensation::add_latency(INetChannel * net_channel) {
	for (auto& seq : sequences) {
		if (g_csgo.m_globals()->m_realtime - seq.m_current_time >= g_cfg.misc.ping_spike_value / 1000.f) {
			net_channel->m_nInReliableState = seq.m_in_reliable_state;
			net_channel->m_nInSequenceNr = seq.m_sequence_nr;

			break;
		}
	}
}
#include "..\hooks.hpp"

#include "..\..\cheats\ragebot\antiaim.h"
#include "..\..\cheats\sdk\animation_state.h"
#include "..\..\cheats\visuals\other_esp.h"
#include "..\..\cheats\misc\fakelag.h"
#include "..\..\cheats\misc\circlestrafer.h"
#include "..\..\cheats\misc\prediction_system.h"
#include "..\..\cheats\ragebot\aimbot.h"
#include "..\..\cheats\legit\legit_backtrack.h"
#include "..\..\cheats\misc\bunnyhop.h"
#include "..\..\cheats\misc\airstrafe.h"
#include "..\..\cheats\lagcompensation\lagcompensation.h"
#include "..\..\cheats\misc\spammers.h"
#include "..\..\cheats\fakewalk\fakewalk.h"
#include "..\..\extrapolation.h"
using CreateMove_t = bool(__thiscall*)(IClientMode *, float, CUserCmd *);

bool datagram_setup = false;

void NotReallyUsefull(player_t * e)
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

		local_player->get_ragdoll_pos() = old_ragpos;
		e->update_clientside_animation();

		g_csgo.m_globals()->m_curtime = old_curtime;
		g_csgo.m_globals()->m_frametime = old_frametime;
		e->set_abs_angles(vec3_t(0, animation_state->m_flGoalFeetYaw, 0.f));//if u not doin dis it f*cks up the model lol

		e->m_bClientSideAnimation() = false;
	}
}


bool __stdcall hooks::hooked_createmove(float smt, CUserCmd * m_pcmd) {
	static auto original_fn = clientmode_hook->get_func_address< CreateMove_t >(24);
	for (int i = 1; i <= g_csgo.m_globals()->m_maxclients; i++) {
		auto e = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(i));
		if (!m_pcmd->m_command_number || !g_ctx.available() || !g_ctx.m_local)
			return original_fn(g_csgo.m_clientmode(), smt, m_pcmd);

		if (g_cfg.misc.clantag)
			spammers::get().clan_tag();

		if (!g_ctx.m_local->is_alive())
			return original_fn(g_csgo.m_clientmode(), smt, m_pcmd);

		g_ctx.set_command(m_pcmd);

		// update fixed server time
		util::server_time(m_pcmd);

		// ping spike
		if (g_cfg.misc.ping_spike && !netchannel_hook) {
			auto netchannel = g_csgo.m_clientstate()->m_NetChannel;

			if (netchannel) {
				netchannel_hook = new vmthook(reinterpret_cast<DWORD**>(netchannel));
				netchannel_hook->hook_function(reinterpret_cast<uintptr_t>(hooked_senddatagram), 46);
			}
		}

		if (g_cfg.misc.sniper_crosshair) {
			static bool o = false;
			if (!o) { o = true;	g_csgo.m_cvar()->FindVar("weapon_debug_spread_show")->SetValue(3); }
		}
		else {
			static bool o = false;
			if (!o) { o = true;	g_csgo.m_cvar()->FindVar("weapon_debug_spread_show")->SetValue(0); }
		}


		// update animations
		animation_state::get().create_move();


		uintptr_t *frame_ptr;
		__asm mov frame_ptr, ebp;

		g_ctx.send_packet = true;
		g_ctx.m_globals.fakelagging = false;
		vec3_t original_angle = m_pcmd->m_viewangles;



		// upredicted velocity for lby breaker
		g_ctx.unpred_velocity = g_ctx.m_local->m_vecVelocity();

		// engine prediction
		engineprediction::get().start(); {
			// fakewalk
			fakewalk::get().create_move();

			// fakeduck
			fakewalk::get().fake_duck();

			// fakelag
			fakelag::get().create_move();

			if (g_cfg.esp.knifeleft)
				otheresp::get().KnifeLeft();

			// bunnyhopping
			if (g_cfg.misc.bunnyhop)
				bunnyhop::get().create_move();

			// airstrafe
			if (g_cfg.misc.airstrafe)
				airstrafe::get().create_move();

			if (g_cfg.ragebot.extrapolation)
				LinearExtrapolationsrun();

			vec3_t wish_angle = m_pcmd->m_viewangles;
			m_pcmd->m_viewangles = original_angle;

			// update lowerbody breaker
			if (!g_csgo.m_clientstate()->m_nChokedCommands)
				antiaim::get().update_lowerbody_breaker();

			// antiaim
			antiaim::get().create_move();

			// aimbot
			if (g_cfg.ragebot.enable) {
				aimbot::get().create_move();

			}

			// add shots to counter
			g_ctx.m_globals.add_shot();

			// fix movement
			util::movement_fix(wish_angle, m_pcmd);

			//instant unduck
			if (g_cfg.misc.no_crouch_cooldown)
			{
				g_ctx.get_command()->m_buttons |= IN_BULLRUSH;
			}

		}

		lagcompensation::get().create_move();


		// clamp choked commands
		if (g_csgo.m_clientstate()->m_nChokedCommands > 14)
			g_ctx.send_packet = true;

		// set send_packet
		*(bool*)(*frame_ptr - 0x1C) = g_ctx.send_packet;

		// get angles for thirdperson
		if (!g_ctx.send_packet) {
			g_ctx.m_globals.real_angles = m_pcmd->m_viewangles;
		}
		else
			g_ctx.m_globals.real_angles = m_pcmd->m_viewangles;


		// clamp angles for anti-ut
		if (g_cfg.misc.anti_untrusted)
			math::normalize_angles(m_pcmd->m_viewangles);

		// movement clamp
		if (m_pcmd->m_forwardmove > 450.0f)
			m_pcmd->m_forwardmove = 450.0f;
		else if (m_pcmd->m_forwardmove < -450.0f)
			m_pcmd->m_forwardmove = -450.0f;

		if (m_pcmd->m_sidemove > 450.0f)
			m_pcmd->m_sidemove = 450.0f;
		else if (m_pcmd->m_sidemove < -450.0f)
			m_pcmd->m_sidemove = -450.0f;

		return false;
	}
}







































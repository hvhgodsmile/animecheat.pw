#include "fakewalk.h"
#include "..\ragebot\antiaim.h"
#include "..\sdk\animation_state.h"
#define square( x ) ( x * x )
Vector unpredictedVelocity = Vector(0, 0, 0);
inline float FastSqrt(float x) {

	unsigned int i = *(unsigned int*)&x;
	i += 127 << 23;
	i >>= 1;

	return *(float*)&i;
}

void fakewalk::create_move()
{
	if (g_cfg.misc.fakewalk && g_csgo.m_inputsys()->IsButtonDown(g_cfg.misc.fakewalk_key))
	{
		auto get_speed = 31;

		float min_speed = (float)(FastSqrt(square(g_ctx.get_command()->m_forwardmove) + square(g_ctx.get_command()->m_sidemove) + square(g_ctx.get_command()->m_upmove)));
		if (min_speed <= 0.f)
			return;	

		if (g_ctx.get_command()->m_buttons & IN_DUCK)
			get_speed *= 2.94117647f;

		if (min_speed <= get_speed)
			return;

		float kys = get_speed / min_speed;
		g_cfg.misc.fakewalk = true;
		g_ctx.get_command()->m_forwardmove *= kys;
		g_ctx.get_command()->m_sidemove *= kys;
		g_ctx.get_command()->m_upmove *= kys;

	}
}


void fakewalk::fake_duck() {

	if (g_csgo.m_inputsys()->IsButtonDown(g_cfg.misc.fakeduck))
	{
		int fakelag_limit = g_cfg.antiaim.fakelag_amount >= 14 ? 14 : g_cfg.antiaim.fakelag_amount;
		int choked_goal = fakelag_limit / 2;
		bool should_crouch = g_csgo.m_clientstate()->m_nChokedCommands >= choked_goal;

		if (g_ctx.m_local->m_fFlags() & FL_ONGROUND)
		{
			g_ctx.get_command()->m_buttons |= IN_BULLRUSH;
			if (should_crouch) {
				g_ctx.get_command()->m_buttons |= IN_DUCK;
			}
			else {
				g_ctx.get_command()->m_buttons &= ~IN_DUCK;
				g_ctx.send_packet = true;
			}
		}
	}
}


































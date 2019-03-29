#include "airstrafe.h"
#include "circlestrafer.h"

#define CheckIfNonValidNumber(x) (fpclassify(x) == FP_INFINITE || fpclassify(x) == FP_NAN || fpclassify(x) == FP_SUBNORMAL)


void airstrafe::create_move() {

	static auto down = [](ButtonCode_t bt) -> bool {
		return g_csgo.m_inputsys()->IsButtonDown(bt);
	};

	if (g_ctx.m_local->get_move_type() == MoveType_t::MOVETYPE_NOCLIP || g_ctx.m_local->get_move_type() == MoveType_t::MOVETYPE_LADDER)
		return;

	if (!GetAsyncKeyState(VK_SPACE) || g_ctx.m_local->m_vecVelocity().Length2D() < 0.5)
		return;

	if (!(g_ctx.m_local->m_fFlags() & FL_ONGROUND)) {
		static float cl_sidespeed = g_csgo.m_cvar()->FindVar("cl_sidespeed")->GetFloat();
		if (fabsf(g_ctx.get_command()->m_mousedx > 2)) {
			g_ctx.get_command()->m_sidemove = (g_ctx.get_command()->m_mousedx < 0.f) ? -cl_sidespeed : cl_sidespeed;
			return;
		}


		if (GetAsyncKeyState('S')) {
			g_ctx.get_command()->m_viewangles.y -= 180;
		}
		else if (GetAsyncKeyState('D')) {
			g_ctx.get_command()->m_viewangles.y -= 90;
		}
		else if (GetAsyncKeyState('A')) {
			g_ctx.get_command()->m_viewangles.y += 90;
		}


		if (!g_ctx.m_local->m_vecVelocity().Length2D() > 0.5 || g_ctx.m_local->m_vecVelocity().Length2D() == NAN || g_ctx.m_local->m_vecVelocity().Length2D() == INFINITE)
		{
			g_ctx.get_command()->m_forwardmove = 400;
			return;
		}

		g_ctx.get_command()->m_forwardmove = math::clamp(5850.f / g_ctx.m_local->m_vecVelocity().Length2D(), -400, 400);
		if ((g_ctx.get_command()->m_forwardmove < -400 || g_ctx.get_command()->m_forwardmove > 400))
			g_ctx.get_command()->m_forwardmove = 0;

		const auto vel = g_ctx.m_local->m_vecVelocity();
		const float y_vel = RAD2DEG(atan2(vel.y, vel.x));
		const float diff_ang = math::normalize_yaw(g_ctx.get_command()->m_viewangles.y - y_vel);

		g_ctx.get_command()->m_sidemove = (diff_ang > 0.0) ? -cl_sidespeed : cl_sidespeed;
		g_ctx.get_command()->m_viewangles.y = math::normalize_yaw(g_ctx.get_command()->m_viewangles.y - diff_ang);
	}
}




























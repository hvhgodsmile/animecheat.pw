#include "manual_aa.h"
#include "../../hooks/hooks.hpp"
#include "antiaim.h"

void manual_aa::update() {
	static bool
		holding_left = false,
		holding_right = false,
		holding_back = false;

	if (g_csgo.m_inputsys()->IsButtonDown(g_cfg.antiaim.left_manualkey) && !holding_left) {
		if (side == SIDE_LEFT) {
			side = SIDE_NONE;
		}
		else {
			side = SIDE_LEFT;
		}

		holding_left = true;
	}
	else if (holding_left && !g_csgo.m_inputsys()->IsButtonDown(g_cfg.antiaim.left_manualkey)) {
		holding_left = false;
	}

	if (g_csgo.m_inputsys()->IsButtonDown(g_cfg.antiaim.right_manualkey) && !holding_right) {
		if (side == SIDE_RIGHT) {
			side = SIDE_NONE;
		}
		else {
			side = SIDE_RIGHT;
		}

		holding_right = true;
	}
	else if (holding_right && !g_csgo.m_inputsys()->IsButtonDown(g_cfg.antiaim.right_manualkey)) {
		holding_right = false;
	}

	if (g_csgo.m_inputsys()->IsButtonDown(g_cfg.antiaim.back_manualkey) && !holding_back) {
		if (side == SIDE_BACK) {
			side = SIDE_NONE;
		}
		else {
			side = SIDE_BACK;
		}

		holding_back = true;
	}
	else if (holding_back && !g_csgo.m_inputsys()->IsButtonDown(g_cfg.antiaim.back_manualkey)) {
		holding_back = false;
	}
}

bool manual_aa::run() {
	vec3_t engine_angles; g_csgo.m_engine()->GetViewAngles(engine_angles);
	float yaw = engine_angles.y;

	switch (side) {
	case SIDE_LEFT:
	{
		g_ctx.get_command()->m_viewangles.y = yaw + (90.0f + (math::random_float(0.f, 30.f) - 15.f));
		g_ctx.get_command()->m_viewangles.x = antiaim::get().get_pitch();
	}

	break;
	case SIDE_RIGHT:
	{
		g_ctx.get_command()->m_viewangles.y = yaw - (90.0f + (math::random_float(0.f, 30.f) - 15.f));
		g_ctx.get_command()->m_viewangles.x = antiaim::get().get_pitch();

	}

	break;
	case SIDE_BACK:
	{
		g_ctx.get_command()->m_viewangles.y = yaw + (180 + (math::random_float(0.f, 30.f) - 15.f));
		g_ctx.get_command()->m_viewangles.x = antiaim::get().get_pitch();
	}

	break;
	}

	return side != SIDE_NONE;
}
#pragma once
#include "includes.hpp"

template<class T, class U>
T myheadhurts(T in, U low, U high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}

void LinearExtrapolationsrun()
{
	if (g_cfg.ragebot.extrapolation)
	{
		if (g_ctx.m_local && g_ctx.m_local->is_alive()) {
			for (int i = 1; i < g_csgo.m_globals()->m_maxclients; i++)
			{
				auto m_entity = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(i));
				if (!m_entity || m_entity == g_ctx.m_local || m_entity->GetClientClass()->m_ClassID != ClassId_CCSPlayer || !m_entity->is_alive())
					continue;

				float simtime_delta = m_entity->m_flSimulationTime() - m_entity->get_old_simulation_time();
				int choked_ticks = myheadhurts(TIME_TO_TICKS(simtime_delta), 1, 15);
				Vector lastOrig;

				if (lastOrig.Length() != m_entity->m_vecOrigin().Length())
					lastOrig = m_entity->m_vecOrigin();

				float delta_distance = (m_entity->m_vecOrigin() - lastOrig).LengthSqr();
				if (delta_distance > 4096.f)
				{
					Vector velocity_per_tick = m_entity->m_vecVelocity() * g_csgo.m_globals()->m_interval_per_tick;
					auto new_origin = m_entity->m_vecOrigin() + (velocity_per_tick * choked_ticks);
					m_entity->set_abs_origin(new_origin);
				}
			}
		}
	}
}
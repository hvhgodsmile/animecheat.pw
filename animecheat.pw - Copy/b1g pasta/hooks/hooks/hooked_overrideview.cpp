#include "..\hooks.hpp"
#include "..\..\grenedepred.hpp"
using OverrideView_t = void( __stdcall* )( CViewSetup* );
void aspectration(float aspect_ratio_multiplier)
{
	int screen_width, screen_height;
	g_csgo.m_engine()->GetScreenSize(screen_width, screen_height);

	float aspectratio_value = (screen_width * aspect_ratio_multiplier) / screen_height;

	static ConVar* aspectratio = g_csgo.m_cvar()->FindVar("r_aspectratio");

	aspectratio->SetValue(aspectratio_value);
}

void on_aspect_ratio_changed()
{
	float aspect_ratio = (g_cfg.esp.aspectratio) * 0.01;
	aspect_ratio = 2 - aspect_ratio;
	if (g_cfg.esp.aspectratioenable)
		aspectration(aspect_ratio);
	else
		aspectration(0);
}

void __stdcall hooks::hooked_overrideview( CViewSetup* viewsetup ) {
	static auto original_fn = clientmode_hook->get_func_address<OverrideView_t >( 18 );

	static bool
		setting,
		holding,
		in_thirdperson;

	if (g_csgo.m_inputsys()->IsButtonDown(g_cfg.misc.thirdperson_toggle))
	{
		setting = !setting;
	}
	in_thirdperson = setting;

	auto local_player = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(g_csgo.m_engine()->GetLocalPlayer()));

	if (in_thirdperson)
	{
		if (g_csgo.m_inputsys()->IsButtonDown(g_cfg.misc.fakeduck) && g_ctx.m_local && g_ctx.m_local->is_alive() && g_csgo.m_input()->m_fCameraInThirdPerson)
			local_player->m_vecOrigin().z = local_player->GetAbsOrigin().z + 64.f;
	}

	if ( g_ctx.available( ) ) {
		auto scoped = g_ctx.m_local->m_bIsScoped( );
		if ( !scoped || ( scoped && g_cfg.esp.override_fov_scoped ) ) {
			viewsetup->fov += g_cfg.esp.fov;
		}

	}
	g_csgo.m_cvar()->FindVar("viewmodel_offset_x")->SetValue(g_cfg.esp.viewmodel_x);
	g_csgo.m_cvar()->FindVar("viewmodel_offset_y")->SetValue(g_cfg.esp.viewmodel_y);
	g_csgo.m_cvar()->FindVar("viewmodel_offset_z")->SetValue(g_cfg.esp.viewmodel_z - 10);


	on_aspect_ratio_changed();

	return original_fn( viewsetup );

}








































#include "..\hooks.hpp"
#include "..\..\cheats\visuals\player_esp.h"
#include "..\..\cheats\visuals\other_esp.h"
#include "..\..\cheats\misc\logs.h"
#include "..\..\cheats\legit\legit_backtrack.h"
#include "..\..\cheats\visuals\world_esp.h"
#include "..\..\grenedepred.hpp"
#include "..\..\structs.hpp"
using PaintTraverse_t = void(__thiscall*)(void*, vgui::VPANEL, bool, bool);
template<class T>
static T* Find_Hud_Element(const char* name)
{
	static auto pThis = *reinterpret_cast<DWORD**>(util::pattern_scan("client_panorama.dll", "B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08") + 1);

	static auto find_hud_element = reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(util::pattern_scan("client_panorama.dll", "55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28"));
	return (T*)find_hud_element(pThis, name);
}

bool reload_fonts()
{
	static int old_width, old_height;
	int width, height;
	g_csgo.m_engine()->GetScreenSize(width, height);

	if (width != old_width || height != old_height)
	{
		old_width = width;
		old_height = height;
		return true;
	}
	return false;
}

void __fastcall hooks::hooked_painttraverse(void* ecx, void* edx, vgui::VPANEL panel, bool force_repaint, bool allow_force) {
	auto local_player = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(g_csgo.m_engine()->GetLocalPlayer()));
	int
		w_s, h_s, w0, h0, x = 630, y = 500, w = 250, h = 20;

	g_csgo.m_engine()->GetScreenSize(w_s, h_s);
	w0 = w_s / 2; h0 = h_s / 2;

	static auto draw_animation = []() -> void {
		if (!g_ctx.m_local->is_alive())
			return;

		auto screen = render::get().viewport();

		for (int i = 0; i < 13; i++) {
			AnimationLayer layer = g_ctx.m_local->get_animlayers()[i];

			render::get().text(fonts[VERDANA12BOLDOUT], screen.right / 2 + 10, screen.bottom / 2 + 10 + (i * 13), Color::White, HFONT_CENTERED_NONE,
				"layer: %i, activity: %i, weight: %0.2f, cycle: %0.2f", i, g_ctx.m_local->sequence_activity(layer.m_nSequence), layer.m_flWeight, layer.m_flCycle);
		}
	};

	static uint32_t HudZoomPanel;
	if (!HudZoomPanel)
		if (!strcmp("HudZoom", g_csgo.m_panel()->GetName(panel)))
			HudZoomPanel = panel;

	if (g_cfg.esp.removals[REMOVALS_SCOPE].enabled && HudZoomPanel == panel)
		return;

	static auto panel_id = vgui::VPANEL{ 0 };
	static auto original_fn = panel_hook->get_func_address< PaintTraverse_t >(41);

	original_fn(ecx, panel, force_repaint, allow_force);

	if (!panel_id) {
		const auto panelName = g_csgo.m_panel()->GetName(panel);
		if (strstr(panelName, "MatSystemTopPanel"))
			panel_id = panel;
	}

	const auto charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*abcdefghijklmnopqrstuvwxyz";
	static wchar_t buildDate[256];
	swprintf(buildDate, L"athena build on %hs", __DATE__);
	static int len = std::wstring(buildDate).length();
	static bool bSleep = false;
	static int delayTime;
	static int pos = 0;

	if (pos > -1) {
		for (int i = 0; i < len; ++i) {
			if (i >= pos) buildDate[i] = charset[rand() % strlen(charset) - 1];
		}
	}

	if (!bSleep) {
		pos++;
		bSleep = true;
		delayTime = GetTickCount() + 50;
		if (pos == len) {
			pos = -1;
			delayTime += 500;
		}
	}
	else if (delayTime < GetTickCount()) {
		bSleep = false;
	}


	if (reload_fonts())
		render::get().setup(); //fix for fonts not reloading whilst changing resolutions.

	

	if (panel_id == panel) {
		if (g_ctx.available() && local_player) {
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					g_ctx.m_globals.w2s_matrix[i][j] = g_csgo.m_engine()->WorldToScreenMatrix()[i][j];
				}
			}

			if (g_cfg.player.enable)
				playeresp::get().paint_traverse();
			
			if (g_cfg.esp.spectators)  /// indicators
				otheresp::get().spectators();

			if (g_cfg.esp.zeusrange)   /// indicators
				otheresp::get().DrawZeusRange();


			if (g_cfg.antiaim.lby_breaker)  /// indicators
				otheresp::get().spectators();

			if (g_cfg.antiaim.DESYNCANTIAIM) /// indicators
				otheresp::get().desyncindicator();

			for (int i = 1; i < g_csgo.m_entitylist()->GetHighestEntityIndex(); i++) {
				auto e = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(i));

				if (!e)
					continue;
				if (g_cfg.esp.bombesp)
				{

					if (e->GetClientClass()->m_ClassID == (int)ClassId_CPlantedC4)
						worldesp::get().DrawBombPlanted(e);
				}
			}
		


			if (g_csgo.m_inputsys()->IsButtonDown(g_cfg.ragebot.baim_key))/// indicators
				otheresp::get().baimindicator();

			if (g_cfg.esp.watermark)   /// indicators
				otheresp::get().watermark();



			otheresp::get().spread_crosshair();

			otheresp::get().indicators();
			worldesp::get().paint_traverse();

			if (g_cfg.esp.removals[REMOVALS_SCOPE].enabled && local_player->m_bIsScoped()) {
				auto resolution = render::get().viewport();

				render::get().line(0, resolution.bottom / 2, resolution.right, resolution.bottom / 2, Color::Black);
				render::get().line(resolution.right / 2, 0, resolution.right / 2, resolution.bottom, Color::Black);
			}


			otheresp::get().antiaim_indicator();

			otheresp::get().penetration_reticle();

			playeresp::get().pov_arrow();
		}

		cfg_manager->config_files();

		eventlogs::get().paint_traverse();

		menu::get().update();
	}
}








































#include "other_esp.h"
#include "..\autowall\autowall.h"
#include "..\ragebot\antiaim.h"
#include <chrono>
static void(__thiscall *ClearDeathNotices)(DWORD);
static DWORD* deathNotice;

void otheresp::PreserveKillfeed()
{
}

std::vector<impact_info> impacts;
std::vector<hitmarker_info> hitmarkers;

void otheresp::antiaim_indicator() {
	auto local_player = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(g_csgo.m_engine()->GetLocalPlayer()));

	static auto degrees_to_radians = [](float deg) -> float { return deg * (DirectX::XM_PI / 180.f); };

	auto rotated_position = [](Vector start, const float rotation, const float distance) -> Vector {
		const auto rad = degrees_to_radians(rotation);
		start.x += cosf(rad) * distance;
		start.y += sinf(rad) * distance;

		return start;
	};

	if (!g_ctx.available() || !local_player)
		return;

	static const auto real_color = Color(153, 51, 255, 255);
	//static const auto fake_color = Color(255, 0, 0, 255);
	//static const auto lby_color = Color(0, 0, 255, 255);

	if (!local_player->is_alive())
		return;

	if (g_cfg.esp.antiaim_indicator) {
		auto client_viewangles = Vector();
		auto
			screen_width = render::get().viewport().right,
			screen_height = render::get().viewport().bottom;

		g_csgo.m_engine()->GetViewAngles(client_viewangles);

		constexpr auto radius = 80.f;

		const auto screen_center = Vector2D(screen_width / 2.f, screen_height / 2.f);
		const auto real_rot = degrees_to_radians(client_viewangles.y - g_ctx.m_globals.real_angles.y - 90);
		//const auto fake_rot = degrees_to_radians(client_viewangles.y - g_ctx.m_globals.fake_angles.y - 90);
		//const auto lby_rot = degrees_to_radians(client_viewangles.y - g_ctx.m_local->m_flLowerBodyYawTarget() - 90);

		auto draw_arrow = [&](float rot, Color color) -> void {
			auto pos_one = Vector2D(screen_center.x + cosf(rot) * radius, screen_center.y + sinf(rot) * radius);
			auto pos_two = Vector2D(screen_center.x + cosf(rot + degrees_to_radians(8)) * (radius - 13.f),
				screen_center.y + sinf(rot + degrees_to_radians(8)) * (radius - 13.f));
			auto pos_three = Vector2D(screen_center.x + cosf(rot - degrees_to_radians(8)) * (radius - 13.f),
				screen_center.y + sinf(rot - degrees_to_radians(8)) * (radius - 13.f));

			render::get().triangle(
				Vector2D(pos_one.x, pos_one.y),
				Vector2D(pos_two.x, pos_two.y),
				Vector2D(pos_three.x, pos_three.y),
				color);
		};

		draw_arrow(real_rot, real_color);
		//draw_arrow(fake_rot, fake_color);
		//draw_arrow(lby_rot, lby_color);
	}
}
void Draw_Line(int x1, int y1, int x2, int y2, Color color)
{
	g_csgo.m_surface()->DrawSetColor(color);
	g_csgo.m_surface()->DrawLine(x1, y1, x2, y2);
}
void otheresp::DrawZeusRange() {
	if (!g_csgo.m_engine()->IsConnected() || !g_csgo.m_engine()->IsInGame())
		return;

	if (!g_ctx.m_local) return;

	if (!*(bool*)((uintptr_t)g_csgo.m_input() + 0xAD))
		return;

	if (!g_cfg.esp.zeusrange)
		return;

	auto weapon = g_ctx.m_local->m_hActiveWeapon().Get();
	if (weapon->m_iItemDefinitionIndex() != ItemDefinitionIndex::WEAPON_TASER)
		return;

	float step = M_PI * 2.0 / 1023;
	float rad = weapon->get_csweapon_info()->range;
	Vector origin = g_ctx.m_local->get_eye_pos();

	static double rainbow;

	Vector screenPos;
	static Vector prevScreenPos;

	for (float rotation = 0; rotation < (M_PI * 2.0); rotation += step)
	{
		Vector pos(rad * cos(rotation) + origin.x, rad * sin(rotation) + origin.y, origin.z);

		trace_t trace;
		CTraceFilter filter;

		filter.pSkip = g_ctx.m_local;


		g_csgo.m_trace()->TraceRay(Ray_t(origin, pos), MASK_SHOT_BRUSHONLY, &filter, &trace);


		if (g_csgo.m_debugoverlay()->ScreenPosition(trace.endpos, screenPos))
			continue;

		if (!prevScreenPos.IsZero() && !screenPos.IsZero() && screenPos.DistTo(Vector(-107374176, -107374176, -107374176)) > 3.f&& prevScreenPos.DistTo(Vector(-107374176, -107374176, -107374176)) > 3.f) {
			rainbow += 0.00001;
			if (rainbow > 1.f)
				rainbow = 0;

			Color color = Color::FromHSB(rainbow, 1.f, 1.f);
			Draw_Line(prevScreenPos.x, prevScreenPos.y, screenPos.x, screenPos.y, color);
			//DrawLine(prevScreenPos.x, prevScreenPos.y + 1, screenPos.x, screenPos.y + 1, color);
			//DrawLine(Vector2D(prevScreenPos.x, prevScreenPos.y - 1), Vector2D(screenPos.x, screenPos.y - 1), color);
		}
		prevScreenPos = screenPos;
	}
}

void otheresp::penetration_reticle() {
	auto local_player = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(g_csgo.m_engine()->GetLocalPlayer()));

	if (!local_player->is_alive())
		return;

	auto draw_xhair = [](float size, Color color) -> void {
		int x_1, x_2, y_1, y_2;

		int
			screen_w = render::get().viewport().right / 2,
			screen_h = render::get().viewport().bottom / 2;

		x_1 = screen_w - (size / 4);
		x_2 = screen_w + (size / 4) + 1;

		y_1 = screen_h - (size / 4);
		y_2 = screen_h + (size / 4) + 1;

		render::get().rect_filled(screen_w - 1, screen_h - 1, 3, 3, color);
	};

	if (!g_cfg.esp.penetration_reticle || !local_player->is_alive())
		return;

	auto weapon = local_player->m_hActiveWeapon().Get();
	if (!weapon)
		return;

	if (weapon->is_non_aim())
		return;

	auto weapon_info = weapon->get_csweapon_info();
	if (!weapon_info)
		return;

	vec3_t view_angles;
	g_csgo.m_engine()->GetViewAngles(view_angles);

	vec3_t direction;
	math::angle_vectors(view_angles, direction);

	vec3_t
		start = local_player->get_eye_pos(),
		end = start + (direction * 8192.f);

	Color color = Color(255, 0, 0);
	if (autowall::get().calculate_return_info(start, end, local_player).m_did_penetrate_wall)
		color = Color(163, 225, 21);

	draw_xhair(2, color);

}

#define M_PI 3.14159265358979323846

void otheresp::draw_percent_circle(int x, int y, int size, float amount_full, Color fill)
{
	int texture = g_csgo.m_surface()->CreateNewTextureID(true);
	g_csgo.m_surface()->DrawSetTexture(texture);
	g_csgo.m_surface()->DrawSetColor(fill);

	Vertex_t vertexes[100];
	for (int i = 0; i < 100; i++) {
		float angle = ((float)i / -100) * (M_PI * (2 * amount_full));
		vertexes[i].Init(Vector2D(x + (size * sin(angle)), y + (size * cos(angle))));
	}

	g_csgo.m_surface()->DrawTexturedPolygon(100, vertexes, true);
}

void otheresp::spectators() {

	if (g_csgo.m_engine()->IsInGame() && g_csgo.m_engine()->IsConnected()) {
		int index = 0;
		int w, h;
		g_csgo.m_engine()->GetScreenSize(w, h);

		render::get().text(fonts[TAHOMA12], w - 80, h / 2 - 10, Color(255, 255, 255, 255), HFONT_OFF_SCREEN, "spectators:");
		for (int i = 1; i < 65; i++) {
			auto p_local = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(g_csgo.m_engine()->GetLocalPlayer())); // what's csgo::local_player???????
			auto p_entity = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(i));
			player_info_t e_info;

			if (p_entity && p_entity != p_local) {
				g_csgo.m_engine()->GetPlayerInfo(i, &e_info);
				if (!p_entity->is_alive() && !p_entity->IsDormant()) {
					auto target = p_entity->m_hObserverTarget();
					if (target) {
						auto p_target = g_csgo.m_entitylist()->GetClientEntityFromHandle(target);
						if (p_target == p_local) {
							int x, y;
							g_csgo.m_surface()->GetTextSize(fonts[ESPFLAG], util::to_wchar(e_info.szName), x, y);

							std::string player_name = e_info.szName;
							player_info_t p_info;
							g_csgo.m_engine()->GetPlayerInfo(i, &e_info);

							render::get().text(fonts[ESPFLAG], w - x - 35, h / 2 + (10 * index), Color(255, 255, 255, 255), HFONT_OFF_SCREEN, player_name.c_str());
							index++;
						}
					}
				}
			}
		}
	}
}

void otheresp::indicators()
{
	auto local_player = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(g_csgo.m_engine()->GetLocalPlayer()));
	auto weapon = local_player->m_hActiveWeapon().Get();
	int w = 0, h = 0;
	g_csgo.m_engine()->GetScreenSize(w, h);

	static auto draw_bar = [](int x, int y, float val, float max, const wchar_t* name) {
		if (val > max)
			val = max;

		render::get().draw_wide_string(true, fonts[INDICATORFONT], x + 125 / 2, y - 8, Color(255, 255, 255, 255), const_cast<wchar_t*>(name));
		render::get().draw_filled_rect(x, y, 125, 15, Color(50, 50, 50, 75));
		render::get().draw_filled_rect(x, y, 125 * (val / max), 15, Color(0, 255, 0, 75));
	};
	static auto last_fake_origin = Vector();
	static auto last_origin = Vector();

	if(!g_csgo.m_clientstate()->m_nChokedCommands) {
		last_fake_origin = last_origin;
		last_origin = local_player->m_vecOrigin();
	}
	auto nci = g_csgo.m_engine()->GetNetChannelInfo();

	if (!nci)
		return;
	if (local_player->is_valid_player() && local_player->is_alive()) {
		if (g_cfg.esp.croked_indicator)
			draw_bar(10, h / 2 - 100, g_csgo.m_clientstate()->m_nChokedCommands, 15.0f, L"choke");


		if (g_cfg.esp.extend_indicator)
			draw_bar(10, h / 2, (weapon->m_iClip1()) * 1000.0f, 400.0f, L"extend");

		if (g_cfg.esp.lc_indicator) {
			draw_bar(10, h / 2 + 100, last_fake_origin.DistToSqr(last_origin), 15.0f, L"lc");
		}
	}
	else if (local_player && g_csgo.m_engine()->IsInGame() && g_csgo.m_engine()->IsConnected()) {
		if (g_cfg.esp.croked_indicator)
			draw_bar(15, h / 2 - 150, 0.0f, 1.0f, L"choke");

		if (g_cfg.esp.extend_indicator)
			draw_bar(15, h / 2, 0.0f, 2.0f, L"extend");

		if (g_cfg.esp.lc_indicator)
			draw_bar(15, h / 2 + 150, 0.0f, 2.0f, L"lc");
	}
}

void otheresp::watermark() {
	const auto charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*abcdefghijklmnopqrstuvwxyz";
	static wchar_t buildDate[256];
	swprintf(buildDate, L"animecheat.pw", __DATE__);
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

	render::get().textgay(buildDate, 5, 10, g_cfg.menu.menu_theme[0], fonts[INDICATORFONT], HFONT_OFF_SCREEN);
}
void otheresp::lby_indicator() {
	auto local_player = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(g_csgo.m_engine()->GetLocalPlayer()));

	int screen_width, screen_height;
	g_csgo.m_engine()->GetScreenSize(screen_width, screen_height);

	float breaking_lby_fraction = fabs(math::normalize_yaw(g_ctx.m_globals.real_angles.y - local_player->m_flLowerBodyYawTarget())) / 180.f;
	color colour((1.f - breaking_lby_fraction) * 255.f, breaking_lby_fraction * 255.f, 0);
	Color clr(colour.RGBA[0], colour.RGBA[1], colour.RGBA[2]);
	if (local_player->m_fFlags() & FL_ONGROUND && local_player->m_vecVelocity().Length() < 0.1f) {


		if (local_player->m_bGunGameImmunity() || local_player->m_fFlags() & FL_FROZEN)
			return;

		static float add = 0.000f;
		if (add < 1)
			add += 0.01;

		if (add >= 1)
			add = 0.000f;
		draw_percent_circle(50, screen_height / 2 + 11, 8, add * 2, Color{ 0, 255, 0, 255 });


	}
	auto fps = static_cast<int>(1.f / g_csgo.m_globals()->m_frametime);

	render::get().text(fonts[INDICATORFONT], 2, screen_height / 2, clr, HFONT_OFF_SCREEN, "LBY");
	INetChannelInfo *nci = g_csgo.m_engine()->GetNetChannelInfo();

	if (&g_cfg.ragebot.lagcomp) {
		if (local_player->m_vecVelocity().Length() > 4096 / g_cfg.antiaim.fakelag_amount)
		{
			render::get().text(fonts[INDICATORFONT], 2, screen_height / 2 - 20, Color{ 255, 0, 0, 255 }, HFONT_OFF_SCREEN, "LC");
		}
		else
		{
			render::get().text(fonts[INDICATORFONT], 2, screen_height / 2 - 20, Color{ 0, 255, 0, 255 }, HFONT_OFF_SCREEN, "LC");

		}
	}
}


void otheresp::KnifeLeft()
{

	static auto left_knife = g_csgo.m_cvar()->FindVar("cl_righthand");

	if (!g_ctx.m_local || !g_ctx.m_local->is_alive())
	{
		left_knife->SetValue(1);
		return;
	}

	auto weapon = g_ctx.m_local->m_hActiveWeapon();
	if (!weapon) return;

	left_knife->SetValue(!weapon->is_knife());
}
void otheresp::desyncindicator() {
	auto local_player = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(g_csgo.m_engine()->GetLocalPlayer()));
	int screen_width, screen_height;
	g_csgo.m_engine()->GetScreenSize(screen_width, screen_height);
	if (g_ctx.send_packet) {
		char test[64];
		sprintf_s(test, " DESYNC: %.1f", g_ctx.m_globals.real_angles.y);
		render::get().text(fonts[INDICATORFONT], 5, screen_height / 2 + 62, Color{ 255, 255, 255, 255 }, HFONT_OFF_SCREEN, test);

	}
}

void otheresp::baimindicator() {
	auto local_player = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(g_csgo.m_engine()->GetLocalPlayer()));

	int screen_width, screen_height;
	g_csgo.m_engine()->GetScreenSize(screen_width, screen_height);

	render::get().text(fonts[INDICATORFONT], 5, screen_height / 2 + 80, Color{ 255, 255, 255, 255 }, HFONT_OFF_SCREEN, "baim enable");
	INetChannelInfo *nci = g_csgo.m_engine()->GetNetChannelInfo();


}

player_t* get_player(int userid) {
	int i = g_csgo.m_engine()->GetPlayerForUserID(userid);
	return static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(i));
}

void otheresp::hitmarker_event(IGameEvent * event) {

	auto local_player = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(g_csgo.m_engine()->GetLocalPlayer()));

	if (!strcmp(event->GetName(), "player_hurt")) {

		auto attacker = event->GetInt("attacker");
		auto victim = get_player(event->GetInt("userid"));

		if (g_csgo.m_engine()->GetPlayerForUserID(attacker) == g_csgo.m_engine()->GetLocalPlayer())

			hurt_time = g_csgo.m_globals()->m_curtime;
		impact_info best_impact;
		float best_impact_distance = -1;
		float time = g_csgo.m_globals()->m_curtime;


		for (int i = 0; i < impacts.size(); i++) {
			auto iter = impacts[i];
			if (time > iter.time + 1.f) {
				impacts.erase(impacts.begin() + i);
				continue;
			}
			vec3_t position = vec3_t(iter.x, iter.y, iter.z);
			vec3_t enemy_pos = victim->m_vecOrigin();
			float distance = position.DistTo(enemy_pos);
			if (distance < best_impact_distance || best_impact_distance == -1) {
				best_impact_distance = distance;
				best_impact = iter;
			}
		}

		if (best_impact_distance == -1)
			return;


		hitmarker_info info;
		info.impact = best_impact;
		info.alpha = 255;
		hitmarkers.push_back(info);

	}

	if (strstr(event->GetName(), "bullet_impact"))
	{
		auto entity = get_player(event->GetInt("userid"));
		if (!entity || entity != local_player)
			return;

		impact_info info;
		info.x = event->GetFloat("x");
		info.y = event->GetFloat("y");
		info.z = event->GetFloat("z");

		info.time = g_csgo.m_globals()->m_curtime;

		impacts.push_back(info);
	}

}


float flPlayerAlpha[65];
void otheresp::bomb_timer(IClientEntity* pBomb, weapon_t * e, Box box)
{
	if (!g_cfg.esp.bomb_timer)
		return;

	auto c4_timer = g_csgo.m_cvar()->FindVar("mp_c4timer")->GetFloat();
	auto color = Color(255, 255, 255, 255);
	render::get().text(fonts[ESPNAME], box.x + (box.w / 2), box.y + 13, color, flPlayerAlpha[pBomb->EntIndex()], "%s", c4_timer);
}

void otheresp::hitmarkerdynamic_paint() {

	float time = g_csgo.m_globals()->m_curtime;

	for (int i = 0; i < hitmarkers.size(); i++) {
		bool expired = time >= hitmarkers.at(i).impact.time + 2.f;

		if (expired)
			hitmarkers.at(i).alpha -= 1;

		if (expired && hitmarkers.at(i).alpha <= 0) {
			hitmarkers.erase(hitmarkers.begin() + i);
			continue;
		}

		Vector pos3D = Vector(hitmarkers.at(i).impact.x, hitmarkers.at(i).impact.y, hitmarkers.at(i).impact.z), pos2D;
		if (!math::world_to_screen(pos3D, pos2D))
			continue;

		auto linesize = 8;

		g_csgo.m_surface()->DrawSetColor(255, 255, 255, hitmarkers.at(i).alpha);
		g_csgo.m_surface()->DrawLine(pos2D.x - linesize, pos2D.y - linesize, pos2D.x - (linesize / 4), pos2D.y - (linesize / 4));
		g_csgo.m_surface()->DrawLine(pos2D.x - linesize, pos2D.y + linesize, pos2D.x - (linesize / 4), pos2D.y + (linesize / 4));
		g_csgo.m_surface()->DrawLine(pos2D.x + linesize, pos2D.y + linesize, pos2D.x + (linesize / 4), pos2D.y + (linesize / 4));
		g_csgo.m_surface()->DrawLine(pos2D.x + linesize, pos2D.y - linesize, pos2D.x + (linesize / 4), pos2D.y - (linesize / 4));
	}
}

void otheresp::spread_crosshair(IDirect3DDevice9* m_device) {
	if (!g_cfg.esp.show_spread)
		return;

	if (!g_ctx.m_local)
		return;

	if (!g_ctx.m_local->is_alive())
		return;

	if (!g_csgo.m_engine()->IsConnected() && !g_csgo.m_engine()->IsInGame())
		return;

	auto weapon = g_ctx.m_local->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	static float rot = 0.f;

	int w, h;

	g_csgo.m_engine()->GetScreenSize(w, h);

	w /= 2, h /= 2;

	int r, g, b;
	r = g_cfg.esp.show_spread_color.r();
	g = g_cfg.esp.show_spread_color.g();
	b = g_cfg.esp.show_spread_color.b();

	switch (g_cfg.esp.show_spread_type) {
	case 0:
		render::get().circle_dual_colour(w, h, weapon->get_innacuracy() * 500.0f, 0, 1, 50, D3DCOLOR_RGBA(r, g, b, 100), D3DCOLOR_RGBA(0, 0, 0, 0), m_device);
		break;
	case 1:
		render::get().circle_dual_colour(w, h, weapon->get_innacuracy() * 500.0f, rot, 1, 50, m_device);
		break;
	}

	rot += 0.5f;
	if (rot > 360.f)
		rot = 0.f;
}

void otheresp::spread_crosshair() {
	if (g_cfg.esp.show_spread != 1)
		return;

	if (!g_ctx.m_local)
		return;

	if (!g_ctx.m_local->is_alive())
		return;

	if (!g_csgo.m_engine()->IsConnected() && !g_csgo.m_engine()->IsInGame())
		return;

	auto weapon = g_ctx.m_local->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	if (weapon) {
		int screen_w, screen_h;
		g_csgo.m_engine()->GetScreenSize(screen_w, screen_h);
		int cross_x = screen_w / 2, cross_y = screen_h / 2;

		float recoil_step = screen_h / 180;

		cross_x -= (int)(g_ctx.m_local->m_aimPunchAngle().y * recoil_step);
		cross_y += (int)(g_ctx.m_local->m_aimPunchAngle().x * recoil_step);

		weapon->update_accuracy_penality();
		float inaccuracy = weapon->get_innacuracy();
		float spread = weapon->get_spread();

		float cone = inaccuracy * spread;
		cone *= screen_h * 2.0f;
		cone *= 90.f / 180;

		for (int seed{ }; seed < 512; ++seed) {
			math::random_seed(math::random_float(0, 512));

			float	rand_a = math::random_float(0.f, 1.f);
			float	pi_rand_a = math::random_float(0.f, 2.f * DirectX::XM_PI);
			float	rand_b = math::random_float(0.0f, 1.f);
			float	pi_rand_b = math::random_float(0.f, 2.f * DirectX::XM_PI);

			float spread_x = cos(pi_rand_a) * (rand_a * inaccuracy) + cos(pi_rand_b) * (rand_b * spread);
			float spread_y = sin(pi_rand_a) * (rand_a * inaccuracy) + sin(pi_rand_b) * (rand_b * spread);

			float max_x = cos(pi_rand_a) * cone + cos(pi_rand_b) * cone;
			float max_y = sin(pi_rand_a) * cone + sin(pi_rand_b) * cone;

			float step = screen_h / 180 * 90.f;
			int screen_spread_x = (int)(spread_x * step * 0.7f);
			int screen_spread_y = (int)(spread_y * step * 0.7f);

			float percentage = (rand_a * inaccuracy + rand_b * spread) / (inaccuracy + spread);

			render::get().rect(cross_x + screen_spread_x, cross_y + screen_spread_y, 1, 1,
				g_cfg.esp.show_spread_color);
		}
	}
}
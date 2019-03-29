#pragma once

#include "..\..\includes.hpp"
#include "..\..\structs.hpp"

class otheresp : public singleton< otheresp > {
public:
	void antiaim_indicator();
	void DrawZeusRange();
	void PreserveKillfeed();
	void penetration_reticle();
	void draw_percent_circle(int x, int y, int size, float amount_full, Color fill);
	void lby_indicator();
	void spectators();
	void desyncindicator();
	void KnifeLeft();
	void baimindicator();
	void indicators();
	void watermark();
	void hitmarkerdynamic_paint();
	void run_viewmodel();
	void hitmarker_event(IGameEvent * event);
	void spread_crosshair(IDirect3DDevice9*);
	void spread_crosshair();
	void bomb_timer(IClientEntity* pBomb, weapon_t * e, Box box);
	bool reset;
private:
	float hurt_time;
};

struct impact_info {
	float x, y, z;
	long long time;
};

struct hitmarker_info {
	impact_info impact;
	int alpha;
};

extern std::vector<impact_info> impacts;
extern std::vector<hitmarker_info> hitmarkers;
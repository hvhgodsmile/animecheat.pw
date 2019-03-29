#include "animation_state.h"

float spawn_time = 0.0f;
CBaseHandle * entity_handle = nullptr;
c_baseplayeranimationstate * server_animation_state = nullptr;

void animation_state::create_move() {
}


void animation_state::create_state(c_baseplayeranimationstate * state, player_t * player) {
	using Fn = void(__thiscall*)(c_baseplayeranimationstate*, player_t*);
	static auto fn = reinterpret_cast<Fn>(util::pattern_scan("client_panorama.dll", "55 8B EC 56 8B F1 B9 ? ? ? ? C7 46"));
	fn(state, player);
}

void animation_state::update_state(c_baseplayeranimationstate * state, vec3_t angles) {
	using Fn = void(__vectorcall*)(void *, void *, float, float, float, void *);
	static auto fn = reinterpret_cast<Fn>(util::pattern_scan("client_panorama.dll", "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24"));
	fn(state, nullptr, 0.0f, angles[1], angles[0], nullptr);
}

void animation_state::reset_state(c_baseplayeranimationstate * state) {
	using Fn = void(__thiscall*)(c_baseplayeranimationstate *);
	static auto fn = reinterpret_cast<Fn>(util::pattern_scan("client_panorama.dll", "56 6A 01 68 ? ? ? ? 8B F1"));
	fn(state);
}

c_baseplayeranimationstate * animation_state::animstate()
{
	return server_animation_state;
}










































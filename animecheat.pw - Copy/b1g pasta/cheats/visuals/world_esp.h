#pragma once

#include "..\..\includes.hpp"
#include "..\..\structs.hpp"

class weapon_t;
class Box;

class worldesp : public singleton< worldesp > {
public:
	void paint_traverse();

	void grenades(weapon_t * m_nade, Box box);
	void weapons(weapon_t * m_weapon, Box box);
	void DrawBombus(player_t* e);
	void DrawBombPlanted(player_t* e);
	void DrawBomb(player_t* e);
private:
	player_t * BombCarrier;
};
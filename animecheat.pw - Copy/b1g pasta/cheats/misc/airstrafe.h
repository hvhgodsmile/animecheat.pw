#pragma once

#include "..\..\includes.hpp"

class airstrafe : public singleton< airstrafe > {
public:
	void create_move();
	float get_move_angle(float speed) {
		auto move_angle = RAD2DEG(asin(15.f / speed));

		if (!isfinite(move_angle) || move_angle > 90.f)
			move_angle = 90.f;
		else if (move_angle < 0.f)
			move_angle = 0.f;

		return move_angle;
	}
};
#include "..\cmenu.hpp"

C_CheckBox::C_CheckBox(std::string n_label, bool * n_state, std::vector< C_ControlFlag * > n_flags) {
	parent = g_cfg.menu.last_group;
	label = n_label;
	state = n_state;

	flags = n_flags;

	area.h = 11;

	parent->add_control(this);
}

void C_CheckBox::draw() {
	render::get().gradient(area.x, area.y, 8, 8, Color{ 62, 62, 62, 255 }, Color{ 55, 55, 55, 255 }, GRADIENT_VERTICAL);

	if (*state) {
		render::get().gradient(area.x, area.y, 8, 8, g_cfg.menu.menu_theme[0], g_cfg.menu.menu_theme[1], GRADIENT_VERTICAL);
	}

	render::get().rect(area.x, area.y, 8, 8, Color::Black);

	render::get().text(fonts[TAHOMA12], area.x + 15, area.y + 6, Color::White, HFONT_CENTERED_Y, label.c_str());
}

void C_CheckBox::update() {
}

void C_CheckBox::click() {
	*state = !(*state);
}








































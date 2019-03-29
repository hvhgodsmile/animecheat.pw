#include "cmenu.hpp"

menu::menu() {
	area = rect_t(200, 200, 600, 500);
}

void menu::render() {
	// lamba definitions
	auto top_bar = [this]() -> void {
		rect_t top_area = rect_t(
			area.x - 95, area.y - 40,
			area.w + 95, 40
		);

	};


	render::get().rect_filled(area.x - 99, area.y - 4, area.w + 103, area.h + 8, Color(40, 40, 40));
	render::get().rect(area.x - 96, area.y - 1, area.w + 97, area.h + 2, Color(60, 60, 60));
	render::get().rect(area.x - 100, area.y - 5, area.w + 105, area.h + 10, Color(60, 60, 60));
	render::get().rect(area.x - 101, area.y - 6, area.w + 106, area.h + 11, Color(0, 0, 0));


	//render::get().rect_filled(area.x - 95, area.y, area.w + 95, area.h, Color(21, 21, 21));

	render::get().rect_filled(area.x - 95, area.y, area.w + 95, area.h, Color(21, 21, 21));

	rect_t main_tab_area = rect_t(
		area.x - 85, area.y + 10,
		95, area.h - 10
	);

	render::get().rect_filled(main_tab_area.x, main_tab_area.y, main_tab_area.w, area.h - 20, Color(31, 31, 31));
	render::get().rect(main_tab_area.x, main_tab_area.y, main_tab_area.w, area.h - 20, Color(81, 81, 81));


	// + 20

	render::get().rect_filled(main_tab_area.x + 115, main_tab_area.y, main_tab_area.w + 465, area.h - 20, Color(31, 31, 31));
	render::get().rect(main_tab_area.x + 115, main_tab_area.y, main_tab_area.w + 465, area.h - 20, Color(81, 81, 81));


	for (int i = 0; i < tabs.size(); i++) {
		tabs[i]->paint();
	}

	render::get().rect(area.x - 94, area.y + 2, area.w + 93, 1, Color(12, 12, 12));
	//top_shadow( );



}

void menu::update() {
	// update input
	poll_keyboard();

	// update menu state
	int gradient[3] = {
		g_cfg.menu.menu_theme[0].r() - 20,
		g_cfg.menu.menu_theme[0].g() - 20,
		g_cfg.menu.menu_theme[0].b() - 20
	};

	for (int i = 0; i < 3; i++) {
		if (gradient[i] < 42) {
			gradient[i] = g_cfg.menu.menu_theme[0][i] + 20;
		}
	}

	g_cfg.menu.menu_theme[1] = Color(gradient[0], gradient[1], gradient[2]);

	if (key_press(g_csgo.m_inputsys()->ButtonCodeToVirtualKey(g_cfg.menu.menu_bind))) {
		toggle();

		g_csgo.m_inputsys()->EnableInput(!active());
		g_csgo.m_inputsys()->ResetInputState();
	}

	if (!active()) {
		return;
	}

	// update our mouse and window position
	poll_mouse();

	// draw our menu
	render();

	// tab clicks
	rect_t main_tab_area = rect_t(
		area.x - 95, area.y,
		95, area.h
	);

	for (int i = 0; i < tabs.size(); i++) {
		tabs[i]->update();
	}

	if (animating) {
		if (animate_direction == TAB_ANIMATION_DOWN) {
			if ((main_tab_area.y + animation_offset) < animation_destination) {
				animation_offset += 2;
			}
			else {
				animating = false;
			}
		}
		else {
			if ((main_tab_area.y + animation_offset) > animation_destination) {
				animation_offset -= 2;
			}
			else {
				animating = false;
			}
		}
	}

	// draw mouse
	c_mouse.paint();
}

void menu::poll_keyboard() {
	// save old states
	std::copy(m_keystate, m_keystate + 255, m_oldstate);

	// simple keyboard iteration
	for (auto n = 0; n < 255; ++n) {
		// update current state
		m_keystate[n] = GetAsyncKeyState(n);
	}
}

void menu::poll_mouse() {

	rect_t top_area = rect_t(
		area.x - 95, area.y - 40,
		area.w + 95, 40
	);

	if (dragging && !GetAsyncKeyState(VK_LBUTTON)) {
		dragging = false;
	}

	if (GetAsyncKeyState(VK_LBUTTON) && top_area.contains_point(c_mouse.cursor)) {
		dragging = true;
	}

	if (dragging) {
		drag_x = c_mouse.cursor.x - area.x;
		drag_y = c_mouse.cursor.y - area.y;

		GetCursorPos(&c_mouse.cursor);

		area.x = c_mouse.cursor.x - drag_x;
		area.y = c_mouse.cursor.y - drag_y;
	}
	else {
		GetCursorPos(&c_mouse.cursor);
	}
}

void menu::Mouse_t::paint() {

}








































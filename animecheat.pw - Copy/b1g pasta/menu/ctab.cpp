#include "cmenu.hpp"

C_Tab::C_Tab(menu * n_parent, int n_index, std::string n_icon, std::string n_name, std::vector< subtab_t > n_subtabs) {
	parent = n_parent;
	index = n_index;
	icon = n_icon;
	name = n_name;

	selected_sub = -1;

	if (n_subtabs.size()) {
		subtabs = n_subtabs;
		selected_sub = subtabs[0].index;
	}

	n_parent->add_tab(this);

	g_cfg.menu.tab_sub = -1;
	g_cfg.menu.last_tab = this;
}

C_Tab::subtab_t::subtab_t(char * _name, int _index) {
	name = _name;
	index = _index;
	font = fonts[TAHOMA12];
}

C_Tab::subtab_t::subtab_t(char * _name, int _index, vgui::HFont _font) {
	name = _name;
	index = _index;
	font = _font;
}

void C_Tab::paint_subtabs() {
	if (subtabs.size()) {
		rect_t area = rect_t(
			menu::get().area.x + 2,
			menu::get().area.y + 8,
			menu::get().area.w - 3,
			30
		);

		float sub_width = area.w / subtabs.size();

		render::get().rect_filled(area.x, area.y, area.w, 3, Color(20, 20, 20));
		render::get().rect(area.x, area.y, area.w, 3, Color::Black);

		render::get().rect_filled(area.x + (sub_width * selected_sub) + 1, area.y + 1, sub_width - 2, 3, Color(31, 31, 31));
		render::get().rect_filled(area.x + animation_offset, area.y + 26, sub_width, 3, g_cfg.menu.menu_theme[0]);

		for (int i = 0; i < subtabs.size(); i++) {
			int text_x = area.x + (sub_width * i) + (sub_width / 2);

			int add = 0;

			if (subtabs[i].font == fonts[SUBTABWEAPONS])
				add += 3;

			render::get().text(subtabs[i].font, text_x, area.y + 13 + add, selected_sub == subtabs[i].index ? Color::White : Color(153, 153, 153), HFONT_CENTERED_X | HFONT_CENTERED_Y, subtabs[i].name);
		}
	}
}

void C_Tab::update_subtabs() {
	if (subtabs.size()) {
		rect_t area = rect_t(
			menu::get().area.x + 8,
			menu::get().area.y + 8,
			menu::get().area.w - 16,
			30
		);

		float sub_width = area.w / subtabs.size();

		for (int i = 0; i < subtabs.size(); i++) {
			rect_t tab_area = rect_t(
				area.x + (sub_width * i),
				area.y, sub_width, 26
			);

			if (menu::get().key_press(VK_LBUTTON) && tab_area.contains_point(menu::get().cursor())) {
				if (!animating && selected_sub != subtabs[i].index) {
					animating = true;
					animation_direction = (subtabs[i].index < selected_sub) ? SUBTAB_ANIMATION_LEFT : SUBTAB_ANIMATION_RIGHT;
					animation_destination = tab_area.x;
					selected_sub = subtabs[i].index;

					reset_inputblock = true;
				}
			}
		}

		for (int i = 0; i < 9; i++) {
			if (!animating)
				break;

			if (animation_direction == SUBTAB_ANIMATION_RIGHT) {
				if ((area.x + animation_offset) < animation_destination) {
					animation_offset += 1;
					math::clamp(animation_offset, 0.0f, area.x - animation_destination);
				}
				else {
					animating = false;
				}
			}
			else {
				if ((area.x + animation_offset) > animation_destination) {
					animation_offset -= 1;
					math::clamp(animation_offset, area.x - animation_destination, (float)area.w);
				}
				else {
					animating = false;
				}
			}
		}
	}
}

void C_Tab::paint() {

	Color tabtext(54, 54, 54);
	rect_t tab_area = rect_t(
		menu::get().area.x - 85,
		menu::get().area.y + (index * 20) - 10,
		95, 20
	);

	rect_t tab_bar = rect_t(
		menu::get().area.x - 10,
		menu::get().area.y + menu::get().animation_offset,
		10, 20
	);

	if (index == menu::get().selected_tab) {

		paint_subtabs();
	}

	int icon_width = render::get().text_width(fonts[VITALITYFONT], icon.c_str());
	render::get().text(fonts[VITALITYFONT], tab_area.x + 5, tab_area.y + 10, Color::White, HFONT_CENTERED_Y, icon.c_str());
	render::get().text(fonts[TAHOMA12], tab_area.x + 8 + icon_width, tab_area.y + 10, index == menu::get().selected_tab ? Color::White : Color(153, 153, 153), HFONT_CENTERED_Y, name.c_str());
}

void C_Tab::update() {
	rect_t main_tab_area = rect_t(
		menu::get().area.x - 85, menu::get().area.y,
		95, menu::get().area.h
	);

	rect_t tab_area = rect_t(
		main_tab_area.x,
		main_tab_area.y + (index * 20) - 10,
		95, 20
	);

	if (tab_area.contains_point(menu::get().cursor()) && menu::get().key_press(VK_LBUTTON)) {
		if (!menu::get().animating && index != menu::get().selected_tab) {
			menu::get().animating = true;
			menu::get().animate_direction = (index > menu::get().selected_tab) ? TAB_ANIMATION_DOWN : TAB_ANIMATION_UP;
			menu::get().animation_destination = tab_area.y;
			menu::get().selected_tab = index;
		}
	}

	if (index == menu::get().selected_tab) {
		update_subtabs();
	}

	// controls
	if (index == menu::get().selected_tab) {
		for (int c = group_boxes.size() - 1; c >= 0; c--) {
			if (group_boxes[c]->get_parent_sub() == selected_sub)
				group_boxes[c]->update(menu::get().area);
		}
	}
}








































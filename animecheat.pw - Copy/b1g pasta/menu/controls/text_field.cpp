#include "..\cmenu.hpp"

extern char* lowercase[254];
extern char* uppercase[254];

C_TextField::C_TextField(std::string n_label, std::string * n_input) {
	parent = g_cfg.menu.last_group;
	label = n_label;
	input = n_input;

	area.h = 12;

	if (n_label != "") {
		height_offset = 14;
		area.h += 14;
	}

	parent->add_control(this);
}

void C_TextField::draw() {
	rect_t n_area = rect_t(
		area.x,
		area.y + height_offset,
		area.w,
		15
	);

	if (label != "")
		render::get().text(fonts[TAHOMA12], area.x, area.y, Color::White, HFONT_CENTERED_NONE, label.c_str());

	float
		left_brack_size = render::get().text_width(fonts[TAHOMA12], "["),
		right_brack_size = render::get().text_width(fonts[TAHOMA12], "]");

	render::get().text(fonts[TAHOMA12], n_area.x, n_area.y, Color::White, HFONT_CENTERED_NONE, "[");
	render::get().text(fonts[TAHOMA12], n_area.x + n_area.w - right_brack_size, n_area.y, Color::White, HFONT_CENTERED_NONE, "]");

	std::string to_render = *input;

	if (GetTickCount64() >= next_blink) {
		next_blink = GetTickCount64() + 800;
	}

	if (taking_input && GetTickCount64() > (next_blink - 400)) {
		to_render += "_";
	}

	render::get().text(fonts[TAHOMA12], n_area.x + left_brack_size, n_area.y, Color::White, HFONT_CENTERED_NONE, to_render.c_str());

}

void C_TextField::update() {
	if (!taking_input) {
		return;
	}

	std::string str = *input;

	for (int i = 0; i < 255; i++) {
		if (menu::get().key_press(i)) {
			if (i == VK_ESCAPE || i == VK_RETURN || i == VK_INSERT) {
				taking_input = false;
				return;
			}

			if (i == VK_BACK && strlen(str.c_str()) != 0) {
				*input = str.substr(0, strlen(str.c_str()) - 1);
			}

			if (strlen(str.c_str()) < 42 && i != NULL && uppercase[i] != nullptr) {
				if (GetAsyncKeyState(VK_SHIFT)) {
					*input = str + uppercase[i];
				}
				else {
					*input = str + lowercase[i];
				}

				return;
			}

			if (strlen(str.c_str()) < 42 && i == 32) {
				*input = str + " ";
				return;
			}
		}
	}
}

void C_TextField::click() {
	taking_input = !taking_input;
}

char* lowercase[254] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x",
"y", "z", nullptr, nullptr, nullptr, nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6",
"7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, ";", "+", ",", "-", ".", "/?", "~", nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "[", "\\", "]", "'", nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

char* uppercase[254] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X",
"Y", "Z", nullptr, nullptr, nullptr, nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6",
"7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, ";", "+", ",", "_", ".", "?", "~", nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "{", "|", "}", "\"", nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };








































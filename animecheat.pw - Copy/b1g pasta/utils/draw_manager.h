#pragma once

#include "..\includes.hpp"
#include "singleton.h"

enum FontCenteringFlags {
	HFONT_CENTERED_NONE = (1 << 0),
	HFONT_CENTERED_X = (1 << 1),
	HFONT_CENTERED_Y = (1 << 2),
	HFONT_OFF_SCREEN,
};

enum GradientType {
	GRADIENT_HORIZONTAL = 0,
	GRADIENT_VERTICAL
};

enum FONT_INDEX {
	SUBLIMINALTEXT,
	SUBLIMINALBOLD,
	ESPFLAG,
	TAHOMA12,
	MENUNAME,
	VERDANA12,
	VERDANA12OUT,
	VERDANA12BOLD,
	VERDANA12BOLDOUT,
	ESPNAME,
	ESPHEALTH,
	WEAPONICONS,
	BOMBICON,
	SUBTABWEAPONS,
	INDICATORFONT,
	VITALITYFONT,
	TABFONT,
	SUBLIMINALFONT,
	FONT_MAX
};

extern vgui::HFont fonts[FONT_MAX];

template<typename T>
class singleton;

struct CUSTOMVERTEX {
	FLOAT x, y, z;
	FLOAT rhw;
	DWORD color;
};

class render : public singleton< render > {
private:
	float alpha_factor = 1.f;
	LPDIRECT3DDEVICE9 m_device;
	D3DVIEWPORT9      m_viewport;
public:
	bool initalized = false;
	void setup();

	float get_alphafactor();
	void set_alphafactor(float factor);
	void setup_states() const;
	void restore_objects(LPDIRECT3DDEVICE9 m_device);
	void textbullshit(vgui::HFont font, int x, int y, Color color, DWORD flags, std::string szText, wchar_t * msg, ...);
	void custom_box(int x, int y, int w, int h);
	void invalidate_objects();
	void rect(int x, int y, int w, int h, Color color);
	void draw_filled_rect(int x, int y, int w, int h, Color color);
	void rect1(int x, int y, int w, int h, Color color);
	void rect_filled(int x, int y, int w, int h, Color color);
	void Line(vec3_t start_pos, vec3_t end_pos, Color col);
	void gradient(int x, int y, int w, int h, Color first, Color second, GradientType type);
	void rounded_box(int x, int y, int w, int h, int points, int radius, Color color);
	void gradient_horizontal(int x, int y, int w, int h, Color c1, Color c2);
	void textT(int x, int y, int font, Color color, const char * _Input, ...);
	void draw_wide_string(bool center, unsigned long font, int x, int y, Color color, wchar_t* str);
	void initalize_objects(LPDIRECT3DDEVICE9 m_device);
	void circle_dual_colour(float x, float y, float rad, float rotate, int type, int resolution, IDirect3DDevice9 * m_device);
	void circle_dual_colour(float x, float y, float rad, float rotate, int type, int resolution, DWORD color, DWORD color2, IDirect3DDevice9 * m_device);
	void circle(int x, int y, int points, int radius, Color color);
	void circle_filled(int x, int y, int points, int radius, Vector2D point_one, Vector2D point_two, Vector2D point_three, Color color);
	void triangle(Vector2D point_one, Vector2D point_two, Vector2D point_three, Color color);
	void line(int x, int y, int x2, int y2, Color color);
	void textonlybomb(int x, int y, const char* _Input, int font, Color color);
	void Line(Vector2D start_pos, Vector2D end_pos, Color col);
	void text(vgui::HFont font, int x, int y, Color color, DWORD flags, const char* msg, ...);
	void textgay(const wchar_t * buf, int pos_x, int pos_y, Color color, vgui::HFont font, DWORD flags);
	void nonamegey(int x, int y, Color color, DWORD font, const char* text);
	void Line(int x0, int y0, int x1, int y1, Color col);
	void wtext(vgui::HFont font, int x, int y, Color color, DWORD flags, wchar_t* msg);
	float text_width(vgui::HFont font, const char* msg, ...);
	void xyinyatext(int x, int y, Color color, DWORD font, const char * text, ...);
	void Textf(int x, int y, Color color, DWORD font, const char * fmt, ...);
	RECT viewport();
};
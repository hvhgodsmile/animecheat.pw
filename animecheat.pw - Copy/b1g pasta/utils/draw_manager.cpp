#include "draw_manager.h"

vgui::HFont fonts[FONT_MAX];

void render::setup() {
	static auto create_font = [](char * name, int size, int weight, DWORD flags) -> vgui::HFont {
		vgui::HFont font = g_csgo.m_surface()->FontCreate();
		g_csgo.m_surface()->SetFontGlyphSet(font, name, size, weight, 0, 0, flags);

		return font;
	};

	fonts[SUBLIMINALTEXT] = create_font("Tahoma", 12, FW_NORMAL, FONTFLAG_OUTLINE);
	fonts[SUBLIMINALBOLD] = create_font("Tahoma", 12, FW_NORMAL, FONTFLAG_OUTLINE);
	fonts[ESPFLAG] = create_font("Tahoma", 12, FW_THIN, FONTFLAG_OUTLINE);
	fonts[TAHOMA12] = create_font("Tahoma", 12, FW_NORMAL, FONTFLAG_OUTLINE);
	fonts[MENUNAME] = create_font("Tahoma", 15, FW_THIN, FONTFLAG_OUTLINE);
	fonts[VERDANA12] = create_font("Tahoma", 12, FW_NORMAL, FONTFLAG_NONE);
	fonts[VERDANA12OUT] = create_font("Tahoma", 12, FW_NORMAL, FONTFLAG_OUTLINE);
	fonts[VERDANA12BOLD] = create_font("Tahoma", 12, FW_BOLD, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	fonts[VERDANA12BOLDOUT] = create_font("Tahoma", 12, FW_BOLD, FONTFLAG_ANTIALIAS | FONTFLAG_OUTLINE);
	fonts[ESPNAME] = create_font("Tahoma", 12, FW_NORMAL, FONTFLAG_OUTLINE);
	fonts[ESPHEALTH] = create_font("Tahoma", 12, FW_THIN, FONTFLAG_OUTLINE);
	fonts[WEAPONICONS] = create_font("undefeated", 10, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_OUTLINE);
	fonts[BOMBICON] = create_font("undefeated", 20, FW_MEDIUM, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);
	fonts[SUBTABWEAPONS] = create_font("undefeated", 12, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_OUTLINE);
	fonts[INDICATORFONT] = create_font("Tahoma", 15, FW_SEMIBOLD, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW | FONTFLAG_OUTLINE);
	fonts[TABFONT] = create_font("subliminal", 12, FW_NORMAL, FONTFLAG_ANTIALIAS);
	fonts[VITALITYFONT] = create_font("subliminal", 12, FW_NORMAL, FONTFLAG_ANTIALIAS);
	fonts[SUBLIMINALFONT] = create_font("subliminal", 40, FW_NORMAL, FONTFLAG_ANTIALIAS);
}

void render::set_alphafactor(float factor) {
	alpha_factor = factor;
}

float render::get_alphafactor() {
	return alpha_factor;
}
void render::rect1(int x, int y, int w, int h, Color color)
{
	g_csgo.m_surface()->DrawSetColor(color);
	g_csgo.m_surface()->DrawFilledRect(x, y, x + w, y + h);
}

void render::draw_filled_rect(int x, int y, int w, int h, Color color) {
	g_csgo.m_surface()->DrawSetColor(color);
	g_csgo.m_surface()->DrawFilledRect(x, y, x + w, y + h);
}


void render::rect(int x, int y, int w, int h, Color color) {
	color.SetAlpha(static_cast<int>(color.a() * alpha_factor));

	g_csgo.m_surface()->DrawSetColor(color);
	g_csgo.m_surface()->DrawOutlinedRect(x, y, x + w, y + h);
}

void render::rect_filled(int x, int y, int w, int h, Color color) {
	color.SetAlpha(static_cast<int>(color.a() * alpha_factor));

	g_csgo.m_surface()->DrawSetColor(color);
	g_csgo.m_surface()->DrawFilledRect(x, y, x + w, y + h);
}

auto box = [](int x, int y, int w, int h, Color c) {
	render::get().line(x, y, x, y + h, c);
	render::get().line(x, y + h, x + w + 1, y + h, c);
	render::get().line(x + w, y, x + w, y + h, c);
	render::get().line(x, y, x + w, y, c);
};
void render::textbullshit(vgui::HFont font, int x, int y, Color color, DWORD flags, std::string szText, wchar_t* msg, ...)
{
	color.SetAlpha(static_cast<int>(color.a() * alpha_factor));

	wchar_t buffer[256];
	va_list args;

	va_start(args, msg);
	wvsprintfW(buffer, msg, args);
	va_end(args);

	int width, height;
	g_csgo.m_surface()->GetTextSize(font, buffer, width, height);

	if (!(flags & HFONT_CENTERED_NONE)) {
		if (flags & HFONT_CENTERED_X)
			x -= width * 0.5f;

		if (flags & HFONT_CENTERED_Y)
			y -= height * 0.5f;
	}

	g_csgo.m_surface()->DrawSetTextFont(font);
	g_csgo.m_surface()->DrawSetTextColor(color);
	g_csgo.m_surface()->DrawSetTextPos(x, y);
	g_csgo.m_surface()->DrawPrintText(buffer, wcslen(buffer));
}
void render::custom_box(int x, int y, int w, int h)
{
	render::get().rect_filled(x, y, w, h, Color(30, 30, 30));
	box(x - 0, y - 0, w + 0 * 2, h + 0 * 2, Color::Gray_v2);
	box(x - 1, y - 1, w + 1 * 2, h + 1 * 2, Color::Black);
	box(x - 2, y - 2, w + 2 * 2, h + 2 * 2, Color::Gray_v3);
	box(x - 3, y - 3, w + 3 * 2, h + 3 * 2, Color::Gray_v3);
	box(x - 4, y - 4, w + 4 * 2, h + 4 * 2, Color::Gray_v3);
	box(x - 5, y - 5, w + 5 * 2, h + 5 * 2, Color::Black);
	box(x - 6, y - 6, w + 6 * 2, h + 6 * 2, Color::Gray_v2);
}
void render::setup_states() const {
	this->m_device->SetVertexShader(nullptr);
	this->m_device->SetPixelShader(nullptr);
	this->m_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	this->m_device->SetRenderState(D3DRS_LIGHTING, FALSE);
	this->m_device->SetRenderState(D3DRS_FOGENABLE, FALSE);
	this->m_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	this->m_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	this->m_device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	this->m_device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
	this->m_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	this->m_device->SetRenderState(D3DRS_STENCILENABLE, FALSE);

	this->m_device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	this->m_device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

	this->m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	this->m_device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	this->m_device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
	this->m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	this->m_device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA);
	this->m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	this->m_device->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);

	this->m_device->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
	this->m_device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN |
		D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
}
void render::initalize_objects(LPDIRECT3DDEVICE9 m_device) {
	this->m_device = m_device;

	this->m_device->GetViewport(&m_viewport);
}
void render::invalidate_objects() {
	this->m_device = nullptr;
}
void render::restore_objects(LPDIRECT3DDEVICE9 m_device) {
	this->m_device = m_device;
	this->m_device->GetViewport(&m_viewport);
}
void render::circle_dual_colour(float x, float y, float rad, float rotate, int type, int resolution, IDirect3DDevice9* m_device)
{
	LPDIRECT3DVERTEXBUFFER9 g_pVB2;

	std::vector<CUSTOMVERTEX> circle(resolution + 2);

	float angle = rotate * D3DX_PI / 180, pi = D3DX_PI;

	if (type == 1)
		pi = D3DX_PI; // Full circle
	if (type == 2)
		pi = D3DX_PI / 2; // 1/2 circle
	if (type == 3)
		pi = D3DX_PI / 4; // 1/4 circle

	pi = D3DX_PI / type; // 1/4 circle

	circle[0].x = x;
	circle[0].y = y;
	circle[0].z = 0;
	circle[0].rhw = 1;
	circle[0].color = D3DCOLOR_RGBA(0, 0, 0, 0);

	float hue = 0.f;

	for (int i = 1; i < resolution + 2; i++)
	{
		circle[i].x = (float)(x - rad * cos(pi*((i - 1) / (resolution / 2.0f))));
		circle[i].y = (float)(y - rad * sin(pi*((i - 1) / (resolution / 2.0f))));
		circle[i].z = 0;
		circle[i].rhw = 1;

		auto clr = Color::FromHSB(hue, 1.f, 1.f);
		circle[i].color = D3DCOLOR_RGBA(clr.r(), clr.g(), clr.b(), 150);
		hue += 0.02;
	}

	// Rotate matrix
	int _res = resolution + 2;
	for (int i = 0; i < _res; i++)
	{
		float Vx1 = x + (cosf(angle) * (circle[i].x - x) - sinf(angle) * (circle[i].y - y));
		float Vy1 = y + (sinf(angle) * (circle[i].x - x) + cosf(angle) * (circle[i].y - y));

		circle[i].x = Vx1;
		circle[i].y = Vy1;
	}

	m_device->CreateVertexBuffer((resolution + 2) * sizeof(CUSTOMVERTEX), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &g_pVB2, NULL);

	VOID* pVertices;
	g_pVB2->Lock(0, (resolution + 2) * sizeof(CUSTOMVERTEX), (void**)&pVertices, 0);
	memcpy(pVertices, &circle[0], (resolution + 2) * sizeof(CUSTOMVERTEX));
	g_pVB2->Unlock();

	m_device->SetTexture(0, NULL);
	m_device->SetPixelShader(NULL);
	m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_device->SetStreamSource(0, g_pVB2, 0, sizeof(CUSTOMVERTEX));
	m_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	m_device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, resolution);
	if (g_pVB2 != NULL)
		g_pVB2->Release();
}
void render::circle_dual_colour(float x, float y, float rad, float rotate, int type, int resolution, DWORD color, DWORD color2, IDirect3DDevice9* m_device)
{
	LPDIRECT3DVERTEXBUFFER9 g_pVB2;

	std::vector<CUSTOMVERTEX> circle(resolution + 2);

	float angle = rotate * D3DX_PI / 180, pi = D3DX_PI;

	if (type == 1)
		pi = D3DX_PI; // Full circle
	if (type == 2)
		pi = D3DX_PI / 2; // 1/2 circle
	if (type == 3)
		pi = D3DX_PI / 4; // 1/4 circle

	pi = D3DX_PI / type; // 1/4 circle

	circle[0].x = x;
	circle[0].y = y;
	circle[0].z = 0;
	circle[0].rhw = 1;
	circle[0].color = color2;

	for (int i = 1; i < resolution + 2; i++)
	{
		circle[i].x = (float)(x - rad * cos(pi*((i - 1) / (resolution / 2.0f))));
		circle[i].y = (float)(y - rad * sin(pi*((i - 1) / (resolution / 2.0f))));
		circle[i].z = 0;
		circle[i].rhw = 1;
		circle[i].color = color;
	}

	// Rotate matrix
	int _res = resolution + 2;
	for (int i = 0; i < _res; i++)
	{
		circle[i].x = x + cos(angle)*(circle[i].x - x) - sin(angle)*(circle[i].y - y);
		circle[i].y = y + sin(angle)*(circle[i].x - x) + cos(angle)*(circle[i].y - y);
	}

	m_device->CreateVertexBuffer((resolution + 2) * sizeof(CUSTOMVERTEX), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &g_pVB2, NULL);

	VOID* pVertices;
	g_pVB2->Lock(0, (resolution + 2) * sizeof(CUSTOMVERTEX), (void**)&pVertices, 0);
	memcpy(pVertices, &circle[0], (resolution + 2) * sizeof(CUSTOMVERTEX));
	g_pVB2->Unlock();

	m_device->SetTexture(0, NULL);
	m_device->SetPixelShader(NULL);
	m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_device->SetStreamSource(0, g_pVB2, 0, sizeof(CUSTOMVERTEX));
	m_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	m_device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, resolution);
	if (g_pVB2 != NULL)
		g_pVB2->Release();
}

void render::gradient(int x, int y, int w, int h, Color first, Color second, GradientType type) {
	first.SetAlpha(static_cast<int>(first.a() * alpha_factor));
	second.SetAlpha(static_cast<int>(second.a() * alpha_factor));

	auto filled_rect_fade = [&](bool reversed, float alpha) {
		using Fn = void(__thiscall*)(VOID*, int, int, int, int, unsigned int, unsigned int, bool);
		call_virtual< Fn >(g_csgo.m_surface(), 123) (
			g_csgo.m_surface(), x, y, x + w, y + h,
			reversed ? alpha : 0,
			reversed ? 0 : alpha,
			type == GRADIENT_HORIZONTAL);
	};

	static auto blend = [](const Color & first, const Color & second, float t) -> Color {
		return Color(
			first.r() + t * (second.r() - first.r()),
			first.g() + t * (second.g() - first.g()),
			first.b() + t * (second.b() - first.b()),
			first.a() + t * (second.a() - first.a()));
	};

	if (first.a() == 255 || second.a() == 255) {
		g_csgo.m_surface()->DrawSetColor(blend(first, second, 0.5f));
		g_csgo.m_surface()->DrawFilledRect(x, y, x + w, y + h);
	}

	g_csgo.m_surface()->DrawSetColor(first);
	filled_rect_fade(true, first.a());

	g_csgo.m_surface()->DrawSetColor(second);
	filled_rect_fade(false, second.a());
}
void render::textT(int x, int y, int font, Color color, const char* _Input, ...)
{
	int apple = 0;
	char Buffer[2048] = { '\0' };
	va_list Args;
	va_start(Args, _Input);
	vsprintf_s(Buffer, _Input, Args);
	va_end(Args);
	size_t Size = strlen(Buffer) + 1;
	wchar_t* WideBuffer = new wchar_t[Size];
	mbstowcs_s(0, WideBuffer, Size, Buffer, Size - 1);

	g_csgo.m_surface()->DrawSetTextColor(color);
	g_csgo.m_surface()->DrawSetTextFont(font);
	g_csgo.m_surface()->DrawSetTextPos(x, y);
	g_csgo.m_surface()->DrawPrintText(WideBuffer, wcslen(WideBuffer));
}


void render::draw_wide_string(bool center, unsigned long font, int x, int y, Color c, wchar_t* str) {
	if (center) {
		int wide = 0, tall = 0;
		g_csgo.m_surface()->GetTextSize(font, str, wide, tall);
		x -= wide / 2;
		y -= tall / 2;
	}

	g_csgo.m_surface()->DrawSetTextColor(c);
	g_csgo.m_surface()->DrawSetTextFont(font);
	g_csgo.m_surface()->DrawSetTextPos(x, y);
	g_csgo.m_surface()->DrawPrintText(str, (int)wcslen(str));
}
void render::gradient_horizontal(int x, int y, int w, int h, Color c1, Color c2)
{
	render::rect1(x, y, w, h, c1);
	BYTE first = c2.r();
	BYTE second = c2.g();
	BYTE third = c2.b();
	for (int i = 0; i < w; i++)
	{
		float fi = i, fw = w;
		float a = fi / fw;
		DWORD ia = a * 255;
		render::rect1(x + i, y, 1, h, Color(first, second, third, ia));
	}
}
void render::rounded_box(int x, int y, int w, int h, int points, int radius, Color color) {
	color.SetAlpha(static_cast<int>(color.a() * alpha_factor));

	Vertex_t *round = new Vertex_t[4 * points];

	for (int i = 0; i < 4; i++) {
		int _x = x + ((i < 2) ? (w - radius) : radius);
		int _y = y + ((i % 3) ? (h - radius) : radius);

		float a = 90.f * i;

		for (int j = 0; j < points; j++) {
			float _a = DEG2RAD(a + (j / (float)(points - 1)) * 90.f);

			round[(i * points) + j] = Vertex_t(Vector2D(_x + radius * sin(_a), _y - radius * cos(_a)));
		}
	}

	static int Texture = g_csgo.m_surface()->CreateNewTextureID(true);
	unsigned char buffer[4] = { 255, 255, 255, 255 };

	g_csgo.m_surface()->DrawSetTextureRGBA(Texture, buffer, 1, 1);
	g_csgo.m_surface()->DrawSetColor(color);
	g_csgo.m_surface()->DrawSetTexture(Texture);

	g_csgo.m_surface()->DrawTexturedPolygon(4 * points, round);
}

void render::circle(int x, int y, int points, int radius, Color color) {
	color.SetAlpha(static_cast<int>(color.a() * alpha_factor));

	g_csgo.m_surface()->DrawSetColor(color);
	g_csgo.m_surface()->DrawOutlinedCircle(x, y, radius, points);
}
//here
void render::triangle(Vector2D point_one, Vector2D point_two, Vector2D point_three, Color color) {
	color.SetAlpha(static_cast<int>(color.a() * alpha_factor));

	Vertex_t verts[3] = {
		Vertex_t(point_one),
		Vertex_t(point_two),
		Vertex_t(point_three)
	};

	static int texture = g_csgo.m_surface()->CreateNewTextureID(true);
	unsigned char buffer[4] = { 255, 255, 255, 255 };

	g_csgo.m_surface()->DrawSetTextureRGBA(texture, buffer, 1, 1);
	g_csgo.m_surface()->DrawSetColor(color);
	g_csgo.m_surface()->DrawSetTexture(texture);

	g_csgo.m_surface()->DrawTexturedPolygon(3, verts);
}

void render::line(int x, int y, int x2, int y2, Color color) {
	color.SetAlpha(static_cast<int>(color.a() * alpha_factor));

	g_csgo.m_surface()->DrawSetColor(color);
	g_csgo.m_surface()->DrawLine(x, y, x2, y2);
}

void render::textonlybomb(int x, int y, const char* _Input, int font, Color color)
{
	int apple = 0;
	char Buffer[2048] = { '\0' };
	va_list Args;
	va_start(Args, _Input);
	vsprintf_s(Buffer, _Input, Args);
	va_end(Args);
	size_t Size = strlen(Buffer) + 1;
	wchar_t* WideBuffer = new wchar_t[Size];
	mbstowcs_s(0, WideBuffer, Size, Buffer, Size - 1);

	g_csgo.m_surface()->DrawSetTextColor(color);
	g_csgo.m_surface()->DrawSetTextFont(font);
	g_csgo.m_surface()->DrawSetTextPos(x, y);
	g_csgo.m_surface()->DrawPrintText(WideBuffer, wcslen(WideBuffer));
}
void render::textgay(const wchar_t* buf, int pos_x, int pos_y, Color color, vgui::HFont font, DWORD flags)
{
	int tw, th;
	g_csgo.m_surface()->GetTextSize(font, buf, tw, th);
	if (!(flags & HFONT_CENTERED_NONE)) {
		if (flags & HFONT_CENTERED_X)
			pos_x -= tw * 0.1f;

		if (flags & HFONT_CENTERED_Y)
			pos_y -= th * 0.1f;
	}
	g_csgo.m_surface()->DrawSetTextFont(font);
	g_csgo.m_surface()->DrawSetTextColor(color);
	g_csgo.m_surface()->DrawSetTextPos(pos_x, pos_y);
	g_csgo.m_surface()->DrawPrintText(buf, wcslen(buf));

}

void render::nonamegey(int x, int y, Color color, DWORD font, const char* text)
{
	size_t origsize = strlen(text) + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t wcstring[newsize];
	mbstowcs_s(&convertedChars, wcstring, origsize, text, _TRUNCATE);

	g_csgo.m_surface()->DrawSetTextFont(font);

	g_csgo.m_surface()->DrawSetTextColor(color);
	g_csgo.m_surface()->DrawSetTextPos(x, y);
	g_csgo.m_surface()->DrawPrintText(wcstring, wcslen(wcstring));
	return;
}

void render::Line(int x0, int y0, int x1, int y1, Color col)
{
	g_csgo.m_surface()->DrawSetColor(col);
	g_csgo.m_surface()->DrawLine(x0, y0, x1, y1);
}

void render::Line(Vector2D start_pos, Vector2D end_pos, Color col)
{
	Line(start_pos.x, start_pos.y, end_pos.x, end_pos.y, col);
}
void render::text(vgui::HFont font, int x, int y, Color color, DWORD flags, const char* msg, ...) {
	color.SetAlpha(static_cast<int>(color.a() * alpha_factor));

	va_list va_alist;
	char buffer[1024];
	va_start(va_alist, msg);
	_vsnprintf(buffer, sizeof(buffer), msg, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];

	MultiByteToWideChar(CP_UTF8, 0, buffer, 256, wbuf, 256);

	int width, height;
	g_csgo.m_surface()->GetTextSize(font, wbuf, width, height);

	if (!(flags & HFONT_CENTERED_NONE)) {
		if (flags & HFONT_CENTERED_X)
			x -= width * 0.5f;

		if (flags & HFONT_CENTERED_Y)
			y -= height * 0.5f;
	}

	g_csgo.m_surface()->DrawSetTextFont(font);
	g_csgo.m_surface()->DrawSetTextColor(color);
	g_csgo.m_surface()->DrawSetTextPos(x, y);
	g_csgo.m_surface()->DrawPrintText(wbuf, wcslen(wbuf));
}

void render::wtext(vgui::HFont font, int x, int y, Color color, DWORD flags, wchar_t* msg) {
	color.SetAlpha(static_cast<int>(color.a() * alpha_factor));

	wchar_t buffer[256];
	va_list args;

	va_start(args, msg);
	wvsprintfW(buffer, msg, args);
	va_end(args);

	int width, height;
	g_csgo.m_surface()->GetTextSize(font, buffer, width, height);

	if (!(flags & HFONT_CENTERED_NONE)) {
		if (flags & HFONT_CENTERED_X)
			x -= width * 0.5f;

		if (flags & HFONT_CENTERED_Y)
			y -= height * 0.5f;
	}

	g_csgo.m_surface()->DrawSetTextFont(font);
	g_csgo.m_surface()->DrawSetTextColor(color);
	g_csgo.m_surface()->DrawSetTextPos(x, y);
	g_csgo.m_surface()->DrawPrintText(buffer, wcslen(buffer));
}



float render::text_width(vgui::HFont font, const char* msg, ...) {
	va_list va_alist;
	char buffer[1024];
	va_start(va_alist, msg);
	_vsnprintf(buffer, sizeof(buffer), msg, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];

	MultiByteToWideChar(CP_UTF8, 0, buffer, 256, wbuf, 256);

	int width, height;
	g_csgo.m_surface()->GetTextSize(font, wbuf, width, height);

	return width;
}

RECT render::viewport() {
	RECT viewport = { 0, 0, 0, 0 };

	int width, height;
	g_csgo.m_engine()->GetScreenSize(width, height);
	viewport.right = width;
	viewport.bottom = height;

	return viewport;
}

























































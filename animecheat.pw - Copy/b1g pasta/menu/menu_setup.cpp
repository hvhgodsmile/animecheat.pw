#include "cmenu.hpp"
#include "..\cheats\misc\logs.h"

#include <shlobj.h>
#include <shlwapi.h>
struct hud_weapons_t
{
	std::int32_t* get_weapon_count()
	{
		return reinterpret_cast<std::int32_t*>(std::uintptr_t(this) + 0x80);
	}
};
template<class T>
static T* FindHudElement(const char* name)
{
	static auto pThis = *reinterpret_cast<DWORD**>(util::FindSignature("client_panorama.dll", "B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08") + 1);

	static auto find_hud_element = reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(util::FindSignature("client_panorama.dll", "55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28"));
	return (T*)find_hud_element(pThis, name);
}
void KnifeApplyCallbk()
{
	static auto clear_hud_weapon_icon_fn = reinterpret_cast<std::int32_t(__thiscall*)(void*, std::int32_t)>(util::FindSignature("client_panorama.dll", "55 8B EC 51 53 56 8B 75 08 8B D9 57 6B FE 2C"));

	auto element = FindHudElement<std::uintptr_t*>("CCSGO_HudWeaponSelection");

	auto hud_weapons = reinterpret_cast<hud_weapons_t*>(std::uintptr_t(element) - 0xA0);

	if (hud_weapons == nullptr)
		return;

	if (!*hud_weapons->get_weapon_count())
		return;

	for (std::int32_t i = 0; i < *hud_weapons->get_weapon_count(); i++)
		i = clear_hud_weapon_icon_fn(hud_weapons, i);

	typedef void(*ForceUpdate) (void);
	ForceUpdate FullUpdate = (ForceUpdate)util::FindSignaturenew("engine.dll", "fullUpdate", "A1 ? ? ? ? B9 ? ? ? ? 56 FF 50 14 8B 34 85");
	FullUpdate();
}

ConVar * Meme = nullptr;
void SkinApplyCallbk()
{
	if (!Meme) {
		static ConVar* Meme = g_csgo.m_cvar()->FindVar(("cl_fullupdate"));
		Meme->m_nFlags &= ~FCVAR_CHEAT;
		g_csgo.m_engine()->ClientCmd_Unrestricted("cl_fullupdate");
	}
}
std::string get_config_dir() {
	std::string folder;
	static TCHAR path[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, 0, path))) {
		folder = std::string(path) + "\\animecheat.pw\\";
	}

	CreateDirectory(folder.c_str(), NULL);

	return folder;
}

void load_config() {
	if (!cfg_manager->files.size()) {
		return;
	}

	eventlogs::get().add("loaded " + cfg_manager->files[g_cfg.selected_config], Color::White);

	cfg_manager->load(cfg_manager->files[g_cfg.selected_config]);
}

void save_config() {
	if (!cfg_manager->files.size()) {
		return;
	}

	eventlogs::get().add("saved " + cfg_manager->files[g_cfg.selected_config], Color::White);

	cfg_manager->save(cfg_manager->files[g_cfg.selected_config]);
	cfg_manager->config_files();
}

void remove_config() {
	if (!cfg_manager->files.size()) {
		return;
	}

	cfg_manager->remove(cfg_manager->files[g_cfg.selected_config]);
	cfg_manager->config_files();

	if (g_cfg.selected_config > cfg_manager->files.size() - 1) {
		g_cfg.selected_config = cfg_manager->files.size() - 1;
	}
}

void add_config() {
	if (g_cfg.new_config_name.find(".cfg") == -1)
		g_cfg.new_config_name += ".cfg";

	cfg_manager->save(g_cfg.new_config_name.c_str());
	cfg_manager->config_files();

	g_cfg.new_config_name = "";

	g_cfg.selected_config = cfg_manager->files.size() - 1;
}

void unload_cheat() {
	unload = true;
}

std::vector<std::string> KnifeModel = { "default",
"bayonet",
"flip knife",
"gut knife",
"karambit",
"m9 bayonet",
"huntsman knife",
"butterfly knife",
"falchion knife",
"shadow daggers",
"bowie knife",
"navaja knife",
"stiletto knife",
"ursus knife",
"talon knife" };
std::vector<std::string> M4A4 = { "none",
"asiimov",
"howl",
"dragon king",
"poseidon",
"daybreak",
"royal paladin",
"battleStar",
"desolate space",
"buzz kill",
"bullet rain",
"hell fire",
"evil daimyo",
"griffin",
"zirka",
"radiation harzard", };
std::vector<std::string> knifeskins = { "none",
"crimson web",
"bone mask",
"fade",
"night",
"blue steel",
"stained",
"case hardened",
"slaughter",
"safari mesh",
"boreal forest",
"ultraviolet",
"urban masked",
"scorched",
"rust coat",
"tiger tooth",
"damascus steel",
"damascus steel",
"marble fade",
"rust coat",
"doppler ruby",
"doppler sapphire",
"doppler blackpearl",
"doppler phase 1",
"doppler phase 2",
"doppler phase 3",
"doppler phase 4",
"doppler phase 4",
"gamma doppler phase 1",
"gamma doppler phase 2",
"gamma doppler phase 3",
"gamma doppler phase 4",
"gamma doppler emerald",
"lore",
"black laminate",
"autotronic",
"freehand" };
std::vector<std::string> ak47 = { "none",
"fire serpent",
"fuel injector",
"bloodsport",
"vulcan",
"case hardened",
"hydroponic",
"aquamarine revenge",
"frontside misty",
"point disarray",
"neon revolution",
"red laminate",
"redline",
"jaguar",
"jet set",
"wasteland rebel",
"the empress",
"elite build",
"neon rider" };
std::vector<std::string> GloveModel = { "none",
"bloodhound",
"sport",
"driver",
"wraps",
"moto",
"specialist" };

std::vector<std::string> Duals = { "none",
"panther",
"dualing dragons",
"cobra strike",
"royal consorts",
"duelist" };
std::vector<std::string> M4A1 = { "none",
"decimator",
"knight",
"chantico's fire",
"golden koi",
"hyper beast",
"master piece",
"hot rod",
"mecha industries",
"cyrex",
"icarus fell",
"flashback",
"hyper beast",
"atomic alloy",
"guardian",
"briefing" };
std::vector<std::string> Usp = { "none",
"neo-noir",
"cyrex",
"orion",
"kill confirmed",
"overgrowth",
"caiman",
"serum",
"guardian",
"road rash" };
std::vector<std::string> Glock = { "none",
"fade",
"dragon tattoo",
"twilight galaxy",
"wasteland rebel",
"water elemental",
"off world",
"weasel",
"royal legion",
"grinder",
"steel disruption",
"brass",
"ironwork",
"bunsen burner",
"reactor" };
std::vector<std::string> Deagle = { "none",
"blaze",
"kumicho dragon",
"oxide blaze",
"golden koi",
"cobalt disruption",
"directive" };
std::vector<std::string> Five7 = { "none",
"monkey business",
"hyper beast",
"fowl play",
"triumvirate",
"retrobution",
"capillary",
"violent daimyo" };
std::vector<std::string> Aug = { "none",
"bengal tiger",
"hot rod",
"chameleon",
"akihabara accept" };
std::vector<std::string> Famas = { "none",
"djinn",
"styx",
"neural net",
"survivor z" };
std::vector<std::string> G3sg1Skin = { "none",
"hunter",
"the executioner",
"terrace" };
std::vector<std::string> Galil = { "none",
"chatterbox",
"crimson tsunami",
"sugar rush",
"eco",
"cerberus" };
std::vector<std::string> M249 = { "none",
"nebula crusader",
"system lock",
"magma" };
std::vector<std::string> Mac10 = { "none",
"neon rider",
"last dive",
"curse",
"rangeen" };
std::vector<std::string> Ump45 = { "none",
"blaze",
"minotaur's labyrinth",
"pandora's box",
"primal saber",
"exposure" };
std::vector<std::string> XM1014 = { "none",
"seasons",
"traveler",
"ziggy" };
std::vector<std::string> Cz75 = { "none",
"red astor",
"pole position",
"victoria",
"xiangliu" };
std::vector<std::string> PPBizon = { "none",
"high roller",
"judgement of anubis",
"fuel rod" };
std::vector<std::string> Mag7 = { "none",
"bulldozer",
"heat",
"petroglyph" };
std::vector<std::string> Awpskinse = { "none",
"asiimov",
"dragon lore",
"fever dream",
"medusa",
"hyper beast",
"boom",
"lightning strike",
"pink ddpat",
"corticera",
"redline",
"man o' war",
"graphite",
"electric hive",
"sun in leo",
"pit viper",
"phobos",
"elite build",
"worm god",
"oni taiji",
"fever dream" };
std::vector<std::string> negev = { "none",
"power loader",
"loudmouth",
"man o' war" };
std::vector<std::string> Sawedoff = { "none",
"wasteland princess",
"the kraken",
"yorick" };
std::vector<std::string> tec9 = { "none",
"nuclear threat",
"red quartz",
"blue titanium",
"titanium bit",
"sandstorm",
"isaac",
"toxic",
"re-entry",
"fuel injector" };
std::vector<std::string> P2000 = { "none",
"handgun",
"fade",
"corticera",
"ocean foam",
"fire elemental",
"asterion",
"pathfinder",
"imperial",
"oceanic",
"imperial dragon" };
std::vector<std::string> Mp7 = { "none",
"nemesis",
"impire",
"special delivery" };
std::vector<std::string> Mp9 = { "none",
"rose iron",
"ruby poison dart",
"airlock" };
std::vector<std::string> Nova = { "none",
"hyper beast",
"koi",
"antique" };
std::vector<std::string> P250 = { "none",
"whiteout",
"crimson kimono",
"mint kimono",
"eingshot",
"asiimov",
"see ya later" };
std::vector<std::string> SCAR20 = { "none",
"splash jam",
"storm",
"contractor",
"carbon fiber",
"sand mesh",
"palm",
"crimson web",
"cardiac",
"army sheen",
"cyrex",
"grotto",
"bloodsport" };
std::vector<std::string> Sg553 = { "none",
"tiger moth",
"cyrex",
"pulse",
"fallout warning" };
std::vector<std::string> SSG08 = { "none",
"lichen dashed",
"dark water",
"blue spruce",
"sand dune",
"palm",
"mayan dreams",
"blood in the water",
"tropical storm",
"acid fade",
"slashed",
"detour",
"abyss",
"big iron",
"necropos",
"ghost crusader",
"dragonfire" };
std::vector<std::string> Revolver = { "none",
"llama cannon",
"fade",
"crimson web", };

void setup_main_menu() {
	static auto set_sub = [](int sub) -> void {
		g_cfg.menu.group_sub = sub;
	};

	static auto set_tabsub = [](int sub) -> void {
		g_cfg.menu.tab_sub = sub;
	};

	auto rage_tab = new C_Tab(&menu::get(), 1, "", "aimbot", {
		C_Tab::subtab_t("v", 0, fonts[SUBTABWEAPONS]),
		C_Tab::subtab_t("E", 1, fonts[SUBTABWEAPONS]),
		C_Tab::subtab_t("A", 2, fonts[SUBTABWEAPONS]),
		C_Tab::subtab_t("W", 3, fonts[SUBTABWEAPONS]),
		C_Tab::subtab_t("O", 4, fonts[SUBTABWEAPONS]),
		C_Tab::subtab_t("a", 5, fonts[SUBTABWEAPONS]),
		C_Tab::subtab_t("Y", 6, fonts[SUBTABWEAPONS]),
		C_Tab::subtab_t("e", 7, fonts[SUBTABWEAPONS])
		}); {
		set_tabsub(0);

		auto rage = new C_GroupBox(GROUP_LEFT, 8, ""); {
			auto enable = new C_CheckBox("enable", &g_cfg.ragebot.enable);

			auto fov = new C_SliderInt("field of view", &g_cfg.ragebot.field_of_view, 0, 360, "°");

			auto selection_type = new C_Dropdown("selection type", &g_cfg.ragebot.selection_type, { "field of view", "distance", "health" });

			auto silent_aim = new C_CheckBox("silent aim", &g_cfg.ragebot.silent_aim);

			auto anti_recoil = new C_CheckBox("anti recoil", &g_cfg.ragebot.anti_recoil);

			auto anti_spread = new C_CheckBox("anti spread (nospread)", &g_cfg.ragebot.anti_spread);

			auto zeus_bot = new C_CheckBox("zeus bot", &g_cfg.ragebot.zeus_bot);
			auto auto_pistols = new C_CheckBox("auto pistols", &g_cfg.ragebot.auto_pistols);
			auto auto_revolver = new C_CheckBox("auto revolver", &g_cfg.ragebot.auto_revolver);
			auto extrapolation = new C_CheckBox("extrapolation", &g_cfg.ragebot.extrapolation);
		}

		auto rage2 = new C_GroupBox(GROUP_RIGHT, 8, ""); {
			auto aa_correction = new C_CheckBox("anti-aim correction", &g_cfg.ragebot.antiaim_correction);

			auto desync_fix = new C_CheckBox("resolver", &g_cfg.ragebot.desync_fix);
			auto override_key = new C_KeyBind("override key", &g_cfg.ragebot.override_key);

			auto aimstep = new C_CheckBox("aim step", &g_cfg.ragebot.aimstep);

			//auto lagcomp = new C_CheckBox("lag compensation", &g_cfg.ragebot.lagcomp);

			//auto lagcomp_time = new C_SliderFloat("", &g_cfg.ragebot.lagcomp_time, 0, 0.20, "s");

			auto lag_compensation_type = new C_Dropdown("compensation type", &g_cfg.ragebot.lag_compensation_type, { "refine shot", "prediction" });
		}

		for (int i = 0; i < 7; i++) {
			set_tabsub(i + 1);

			auto weapon = new C_GroupBox(GROUP_LEFT, 8, ""); {
				auto autoshoot = new C_CheckBox("autoshoot", &g_cfg.ragebot.autoshoot[i]);
				auto autowall = new C_CheckBox("minimum damage", &g_cfg.ragebot.autowall[i]);
				auto minimum_damage = new C_SliderInt("", &g_cfg.ragebot.minimum_damage[i], 1, 100, "");

				if (i == 2 || i == 4 || i == 5) {
					int autoscope_element;

					if (i == 2) autoscope_element = 0;
					if (i == 4) autoscope_element = 1;
					if (i == 5) autoscope_element = 2;

					auto autoscope = new C_CheckBox("autoscope", &g_cfg.ragebot.autoscope[autoscope_element]);
				}

				auto hitchance = new C_CheckBox("hitchance", &g_cfg.ragebot.hitchance[i]);
				auto hitchance_amount = new C_SliderInt("", &g_cfg.ragebot.hitchance_amount[i], 0, 100, "%");

				auto hitscan = new C_MultiDropdown("hitboxes", &g_cfg.ragebot.hitscan[i]);

				auto hitscan_history = new C_MultiDropdown("hitbox history", &g_cfg.ragebot.hitscan_history[i]);
				auto multipoint = new C_MultiDropdown("multipoint", &g_cfg.ragebot.multipoint_hitboxes[i]);
				auto pointscale = new C_SliderFloat("pointscale", &g_cfg.ragebot.pointscale[i], 0, 1, "");
			}

			auto weapon2 = new C_GroupBox(GROUP_RIGHT, 8, ""); {
				auto baim = new C_MultiDropdown("baim", &g_cfg.ragebot.baim_settings[i]);

				auto baim_hp = new C_SliderInt("baim if hp less than", &g_cfg.ragebot.baim_health[i], 0, 100, "hp");
				auto baim_key = new C_KeyBind("baim key", &g_cfg.ragebot.baim_key);
				//auto baimshotenable = new C_CheckBox("baimshotenable", &g_cfg.ragebot.baimshotenable);
				//auto baimshotenableslider = new C_SliderInt("", &g_cfg.ragebot.baimshotenableslider, 0, 10, "");
				auto quickstop = new C_CheckBox("autostop", &g_cfg.ragebot.quickstop[i]);
				auto quickstop_type = new C_Dropdown("", &g_cfg.ragebot.quickstoptype, { "defaut", "aggresive" });
			}
		}
	}

	auto antiaim_tab = new C_Tab(&menu::get(), 2, "", "antiaim", {
		C_Tab::subtab_t("stand", 1, fonts[TAHOMA12]),
		C_Tab::subtab_t("move", 2, fonts[TAHOMA12]),
		C_Tab::subtab_t("air", 3, fonts[TAHOMA12]),
		}); {
		set_tabsub(0);

		for (int i = 0; i < 3; i++) {
			set_tabsub(i + 1);

			auto antiaim = new C_GroupBox(GROUP_LEFT, 8, ""); {

				auto base_angle = new C_Dropdown("base angle", &g_cfg.antiaim.base_angle[i], { "static", "crosshair", "at targets" });

				auto pitch = new C_Dropdown("pitch", &g_cfg.antiaim.pitch[i], { "off", "down", "up", "minimal", "fake zero", "fake down", "fake up" });

				std::vector< std::string > reals{ "off", "sideways", "backwards", "rotate", "jitter", "local view", "lowerbody", "desync+freestanding","goodfreestanding", };

				auto yaw = new C_Dropdown("yaw", &g_cfg.antiaim.yaw[i], reals);
				auto yaw_offset = new C_SliderInt("", &g_cfg.antiaim.yaw_offset[i], -180, 180, "°");

				std::vector< std::string > fakes{ "off", "use!" };


				auto DESYNCANTIAIM = new C_KeyBind("syncjitter key", &g_cfg.antiaim.DESYNCANTIAIM);


				auto lby_breaker = new C_CheckBox("break lby", &g_cfg.antiaim.lby_breaker);
				auto lby_delta = new C_SliderInt("", &g_cfg.antiaim.lby_delta, -180, 180, "°");
				auto flick_up = new C_CheckBox("flick up", &g_cfg.antiaim.pitch_flick);
				auto balance_breaker = new C_CheckBox("balance breaker", &g_cfg.antiaim.break_adjustment);
			}

			auto antiaim_2 = new C_GroupBox(GROUP_RIGHT, 8, ""); {
				auto manualkey_left = new C_KeyBind("manual aa left", &g_cfg.antiaim.left_manualkey);
				auto manualkey_right = new C_KeyBind("manual aa right", &g_cfg.antiaim.right_manualkey);
				auto manualkey_back = new C_KeyBind("manual aa back", &g_cfg.antiaim.back_manualkey);

				auto fakelag_enablers = new C_MultiDropdown("fakelag", &g_cfg.antiaim.fakelag_enablers);
				auto fakelag_mode = new C_Dropdown("", &g_cfg.antiaim.fakelag_mode, { "factor", "adaptive", "max" });
				auto fakelag_amount = new C_SliderInt("", &g_cfg.antiaim.fakelag_amount, 1, 14, "");
				//	auto fakelag_onpeek = new C_CheckBox( "on peek *todo*", &g_cfg.antiaim.fakelag_onpeek );
				//	auto fakelag_onpeek_amt = new C_SliderInt( "", &g_cfg.antiaim.fakelag_onpeek_amt, 1, 14, "" );
			}
		}
	}
	

	auto player_tab = new C_Tab(&menu::get(), 3, "", "players"); {
		auto player = new C_GroupBox(GROUP_LEFT, 8, ""); {
			auto enable = new C_CheckBox("enable", &g_cfg.player.enable);

			auto alpha = new C_SliderInt("alpha", &g_cfg.player.alpha, 0, 255, "");

			auto pov_arrow = new C_CheckBox("out of view indicators", &g_cfg.player.out_pov_arrow,
				{ new C_ColorSelector(&g_cfg.player.out_pov_arrow_color) });

			auto pov_distance = new C_SliderInt("", &g_cfg.player.out_pov_distance, 25, 400, "");

			auto box = new C_CheckBox("box esp", &g_cfg.player.box,
				{ new C_ColorSelector(&g_cfg.player.box_color) });

			auto health = new C_CheckBox("health", &g_cfg.player.health);

			auto name = new C_CheckBox("name", &g_cfg.player.name,
				{ new C_ColorSelector(&g_cfg.player.name_color) });

			auto weapon = new C_CheckBox("weapon", &g_cfg.player.weapon,
				{ new C_ColorSelector(&g_cfg.player.weapon_color) });

			auto ammo = new C_Dropdown("ammunition", &g_cfg.player.ammo, { "off", "ammo bar" });

			auto ammobar_color = new C_TextSeperator("color bar", SEPERATOR_NORMAL,
				{ new C_ColorSelector(&g_cfg.player.ammobar_color) });


			auto flags = new C_MultiDropdown("flags", &g_cfg.player.flags);

			auto skeleton = new C_MultiDropdown("skeleton", &g_cfg.player.skeleton);
		}

		auto player_2 = new C_GroupBox(GROUP_RIGHT, 8, ""); {
			auto glow = new C_MultiDropdown("glow", &g_cfg.player.glow);

			auto glow_color_enemy = new C_TextSeperator("color enemy", SEPERATOR_NORMAL,
				{ new C_ColorSelector(&g_cfg.player.glow_color_enemy) });

			auto glow_color_teammate = new C_TextSeperator("color teammate", SEPERATOR_NORMAL,
				{ new C_ColorSelector(&g_cfg.player.glow_color_teammate) });

			auto glow_color_local= new C_TextSeperator("color local", SEPERATOR_NORMAL,
				{ new C_ColorSelector(&g_cfg.player.glow_color_local) });

			auto glow_opacity = new C_SliderInt("", &g_cfg.player.glowopacity, 1, 100, "%%");

			auto glow_bloom = new C_SliderInt("", &g_cfg.player.glowbloom, 1, 100, "%%");

			auto glow_type = new C_Dropdown("", &g_cfg.player.glow_type, { "outline outer", "cover", "outline inner" });

			auto chams = new C_MultiDropdown("chams", &g_cfg.player.chams);

			auto chams_color_visible = new C_TextSeperator("visible chams", SEPERATOR_NORMAL,
				{ new C_ColorSelector(&g_cfg.player.chams_color) });

			auto chams_color_invisible = new C_TextSeperator("xqz chams", SEPERATOR_NORMAL,
				{ new C_ColorSelector(&g_cfg.player.xqz_color) });

			auto chams_opacity = new C_SliderInt("", &g_cfg.player.chams_opacity, 0, 100, "%%");

			auto chams_type = new C_Dropdown("", &g_cfg.player.chams_type, { "regular", "flat", "rubber", "metallic" });

			auto backtrack_chams = new C_CheckBox("back track model", &g_cfg.player.backtrack_chams,
				{ new C_ColorSelector(&g_cfg.player.backtrack_chams_color) });

			auto backtrack_chams_opacity = new C_SliderInt("", &g_cfg.player.backtrack_chams_opacity, 0, 100, "%%");
		}

		auto esp_tab = new C_Tab(&menu::get(), 4, "", "other visuals"); {
			auto esp = new C_GroupBox(GROUP_LEFT, 8, ""); {
				auto fov = new C_SliderInt("override fov", &g_cfg.esp.fov, 0, 89, "°");
				auto viewmodel_x = new C_SliderInt("view model[x]", &g_cfg.esp.viewmodel_x, 0, 30, "°");
				auto viewmodel_y = new C_SliderInt("view model[y]", &g_cfg.esp.viewmodel_y, 0, 30, "°");
				auto viewmodel_z = new C_SliderInt("view model[z]", &g_cfg.esp.viewmodel_z, 0, 30, "°");
				auto override_fov_scoped = new C_CheckBox("override fov when scoped", &g_cfg.esp.override_fov_scoped);

				auto knifeleft = new C_CheckBox("left hand knife", &g_cfg.esp.knifeleft);
				auto aspectratio = new C_SliderInt("range", &g_cfg.esp.aspectratio, 0, 199, "");


				auto show_spread = new C_CheckBox("show spread", &g_cfg.esp.show_spread,
					{ new C_ColorSelector(&g_cfg.esp.show_spread_color) });

				auto crosshair = new C_CheckBox("crosshair", &g_cfg.esp.crosshair);

				auto hands = new C_CheckBox("hands", &g_cfg.esp.hands,
				{ new C_ColorSelector(&g_cfg.esp.hands_color) });
				auto hands_alpha = new C_SliderInt("hands alpha", &g_cfg.esp.hands_alpha, 0, 100, "%%");

				auto pulsation = new C_CheckBox("pulsation", &g_cfg.esp.pulsation);

				auto penetration_reticle = new C_CheckBox("penetration reticle", &g_cfg.esp.penetration_reticle);

				auto antiaim_indicator = new C_CheckBox("anti-aim indicator", &g_cfg.esp.antiaim_indicator);

				auto croked_indicator = new C_CheckBox("choked indicator", &g_cfg.esp.croked_indicator);
				auto extend_indicator = new C_CheckBox("extend indicator", &g_cfg.esp.extend_indicator);
				auto lc_indicator = new C_CheckBox("lag comp indicator", &g_cfg.esp.lc_indicator);

				auto ghost_chams = new C_CheckBox("ghost chams", &g_cfg.esp.ghost_chams,
					{ new C_ColorSelector(&g_cfg.esp.ghost_chams_color) });

				auto local_chams = new C_CheckBox("local chams", &g_cfg.esp.local_chams,
					{ new C_ColorSelector(&g_cfg.esp.local_chams_color) });

				auto local_chams_fakelag = new C_CheckBox("local chams fakelag", &g_cfg.esp.local_chams_fakelag,
					{ new C_ColorSelector(&g_cfg.esp.localfakelag_chams_color) });

				auto chams_opacitylocal1 = new C_SliderInt("opacity chams", &g_cfg.player.chams_opacitylocal1, 0, 50, "%%");
			}

			auto esp_2 = new C_GroupBox(GROUP_RIGHT, 8, ""); {
				auto dropped_weapon = new C_CheckBox("dropped weapon", &g_cfg.esp.dropped_weapons,
					{ new C_ColorSelector(&g_cfg.esp.dropped_weapons_color) });

				auto projectiles = new C_CheckBox("projectiles", &g_cfg.esp.projectiles,
					{ new C_ColorSelector(&g_cfg.esp.projectiles_color) });


				auto dont_render_teammates = new C_CheckBox("dont't render teammates", &g_cfg.esp.dont_render_teammates);

				auto spectators = new C_CheckBox("spectators", &g_cfg.esp.spectators);

				auto zeus_tanger = new C_CheckBox("zeus range\\crash", &g_cfg.esp.zeusrange,
					{ new C_ColorSelector(&g_cfg.esp.zeusrange_color) });

				auto damageindicator = new C_CheckBox("damage indicator", &g_cfg.esp.damageindicator);

				auto bombesp = new C_CheckBox("bomb esp", &g_cfg.esp.bombesp);

				auto hitmarker = new C_MultiDropdown("hitmarker", &g_cfg.esp.hitmarker);

				auto hitsound = new C_CheckBox("hitsound", &g_cfg.esp.hitsound);

				auto removals = new C_MultiDropdown("removals", &g_cfg.esp.removals);

				auto bullet_tracer = new C_CheckBox("bullet tracers", &g_cfg.esp.bullet_tracer,
					{ new C_ColorSelector(&g_cfg.esp.bullet_tracer_color) });

				auto worldsupremacy = new C_MultiDropdown("world", &g_cfg.esp.worldsupremacy); // supremacy hi
				auto skybox = new C_Dropdown("sky box (turn on and off nightmode to apply)", &g_cfg.esp.skybox,
					{ "Tibet",
					"Baggage",
					"Monastery",
					"Italy/OldInferno",
					"Aztec",
					"Vertigo",
					"Daylight",
					"Daylight (2)",
					"Clouds",
					"Clouds (2)",
					"Gray",
					"Clear",
					"Canals",
					"Cobblestone",
					"Assault",
					"Clouds (Dark)",
					"Night",
					"Night (2)",
					"Night (Flat)",
					"Dusty",
					"Rainy",
					"Custom: Sunrise",
					"Custom: Galaxy",
					"Custom: Galaxy (2)",
					"Custom: Galaxy (3)",
					"Custom: Clouds (Night)",
					"Custom: Ocean",
					"Custom: Grimm Night",
					"Custom: Heaven",
					"Custom: Heaven (2)",
					"Custom: Clouds",
					"Custom: Night (Blue)" });
				auto ColorSkyr = new C_SliderInt("sky color [r]", &g_cfg.esp.ColorSkyr, 0, 25, "°");
				auto ColorSkyg = new C_SliderInt("sky color [g]", &g_cfg.esp.ColorSkyg, 0, 25, "°");
				auto ColorSkyb = new C_SliderInt("sky color [b]", &g_cfg.esp.ColorSkyb, 0, 25, "°");
			}
		}
	}
	auto skins_tab = new C_Tab(&menu::get(), 5, "", "skins"); {
		auto skinchanger = new C_GroupBox(GROUP_LEFT, 8, ""); {

			auto knifechanger = new C_GroupBox(GROUP_LEFT, 12, "skin changer");
			{
				auto skinson = new C_CheckBox("enable skinchanger", &g_cfg.skins.skinenabled);


				auto awpns = new C_Dropdown("awp skins", &g_cfg.skins.AWPSkin,
					{ "none",
						"asiimov",
						"dragon lore",
						"fever dream",
						"medusa",
						"hyper beast",
						"boom",
						"lightning strike",
						"pink ddpat",
						"corticera",
						"redline",
						"man o' war",
						"graphite",
						"electric hive",
						"sun in leo",
						"pit viper",
						"phobos",
						"elite build",
						"worm god",
						"oni taiji",
						"fever dream",
				        "whiteout" });
				auto scoutns = new C_Dropdown("ssg08 skins", &g_cfg.skins.SSG08Skin,
					{ "none",
						"lichen dashed",
						"dark water",
						"blue spruce",
						"sand dune",
						"palm",
						"mayan dreams",
						"blood in the water",
						"tropical storm",
						"acid fade",
						"slashed",
						"detour",
						"abyss",
						"big iron",
						"necropos",
						"ghost crusader",
						"dragonfire",
				        "whiteout" });
				auto scarns = new C_Dropdown("scar skins", &g_cfg.skins.SCAR20Skin,
					{ "none",
						"splash jam",
						"storm",
						"contractor",
						"carbon fiber",
						"sand mesh",
						"palm",
						"crimson web",
						"cardiac",
						"army sheen",
						"cyrex",
						"grotto",
						"bloodsport",
						"whiteout" });
				auto glockns = new C_Dropdown("glock skins", &g_cfg.skins.GlockSkin,
					{ "none",
						"fade",
						"dragon tattoo",
						"twilight galaxy",
						"wasteland rebel",
						"water elemental",
						"off world",
						"weasel",
						"royal legion",
						"grinder",
						"steel disruption",
						"brass",
						"ironwork",
						"bunsen burner",
						"reactor" });
				auto G3sg1ns = new C_Dropdown("g3sg1 skins", &g_cfg.skins.G3sg1Skin,
					{ "none",
						"hunter",
						"the executioner",
						"terrace",
						"neon kimono",
						"orange kimono",
						"predator",
						"chronos",
						"whiteout" });

				auto dualsns = new C_Dropdown("beretta skins", &g_cfg.skins.DualSkin,
					{ "none",
						"panther",
						"dualing dragons",
						"cobra strike",
						"royal consorts",
						"duelist" });

				auto deaglens = new C_Dropdown("deagle skins", &g_cfg.skins.DeagleSkin,
					{ "none",
						"blaze",
						"kumicho dragon",
						"oxide blaze",
						"golden koi",
						"cobalt disruption",
						"directive",
						"whiteout" });



				auto uspns = new C_Dropdown("usp skins", &g_cfg.skins.USPSkin,
					{ "none",
						"neo-noir",
						"cyrex",
						"orion",
						"kill confirmed",
						"overgrowth",
						"caiman",
						"serum",
						"guardian",
						"road rash",
						"whiteout" });

				auto ak = new C_Dropdown("ak47 skins", &g_cfg.skins.AK47Skin,
					{ "none",
						"fire serpent",
						"fuel injector",
						"bloodsport",
						"vulcan",
						"case hardened",
						"hydroponic",
						"aquamarine revenge",
						"frontside misty",
						"point disarray",
						"neon revolution",
						"red laminate",
						"redline",
						"jaguar",
						"jetset",
						"wasteland rebel",
						"the empress",
						"elite build" });
				auto P2000 = new C_Dropdown("p2000 skins", &g_cfg.skins.P2000Skin,
					{ "none",
						"handgun",
						"fade",
						"corticera",
						"ocean foam",
						"fire elemental",
						"asterion",
						"pathfinder",
						"imperial",
						"oceanic",
						"imperial dragon"});
				auto r8 = new C_Dropdown("revolver skins", &g_cfg.skins.RevolverSkin,
					{ "none",
						"llama cannon",
						"fade",
						"crimson web",
						"reboot",
						"nitro",
						"bone mask" });

			}
			auto knifechanger2 = new C_GroupBox(GROUP_RIGHT, 12, "knife changer");
			{
				//auto knifeshahaha = new C_CheckBox("Enable Knife Changer", &g_cfg.skins.knifes);
				auto knif = new C_Dropdown("knife Model", &g_cfg.skins.Knife,
					{ "bayonet",
						"flip Knife",
						"gut Knife",
						"karambit",
						"m9 bayonet",
						"huntsman knife",
						"butterfly knife",
						"falchion knife",
						"shadow daggers",
						"bowie knife",
						"navaja knife",
						"stiletto knife",
						"ursus knife",
						"talon knife" });

				auto knifskin = new C_Dropdown("knife Skin", &g_cfg.skins.KnifeSkin,
					{ "none",
						"crimson web",
						"bone mask",
						"fade",
						"night",
						"blue steel",
						"stained",
						"case hardened",
						"slaughter",
						"safari mesh",
						"boreal forest",
						"ultraviolet",
						"urban masked",
						"scorched",
						"rust coat",
						"tiger tooth",
						"damascus steel",
						"marble fade",
						"doppler ruby",
						"doppler sapphire",
						"doppler blackpearl",
						"doppler phase 1",
						"doppler phase 2",
						"doppler phase 3",
						"doppler phase 4",
						"gamma doppler phase 1",
						"gamma doppler phase 2",
						"gamma doppler phase 3",
						"gamma doppler phase 4",
						"gamma doppler emerald",
						"lore",
						"black laminate",
						"autotronic",
						"abyss", });
				auto forceupdate = new C_Button("full Update", []() { KnifeApplyCallbk(); });
				set_tabsub(1);
			}
		}
	}


	


		

	auto misc_tab = new C_Tab(&menu::get(), 6, "", "misc"); {
		auto miscellaneous = new C_GroupBox(GROUP_LEFT, 8, ""); {
			auto bunnyhop = new C_CheckBox("automatic jump", &g_cfg.misc.bunnyhop); 
			auto airstrafe = new C_CheckBox("automatic strafe", &g_cfg.misc.airstrafe);
			auto circle_stafer = new C_KeyBind("circle strafe", &g_cfg.misc.circle_strafe);
			auto thirdperson = new C_KeyBind("thirdperson", &g_cfg.misc.thirdperson_toggle);
			auto events_to_log = new C_MultiDropdown("activity log", &g_cfg.misc.events_to_log);
			auto sniper_crosshair = new C_CheckBox("sniper crosshair", &g_cfg.misc.sniper_crosshair);
			auto clantag = new C_CheckBox("clantag", &g_cfg.misc.clantag);
			auto watermark = new C_CheckBox("watermark", &g_cfg.esp.watermark);
		}

		auto miscellaneous_2 = new C_GroupBox(GROUP_RIGHT, 8, ""); {
			auto exaggerated_ragdoll = new C_CheckBox("force ragdolls", &g_cfg.misc.exaggerated_ragdolls);
			auto ping_spike = new C_CheckBox("fake latency", &g_cfg.misc.ping_spike);
			auto ping_spike_value = new C_SliderInt("", &g_cfg.misc.ping_spike_value, 0, 200, "ms");
			auto fakewalk = new C_CheckBox("slow walk", &g_cfg.misc.fakewalk);
			auto fakewalk_key = new C_KeyBind("", &g_cfg.misc.fakewalk_key);
			auto fakeduck = new C_KeyBind("fake duck", &g_cfg.misc.fakeduck);
			auto no_crouch_cooldown = new C_CheckBox("infinite duck", &g_cfg.misc.no_crouch_cooldown);
			auto presentkiller = new C_CheckBox("preserve killfeed", &g_cfg.misc.preversekillfeed);
			// Buybot
			auto buybot_enable = new C_CheckBox("buybot", &g_cfg.misc.buybot_enable); // Yeah, just a checkbox

			auto buybot_rifleschoice = new C_Dropdown("", &g_cfg.misc.buybot_rifleschoice, { "None", "auto", "scout", "awp", "ak/m4", "aug/sg" }); // Creates Combobox
			auto buybot_pistolchoice = new C_Dropdown("", &g_cfg.misc.buybot_pistolchoice, { "None", "elites", "deag/r8", "p250" }); // Creates Combobox
			auto buybot_extras = new C_MultiDropdown("extras", &g_cfg.misc.buybot_extras); // Creates multibox :)
		}
	}

	auto configs_tab = new C_Tab(&menu::get(), 7, "", "config"); {
		auto config = new C_GroupBox(GROUP_LEFT, 8, ""); {
			auto menu_color = new C_TextSeperator("menu color", SEPERATOR_NORMAL,
				{ new C_ColorSelector(&g_cfg.menu.menu_theme[0]) });

			auto menu_bind = new C_KeyBind("menu bind", &g_cfg.menu.menu_bind);

			auto anti_untrusted = new C_CheckBox("anti-unstrusted", &g_cfg.misc.anti_untrusted);
		}

		auto config_2 = new C_GroupBox(GROUP_RIGHT, 8, ""); {
			auto config_dropdown = new C_Dropdown("", &g_cfg.selected_config, &cfg_manager->files);

			auto function_seperator = new C_TextSeperator("functions", SEPERATOR_BOLD);

			auto load = new C_Button("load", []() { load_config(); });
			auto save = new C_Button("save", []() { save_config(); });
			auto remove = new C_Button("remove", []() { remove_config(); });
			new C_TextField("config name", &g_cfg.new_config_name);
			auto add = new C_Button("add", []() { add_config(); });
			new C_Button("unload cheat", []() { unload_cheat(); });
		}
	}
}

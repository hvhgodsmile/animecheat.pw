#pragma once

#include "..\menu\cmenu.hpp"

#include "..\sdk\misc\Color.hpp"
#include "..\sdk\interfaces\IInputSystem.hpp"

#include "..\depositories\json.hpp"

class Color;
class C_GroupBox;
class C_Tab;
struct MultiDropdownItem_t;

using json = nlohmann::json;
class Weapon_tTT
{
public:
	int SkinsWeapon;
	int SkinsKnife;
	int VremennyiWeapon;
	int VremennyiKnife;
	int Stikers1;
	int Stikers2;
	int Stikers3;
	int Stikers4;
	float ChangerWear = 0;
	int ChangerStatTrak = 0;
	char ChangerName[32] = "";
	bool ChangerEnabled;
};
class C_ConfigManager {
	class C_ConfigItem {
	public:
		std::string name;
		void *pointer;
		std::string type;

		C_ConfigItem(std::string name, void *pointer, std::string type) {
			this->name = name;
			this->pointer = pointer;
			this->type = type;
		}
	};

protected:
	std::vector<C_ConfigItem*> items;
private:
	void add_item(void * pointer, const char * name, std::string type);
	void setup_item(int *, int, std::string);
	void setup_item(bool *, bool, std::string);
	void setup_item(float *, float, std::string);
	void setup_item(ButtonCode_t *, ButtonCode_t, std::string);
	void setup_item(Color *, Color, std::string);
	void setup_item(std::vector< bool > *, std::string);
	void setup_item(std::vector< int > *, std::string);
	void setup_item(std::vector<MultiDropdownItem_t> *, std::vector< MultiDropdownItem_t >, std::string);
public:
	C_ConfigManager() { setup(); };

	void setup();
	void save(std::string config);
	void load(std::string config);
	void remove(std::string config);

	std::vector<std::string> files;
	void config_files();
};

extern C_ConfigManager * cfg_manager;

enum {
	FLAGS_SCOPED,
	FLAGS_BALANCE,
	FLAGS_ARMOR,
	FLAGS_VULNERABLE,
	FLAGS_LBY
};

enum {
	PLAYER_CHAMS_VISIBLE,
	PLAYER_CHAMS_INVISIBLE
};

enum {
	SKELETON_NORMAL,
	SKELETON_BACKTRACK
};

enum {
	GLOW_ENEMY,
	GLOW_TEAMMATE,
	GLOW_LOCAL
};

enum {
	AAINDICATOR_FIRSTPERSON,
	AAINDICATOR_THIRDPERSON
};

enum {
	REMOVALS_SCOPE,
	REMOVALS_SMOKE,
	REMOVALS_FLASH,
	REMOVALS_RECOIL
};

enum {
	HITMARKER_STATIC,
	HITMARKER_DYNAMIC
};

enum {
	BAIM_INAIR,
	BAIM_NOTRESOLVED,
	BAIM_CANKILL,
	BAIM_SLOWWALK,
	BAIM_ALWAYS
};

enum {
	EVENTLOG_HIT,
	EVENTLOG_ITEM_PURCHASES,
	EVENTLOG_PLANTING,
	EVENTLOG_DEFUSING
};

enum {
	FAKELAG_MOVING,
	FAKELAG_AIR,
	FAKELAG_STANDING,
	FAKELAG_SHOOTING
};
enum {
	KEVLAR, // I guess you can say it calls the shit in the multibox?
	KEVLARHELM,
	DEFUSE,
	ZEUS,
	HENADE,
	SMOKE,
	MOLLY,
	FLASH,
	FLASHX2,
};
enum {
	nightmode1, //supremacy dump shit
	fullbright1,
};


struct Config {
	struct Ragebot_t {
		bool enable;
		int field_of_view;
		int selection_type;
		int lag_compensation_type;
		bool silent_aim;
		bool anti_recoil;
		bool anti_spread;
		bool zeus_bot;
		bool auto_pistols;
		bool auto_revolver;
		bool extrapolation;
		bool aimstep;
		bool desync_fix;
		bool antiaim_correction;
		ButtonCode_t override_key;
		bool velocity_prediction;
		bool lagcomp;
		float lagcomp_time;
		ButtonCode_t baim_key;
		std::vector< char * > weapon_names{ "pistols", "deagle", "rifles", "smgs", "snipers", "autos", "shotguns" };

		bool autoshoot[7];
		bool autowall[7];
		int minimum_damage[7];
		bool autoscope[3];
		std::vector< MultiDropdownItem_t > hitscan[7];
		std::vector< MultiDropdownItem_t > hitscan_history[7];
		std::vector< MultiDropdownItem_t > multipoint_hitboxes[7];
		std::vector< MultiDropdownItem_t > m_multipoint;
		float pointscale[7];
		float multipointscale[7];
		bool hitchance[7];
		bool multipointnew[7];
		int hitchance_amount[7];
		std::vector< MultiDropdownItem_t > baim_settings[7];
		bool quickstop[7];
		int quickstoptype;

		int baim_health[7];
		int baimaftershoots[7];
		int resolver_mode;
	}ragebot;

	struct AntiAim_t {
		bool lby_breaker;
		ButtonCode_t DESYNCANTIAIM;
		int lby_delta;
		bool pitch_flick;
		bool break_adjustment;
		float jitteer_tick;

		int base_angle[3];

		int pitch[3];

		int yaw[3];
		int yaw_offset[3];

		int fake_yaw[3];
		int fake_yaw_offset[3];


		int desync[3];
		int yaw_desync[3];

		bool enable_edge;

		ButtonCode_t left_manualkey;
		ButtonCode_t right_manualkey;
		ButtonCode_t back_manualkey;

		bool render_manual;
		int fakelag_mode;
		std::vector< MultiDropdownItem_t > fakelag_enablers;
		int fakelag_amount;

		bool fakelag_onpeek;
		int fakelag_onpeek_amt;
	}antiaim;

	struct Legitbot_t {
		bool backtracking;
		bool backtracking_visualize;
		Color backtracking_visualize_color;
		float backtracking_max_time;

		bool aimassist;
		ButtonCode_t aim_assist_bind;
		float smoothing;
		float fov;
	}legit;

	struct Player_t {
		bool enable;
		bool dormant_players;
		int alpha;

		bool out_pov_arrow;
		int out_pov_distance;
		Color out_pov_arrow_color;

		bool box;
		Color box_color;
		bool health;
		bool name;
		Color name_color;
		bool weapon;
		Color weapon_color;
		int ammo;
		Color ammobar_color;
		bool lby;
		Color lbybar_color;
		std::vector< MultiDropdownItem_t > flags;
		std::vector< MultiDropdownItem_t > skeleton;

		std::vector< MultiDropdownItem_t > chams;
		Color chams_color;
		Color xqz_color;
		int chams_opacity;
		int chams_opacitylocal1;
		int chams_opacitylocalfakelag;
		bool lowerbody_indicator;
		int chams_type;

		bool backtrack_chams;
		Color backtrack_chams_color;
		int backtrack_chams_opacity;

		std::vector< MultiDropdownItem_t > glow;
		int glow_type;
		Color glow_color_local;
		Color glow_color_enemy;
		Color glow_color_teammate;
		int glowopacity;
		int glowbloom;
	}player;

	struct Visuals_t {
		int fov;
		bool override_fov_scoped;
		bool knifeleft;
		int viewmodel_x = 0;
		int viewmodel_y = 0;
		int viewmodel_z = 0;
		int aspectratio;
		bool aspectratioenable;
		bool transparency_scoped;
		int transparency_scoped_alpha;

		bool spectator_list;
		int hands_alpha;
		bool pulsation;
		bool hands;
		Color hands_color;
		bool show_spread;
		Color show_spread_color;
		int show_spread_type;
		int show_spread_opacity;

		bool crosshair;
		Color crosshair_color;
		bool penetration_reticle;

		bool antiaim_indicator;
		bool croked_indicator;
		bool extend_indicator;
		bool lc_indicator;
		bool ghost_chams;
		Color ghost_chams_color;

		bool local_chams;
		Color local_chams_color;
		bool glowlocal;
		Color glowlocalcolor;
		int style1;
		bool zeusrange;
		Color zeusrange_color;

		bool grenadeprediction;
		Color grenadeprediction_color;

		bool damageindicator;

		bool bombesp;

		bool local_chams_fakelag;
		Color localfakelag_chams_color;


		bool bullet_tracer;
		Color bullet_tracer_color;

		bool dropped_weapons;
		Color dropped_weapons_color;

		bool projectiles;
		Color projectiles_color;

		bool planted_bomb;
		Color planted_bomb_color;

		bool bomb_timer;
		bool asusprod;

		bool dont_render_teammates;
		bool spectators;
		bool nightmode;
		Color ColorSky;
		int ColorSkyr;
		int ColorSkyg;
		int ColorSkyb;

		bool watermark;
		bool hitsound;
		bool fullbright;
		int skybox;
		std::vector< MultiDropdownItem_t > hitmarker;

		std::vector< MultiDropdownItem_t > worldsupremacy;


		std::vector< MultiDropdownItem_t > removals;
	}esp;

	struct Misc_t {
		bool bunnyhop;
		bool airstrafe;
		bool duck_jump;
		ButtonCode_t circle_strafe;
		ButtonCode_t thirdperson_toggle;
		std::vector< MultiDropdownItem_t > events_to_log;
		bool model_changer;
		bool clantag;
		bool sniper_crosshair;
		bool bomb_timer;

		bool exaggerated_ragdolls;

		bool ping_spike;
		int ping_spike_value;

		bool fakewalk;
		ButtonCode_t fakeduck;
		ButtonCode_t fakewalk_key;
		int fakewalk_speed;
		bool anti_untrusted;
		bool no_crouch_cooldown;
		bool preversekillfeed;
		bool killspam;
		// Buybot definitions.
		bool buybot_enable;
		int buybot_rifleschoice; // Just for the menu and shit idk
		int buybot_pistolchoice;
		std::vector< MultiDropdownItem_t > buybot_extras;
	}misc;

	struct Skins_t {
		Weapon_tTT weapons[520];
		bool skinenabled = true;
		int wear;
		int Knife = 4;
		int gloves;
		int skingloves;
		float glovewear;
		bool glovesenabled;
		bool rankchanger;
		int KnifeSkin = 2;
		std::string awptest;
		int AK47Skin = 1;
		int GalilSkin;
		int M4A1SSkin;
		int M4A4Skin;
		int AUGSkin;
		int FAMASSkin;
		int AWPSkin;
		int SSG08Skin;
		bool fixshit;
		bool knifes = true;
		int SCAR20Skin;
		int P90Skin;
		int Mp7Skin;
		int NovaSkin;
		int UMP45Skin;
		int GlockSkin;
		int SawedSkin;
		int USPSkin;
		int MagSkin;
		int XmSkin;
		int DeagleSkin;
		int DualSkin;
		int FiveSkin;
		int RevolverSkin;
		int Mac10Skin;
		int tec9Skin;
		int Cz75Skin;
		int NegevSkin;
		int M249Skin;
		int Mp9Skin;
		int P2000Skin;
		int BizonSkin;
		int Sg553Skin;
		int P250Skin;
		int G3sg1Skin;
	}skins;

	struct Menu_t {
		Color menu_theme[2];
		ButtonCode_t menu_bind;

		C_GroupBox * last_group;
		C_Tab * last_tab;
		int group_sub;
		int tab_sub;
	}menu;

	int selected_config = 0;
	std::string new_config_name;
	bool old_pressed_keys[256];
};

extern Config g_cfg;
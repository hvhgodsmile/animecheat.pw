#include "spammers.h"
std::string Tag = " animecheat.pw ";
std::string Tag2 = "";
void set_clan_tag(const char* tag, const char* clan_name)
{
	static auto pSetClanTag = reinterpret_cast<void(__fastcall*)(const char*, const char*)>(util::pattern_scan(("engine.dll"), ("53 56 57 8B DA 8B F9 FF 15")));
	pSetClanTag(tag, clan_name);
}

void spammers::clan_tag() {
	auto local_player = static_cast<player_t *>(g_csgo.m_entitylist()->GetClientEntity(g_csgo.m_engine()->GetLocalPlayer()));
	auto m_local = local_player;

	if (g_cfg.misc.clantag)
	{
		if (!m_local || !m_local->is_alive() || !g_csgo.m_engine()->IsInGame() || !g_csgo.m_engine()->IsConnected())
		{
			Tag2 += Tag.at(0);
			Tag2.erase(0, 1);
			set_clan_tag(Tag2.c_str(), " animecheat.pw ");
		}
		static size_t lastTime = 0;

		if (GetTickCount() > lastTime)
		{
			Tag += Tag.at(0);
			Tag.erase(0, 1);
			set_clan_tag(Tag.c_str(), " animecheat.pw ");
			lastTime = GetTickCount() + 650;
		}

	}
}


































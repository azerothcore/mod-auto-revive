#include "ScriptMgr.h"
#include "Player.h"
#include "AccountMgr.h"
#include "Common.h"
#include "Configuration/Config.h"

uint32 TimeCheck_AutoRevive = 0;

class AutoRevive_PlayerSC : public PlayerScript
{
public:
	AutoRevive_PlayerSC() : PlayerScript("AutoRevive_PlayerSC") {}

	void OnBeforeUpdate(Player* player, uint32 diff)
	{
		if (!sConfigMgr->GetBoolDefault("AutoRevive.Enable", false))
			return;

        if (AccountMgr::IsPlayerAccount(player->GetSession()->GetSecurity()))
            return;

		uint32 NowHealth = player->GetHealth();
		uint32 TimeCheck = 1000;
		uint32 zoneid_conf = sConfigMgr->GetIntDefault("AutoRevive.ZoneID", 0);
		uint32 zoneid = player->GetZoneId();

		if (TimeCheck_AutoRevive < diff)
		{
			if (NowHealth <= 1)
			{
				if (zoneid_conf > 0)
				{
					if (zoneid_conf == zoneid)
					{
						player->ResurrectPlayer(1.0f);
						player->SaveToDB(false, false);
					}
					else
						return;
				}
				else
				{
					player->ResurrectPlayer(1.0f);
					player->SaveToDB(false, false);
				}
			}

			TimeCheck_AutoRevive = TimeCheck;
		}
		else
			TimeCheck_AutoRevive -= diff;
	}
};

class AutoRevive_WorldSC : public WorldScript
{
public:
    AutoRevive_WorldSC() : WorldScript("AutoRevive_WorldSC") {}

	void OnAfterConfigLoad(bool reload)
	{
        if (!reload) {
            std::string conf_path = _CONF_DIR;
            std::string cfg_file = conf_path + "/AutoRevive.conf";
        
            std::string cfg_def_file = cfg_file +".dist";
            sConfigMgr->LoadMore(cfg_def_file.c_str());
            
            sConfigMgr->LoadMore(cfg_file.c_str());

            if (!sConfigMgr->LoadMore(cfg_file.c_str()))
            {
                sLog->outString();
                sLog->outError("Config: Invalid or missing configuration file : %s", cfg_file.c_str());
                sLog->outError("Config: Verify that the file exists and has \'[worldserver]' written in the top of the file!");
                sLog->outError("Config: Use default settings!");
                sLog->outString();
            }
        }
	}
};

void AddAutoReviveScripts()
{
    new AutoRevive_PlayerSC();
	new AutoRevive_WorldSC();
}

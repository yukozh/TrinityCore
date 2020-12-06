#include <algorithm>

#include "DatabaseEnv.h"
#include "GossipDef.h"
#include "Log.h"
#include "MenuMgr.h"
#include "Player.h"
#include "ScriptedGossip.h"

void MenuManager::LoadFromDB() 
{
    //                                               0   1        2       3       4     5           6              7          8          9          10             11       12
    QueryResult result = WorldDatabase.Query("select id, groupId, itemId, iconId, text, `function`, teleportMapId, teleportX, teleportY, teleportZ, requiredLevel, faction, targetMenuGroupId from pomelo_menu_templates");
    if (!result) 
    {
        TC_LOG_INFO("pomelo", ">> Load menu templates failed.");
        return;
    }

    size_t count = 0;
    items.clear();
    do
    {
        Field* fields = result->Fetch();
        TeleportStoneMenuItem item;
        item.id = fields[0].GetUInt32();
        item.groupId = fields[1].GetUInt32();
        item.itemId = fields[2].GetUInt32();
        item.iconId = fields[3].GetUInt32();
        item.text = fields[4].GetString();
        item.function = (MenuFunction)fields[5].GetUInt32();
        item.teleportMapId = fields[6].GetUInt32();
        item.teleportX = fields[7].GetFloat();
        item.teleportY = fields[8].GetFloat();
        item.teleportZ = fields[9].GetFloat();
        item.requiredLevel = fields[10].GetUInt32();
        item.faction = (MenuFaction)fields[11].GetUInt32();
        item.targetMenuGroupId = fields[12].GetUInt32();
        items[item.groupId].push_back(item);
        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("pomelo", ">> Loaded %u menu templates.", count);
}

void MenuManager::GenerateGossipMenu(Player* player, uint32 groupId, ObjectGuid guid)
{
	// Clear menu
    player->PlayerTalkClass->ClearMenus();

	if (items.size())
	{
		if (player->IsAlliance())
		{
			AddFilteredGossipMenuForPlayer(groupId, player, guid, MENU_FACTION_ALLIANCE);
			AddFilteredGossipMenuForPlayer(groupId, player, guid, MENU_FACTION_HORDE);
		}
		else // Horde
		{
			AddFilteredGossipMenuForPlayer(groupId, player, guid, MENU_FACTION_HORDE);
			AddFilteredGossipMenuForPlayer(groupId, player, guid, MENU_FACTION_ALLIANCE);
		}
	}
	AddFilteredGossipMenuForPlayer(groupId, player, guid, MENU_FACTION_NONE);
    player->PlayerTalkClass->GetGossipMenu().SetMenuId(groupId);
    SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, guid);
}

void MenuManager::AddFilteredGossipMenuForPlayer(uint32 groupId, Player* player, ObjectGuid guid, MenuFaction faction)
{
    if (items.find(groupId) == items.end())
    {
        return;
    }

    for (size_t i = 0; i < items[groupId].size(); ++i)
    {
        if (items[groupId][i].faction != faction)
        {
            continue;
        }

        if (items[groupId][i].requiredLevel > player->GetLevel())
        {
            continue;
        }

        player->PlayerTalkClass->GetGossipMenu().AddMenuItem(items[groupId][i].itemId, items[groupId][i].iconId, items[groupId][i].text, items[groupId][i].itemId, items[groupId][i].itemId, "", 0, false);
    }
}

const TeleportStoneMenuItem* MenuManager::GetMenuItem(uint32 groupId, uint32 itemId) const
{
    auto result = items.find(groupId);
    if (result == items.end())
    {
        return nullptr;
    }

    for (size_t i = 0; i < result->second.size(); ++i)
    {
        if (result->second[i].itemId == itemId)
        {
            return &result->second[i];
        }
    }

    return nullptr;
}

MenuManager sMenuMgr;

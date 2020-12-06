#ifndef MENUMGR_H
#define MENUMGR_H

#include <unordered_map>
#include <vector>
#include <string>

#include "Define.h"
#include "Player.h"

enum MenuFaction
{
    MENU_FACTION_NONE           = 0,
    MENU_FACTION_ALLIANCE       = 1,
    MENU_FACTION_HORDE          = 2
};

enum MenuFunction 
{
    MENU_FUNCTION_SHOW_SUBMENU  = 0,
    MENU_FUNCTION_TELEPORT      = 1
};

struct TeleportStoneMenuItem 
{
    uint32         id;
    uint32         groupId;
    uint32         itemId;
    uint32         iconId;
    std::string    text;
    MenuFunction   function;
    uint32         teleportMapId;
    float          teleportX;
    float          teleportY;
    float          teleportZ;
    uint32         requiredLevel;
    MenuFaction    faction;
    uint32         targetMenuGroupId;
};

class MenuManager
{
public:
    void LoadFromDB();
    void GenerateGossipMenu(Player* player, uint32 groupId, ObjectGuid guid);
    const TeleportStoneMenuItem* GetMenuItem(uint32 groupId, uint32 itemId) const;

private:
    std::unordered_map<uint32, std::vector<TeleportStoneMenuItem>> items;
    void AddFilteredGossipMenuForPlayer(uint32, Player*, ObjectGuid, MenuFaction);
};

extern MenuManager sMenuMgr;

#endif

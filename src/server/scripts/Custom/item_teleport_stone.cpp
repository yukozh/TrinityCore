#include "CustomizedCurrencyMgr.h"
#include "GossipDef.h"
#include "Item.h"
#include "Language.h"
#include "MenuMgr.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "World.h"
#include "WorldSession.h"

class item_teleport_stone : public ItemScript
{
public:
    item_teleport_stone() : ItemScript("item_teleport_stone") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets) override
    {
        if (!DetectAttacks(player)) 
        {
            sMenuMgr.GenerateGossipMenu(player, TeleportStoneGossipListId, item->GetGUID());
        }
        return true;
    }

    bool OnGossipSelect(Player* player, Item* item, uint32 menuItemId, uint32 menuGroupId) override
    {
        auto menuItem = sMenuMgr.GetMenuItem(menuGroupId, menuItemId);
        if (menuItem)
        {
            switch (menuItem->function)
            {
            case MENU_FUNCTION_SHOW_SUBMENU:
                sMenuMgr.GenerateGossipMenu(player, menuItem->targetMenuGroupId, item->GetGUID());
                break;
            case MENU_FUNCTION_TELEPORT:
                player->CastSpell(player, TeleportVisualSpell, true);
                player->TeleportTo(menuItem->teleportMapId, menuItem->teleportX, menuItem->teleportY, menuItem->teleportZ, player->GetOrientation(), 0);
                player->PlayerTalkClass->SendCloseGossip();
                break;
            case MENU_FUNCTION_GET_CURRENCY_BALANCES: {
                auto definitions = sCustomizedCurrencyMgr.GetDefinitions();
                for (const auto def : definitions)
                {
                    sCustomizedCurrencyMgr.GetBalance(player->GetSession()->GetAccountId(), def.id, std::function<void(int32)>([player, def](int32 balance) {
                        // Yuko: Will the player already be disposed here?
                        sWorld->SendServerMessage(SERVER_MSG_STRING, Trinity::StringFormat(sObjectMgr->GetTrinityString(LANG_CURRENCY_BALANCE)->Content.at(0), def.name.c_str(), balance), player);
                    }));
                }
                player->PlayerTalkClass->SendCloseGossip();
                break;
            }
            default:
                return true;
            }
        }
        else
        {
            return true;
        }

        return true;
    }

private:
    const uint32 TeleportStoneGossipListId = 7000U;
    const uint32 TeleportVisualSpell = 35517U;

    bool DetectAttacks(Player* player)
    {
        if (player->IsInCombat())
        {
            //player->GetSession()->SendNotification(LANG_TELE_CANNOT_USE_WHEN_ATTACK);
            player->PlayerTalkClass->SendCloseGossip();
            return true;
        }
        return false;
    }
};

void AddSC_item_teleport_stone()
{
    new item_teleport_stone();
}

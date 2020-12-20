#include "CustomizedCurrencyMgr.h"
#include "Group.h"
#include "Item.h"
#include "Language.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "World.h"
#include "WorldSession.h"

class item_revive_all : public ItemScript
{
public:
    item_revive_all() : ItemScript("item_revive_all") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets) override {
        if (!DetectAttacks(player) && player->IsAlive()) {
            Group* pGroup = player->GetGroup();
            if (!pGroup) {
                player->GetSession()->SendNotification(LANG_MUST_IN_TEAM_OR_GROUP);
                return true;
            }

            size_t count = 0;
            for (GroupReference* itr = pGroup->GetFirstMember(); itr != nullptr; itr = itr->next()) {
                Player* member = itr->GetSource();
                uint32 mapId = player->GetMapId();

                if (!member->IsInWorld() || !member->isDead()) {
                    continue;
                }

                if (member->GetMapId() != mapId) {
                    continue;
                }

                float distance = player->GetDistance(member);
                if (distance > 100) {
                    continue;
                }

                player->CastSpell(member, 24341, true);
                ++count;
            }

            if (count == 0) {
                player->GetSession()->SendNotification(LANG_NO_BODY_TO_REVIVE);
                return true;
            }

            player->DestroyItem(item->GetBagSlot(), item->GetSlot(), true);
        }

        return true;
    }

private:
    bool DetectAttacks(Player* player) {
        if (player->IsInCombat()) {
            player->GetSession()->SendNotification(LANG_CANNOT_USE_WHEN_ATTACK);
            return true;
        }
        return false;
    }
};

void AddSC_item_revive_all() {
    new item_revive_all();
}

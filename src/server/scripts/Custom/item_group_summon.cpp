#include "CustomizedCurrencyMgr.h"
#include "Group.h"
#include "Item.h"
#include "Language.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "World.h"
#include "WorldSession.h"

class item_group_summon: public ItemScript
{
public:
    item_group_summon() : ItemScript("item_group_summon") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets) override
    {
        if (!DetectAttacks(player) && player->IsAlive())
        {
            Group* group = player->GetGroup();
            if (!group)
            {
                player->GetSession()->SendNotification(LANG_MUST_IN_TEAM_OR_GROUP);
                return true;
            }

            for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
            {
                Player* member = itr->GetSource();

                if (!member->IsInWorld())
                    continue;

                if (member->GetGUID().GetRawValue() == player->GetGUID().GetRawValue())
                    continue;

                SummonSinglePlayer(player, member);
            }

            player->DestroyItem(item->GetBagSlot(), item->GetSlot(), true);
        }
        return true;
    }

private:
    bool DetectAttacks(Player* player)
    {
        if (player->IsInCombat())
        {
            player->GetSession()->SendNotification(LANG_CANNOT_USE_WHEN_ATTACK);
            return true;
        }
        return false;
    }

    bool SummonSinglePlayer(Player* caster, Player* target)
    {
        if (target->GetAura(23445))
            return false;

        float x, y, z;
        caster->GetPosition(x, y, z);
        target->SendSummonRequestFrom(caster);
        return true;
    }
};

void AddSC_item_group_summon()
{
    new item_group_summon();
}

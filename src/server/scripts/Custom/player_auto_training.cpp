#include <unordered_map>
#include <vector>

#include "DatabaseEnv.h"
#include "DbConfigMgr.h"
#include "Define.h"
#include "ScriptMgr.h"

struct AutoTrainingSpell {
    uint32 spellId;
    uint8  requiredLevel;
    bool   enforce;
};

class player_auto_training : public PlayerScript {
public:
    player_auto_training() : PlayerScript("player_auto_training") {
        //                                               0        1        2              3
        QueryResult result = WorldDatabase.Query("select classId, spellId, requiredLevel, enforce from pomelo_auto_training");
        if (!result)
        {
            return;
        }

        spells.clear();
        do
        {
            Field* fields = result->Fetch();
            AutoTrainingSpell item;
            item.spellId = fields[1].GetUInt32();
            item.requiredLevel = fields[2].GetUInt8();
            item.enforce = fields[3].GetBool();
            spells[fields[0].GetUInt8()].push_back(item);
        } while (result->NextRow());
    }

    void OnLevelChanged(Player* player, uint8 oldLevel) override {
        if (sDbConfigMgr.GetBool("AutoTraining.Spell")) {
            for (auto i = spells[player->GetClass()].begin(); i != spells[player->GetClass()].end(); ++i) {
                if (!i->enforce && player->GetLevel() < i->requiredLevel) {
                    continue;
                }
                player->LearnSpell(i->spellId, false);
            }
            for (auto i = spells[CommonClassId].begin(); i != spells[CommonClassId].end(); ++i) {
                if (!i->enforce && player->GetLevel() < i->requiredLevel) {
                    continue;
                }
                player->LearnSpell(i->spellId, false);
            }
        }

        if (sDbConfigMgr.GetBool("AutoTraining.Skill")) {
            player->UpdateWeaponsSkillsToMaxSkillsForLevel();
        }
    }

private:
    std::unordered_map<uint8, std::vector<AutoTrainingSpell>> spells;
    uint8 CommonClassId = 0;
};

void AddSC_player_auto_training()
{
    new player_auto_training();
}

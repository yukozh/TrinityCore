#include "OnlineRewardMgr.h"
#include "World.h"

void OnlineRewardManager::LoadFromDB() {
    //                                               0   1     2         3       4
    QueryResult result = WorldDatabase.Query("select id, type, rewardId, amount, `interval` from pomelo_online_rewards");
    if (!result)
    {
        TC_LOG_INFO("pomelo", ">> Load online rewards failed.");
        return;
    }

    items.clear();
    do
    {
        Field* fields = result->Fetch();
        OnlineRewardRule item;
        item.id = fields[0].GetUInt32();
        item.type = (OnlineRewardType)fields[1].GetUInt32();
        item.rewardId = fields[2].GetUInt32();
        item.amount = fields[3].GetUInt32();
        item.interval = fields[4].GetUInt32();
        items.push_back(item);
    } while (result->NextRow());

    TC_LOG_INFO("pomelo", ">> Loaded %u online rewards.", items.size());
}

OnlineRewardManager sOnlineRewardMgr;

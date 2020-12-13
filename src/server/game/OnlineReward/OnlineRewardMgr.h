#ifndef ONLINEREWARDMGR_H
#define ONLINEREWARDMGR_H

#include <vector>

#include "Define.h"

enum OnlineRewardType {
    ONLINE_REWARD_TYPE_MONEY,
    ONLINE_REWARD_TYPE_CUSTOMIZED_CURRENCY,
    ONLINE_REWARD_TYPE_ITEM
};

struct OnlineRewardRule {
    uint32           id;
    OnlineRewardType type;
    uint32           rewardId; // Currency ID or Item ID
    uint32           amount;
    uint32           interval; // Unit: seconds
};

class OnlineRewardManager {
public:
    void LoadFromDB();
    const std::vector<OnlineRewardRule> GetRules() const { return items; };

private:
    std::vector<OnlineRewardRule> items;
};

extern OnlineRewardManager sOnlineRewardMgr;

#endif

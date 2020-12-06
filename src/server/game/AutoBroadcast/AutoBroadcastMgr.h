#ifndef AUTOBROADCASTMGR_H
#define AUTOBROADCASTMGR_H

#include <string>
#include <vector>

#include "Define.h"

struct AutoBroadcastMessage
{
    uint32      id;
    std::string message;
};

class AutoBroadcastManager
{
public:
    void LoadFromDB();
    void SendBroadcast();

private:
    const uint32 PlainTextServerStringId = 80000U;

    std::vector<AutoBroadcastMessage> items;
    uint32 current;
};

extern AutoBroadcastManager sAutoBroadcastMgr;

#endif

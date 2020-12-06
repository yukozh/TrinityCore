#include "AutoBroadcastMgr.h"
#include "World.h"

void AutoBroadcastManager::LoadFromDB()
{
    //                                               0   1
    QueryResult result = WorldDatabase.Query("select id, message from pomelo_auto_broadcast order by priority desc");
    if (!result)
    {
        TC_LOG_INFO("pomelo", ">> Load auto broadcast messages failed.");
        return;
    }

    items.clear();
    do
    {
        Field* fields = result->Fetch();
        AutoBroadcastMessage item;
        item.id = fields[0].GetUInt32();
        item.message = fields[1].GetString();
        items.push_back(item);
    } while (result->NextRow());

    current = 0;

    TC_LOG_INFO("pomelo", ">> Loaded %u auto broadcast messages.", items.size());
}

void AutoBroadcastManager::SendBroadcast()
{
    if (items.empty())
    {
        return;
    }

    sWorld->SendServerMessage(SERVER_MSG_STRING, items[current].message);
    ++current;
    if (current >= items.size())
    {
        current = 0;
    }
}

AutoBroadcastManager sAutoBroadcastMgr;

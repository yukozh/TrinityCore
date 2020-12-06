#include <algorithm>
#include <sstream>

#include "DatabaseEnv.h"
#include "DbConfigMgr.h"

void DbConfigManager::LoadFromDB()
{
    //                                               0      1
    QueryResult result = WorldDatabase.Query("select `key`, `value` from pomelo_configurations");
    if (!result)
    {
        TC_LOG_INFO("pomelo", ">> Load DB-based configurations failed.");
        return;
    }

    items.clear();
    do
    {
        Field* fields = result->Fetch();
        std::string key = fields[0].GetString();
        std::string value = fields[1].GetString();
        items[key] = value;
    } while (result->NextRow());

    TC_LOG_INFO("pomelo", ">> Loaded %u DB-based configurations.", items.size());
}

float DbConfigManager::StringToFloat(std::string value)
{
    std::istringstream iss(value);
    float ret;
    iss >> ret;
    return ret;
}

uint32 DbConfigManager::StringToUInt32(std::string value)
{
    std::istringstream iss(value);
    uint32 ret;
    iss >> ret;
    return ret;
}

int32 DbConfigManager::StringToInt32(std::string value)
{
    std::istringstream iss(value);
    int32 ret;
    iss >> ret;
    return ret;
}

bool DbConfigManager::StringToBool(std::string value)
{
    std::string copy = std::string(value);
    std::transform(copy.begin(), copy.end(), copy.begin(), ::tolower);

    if (copy == "false")
    {
        return false;
    }

    return true;
}

DbConfigManager sDbConfigMgr;

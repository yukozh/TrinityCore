#ifndef DBCONFIGMGR_H
#define DBCONFIGMGR_H

#include <string>
#include <unordered_map>

#include "Define.h"

class DbConfigManager
{
public:
    void LoadFromDB();
    bool Exists(std::string key) const { return items.find(key) != items.end(); }
    std::string GetString(std::string key) const { return items.find(key)->second; };
    float GetFloat(std::string key) { return StringToFloat(GetString(key)); };
    uint32 GetUInt32(std::string key) { return StringToUInt32(GetString(key)); };
    int32 GetInt32(std::string key) { return StringToInt32(GetString(key)); };
    bool GetBool(std::string key) { return StringToBool(GetString(key)); };

private:
    std::unordered_map<std::string, std::string> items;
    float StringToFloat(std::string value);
    uint32 StringToUInt32(std::string value);
    int32 StringToInt32(std::string value);
    bool StringToBool(std::string value);
};

extern DbConfigManager sDbConfigMgr;

#endif

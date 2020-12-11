#include <functional>
#include <shared_mutex>
#include <string>
#include <thread>
#include <queue>
#include <vector>

#include "Define.h"

struct CurrencyDefinition
{
    uint32 id;
    std::string name;
};

class CustomizedCurrencyMgr
{
public:
    CustomizedCurrencyMgr();
    ~CustomizedCurrencyMgr();
    void LoadFromDB();
    std::string GetName(uint32 currencyId) const;
    void GetBalance(uint32 accountId, uint32 currencyId, std::function<void(int32)> callback);
    void SetBalance(uint32 accountId, uint32 currencyId, int32 delta, std::function<void(bool)> callback);
    const std::vector<CurrencyDefinition> GetDefinitions() const { return definitions; };

private:
    std::vector<CurrencyDefinition> definitions;
    std::thread workThread;
    std::queue<std::function<void()>> workQueue;
    std::shared_mutex workMutex;
};

extern CustomizedCurrencyMgr sCustomizedCurrencyMgr;

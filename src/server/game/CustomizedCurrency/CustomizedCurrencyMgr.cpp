#ifdef _WINDOWS
#include "Windows.h"
#else
#include "pthread.h"
#endif

#include "CustomizedCurrencyMgr.h"

CustomizedCurrencyMgr::CustomizedCurrencyMgr() {
    workThread = std::thread([this]() {
        while (true) {
            if (workQueue.empty()) {
                std::this_thread::yield();
                continue;
            }

            workMutex.lock();
            auto job = workQueue.front();
            workQueue.pop();
            workMutex.unlock();
            job();
        }
    });
    workThread.detach();
}

CustomizedCurrencyMgr::~CustomizedCurrencyMgr() {
#ifdef _WINDOWS
    TerminateThread(workThread.native_handle(), 0);
#else
    pthread_cancel(workThread.native_handle());
#endif
}

void CustomizedCurrencyMgr::LoadFromDB() {
    //                                               0   1
    QueryResult result = LoginDatabase.Query("select id, name from pomelo_customized_currency order by priority desc");
    if (!result)
    {
        TC_LOG_INFO("pomelo", ">> Load auto broadcast messages failed.");
        return;
    }

    definitions.clear();
    do
    {
        Field* fields = result->Fetch();
        CurrencyDefinition item;
        item.id = fields[0].GetUInt32();
        item.name = fields[1].GetString();
        definitions.push_back(item);
    } while (result->NextRow());

    TC_LOG_INFO("pomelo", ">> Loaded %u customized currencies.", definitions.size());
}

std::string CustomizedCurrencyMgr::GetName(uint32 id) const {
    auto result = std::find_if(definitions.begin(), definitions.end(), [id](CurrencyDefinition item) {
        return item.id == id;
    });

    if (result == definitions.end()) {
        return "";
    }

    return result->name;
}

void CustomizedCurrencyMgr::GetBalance(uint32 accountId, uint32 currencyId, std::function<void(int32)> callback) {
    auto job = std::function<void()>([accountId, currencyId, callback]() {
        QueryResult result = LoginDatabase.PQuery("select amount from pomelo_customized_currency_balance where accountId = %u and currencyId = %u", accountId, currencyId);

        if (!result) {
            callback(0);
            return;
        }

        Field* fields = result->Fetch();
        auto balance = fields[0].GetInt32();
        callback(balance);
    });

    workMutex.lock();
    workQueue.push(job);
    workMutex.unlock();
}

void CustomizedCurrencyMgr::SetBalance(uint32 accountId, uint32 currencyId, int32 delta, std::function<void(bool)> callback) {
    auto job = std::function<void()>([accountId, currencyId, delta, callback]() {
        QueryResult result = LoginDatabase.PQuery("select amount from pomelo_customized_currency_balance where accountId = %u and currencyId = %u", accountId, currencyId);
        int32 balance = 0;
        if (result) {
            Field* fields = result->Fetch();
            balance = fields[0].GetInt32();
        }
        else {
            LoginDatabase.PExecute("insert into pomelo_customized_currency_balance (accountId, currencyId, amount) values (%u, %u, %d)", accountId, currencyId, 0);
        }

        balance += delta;
        if (balance < 0) {
            callback(false);
        }

        LoginDatabase.PExecute("update pomelo_customized_currency_balance set amount = %d where accountId = %u and currencyId = %u", balance, accountId, currencyId);
        callback(true);
    });
}

CustomizedCurrencyMgr sCustomizedCurrencyMgr;

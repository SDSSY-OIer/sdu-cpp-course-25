#pragma once

#include <vector>
#include <memory>
#include <string>

#include "Crypto.h"
#include "Records.h"
#include "Utils.h"

class DesktopAssistant {
public:
    DesktopAssistant() = default;
    void run();  // 驱动主循环
    std::vector<unsigned char> salt_;      // 16 bytes
    KeyMaterial km_;                       // 会话密钥
    bool vault_loaded_{false};


private:
    std::vector<std::shared_ptr<FinanceRecord>> financeRecords;
    std::vector<std::shared_ptr<ReminderRecord>> reminderRecords;
    std::vector<std::shared_ptr<PasswordRecord>> passwordRecords;

    const std::string financeFile  = "finance.txt";
    const std::string reminderFile = "reminder.txt";
    const std::string passwordFile = "password.txt";

    // 数据加载/保存
    void loadFinanceData();
    void loadReminderData();
    void loadPasswordData();
    void saveFinanceData();
    void saveReminderData();
    void savePasswordData();

    // 各种操作
    void addFinance();
    void queryFinanceByMonth(const std::string& month) const;
    void addReminder();
    void queryReminderByDate(const std::string& date) const;
    void addPassword();
    void searchPassword(const std::string& website) const;

    bool openOrInitVault();        // 加载/初始化密码库
    void encryptAndSavePasswords();

    // 辅助
    void waitForEnter() const;
    std::string toLowerCopy(const std::string& s) const;
};


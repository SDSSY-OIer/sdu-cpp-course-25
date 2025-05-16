#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <memory>

// -----------------------------------------------------------------------------
// Record 基类
// -----------------------------------------------------------------------------
class Record {
public:
    virtual ~Record() = default;
    virtual std::string toFileString() const = 0;
    virtual void fromFileString(const std::string& line) = 0;
    virtual void display() const = 0;
};

// -----------------------------------------------------------------------------
// FinanceRecord：财务记录
// -----------------------------------------------------------------------------
class FinanceRecord : public Record {
private:
    std::string date;     // 格式 YYYY-MM-DD
    double amount;
    std::string category;
public:
    FinanceRecord() = default;
    FinanceRecord(const std::string& d, double a, const std::string& c)
        : date(d), amount(a), category(c) {}

    const std::string& getDate() const { return date; }
    double getAmount()    const { return amount; }
    const std::string& getCategory() const { return category; }

    std::string toFileString() const override {
        return date + "," + std::to_string(amount) + "," + category;
    }
    void fromFileString(const std::string& line) override {
        size_t p1 = line.find(',');
        size_t p2 = line.find(',', p1 + 1);
        date     = line.substr(0, p1);
        amount   = std::stod(line.substr(p1 + 1, p2 - p1 - 1));
        category = line.substr(p2 + 1);
    }
    void display() const override {
    std::cout << std::left
              << std::setw(12) << date
              << std::setw(12) << amount
              << std::setw(25) << category  // ← 列宽由 15 → 25
              << '\n';
    }
    static bool compareByDate(const std::shared_ptr<FinanceRecord>& a,
                              const std::shared_ptr<FinanceRecord>& b) {
        return a->date < b->date;
    }
};

// -----------------------------------------------------------------------------
// ReminderRecord：提醒记录
// -----------------------------------------------------------------------------
class ReminderRecord : public Record {
private:
    std::string datetime;  // 格式 YYYY-MM-DD HH:MM
    std::string content;
public:
    ReminderRecord() = default;
//    ReminderRecord(const std::string& dt, const std::string& c)
//        : datetime(dt), content(c) {}

    const std::string& getDatetime() const { return datetime; }
    const std::string& getContent()  const { return content; }

//    std::string toFileString() const override {
//        return datetime + "," + content;
//    }
//    void fromFileString(const std::string& line) override {
//        size_t p = line.find(',');
//        datetime = line.substr(0, p);
//        content  = line.substr(p + 1);
//    }
//    void display() const override {
//        std::cout << std::left << std::setw(18) << datetime
//                  << std::setw(40) << content << "\n";
//    }
//    static bool compareByDatetime(const std::shared_ptr<ReminderRecord>& a,
//                                  const std::shared_ptr<ReminderRecord>& b) {
//        return a->datetime < b->datetime;
//    }
    int priority{1};          // 1‑3，数值越大优先级越高

/* 构造函数 */
    ReminderRecord(const std::string& dt,
                   const std::string& c,
                   int pr = 1)
        : datetime(dt), content(c), priority(pr) {}
    
    /* 序列化 */
    std::string toFileString() const override {
        return datetime + "," + std::to_string(priority) + "," + content;
    }
    
    /* 反序列化 */
    void fromFileString(const std::string& line) override {
        auto p1 = line.find(',');
        auto p2 = line.find(',', p1 + 1);
        datetime = line.substr(0, p1);
        priority = std::stoi(line.substr(p1 + 1, p2 - p1 - 1));
        content  = line.substr(p2 + 1);
    }
    
    /* 输出 */
    void display() const override {
        std::cout << std::left
                  << std::setw(20) << datetime
                  << std::setw(10) << priority  // 新增列
                  << content << '\n';
    }
    
    /* 排序：先日期时间，再优先级（高→低） */
    static bool compareByDatetime(const std::shared_ptr<ReminderRecord>& a,
                                  const std::shared_ptr<ReminderRecord>& b)
    {
        if (a->datetime != b->datetime)
            return a->datetime < b->datetime;
        return a->priority > b->priority;
    }

};

// -----------------------------------------------------------------------------
// PasswordRecord：密码记录
// -----------------------------------------------------------------------------
class PasswordRecord : public Record {
private:
    std::string website;
    std::string username;
    std::string password;
public:
    PasswordRecord() = default;
    PasswordRecord(const std::string& w, const std::string& u, const std::string& p)
        : website(w), username(u), password(p) {}

    const std::string& getWebsite()  const { return website; }
    const std::string& getUsername() const { return username; }
    const std::string& getPassword() const { return password; }

    std::string toFileString() const override {
        return website + "," + username + "," + password;
    }
    void fromFileString(const std::string& line) override {
        size_t p1 = line.find(',');
        size_t p2 = line.find(',', p1 + 1);
        website  = line.substr(0, p1);
        username = line.substr(p1 + 1, p2 - p1 - 1);
        password = line.substr(p2 + 1);
    }
    void display() const override {
        std::cout << std::left << std::setw(20) << website
                  << std::setw(20) << username
                  << std::setw(20) << password << "\n";
    }
    static bool compareByWebsite(const std::shared_ptr<PasswordRecord>& a,
                                 const std::shared_ptr<PasswordRecord>& b) {
        return a->website < b->website;
    }
};


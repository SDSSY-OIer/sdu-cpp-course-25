#include "../include/DesktopAssistant.h"
#include "../include/Crypto.h"

#include <openssl/rand.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <limits>

using namespace std;

/* ---------- 私有辅助 ---------- */
string DesktopAssistant::toLowerCopy(const string& s) const
{
    return toLower(s);
}

void DesktopAssistant::waitForEnter() const
{
    std::cout << "\n按 <Enter> 返回菜单…";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

/* ---------- 数据加载 ---------- */
bool DesktopAssistant::openOrInitVault()
{
    constexpr size_t SALT_LEN = 16;
    const std::string vault_file = "password.dat";
    std::ifstream fin(vault_file, std::ios::binary);

    std::string master;
    if (!fin) {                                   // 首次运行
        std::cout << "未检测到密码库，创建新库。\n"
                  << "请设置 Master Password: ";
        std::getline(std::cin, master);

        salt_.resize(SALT_LEN);
        RAND_bytes(salt_.data(), SALT_LEN);
        km_ = deriveKey(master, salt_);

        encryptAndSavePasswords();                // 空列表 -> 加密存盘
        vault_loaded_ = true;
        return true;
    }

    /* 读取 salt */
    salt_.resize(SALT_LEN);
    fin.read(reinterpret_cast<char*>(salt_.data()), SALT_LEN);
    // 正确读入二进制剩余数据
    std::istreambuf_iterator<char> it(fin);
    std::istreambuf_iterator<char> end;
    std::string enc_str(it, end);
    std::vector<unsigned char> enc;
    enc.assign(enc_str.begin(), enc_str.end());

    std::cout << "输入 Master Password: ";
    std::getline(std::cin, master);
    try {
        km_ = deriveKey(master, salt_);
        auto plain = aes256Decrypt(enc, km_);
        /* 反序列化到 password_records_ */
        std::istringstream iss(
            std::string(reinterpret_cast<char*>(plain.data()), plain.size()));
        std::string line;
        while (std::getline(iss, line) && !line.empty()) {
            auto rec = std::make_shared<PasswordRecord>();
            rec->fromFileString(line);
            passwordRecords.push_back(rec);
        }
        vault_loaded_ = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "解密失败：" << e.what() << '\n';
        return false;
    }
}

template <typename Vec, typename Rec, typename Cmp>
static void load_records(const string& fname, Vec& vec, Cmp cmp)
{
    ifstream f(fname);
    if (!f) return;
    string line;
    while (getline(f, line) && !line.empty()) {
        auto rec = make_shared<Rec>();
        rec->fromFileString(line);
        vec.push_back(rec);
    }
    sort(vec.begin(), vec.end(), cmp);
}

void DesktopAssistant::loadFinanceData()
{ load_records<vector<shared_ptr<FinanceRecord>>,
               FinanceRecord>(financeFile, financeRecords,
                              FinanceRecord::compareByDate); }

void DesktopAssistant::loadReminderData()
{ load_records<vector<shared_ptr<ReminderRecord>>,
               ReminderRecord>(reminderFile, reminderRecords,
                               ReminderRecord::compareByDatetime); }

void DesktopAssistant::loadPasswordData()
{ load_records<vector<shared_ptr<PasswordRecord>>,
               PasswordRecord>(passwordFile, passwordRecords,
                               PasswordRecord::compareByWebsite); }

/* ---------- 数据保存 ---------- */
void DesktopAssistant::encryptAndSavePasswords()
{
    std::ostringstream oss;
    for (const auto& r : passwordRecords)
        oss << r->toFileString() << '\n';
    std::string plain = oss.str();
    auto enc = aes256Encrypt(
        {reinterpret_cast<const unsigned char*>(plain.data()),
         reinterpret_cast<const unsigned char*>(plain.data()) + plain.size()},
        km_);

    std::ofstream fout("password.dat", std::ios::binary|std::ios::trunc);
    fout.write(reinterpret_cast<char*>(salt_.data()), salt_.size());
    fout.write(reinterpret_cast<char*>(enc.data()), enc.size());
}

template <typename Vec>
static void save_records(const string& fname, const Vec& vec)
{
    ofstream f(fname, ios::trunc);
    for (const auto& r : vec) f << r->toFileString() << '\n';
}

void DesktopAssistant::saveFinanceData()  { save_records(financeFile , financeRecords ); }
void DesktopAssistant::saveReminderData() { save_records(reminderFile, reminderRecords); }
void DesktopAssistant::savePasswordData() { save_records(passwordFile, passwordRecords); }

/* ---------- 业务操作 ---------- */
void DesktopAssistant::addFinance()
{
    string d,c; double a;
    cout << "日期(YYYY-MM-DD, 留空=今天): "; getline(cin,d);
    if(d.empty()) d = getCurrentDate();
    else d = normalizeDate(d); 
    cout << "金额: ";           cin >> a; cin.ignore(numeric_limits<streamsize>::max(),'\n');
    cout << "类别: ";           getline(cin,c);

    financeRecords.emplace_back(make_shared<FinanceRecord>(d,a,c));
    sort(financeRecords.begin(),financeRecords.end(),FinanceRecord::compareByDate);
    saveFinanceData();
}

void DesktopAssistant::queryFinanceByMonth(const string& month) const
{
    cout << left << setw(14) << "日期" << setw(14) << "金额" << "类别\n";
    auto month_nor = normalizeMonth(month);
    for (const auto& r : financeRecords)
        if (r->getDate().rfind(month_nor,0)==0) r->display();
}

//void DesktopAssistant::addReminder()
//{
//    string dt,c;
//    cout << "日期时间(留空=当前): "; getline(cin,dt); if(dt.empty()) dt = getCurrentDateTime();
//    cout << "内容: ";               getline(cin,c);
//
//    reminderRecords.emplace_back(make_shared<ReminderRecord>(dt,c));
//    sort(reminderRecords.begin(),reminderRecords.end(),ReminderRecord::compareByDatetime);
//    saveReminderData();
//}

void DesktopAssistant::addReminder()
{
    std::string dt, content;
    int pr = 1;

    std::cout << "日期时间(YYYY-MM-DD HH:MM, 留空=当前): ";
    std::getline(std::cin, dt);
    if (dt.empty()) dt = getCurrentDateTime();
    else {
         // 仅规范化日期部分，时间保持原样
         auto sp = dt.find(' ');
         if (sp != std::string::npos)
             dt = normalizeDate(dt.substr(0, sp)) + dt.substr(sp);
         else
             dt = normalizeDate(dt);
    }

    std::cout << "内容: ";
    std::getline(std::cin, content);

    std::cout << "优先级(1‑3, 留空=1): ";
    std::string tmp;
    std::getline(std::cin, tmp);
    if (!tmp.empty()) pr = std::clamp(std::stoi(tmp), 1, 3);

    reminderRecords.emplace_back(
        std::make_shared<ReminderRecord>(dt, content, pr));

    std::sort(reminderRecords.begin(), reminderRecords.end(),
              ReminderRecord::compareByDatetime);
    saveReminderData();
}

void DesktopAssistant::queryReminderByDate(const string& date) const
{
    cout << left << setw(34) << "日期时间" << "内容\n";
    // 仅规范化日期部分
    auto date_nor = normalizeDate(date);
    for (const auto& r : reminderRecords)
        if (r->getDatetime().rfind(date_nor,0)==0) r->display();
}

void DesktopAssistant::addPassword()
{
    string w,u,p;
    cout << "网站: "; getline(cin,w);
    cout << "用户名: "; getline(cin,u);
    cout << "密码: "; getline(cin,p);

    passwordRecords.emplace_back(make_shared<PasswordRecord>(w,u,p));
    sort(passwordRecords.begin(),passwordRecords.end(),PasswordRecord::compareByWebsite);
    //savePasswordData();
    encryptAndSavePasswords();
}

void DesktopAssistant::searchPassword(const string& website) const
{
    string key = toLowerCopy(website);
    cout << left << setw(22) << "网站" << setw(23) << "用户名" << "密码\n";
    for (const auto& r : passwordRecords)
        if (toLowerCopy(r->getWebsite()).find(key)!=string::npos) r->display();
}

/* ---------- 主循环 ---------- */
void DesktopAssistant::run()
{
    if (!openOrInitVault()) return;
    loadFinanceData();
    loadReminderData();
    loadPasswordData();
    while (true) {
        clearScreen();
        std::cout << "===== 桌面助手 =====\n"
                  << "1. 添加财务记录\n"
                  << "2. 查询财务记录\n"
                  << "3. 添加提醒记录\n"
                  << "4. 查询提醒记录\n"
                  << "5. 添加密码记录\n"
                  << "6. 查询密码记录\n"
                  << "7. 退出\n"
                  << "请选择: ";
        int choice{};
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1: addFinance();                  waitForEnter(); break;
            case 2: {
                std::string m;
                std::cout << "月份(YYYY-MM, 空=本月): ";
                std::getline(std::cin, m);
                if (m.empty()) m = getCurrentMonth();
                queryFinanceByMonth(m);
                waitForEnter();
                break;
            }
            case 3: addReminder();                 waitForEnter(); break;
            case 4: {
                std::string d;
                std::cout << "日期(YYYY-MM-DD, 空=今天): ";
                std::getline(std::cin, d);
                if (d.empty()) d = getCurrentDate();
                queryReminderByDate(d);
                waitForEnter();
                break;
            }
            case 5: addPassword();                 waitForEnter(); break;
            case 6: {
                std::string w;
                std::cout << "关键字: ";
                std::getline(std::cin, w);
                searchPassword(w);
                waitForEnter();
                break;
            }
            case 7: return;
            default: std::cout << "无效选项\n";    waitForEnter(); break;
        }
    }

}


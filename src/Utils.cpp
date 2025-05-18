// src/Utils.cpp
#include "../include/Utils.h"
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <regex>
#include <iostream>

std::string getCurrentMonth() {
    auto now = std::chrono::system_clock::now();
    auto t   = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
    localtime_r(&t, &tm);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m");
    return oss.str();
}

std::string getCurrentDate() {
    auto now = std::chrono::system_clock::now();
    auto t   = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
    localtime_r(&t, &tm);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    return oss.str();
}

std::string getCurrentDateTime() {
    auto now = std::chrono::system_clock::now();
    auto t   = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
    localtime_r(&t, &tm);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M");
    return oss.str();
}

std::string toLower(const std::string& s) {
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return r;
}

std::string normalizeDate(const std::string& date)
{
    std::regex re(R"((\d{4})-(\d{1,2})-(\d{1,2}))");
    std::smatch m;
    if (std::regex_match(date, m, re)) {
        int y = std::stoi(m[1]), mo = std::stoi(m[2]), d = std::stoi(m[3]);
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(4) << y << '-'
            << std::setw(2) << mo << '-'
            << std::setw(2) << d;
        return oss.str();               // YYYY-MM-DD
    }
    return date;                        // 原样返回，留给后续校验
}

std::string normalizeMonth(const std::string& month)
{
    std::regex re(R"((\d{4})-(\d{1,2}))");
    std::smatch m;
    if (std::regex_match(month, m, re)) {
        int y = std::stoi(m[1]), mo = std::stoi(m[2]);
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(4) << y << '-'
            << std::setw(2) << mo;
        return oss.str();               // YYYY-MM
    }
    return month;
}

void clearScreen()
{
#ifdef _WIN32
    std::system("cls");
#else
    std::cout << "\x1B[2J\x1B[H";
#endif
}


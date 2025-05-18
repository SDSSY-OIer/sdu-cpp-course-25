#pragma once

#include <string>
#include <ctime>

// 返回格式 "YYYY-MM"
std::string getCurrentMonth();

// 返回格式 "YYYY-MM-DD"
std::string getCurrentDate();

// 返回格式 "YYYY-MM-DD HH:MM"
std::string getCurrentDateTime();

// 将字符串转成小写
std::string toLower(const std::string& s);

// 清屏（Windows 下）
void clearScreen();

// 将 "YYYY-M-D" 规范化为 "YYYY-MM-DD"
std::string normalizeDate(const std::string& date);

// 将 "YYYY-M" 规范化为 "YYYY-MM"
std::string normalizeMonth(const std::string& month);


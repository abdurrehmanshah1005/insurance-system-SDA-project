#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <iostream>
#include <limits>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <filesystem>

namespace Utils {

    // ── Get current date as YYYY-MM-DD ──
    inline std::string getCurrentDate() {
        std::time_t t = std::time(nullptr);
        std::tm tm;
        localtime_s(&tm, &t);
        char buf[11];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
        return std::string(buf);
    }

    // ── Parse month/year from YYYY-MM-DD ──
    inline bool parseDateMonthYear(const std::string& date, int& month, int& year) {
        if (date.size() < 7) return false;
        try {
            year  = std::stoi(date.substr(0, 4));
            month = std::stoi(date.substr(5, 2));
            return month >= 1 && month <= 12;
        } catch (...) { return false; }
    }

    // ── Validate date format YYYY-MM-DD ──
    inline bool isValidDate(const std::string& date) {
        if (date.size() != 10 || date[4] != '-' || date[7] != '-') return false;
        int m, y;
        if (!parseDateMonthYear(date, m, y)) return false;
        try {
            int d = std::stoi(date.substr(8, 2));
            return d >= 1 && d <= 31;
        } catch (...) { return false; }
    }

    // ── Safe integer input ──
    inline int readInt(const std::string& prompt) {
        int value;
        while (true) {
            std::cout << prompt;
            if (std::cin >> value) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return value;
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "  [!] Invalid number. Try again.\n";
        }
    }

    // ── Safe double input ──
    inline double readDouble(const std::string& prompt) {
        double value;
        while (true) {
            std::cout << prompt;
            if (std::cin >> value) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return value;
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "  [!] Invalid number. Try again.\n";
        }
    }

    // ── Safe string input (whole line) ──
    inline std::string readString(const std::string& prompt) {
        std::string value;
        while (true) {
            std::cout << prompt;
            std::getline(std::cin, value);
            if (!value.empty()) return value;
            std::cout << "  [!] Input cannot be empty. Try again.\n";
        }
    }

    // ── Safe date input ──
    inline std::string readDate(const std::string& prompt) {
        while (true) {
            std::string d = readString(prompt);
            if (isValidDate(d)) return d;
            std::cout << "  [!] Invalid date. Use YYYY-MM-DD format.\n";
        }
    }

    // ── Ensure data directory exists ──
    inline void ensureDataDir(const std::string& dir) {
        std::filesystem::create_directories(dir);
    }

    // ── Print a divider ──
    inline void printDivider() {
        std::cout << "----------------------------------------\n";
    }

} // namespace Utils

#endif // UTILS_H

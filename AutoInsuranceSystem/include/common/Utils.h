/**
 * @file Utils.h
 * @brief Utility functions for input handling, date operations, and formatting.
 *
 * Provides common helper functions used across the presentation layer including:
 * - Date formatting and validation (YYYY-MM-DD format)
 * - Safe console input with validation and error recovery
 * - File system utilities for data directory management
 * - Output formatting helpers for consistent display
 */

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
    // Returns today's date formatted as a string for use in record timestamps
    inline std::string getCurrentDate() {
        std::time_t t = std::time(nullptr);
        std::tm tm;
        localtime_s(&tm, &t);
        char buf[11];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
        return std::string(buf);
    }

    // ── Parse month/year from YYYY-MM-DD ──
    // Extracts month and year components from a date string.
    // Returns false if the date format is invalid or month is out of range.
    inline bool parseDateMonthYear(const std::string& date, int& month, int& year) {
        if (date.size() < 7) return false;  // Need at least YYYY-MM
        try {
            year  = std::stoi(date.substr(0, 4));
            month = std::stoi(date.substr(5, 2));
            return month >= 1 && month <= 12;  // Validate month range
        } catch (...) { return false; }
    }

    // ── Validate date format YYYY-MM-DD ──
    // Ensures the string matches the expected date pattern with valid ranges
    inline bool isValidDate(const std::string& date) {
        if (date.size() != 10 || date[4] != '-' || date[7] != '-') return false;
        int m, y;
        if (!parseDateMonthYear(date, m, y)) return false;
        try {
            int d = std::stoi(date.substr(8, 2));
            return d >= 1 && d <= 31;  // Basic day range check
        } catch (...) { return false; }
    }

    // ── Safe integer input ──
    // Prompts user and reads an integer, re-prompting on invalid input.
    // Clears the input stream on failure to prevent infinite loops.
    inline int readInt(const std::string& prompt) {
        int value;
        while (true) {
            std::cout << prompt;
            if (std::cin >> value) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return value;
            }
            // Clear error flags and discard invalid input
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "  [!] Invalid number. Try again.\n";
        }
    }

    // ── Safe double input ──
    // Prompts user and reads a floating-point number with error handling.
    // Used for premium amounts and estimated repair costs.
    inline double readDouble(const std::string& prompt) {
        double value;
        while (true) {
            std::cout << prompt;
            if (std::cin >> value) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return value;
            }
            // Clear error flags and discard invalid input
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "  [!] Invalid number. Try again.\n";
        }
    }

    // ── Safe string input (whole line) ──
    // Reads a full line of text; rejects empty input to prevent blank records
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
    // Reads and validates a date in YYYY-MM-DD format, re-prompting until valid
    inline std::string readDate(const std::string& prompt) {
        while (true) {
            std::string d = readString(prompt);
            if (isValidDate(d)) return d;
            std::cout << "  [!] Invalid date. Use YYYY-MM-DD format.\n";
        }
    }

    // ── Ensure data directory exists ──
    // Creates the data directory and any parent directories if they don't exist.
    // Called at application startup to guarantee file operations won't fail.
    inline void ensureDataDir(const std::string& dir) {
        std::filesystem::create_directories(dir);
    }

    // ── Print a divider ──
    // Outputs a horizontal line for visual separation in console output
    inline void printDivider() {
        std::cout << "----------------------------------------\n";
    }

} // namespace Utils

#endif // UTILS_H

#ifndef FILE_REPOSITORY_H
#define FILE_REPOSITORY_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <algorithm>

// ─────────────────────────────────────────────────────────
// Generic file-based repository (matches FileRepository in class diagram)
//   T must provide: std::string toCSV() const;
//                   static T fromCSV(const std::string&);
//                   int getId() const;
// ─────────────────────────────────────────────────────────
template <typename T>
class FileRepository {
private:
    std::string filename;

public:
    explicit FileRepository(const std::string& fname) : filename(fname) {}

    // ── loadAllRecords ──
    std::vector<T> loadAllRecords() const {
        std::vector<T> records;
        std::ifstream file(filename);
        if (!file.is_open()) return records;
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            try {
                records.push_back(T::fromCSV(line));
            } catch (const std::exception& e) {
                std::cerr << "[WARN] Skipping malformed record in "
                          << filename << ": " << e.what() << "\n";
            }
        }
        return records;
    }

    // ── saveAllRecords (atomic: write temp → rename) ──
    void saveAllRecords(const std::vector<T>& data) const {
        std::string tempFile = filename + ".tmp";
        std::ofstream file(tempFile);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot write to " + tempFile);
        }
        for (const auto& record : data) {
            file << record.toCSV() << "\n";
        }
        file.close();
        std::remove(filename.c_str());
        std::rename(tempFile.c_str(), filename.c_str());
    }

    // ── appendRecord ──
    void appendRecord(const T& record) const {
        std::ofstream file(filename, std::ios::app);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot append to " + filename);
        }
        file << record.toCSV() << "\n";
    }

    // ── Helper: get next available ID ──
    int getNextId() const {
        auto records = loadAllRecords();
        int maxId = 0;
        for (const auto& r : records) {
            if (r.getId() > maxId) maxId = r.getId();
        }
        return maxId + 1;
    }

    // ── Helper: find by ID ──
    T* findById(std::vector<T>& records, int id) const {
        for (auto& r : records) {
            if (r.getId() == id) return &r;
        }
        return nullptr;
    }
};

#endif // FILE_REPOSITORY_H

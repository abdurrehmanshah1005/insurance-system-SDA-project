/**
 * @file FileRepository.h
 * @brief Generic file-based repository implementing the Data Access Layer.
 *
 * This template class provides CRUD operations for any domain entity that
 * supports CSV serialization. It acts as the persistence layer, replacing
 * a database with simple pipe-delimited CSV files as required by the
 * project specification ("You cannot use any DBMS!").
 *
 * Template type T must provide:
 *   - std::string toCSV() const       → serialize record to CSV line
 *   - static T fromCSV(const string&) → deserialize record from CSV line
 *   - int getId() const               → return the unique identifier
 */

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
    std::string filename;  // Path to the CSV data file

public:
    // Constructor: initialize with the path to the data file
    explicit FileRepository(const std::string& fname) : filename(fname) {}

    // ── loadAllRecords ──
    // Read all records from the CSV file and return as a vector.
    // Skips blank lines and lines starting with '#' (comment lines).
    // Malformed records are logged to stderr and skipped.
    std::vector<T> loadAllRecords() const {
        std::vector<T> records;
        std::ifstream file(filename);
        if (!file.is_open()) return records;  // File doesn't exist yet
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;  // Skip comments
            try {
                records.push_back(T::fromCSV(line));
            } catch (const std::exception& e) {
                // Log warning but continue processing remaining records
                std::cerr << "[WARN] Skipping malformed record in "
                          << filename << ": " << e.what() << "\n";
            }
        }
        return records;
    }

    // ── saveAllRecords (atomic: write temp → rename) ──
    // Overwrites the entire file with the provided records.
    // Uses a temporary file and rename to prevent data corruption
    // in case of crashes during write operations.
    void saveAllRecords(const std::vector<T>& data) const {
        std::string tempFile = filename + ".tmp";
        std::ofstream file(tempFile);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot write to " + tempFile);
        }
        // Write each record as a single CSV line
        for (const auto& record : data) {
            file << record.toCSV() << "\n";
        }
        file.close();
        // Atomic replace: remove old file, rename temp to original
        std::remove(filename.c_str());
        std::rename(tempFile.c_str(), filename.c_str());
    }

    // ── appendRecord ──
    // Add a single record to the end of the file without loading all data.
    // More efficient than loadAll + saveAll for insert operations.
    void appendRecord(const T& record) const {
        std::ofstream file(filename, std::ios::app);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot append to " + filename);
        }
        file << record.toCSV() << "\n";
    }

    // ── Helper: get next available ID ──
    // Scans all records to find the maximum ID and returns maxId + 1.
    // Ensures unique sequential IDs across all records.
    int getNextId() const {
        auto records = loadAllRecords();
        int maxId = 0;
        for (const auto& r : records) {
            if (r.getId() > maxId) maxId = r.getId();
        }
        return maxId + 1;
    }

    // ── Helper: find by ID ──
    // Search through a vector of records and return a pointer to the
    // record matching the given ID, or nullptr if not found.
    T* findById(std::vector<T>& records, int id) const {
        for (auto& r : records) {
            if (r.getId() == id) return &r;
        }
        return nullptr;
    }
};

#endif // FILE_REPOSITORY_H

/**
 * @file Staff.h
 * @brief Domain entity representing a staff member of the insurance company.
 *
 * Staff members can be Salesmen, Surveyors, or Managers, each with
 * different system permissions. Authentication is handled via simple
 * password matching. Staff records are stored in staff.csv.
 */

#ifndef STAFF_H
#define STAFF_H

#include <string>
#include <iostream>
#include <sstream>
#include "Enums.h"

// Represents a staff member with role-based access to the system.
// Each staff member has credentials for login authentication.
struct Staff {
    int    staffId = 0;            // Unique identifier for the staff member
    std::string name;              // Full name of the staff member
    StaffRole role = StaffRole::SALESMAN;  // Role determines available operations
    std::string password;          // Password for login authentication

    // Verify the provided password against the stored password
    bool authenticate(const std::string& inputPassword) const {
        return password == inputPassword;
    }

    // Serialize staff record to pipe-delimited CSV format
    std::string toCSV() const {
        return std::to_string(staffId) + "|" + name + "|" +
               staffRoleToString(role) + "|" + password;
    }

    // Reconstruct a Staff object from a CSV line
    static Staff fromCSV(const std::string& line) {
        Staff s;
        std::stringstream ss(line);
        std::string token;
        std::getline(ss, token, '|'); s.staffId = std::stoi(token);
        std::getline(ss, s.name,     '|');
        std::getline(ss, token, '|'); s.role    = stringToStaffRole(token);
        std::getline(ss, s.password, '|');
        return s;
    }

    // Print staff details to the console (excludes password for security)
    void display() const {
        std::cout << "  ID: " << staffId
                  << " | Name: " << name
                  << " | Role: " << staffRoleToString(role) << "\n";
    }

    // Return unique identifier (required by FileRepository template)
    int getId() const { return staffId; }
};

#endif // STAFF_H

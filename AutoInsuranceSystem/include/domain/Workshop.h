/**
 * @file Workshop.h
 * @brief Domain entity representing a registered repair workshop.
 *
 * The insurance company maintains a list of authorized workshops where
 * damaged vehicles can be repaired. Only workshops with REGISTERED status
 * can be assigned to approved claims. Suspended workshops are excluded
 * from assignment operations.
 */

#ifndef WORKSHOP_H
#define WORKSHOP_H

#include <string>
#include <iostream>
#include <sstream>
#include "Enums.h"

// Represents a vehicle repair workshop registered with the insurance company.
// Workshops must be in REGISTERED status to accept claim assignments.
struct Workshop {
    int    workshopId = 0;         // Unique workshop identifier
    std::string name;              // Workshop business name
    std::string address;           // Physical location of the workshop
    std::string contact;           // Contact phone or email
    WorkshopStatus status = WorkshopStatus::REGISTERED;  // Current operational status

    // Check if the workshop is currently active and can accept assignments
    bool isActive() const { return status == WorkshopStatus::REGISTERED; }

    // Serialize workshop data to pipe-delimited CSV format
    std::string toCSV() const {
        return std::to_string(workshopId) + "|" + name + "|" + address + "|" +
               contact + "|" + workshopStatusToString(status);
    }

    // Reconstruct a Workshop object from a CSV line
    static Workshop fromCSV(const std::string& line) {
        Workshop w;
        std::stringstream ss(line);
        std::string token;
        std::getline(ss, token, '|'); w.workshopId = std::stoi(token);
        std::getline(ss, w.name,    '|');
        std::getline(ss, w.address, '|');
        std::getline(ss, w.contact, '|');
        std::getline(ss, token, '|'); w.status = stringToWorkshopStatus(token);
        return w;
    }

    // Display workshop details in formatted console output
    void display() const {
        std::cout << "  ID: " << workshopId
                  << " | Name: " << name
                  << " | Address: " << address
                  << " | Contact: " << contact
                  << " | Status: " << workshopStatusToString(status) << "\n";
    }

    // Return unique identifier (required by FileRepository template)
    int getId() const { return workshopId; }
};

#endif // WORKSHOP_H

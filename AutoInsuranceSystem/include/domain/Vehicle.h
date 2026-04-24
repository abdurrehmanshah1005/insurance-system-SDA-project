/**
 * @file Vehicle.h
 * @brief Domain entity representing a vehicle owned by a customer.
 *
 * Each vehicle is linked to a customer via customerId and can have
 * one or more insurance policies associated with it. The registration
 * number must be unique across the system to prevent duplicate entries.
 */

#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>
#include <iostream>
#include <sstream>

// Represents a vehicle registered in the insurance system.
// Each vehicle belongs to exactly one customer and may be insured.
struct Vehicle {
    int    vehicleId = 0;          // Unique identifier assigned by the system
    std::string registrationNo;    // Unique vehicle registration plate number
    std::string make;              // Manufacturer (e.g., Toyota, Honda)
    std::string model;             // Model name (e.g., Corolla, Civic)
    int    year = 0;               // Year of manufacture
    int    customerId = 0;         // Foreign key linking to the owning customer

    // Serialize vehicle data to pipe-delimited CSV format
    std::string toCSV() const {
        return std::to_string(vehicleId) + "|" + registrationNo + "|" + make + "|" +
               model + "|" + std::to_string(year) + "|" + std::to_string(customerId);
    }

    // Parse a CSV line and reconstruct a Vehicle object
    static Vehicle fromCSV(const std::string& line) {
        Vehicle v;
        std::stringstream ss(line);
        std::string token;
        std::getline(ss, token, '|'); v.vehicleId = std::stoi(token);
        std::getline(ss, v.registrationNo, '|');
        std::getline(ss, v.make,           '|');
        std::getline(ss, v.model,          '|');
        std::getline(ss, token, '|'); v.year       = std::stoi(token);
        std::getline(ss, token, '|'); v.customerId = std::stoi(token);
        return v;
    }

    // Display vehicle information in a formatted console output
    void display() const {
        std::cout << "  ID: " << vehicleId
                  << " | Reg#: " << registrationNo
                  << " | " << make << " " << model << " (" << year << ")"
                  << " | Owner ID: " << customerId << "\n";
    }

    // Return unique identifier (required by FileRepository template)
    int getId() const { return vehicleId; }
};

#endif // VEHICLE_H

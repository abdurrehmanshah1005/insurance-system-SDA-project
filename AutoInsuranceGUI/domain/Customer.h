/**
 * @file Customer.h
 * @brief Domain entity representing a customer of the insurance company.
 *
 * Each customer has a unique ID, personal details, and a registration date.
 * Customers can own multiple vehicles and hold multiple insurance policies.
 * Data is persisted in pipe-delimited CSV format via FileRepository.
 */

#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

// Represents a registered customer in the automobile insurance system.
// Stores personal information and registration date for reporting.
struct Customer {
    int    customerId = 0;         // Unique identifier assigned by the system
    std::string name;              // Full name of the customer
    std::string contact;           // Phone or email contact information
    std::string address;           // Residential or mailing address
    std::string createdDate;       // Registration date in YYYY-MM-DD format

    // ── Serialization ──
    // Convert customer record to pipe-delimited string for CSV storage
    std::string toCSV() const {
        return std::to_string(customerId) + "|" + name + "|" + contact + "|" + address + "|" + createdDate;
    }

    // Reconstruct a Customer object from a pipe-delimited CSV line
    static Customer fromCSV(const std::string& line) {
        Customer c;
        std::stringstream ss(line);
        std::string token;
        std::getline(ss, token, '|'); c.customerId  = std::stoi(token);
        std::getline(ss, c.name,    '|');
        std::getline(ss, c.contact, '|');
        std::getline(ss, c.address, '|');
        std::getline(ss, c.createdDate, '|');
        return c;
    }

    // Print formatted customer details to console
    void display() const {
        std::cout << "  ID: " << customerId
                  << " | Name: " << name
                  << " | Contact: " << contact
                  << " | Address: " << address
                  << " | Registered: " << createdDate << "\n";
    }

    // Return unique identifier (required by FileRepository template)
    int getId() const { return customerId; }
};

#endif // CUSTOMER_H

/**
 * @file InsurancePolicy.h
 * @brief Domain entity representing an automobile insurance policy.
 *
 * A policy links a customer to a specific vehicle with coverage dates
 * and a premium amount. Claims can only be filed against ACTIVE policies
 * where the accident date falls within the coverage period (startDate to endDate).
 */

#ifndef INSURANCE_POLICY_H
#define INSURANCE_POLICY_H

#include <string>
#include <iostream>
#include <sstream>
#include "Enums.h"

// Represents an insurance policy that covers a specific vehicle for a customer.
// Policies define the coverage period and premium paid by the customer.
struct InsurancePolicy {
    int    policyId   = 0;         // Unique policy identifier
    int    customerId = 0;         // ID of the customer who holds this policy
    int    vehicleId  = 0;         // ID of the vehicle covered by this policy
    std::string startDate;         // Coverage start date in YYYY-MM-DD format
    std::string endDate;           // Coverage end date in YYYY-MM-DD format
    double premium    = 0.0;       // Premium amount paid for this policy
    PolicyStatus status = PolicyStatus::ACTIVE;  // Current status of the policy

    // Check if policy is active and covers the given date
    bool isActive(const std::string& currentDate) const {
        return status == PolicyStatus::ACTIVE &&
               currentDate >= startDate && currentDate <= endDate;
    }

    // Serialize policy record to pipe-delimited CSV string
    std::string toCSV() const {
        return std::to_string(policyId) + "|" + std::to_string(customerId) + "|" +
               std::to_string(vehicleId) + "|" + startDate + "|" + endDate + "|" +
               std::to_string(premium) + "|" + policyStatusToString(status);
    }

    // Reconstruct an InsurancePolicy object from a CSV line
    static InsurancePolicy fromCSV(const std::string& line) {
        InsurancePolicy p;
        std::stringstream ss(line);
        std::string token;
        std::getline(ss, token, '|'); p.policyId   = std::stoi(token);
        std::getline(ss, token, '|'); p.customerId = std::stoi(token);
        std::getline(ss, token, '|'); p.vehicleId  = std::stoi(token);
        std::getline(ss, p.startDate, '|');
        std::getline(ss, p.endDate,   '|');
        std::getline(ss, token, '|'); p.premium    = std::stod(token);
        std::getline(ss, token, '|'); p.status     = stringToPolicyStatus(token);
        return p;
    }

    // Print policy information to the console
    void display() const {
        std::cout << "  Policy#: " << policyId
                  << " | Customer: " << customerId
                  << " | Vehicle: " << vehicleId
                  << " | " << startDate << " to " << endDate
                  << " | Premium: " << premium
                  << " | Status: " << policyStatusToString(status) << "\n";
    }

    // Return unique identifier (required by FileRepository template)
    int getId() const { return policyId; }
};

#endif // INSURANCE_POLICY_H

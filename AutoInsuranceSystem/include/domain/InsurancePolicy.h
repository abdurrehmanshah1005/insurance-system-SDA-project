#ifndef INSURANCE_POLICY_H
#define INSURANCE_POLICY_H

#include <string>
#include <iostream>
#include <sstream>
#include "Enums.h"

struct InsurancePolicy {
    int    policyId   = 0;
    int    customerId = 0;
    int    vehicleId  = 0;
    std::string startDate;     // YYYY-MM-DD
    std::string endDate;       // YYYY-MM-DD
    double premium    = 0.0;
    PolicyStatus status = PolicyStatus::ACTIVE;

    bool isActive(const std::string& currentDate) const {
        return status == PolicyStatus::ACTIVE &&
               currentDate >= startDate && currentDate <= endDate;
    }

    std::string toCSV() const {
        return std::to_string(policyId) + "|" + std::to_string(customerId) + "|" +
               std::to_string(vehicleId) + "|" + startDate + "|" + endDate + "|" +
               std::to_string(premium) + "|" + policyStatusToString(status);
    }

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

    void display() const {
        std::cout << "  Policy#: " << policyId
                  << " | Customer: " << customerId
                  << " | Vehicle: " << vehicleId
                  << " | " << startDate << " to " << endDate
                  << " | Premium: " << premium
                  << " | Status: " << policyStatusToString(status) << "\n";
    }

    int getId() const { return policyId; }
};

#endif // INSURANCE_POLICY_H

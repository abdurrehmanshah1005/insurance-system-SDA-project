#ifndef CLAIM_H
#define CLAIM_H

#include <string>
#include <iostream>
#include <sstream>
#include "Enums.h"

struct Claim {
    int    claimId      = 0;
    int    policyId     = 0;
    int    vehicleId    = 0;
    std::string accidentDate;
    std::string claimDate;
    ClaimStatus status  = ClaimStatus::SUBMITTED;
    int    surveyorId   = 0;   // 0 = not assigned
    int    workshopId   = 0;   // 0 = not assigned
    std::string damageDescription;

    void updateStatus(ClaimStatus newStatus) { status = newStatus; }

    std::string toCSV() const {
        return std::to_string(claimId) + "|" + std::to_string(policyId) + "|" +
               std::to_string(vehicleId) + "|" + accidentDate + "|" + claimDate + "|" +
               claimStatusToString(status) + "|" + std::to_string(surveyorId) + "|" +
               std::to_string(workshopId) + "|" + damageDescription;
    }

    static Claim fromCSV(const std::string& line) {
        Claim c;
        std::stringstream ss(line);
        std::string token;
        std::getline(ss, token, '|'); c.claimId    = std::stoi(token);
        std::getline(ss, token, '|'); c.policyId   = std::stoi(token);
        std::getline(ss, token, '|'); c.vehicleId  = std::stoi(token);
        std::getline(ss, c.accidentDate,      '|');
        std::getline(ss, c.claimDate,         '|');
        std::getline(ss, token, '|'); c.status     = stringToClaimStatus(token);
        std::getline(ss, token, '|'); c.surveyorId = std::stoi(token);
        std::getline(ss, token, '|'); c.workshopId = std::stoi(token);
        std::getline(ss, c.damageDescription, '|');
        return c;
    }

    void display() const {
        std::cout << "  Claim#: " << claimId
                  << " | Policy: " << policyId
                  << " | Vehicle: " << vehicleId
                  << " | Accident: " << accidentDate
                  << " | Filed: " << claimDate
                  << " | Status: " << claimStatusToString(status)
                  << " | Surveyor: " << (surveyorId ? std::to_string(surveyorId) : "N/A")
                  << " | Workshop: " << (workshopId ? std::to_string(workshopId) : "N/A")
                  << "\n         Damage: " << damageDescription << "\n";
    }

    int getId() const { return claimId; }
};

#endif // CLAIM_H

/**
 * @file Claim.h
 * @brief Domain entity representing a damage claim filed by a customer.
 *
 * A claim tracks the full lifecycle of a vehicle damage report:
 * SUBMITTED → UNDER_REVIEW (surveyor assigned) → INSPECTED (report filed)
 * → APPROVED/REJECTED (manager decision) → WORKSHOP_ASSIGNED → CLOSED.
 * Each transition is enforced by the ClaimService business logic.
 */

#ifndef CLAIM_H
#define CLAIM_H

#include <string>
#include <iostream>
#include <sstream>
#include "Enums.h"

// Represents a damage claim with its complete workflow status.
// Claims are linked to a policy, vehicle, and optionally a surveyor and workshop.
struct Claim {
    int    claimId      = 0;       // Unique claim identifier
    int    policyId     = 0;       // Reference to the insurance policy
    int    vehicleId    = 0;       // Reference to the damaged vehicle
    std::string accidentDate;      // Date when the accident occurred (YYYY-MM-DD)
    std::string claimDate;         // Date when the claim was filed (YYYY-MM-DD)
    ClaimStatus status  = ClaimStatus::SUBMITTED;  // Current workflow status
    int    surveyorId   = 0;       // Assigned surveyor staff ID (0 = not assigned)
    int    workshopId   = 0;       // Assigned workshop ID (0 = not assigned)
    std::string damageDescription; // Description of the damage to the vehicle

    // Transition the claim to a new status in the workflow
    void updateStatus(ClaimStatus newStatus) { status = newStatus; }

    // Serialize claim data to pipe-delimited CSV string for file storage
    std::string toCSV() const {
        return std::to_string(claimId) + "|" + std::to_string(policyId) + "|" +
               std::to_string(vehicleId) + "|" + accidentDate + "|" + claimDate + "|" +
               claimStatusToString(status) + "|" + std::to_string(surveyorId) + "|" +
               std::to_string(workshopId) + "|" + damageDescription;
    }

    // Parse a CSV line and reconstruct a Claim object with all fields
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

    // Display claim details in formatted console output
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

    // Return unique identifier (required by FileRepository template)
    int getId() const { return claimId; }
};

#endif // CLAIM_H

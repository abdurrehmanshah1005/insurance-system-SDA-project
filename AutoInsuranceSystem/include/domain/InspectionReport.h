/**
 * @file InspectionReport.h
 * @brief Domain entity representing a surveyor's inspection report for a claim.
 *
 * After a surveyor inspects the damaged vehicle, they submit an inspection
 * report containing the estimated repair cost and their recommendation.
 * The manager uses this report to decide whether to approve or reject the claim.
 * Each claim can have at most one inspection report.
 */

#ifndef INSPECTION_REPORT_H
#define INSPECTION_REPORT_H

#include <string>
#include <iostream>
#include <sstream>

// Represents an inspection report submitted by a surveyor after vehicle inspection.
// Contains cost estimation and recommendation used for claim approval decisions.
struct InspectionReport {
    int    inspectionId = 0;       // Unique identifier for the inspection report
    int    claimId      = 0;       // Reference to the associated claim
    int    surveyorId   = 0;       // ID of the surveyor who performed the inspection
    std::string inspectionDate;    // Date of inspection (YYYY-MM-DD format)
    double estimatedCost = 0.0;    // Estimated cost of repairs in local currency
    std::string recommendation;    // Surveyor's recommendation (approve, repair, etc.)

    // Serialize inspection report to pipe-delimited CSV format
    std::string toCSV() const {
        return std::to_string(inspectionId) + "|" + std::to_string(claimId) + "|" +
               std::to_string(surveyorId) + "|" + inspectionDate + "|" +
               std::to_string(estimatedCost) + "|" + recommendation;
    }

    // Reconstruct an InspectionReport from a CSV line
    static InspectionReport fromCSV(const std::string& line) {
        InspectionReport r;
        std::stringstream ss(line);
        std::string token;
        std::getline(ss, token, '|'); r.inspectionId  = std::stoi(token);
        std::getline(ss, token, '|'); r.claimId       = std::stoi(token);
        std::getline(ss, token, '|'); r.surveyorId    = std::stoi(token);
        std::getline(ss, r.inspectionDate, '|');
        std::getline(ss, token, '|'); r.estimatedCost = std::stod(token);
        std::getline(ss, r.recommendation, '|');
        return r;
    }

    // Display inspection report details in formatted console output
    void display() const {
        std::cout << "  Inspection#: " << inspectionId
                  << " | Claim: " << claimId
                  << " | Surveyor: " << surveyorId
                  << " | Date: " << inspectionDate
                  << " | Est.Cost: " << estimatedCost
                  << "\n         Recommendation: " << recommendation << "\n";
    }

    // Return unique identifier (required by FileRepository template)
    int getId() const { return inspectionId; }
};

#endif // INSPECTION_REPORT_H

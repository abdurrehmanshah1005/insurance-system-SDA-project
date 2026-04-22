#ifndef INSPECTION_REPORT_H
#define INSPECTION_REPORT_H

#include <string>
#include <iostream>
#include <sstream>

struct InspectionReport {
    int    inspectionId = 0;
    int    claimId      = 0;
    int    surveyorId   = 0;
    std::string inspectionDate;
    double estimatedCost = 0.0;
    std::string recommendation;

    std::string toCSV() const {
        return std::to_string(inspectionId) + "|" + std::to_string(claimId) + "|" +
               std::to_string(surveyorId) + "|" + inspectionDate + "|" +
               std::to_string(estimatedCost) + "|" + recommendation;
    }

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

    void display() const {
        std::cout << "  Inspection#: " << inspectionId
                  << " | Claim: " << claimId
                  << " | Surveyor: " << surveyorId
                  << " | Date: " << inspectionDate
                  << " | Est.Cost: " << estimatedCost
                  << "\n         Recommendation: " << recommendation << "\n";
    }

    int getId() const { return inspectionId; }
};

#endif // INSPECTION_REPORT_H

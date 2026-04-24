/**
 * @file ReportService.h
 * @brief Business logic service for generating management reports.
 *
 * Provides report generation capabilities for the Manager role including:
 * - New customers acquired in a given month/year
 * - Pending claims that require attention
 * - Customer claim history for review before approval
 * - Complete inspection report logs
 *
 * This service is read-only and does not modify any data files.
 * It queries multiple repositories to aggregate report data.
 */

#ifndef REPORT_SERVICE_H
#define REPORT_SERVICE_H

#include "../domain/Customer.h"
#include "../domain/Claim.h"
#include "../domain/InspectionReport.h"
#include "../domain/InsurancePolicy.h"
#include "../repositories/FileRepository.h"
#include <vector>

// ─────────────────────────────────────────────
// ReportService  (Business Layer - Report Generation)
//   Aggregates data from multiple repositories to produce
//   management reports as required by the Manager role.
// ─────────────────────────────────────────────
class ReportService {
private:
    FileRepository<Customer>         customerRepo;    // Access to customer records
    FileRepository<Claim>            claimRepo;       // Access to claim records
    FileRepository<InspectionReport> inspectionRepo;  // Access to inspection reports
    FileRepository<InsurancePolicy>  policyRepo;      // Access to policy records

public:
    // Constructor: initializes all repositories with the data directory path
    ReportService(const std::string& dataDir);

    // Retrieve customers registered during a specific month and year
    std::vector<Customer>         getNewCustomersByMonth(int month, int year) const;

    // Retrieve all claims that are still in progress (not closed/rejected/assigned)
    std::vector<Claim>            getPendingClaims() const;

    // Retrieve complete claim history for a specific customer across all policies
    std::vector<Claim>            getCustomerClaimHistory(int customerId) const;

    // Retrieve all inspection reports submitted by surveyors
    std::vector<InspectionReport> getInspectionReports() const;
};

#endif

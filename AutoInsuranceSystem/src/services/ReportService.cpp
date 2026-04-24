/**
 * @file ReportService.cpp
 * @brief Implementation of the ReportService business logic.
 *
 * All report methods are read-only queries that load data from CSV files,
 * filter/aggregate results, and return them to the presentation layer.
 * No data modification occurs in this service.
 */

#include "../include/services/ReportService.h"
#include "../include/common/Utils.h"

// Initialize all file repositories with paths derived from the data directory
ReportService::ReportService(const std::string& dataDir)
    : customerRepo(dataDir + "/customers.csv"),
      claimRepo(dataDir + "/claims.csv"),
      inspectionRepo(dataDir + "/inspections.csv"),
      policyRepo(dataDir + "/policies.csv") {}

// ── Report: New Customers by Month ──
// Loads all customers and filters by the registration date's month and year.
// Uses Utils::parseDateMonthYear to extract date components for comparison.
std::vector<Customer> ReportService::getNewCustomersByMonth(int month, int year) const {
    auto all = customerRepo.loadAllRecords();
    std::vector<Customer> result;
    for (const auto& c : all) {
        int m, y;
        // Parse the customer's creation date and compare month/year
        if (Utils::parseDateMonthYear(c.createdDate, m, y)) {
            if (m == month && y == year)
                result.push_back(c);
        }
    }
    return result;
}

// ── Report: Pending Claims ──
// Returns all claims that are still active in the workflow.
// Excludes CLOSED, REJECTED, and WORKSHOP_ASSIGNED claims since
// those have reached terminal or near-terminal states.
std::vector<Claim> ReportService::getPendingClaims() const {
    auto all = claimRepo.loadAllRecords();
    std::vector<Claim> result;
    for (const auto& c : all) {
        if (c.status != ClaimStatus::CLOSED &&
            c.status != ClaimStatus::REJECTED &&
            c.status != ClaimStatus::WORKSHOP_ASSIGNED)
            result.push_back(c);
    }
    return result;
}

// ── Report: Customer Claim History ──
// Finds all claims associated with a customer by first looking up their
// policies, then matching claims to those policy IDs. This cross-repository
// query enables the manager to review a customer's complete claim record.
std::vector<Claim> ReportService::getCustomerClaimHistory(int customerId) const {
    auto policies = policyRepo.loadAllRecords();
    auto claims   = claimRepo.loadAllRecords();

    // Step 1: Collect all policy IDs belonging to this customer
    std::vector<int> custPolicyIds;
    for (const auto& p : policies) {
        if (p.customerId == customerId)
            custPolicyIds.push_back(p.policyId);
    }

    // Step 2: Find all claims filed against the customer's policies
    std::vector<Claim> result;
    for (const auto& c : claims) {
        for (int pid : custPolicyIds) {
            if (c.policyId == pid) {
                result.push_back(c);
                break;  // Avoid duplicates if multiple policies match
            }
        }
    }
    return result;
}

// ── Report: All Inspection Reports ──
// Returns the complete list of inspection reports for review by the manager
std::vector<InspectionReport> ReportService::getInspectionReports() const {
    return inspectionRepo.loadAllRecords();
}

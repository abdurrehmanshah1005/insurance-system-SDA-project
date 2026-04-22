#include "../include/services/ReportService.h"
#include "../include/common/Utils.h"

ReportService::ReportService(const std::string& dataDir)
    : customerRepo(dataDir + "/customers.csv"),
      claimRepo(dataDir + "/claims.csv"),
      inspectionRepo(dataDir + "/inspections.csv"),
      policyRepo(dataDir + "/policies.csv") {}

std::vector<Customer> ReportService::getNewCustomersByMonth(int month, int year) const {
    auto all = customerRepo.loadAllRecords();
    std::vector<Customer> result;
    for (const auto& c : all) {
        int m, y;
        if (Utils::parseDateMonthYear(c.createdDate, m, y)) {
            if (m == month && y == year)
                result.push_back(c);
        }
    }
    return result;
}

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

std::vector<Claim> ReportService::getCustomerClaimHistory(int customerId) const {
    auto policies = policyRepo.loadAllRecords();
    auto claims   = claimRepo.loadAllRecords();

    std::vector<int> custPolicyIds;
    for (const auto& p : policies) {
        if (p.customerId == customerId)
            custPolicyIds.push_back(p.policyId);
    }

    std::vector<Claim> result;
    for (const auto& c : claims) {
        for (int pid : custPolicyIds) {
            if (c.policyId == pid) {
                result.push_back(c);
                break;
            }
        }
    }
    return result;
}

std::vector<InspectionReport> ReportService::getInspectionReports() const {
    return inspectionRepo.loadAllRecords();
}

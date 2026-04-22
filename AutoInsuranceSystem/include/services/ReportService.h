#ifndef REPORT_SERVICE_H
#define REPORT_SERVICE_H

#include "../domain/Customer.h"
#include "../domain/Claim.h"
#include "../domain/InspectionReport.h"
#include "../domain/InsurancePolicy.h"
#include "../repositories/FileRepository.h"
#include <vector>

class ReportService {
private:
    FileRepository<Customer>         customerRepo;
    FileRepository<Claim>            claimRepo;
    FileRepository<InspectionReport> inspectionRepo;
    FileRepository<InsurancePolicy>  policyRepo;

public:
    ReportService(const std::string& dataDir);

    std::vector<Customer>         getNewCustomersByMonth(int month, int year) const;
    std::vector<Claim>            getPendingClaims() const;
    std::vector<Claim>            getCustomerClaimHistory(int customerId) const;
    std::vector<InspectionReport> getInspectionReports() const;
};

#endif

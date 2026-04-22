#ifndef REPORT_SERVICE_H
#define REPORT_SERVICE_H
#include "../domain/Customer.h"
#include "../domain/Claim.h"
#include "../domain/InspectionReport.h"
#include "../domain/InsurancePolicy.h"
#include "../repositories/FileRepository.h"
#include <vector>

class ReportService {
    FileRepository<Customer> customerRepo;
    FileRepository<Claim> claimRepo;
    FileRepository<InspectionReport> inspectionRepo;
    FileRepository<InsurancePolicy> policyRepo;
public:
    ReportService(const std::string& d)
        : customerRepo(d+"/customers.csv"),claimRepo(d+"/claims.csv"),
          inspectionRepo(d+"/inspections.csv"),policyRepo(d+"/policies.csv"){}

    std::vector<Customer> getNewCustomersByMonth(int month,int year) const {
        auto a=customerRepo.loadAllRecords(); std::vector<Customer> r;
        for(auto&c:a){ if(c.createdDate.size()>=7){
            try{ int y=std::stoi(c.createdDate.substr(0,4)); int m=std::stoi(c.createdDate.substr(5,2));
                if(m==month&&y==year) r.push_back(c); }catch(...){}
        }} return r;
    }
    std::vector<Claim> getPendingClaims() const {
        auto a=claimRepo.loadAllRecords(); std::vector<Claim> r;
        for(auto&c:a) if(c.status!=ClaimStatus::CLOSED&&c.status!=ClaimStatus::REJECTED&&c.status!=ClaimStatus::WORKSHOP_ASSIGNED) r.push_back(c);
        return r;
    }
    std::vector<Claim> getCustomerClaimHistory(int cid) const {
        auto pols=policyRepo.loadAllRecords(); auto cls=claimRepo.loadAllRecords();
        std::vector<int> pids; for(auto&p:pols) if(p.customerId==cid) pids.push_back(p.policyId);
        std::vector<Claim> r; for(auto&c:cls) for(int pid:pids) if(c.policyId==pid){r.push_back(c);break;}
        return r;
    }
    std::vector<InspectionReport> getInspectionReports() const { return inspectionRepo.loadAllRecords(); }
};
#endif

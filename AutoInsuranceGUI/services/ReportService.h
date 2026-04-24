/**
 * @file ReportService.h
 * @brief Business logic for management report generation (GUI version).
 *
 * Provides read-only report generation for the Manager role:
 * - New customers by month/year for tracking business growth
 * - Pending claims requiring attention for workload management
 * - Customer claim history for pre-approval review
 * - Inspection report logs for audit and tracking purposes
 *
 * This service queries multiple repositories but does not modify any data.
 */

#ifndef REPORT_SERVICE_H
#define REPORT_SERVICE_H
#include "../domain/Customer.h"
#include "../domain/Claim.h"
#include "../domain/InspectionReport.h"
#include "../domain/InsurancePolicy.h"
#include "../repositories/FileRepository.h"
#include <vector>

// Aggregates data from multiple repositories to produce management reports
class ReportService {
    FileRepository<Customer> customerRepo;          // Customer record access
    FileRepository<Claim> claimRepo;                // Claim record access
    FileRepository<InspectionReport> inspectionRepo;// Inspection report access
    FileRepository<InsurancePolicy> policyRepo;     // Policy record access
public:
    // Constructor: initialize repositories from the data directory
    ReportService(const std::string& d)
        : customerRepo(d+"/customers.csv"),claimRepo(d+"/claims.csv"),
          inspectionRepo(d+"/inspections.csv"),policyRepo(d+"/policies.csv"){}

    // Report: Get customers registered in a specific month and year
    // Parses the createdDate field to extract month/year for filtering
    std::vector<Customer> getNewCustomersByMonth(int month,int year) const {
        auto a=customerRepo.loadAllRecords(); std::vector<Customer> r;
        for(auto&c:a){ if(c.createdDate.size()>=7){
            try{ int y=std::stoi(c.createdDate.substr(0,4)); int m=std::stoi(c.createdDate.substr(5,2));
                if(m==month&&y==year) r.push_back(c); }catch(...){}
        }} return r;
    }

    // Report: Get all pending claims (excludes closed, rejected, workshop-assigned)
    std::vector<Claim> getPendingClaims() const {
        auto a=claimRepo.loadAllRecords(); std::vector<Claim> r;
        for(auto&c:a) if(c.status!=ClaimStatus::CLOSED&&c.status!=ClaimStatus::REJECTED&&c.status!=ClaimStatus::WORKSHOP_ASSIGNED) r.push_back(c);
        return r;
    }

    // Report: Get complete claim history for a customer across all their policies
    // Cross-references policies and claims tables using customerId → policyId chain
    std::vector<Claim> getCustomerClaimHistory(int cid) const {
        auto pols=policyRepo.loadAllRecords(); auto cls=claimRepo.loadAllRecords();
        // Step 1: Collect all policy IDs for this customer
        std::vector<int> pids; for(auto&p:pols) if(p.customerId==cid) pids.push_back(p.policyId);
        // Step 2: Find claims matching those policy IDs
        std::vector<Claim> r; for(auto&c:cls) for(int pid:pids) if(c.policyId==pid){r.push_back(c);break;}
        return r;
    }

    // Report: Get all inspection reports submitted by surveyors
    std::vector<InspectionReport> getInspectionReports() const { return inspectionRepo.loadAllRecords(); }
};
#endif

/**
 * @file ClaimService.h
 * @brief Business logic for claim lifecycle management (GUI version).
 *
 * This header-only service handles the complete claim workflow:
 * createClaim → assignSurveyor → addInspectionReport → approve/reject → assignWorkshop
 *
 * Each operation validates preconditions and enforces business rules:
 * - Claims can only be filed against ACTIVE policies within coverage dates
 * - Only staff with SURVEYOR role can be assigned to claims
 * - Inspection reports require UNDER_REVIEW status and assigned surveyor match
 * - Approval/rejection requires an existing inspection report
 * - Only REGISTERED workshops can be assigned to APPROVED claims
 */

#ifndef CLAIM_SERVICE_H
#define CLAIM_SERVICE_H
#include "../domain/Claim.h"
#include "../domain/InspectionReport.h"
#include "../domain/InsurancePolicy.h"
#include "../domain/Workshop.h"
#include "../domain/Staff.h"
#include "../repositories/FileRepository.h"
#include <vector>
#include <optional>

// Manages the complete damage claim lifecycle with validation at each step
class ClaimService {
    FileRepository<Claim> claimRepo;               // Claim data persistence
    FileRepository<InsurancePolicy> policyRepo;     // Policy lookup for validation
    FileRepository<InspectionReport> inspectionRepo;// Inspection report storage
    FileRepository<Workshop> workshopRepo;          // Workshop validation
    FileRepository<Staff> staffRepo;                // Staff role verification
public:
    // Constructor: initialize all repositories from the data directory path
    ClaimService(const std::string& d)
        : claimRepo(d+"/claims.csv"),policyRepo(d+"/policies.csv"),inspectionRepo(d+"/inspections.csv"),
          workshopRepo(d+"/workshops.csv"),staffRepo(d+"/staff.csv"){}

    // Create a new claim — validates policy is ACTIVE, date in coverage, vehicle matches
    Claim createClaim(int polId,int vehId,const std::string& accDate,const std::string& desc){
        // Load policies and find the matching one
        auto pols=policyRepo.loadAllRecords(); InsurancePolicy* pol=nullptr;
        for(auto&p:pols) if(p.policyId==polId){pol=&p;break;}
        if(!pol) throw std::runtime_error("Policy not found.");
        // Validate policy state and coverage period
        if(pol->status!=PolicyStatus::ACTIVE) throw std::runtime_error("Policy not ACTIVE.");
        if(accDate<pol->startDate||accDate>pol->endDate) throw std::runtime_error("Accident date outside policy period.");
        if(pol->vehicleId!=vehId) throw std::runtime_error("Vehicle doesn't match policy.");
        // Build and persist the new claim record
        Claim c; c.claimId=claimRepo.getNextId(); c.policyId=polId; c.vehicleId=vehId; c.accidentDate=accDate;
        c.claimDate=[]{std::time_t t=std::time(nullptr);std::tm tm;localtime_s(&tm,&t);char b[11];std::strftime(b,11,"%Y-%m-%d",&tm);return std::string(b);}();
        c.status=ClaimStatus::SUBMITTED; c.surveyorId=0; c.workshopId=0; c.damageDescription=desc;
        claimRepo.appendRecord(c); return c;
    }

    // Assign a surveyor to a submitted claim — validates role and status
    bool assignSurveyor(int claimId,int survId){
        // Verify the staff member has the SURVEYOR role
        auto staff=staffRepo.loadAllRecords(); bool ok=false;
        for(auto&s:staff) if(s.staffId==survId&&s.role==StaffRole::SURVEYOR){ok=true;break;}
        if(!ok) throw std::runtime_error("Not a valid surveyor.");
        // Find the claim and transition to UNDER_REVIEW
        auto cls=claimRepo.loadAllRecords();
        for(auto&c:cls) if(c.claimId==claimId){
            if(c.status!=ClaimStatus::SUBMITTED) throw std::runtime_error("Claim must be SUBMITTED.");
            c.surveyorId=survId; c.updateStatus(ClaimStatus::UNDER_REVIEW); claimRepo.saveAllRecords(cls); return true;
        }
        throw std::runtime_error("Claim not found.");
    }

    // Submit inspection report — only assigned surveyor can submit for UNDER_REVIEW claims
    InspectionReport addInspectionReport(int claimId,int survId,const std::string& dt,double cost,const std::string& rec){
        // Validate claim exists and surveyor assignment matches
        auto cls=claimRepo.loadAllRecords(); Claim* clm=nullptr;
        for(auto&c:cls) if(c.claimId==claimId){clm=&c;break;}
        if(!clm) throw std::runtime_error("Claim not found.");
        if(clm->surveyorId!=survId) throw std::runtime_error("Not assigned surveyor.");
        if(clm->status!=ClaimStatus::UNDER_REVIEW) throw std::runtime_error("Claim must be UNDER_REVIEW.");
        // Prevent duplicate reports for the same claim
        if(getInspectionByClaim(claimId)) throw std::runtime_error("Report already exists.");
        // Create and save the inspection report, then update claim status
        InspectionReport r; r.inspectionId=inspectionRepo.getNextId(); r.claimId=claimId; r.surveyorId=survId;
        r.inspectionDate=dt; r.estimatedCost=cost; r.recommendation=rec;
        inspectionRepo.appendRecord(r); clm->updateStatus(ClaimStatus::INSPECTED); claimRepo.saveAllRecords(cls); return r;
    }

    // Approve claim — requires inspection report and INSPECTED status
    bool approveClaim(int claimId){
        if(!getInspectionByClaim(claimId)) throw std::runtime_error("No inspection report.");
        auto cls=claimRepo.loadAllRecords();
        for(auto&c:cls) if(c.claimId==claimId){
            if(c.status!=ClaimStatus::INSPECTED) throw std::runtime_error("Claim must be INSPECTED.");
            c.updateStatus(ClaimStatus::APPROVED); claimRepo.saveAllRecords(cls); return true;
        }
        throw std::runtime_error("Claim not found.");
    }

    // Reject claim — requires inspection report and INSPECTED status
    bool rejectClaim(int claimId){
        if(!getInspectionByClaim(claimId)) throw std::runtime_error("No inspection report.");
        auto cls=claimRepo.loadAllRecords();
        for(auto&c:cls) if(c.claimId==claimId){
            if(c.status!=ClaimStatus::INSPECTED) throw std::runtime_error("Claim must be INSPECTED.");
            c.updateStatus(ClaimStatus::REJECTED); claimRepo.saveAllRecords(cls); return true;
        }
        throw std::runtime_error("Claim not found.");
    }

    // Assign workshop — only REGISTERED workshops can be assigned to APPROVED claims
    bool assignWorkshop(int claimId,int wsId){
        // Validate workshop exists and has REGISTERED status
        auto ws=workshopRepo.loadAllRecords(); bool ok=false;
        for(auto&w:ws) if(w.workshopId==wsId&&w.isActive()){ok=true;break;}
        if(!ok) throw std::runtime_error("Workshop not REGISTERED.");
        // Find the approved claim and assign the workshop
        auto cls=claimRepo.loadAllRecords();
        for(auto&c:cls) if(c.claimId==claimId){
            if(c.status!=ClaimStatus::APPROVED) throw std::runtime_error("Claim must be APPROVED.");
            c.workshopId=wsId; c.updateStatus(ClaimStatus::WORKSHOP_ASSIGNED); claimRepo.saveAllRecords(cls); return true;
        }
        throw std::runtime_error("Claim not found.");
    }

    // ── Query: Get all pending claims (not in terminal states) ──
    std::vector<Claim> getPendingClaims() const {
        auto a=claimRepo.loadAllRecords(); std::vector<Claim> r;
        for(auto&c:a) if(c.status!=ClaimStatus::CLOSED&&c.status!=ClaimStatus::REJECTED&&c.status!=ClaimStatus::WORKSHOP_ASSIGNED) r.push_back(c);
        return r;
    }

    // ── Query: Get claims assigned to a specific surveyor ──
    std::vector<Claim> getClaimsBySurveyor(int sid) const {
        auto a=claimRepo.loadAllRecords(); std::vector<Claim> r; for(auto&c:a) if(c.surveyorId==sid) r.push_back(c); return r;
    }

    // ── Query: Look up a single claim by ID ──
    std::optional<Claim> getClaim(int id) const {
        auto a=claimRepo.loadAllRecords(); for(auto&c:a) if(c.claimId==id) return c; return std::nullopt;
    }

    // ── Query: Get inspection report for a claim ──
    std::optional<InspectionReport> getInspectionByClaim(int cid) const {
        auto a=inspectionRepo.loadAllRecords(); for(auto&r:a) if(r.claimId==cid) return r; return std::nullopt;
    }

    // ── Query: Get workshops with REGISTERED status ──
    std::vector<Workshop> getActiveWorkshops() const {
        auto a=workshopRepo.loadAllRecords(); std::vector<Workshop> r; for(auto&w:a) if(w.isActive()) r.push_back(w); return r;
    }

    // ── Query: Get all claims regardless of status ──
    std::vector<Claim> getAllClaims() const { return claimRepo.loadAllRecords(); }

    // ── Query: Get all inspection reports ──
    std::vector<InspectionReport> getAllInspections() const { return inspectionRepo.loadAllRecords(); }

    // ── Query: Filter claims by a specific status ──
    std::vector<Claim> getClaimsByStatus(ClaimStatus st) const {
        auto a=claimRepo.loadAllRecords(); std::vector<Claim> r;
        for(auto&c:a){ if(c.status==st) r.push_back(c); } return r;
    }

    // ── Query: Get all workshops (active and suspended) ──
    std::vector<Workshop> getAllWorkshops() const { return workshopRepo.loadAllRecords(); }

    // ── Query: Get all staff members with SURVEYOR role ──
    std::vector<Staff> getSurveyors() const {
        auto a=staffRepo.loadAllRecords(); std::vector<Staff> r;
        for(auto&s:a){ if(s.role==StaffRole::SURVEYOR) r.push_back(s); } return r;
    }
};
#endif

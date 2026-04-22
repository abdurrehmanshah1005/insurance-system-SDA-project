#include "../include/services/ClaimService.h"
#include "../include/common/Utils.h"
#include <stdexcept>
#include <algorithm>

ClaimService::ClaimService(const std::string& dataDir)
    : claimRepo(dataDir + "/claims.csv"),
      policyRepo(dataDir + "/policies.csv"),
      inspectionRepo(dataDir + "/inspections.csv"),
      workshopRepo(dataDir + "/workshops.csv"),
      staffRepo(dataDir + "/staff.csv") {}

// ── Create a new claim ──
Claim ClaimService::createClaim(int policyId, int vehicleId,
                                 const std::string& accidentDate,
                                 const std::string& damageDesc) {
    // Load and validate policy
    auto policies = policyRepo.loadAllRecords();
    InsurancePolicy* pol = nullptr;
    for (auto& p : policies) {
        if (p.policyId == policyId) { pol = &p; break; }
    }
    if (!pol)
        throw std::runtime_error("Policy ID " + std::to_string(policyId) + " not found.");

    // Check policy is ACTIVE
    if (pol->status != PolicyStatus::ACTIVE)
        throw std::runtime_error("Policy is not ACTIVE. Current status: " +
                                 policyStatusToString(pol->status));

    // Check accident date within policy period
    if (accidentDate < pol->startDate || accidentDate > pol->endDate)
        throw std::runtime_error("Accident date is outside policy coverage period (" +
                                 pol->startDate + " to " + pol->endDate + ").");

    // Check vehicle matches policy
    if (pol->vehicleId != vehicleId)
        throw std::runtime_error("Vehicle ID does not match the policy's vehicle.");

    Claim c;
    c.claimId           = claimRepo.getNextId();
    c.policyId          = policyId;
    c.vehicleId         = vehicleId;
    c.accidentDate      = accidentDate;
    c.claimDate         = Utils::getCurrentDate();
    c.status            = ClaimStatus::SUBMITTED;
    c.surveyorId        = 0;
    c.workshopId        = 0;
    c.damageDescription = damageDesc;
    claimRepo.appendRecord(c);
    return c;
}

// ── Assign surveyor to claim ──
bool ClaimService::assignSurveyor(int claimId, int surveyorId) {
    // Validate surveyor role
    auto staffList = staffRepo.loadAllRecords();
    bool isSurveyor = false;
    for (const auto& s : staffList) {
        if (s.staffId == surveyorId && s.role == StaffRole::SURVEYOR) {
            isSurveyor = true; break;
        }
    }
    if (!isSurveyor)
        throw std::runtime_error("Staff ID " + std::to_string(surveyorId) + " is not a surveyor.");

    auto claims = claimRepo.loadAllRecords();
    for (auto& c : claims) {
        if (c.claimId == claimId) {
            if (c.status != ClaimStatus::SUBMITTED)
                throw std::runtime_error("Claim must be in SUBMITTED status to assign surveyor.");
            c.surveyorId = surveyorId;
            c.updateStatus(ClaimStatus::UNDER_REVIEW);
            claimRepo.saveAllRecords(claims);
            return true;
        }
    }
    throw std::runtime_error("Claim ID " + std::to_string(claimId) + " not found.");
}

// ── Submit inspection report ──
InspectionReport ClaimService::addInspectionReport(int claimId, int surveyorId,
                                                     const std::string& inspDate,
                                                     double estimatedCost,
                                                     const std::string& recommendation) {
    // Validate claim exists and surveyor is assigned
    auto claims = claimRepo.loadAllRecords();
    Claim* clm = nullptr;
    for (auto& c : claims) {
        if (c.claimId == claimId) { clm = &c; break; }
    }
    if (!clm)
        throw std::runtime_error("Claim ID " + std::to_string(claimId) + " not found.");

    if (clm->surveyorId != surveyorId)
        throw std::runtime_error("You are not the assigned surveyor for this claim.");

    if (clm->status != ClaimStatus::UNDER_REVIEW)
        throw std::runtime_error("Claim must be UNDER_REVIEW to submit inspection.");

    // Check no duplicate report
    auto existing = getInspectionByClaim(claimId);
    if (existing.has_value())
        throw std::runtime_error("Inspection report already exists for this claim.");

    InspectionReport r;
    r.inspectionId  = inspectionRepo.getNextId();
    r.claimId       = claimId;
    r.surveyorId    = surveyorId;
    r.inspectionDate = inspDate;
    r.estimatedCost = estimatedCost;
    r.recommendation = recommendation;
    inspectionRepo.appendRecord(r);

    // Update claim status to INSPECTED
    clm->updateStatus(ClaimStatus::INSPECTED);
    claimRepo.saveAllRecords(claims);

    return r;
}

// ── Approve claim ──
bool ClaimService::approveClaim(int claimId) {
    // Must have inspection report
    auto insp = getInspectionByClaim(claimId);
    if (!insp.has_value())
        throw std::runtime_error("Cannot approve: no inspection report for claim " +
                                 std::to_string(claimId) + ".");

    auto claims = claimRepo.loadAllRecords();
    for (auto& c : claims) {
        if (c.claimId == claimId) {
            if (c.status != ClaimStatus::INSPECTED)
                throw std::runtime_error("Claim must be INSPECTED before approval. Current: " +
                                         claimStatusToString(c.status));
            c.updateStatus(ClaimStatus::APPROVED);
            claimRepo.saveAllRecords(claims);
            return true;
        }
    }
    throw std::runtime_error("Claim ID " + std::to_string(claimId) + " not found.");
}

// ── Reject claim ──
bool ClaimService::rejectClaim(int claimId) {
    auto insp = getInspectionByClaim(claimId);
    if (!insp.has_value())
        throw std::runtime_error("Cannot reject: no inspection report for claim " +
                                 std::to_string(claimId) + ".");

    auto claims = claimRepo.loadAllRecords();
    for (auto& c : claims) {
        if (c.claimId == claimId) {
            if (c.status != ClaimStatus::INSPECTED)
                throw std::runtime_error("Claim must be INSPECTED before rejection. Current: " +
                                         claimStatusToString(c.status));
            c.updateStatus(ClaimStatus::REJECTED);
            claimRepo.saveAllRecords(claims);
            return true;
        }
    }
    throw std::runtime_error("Claim ID " + std::to_string(claimId) + " not found.");
}

// ── Assign workshop ──
bool ClaimService::assignWorkshop(int claimId, int workshopId) {
    // Validate workshop is REGISTERED
    auto workshops = workshopRepo.loadAllRecords();
    bool workshopOk = false;
    for (const auto& w : workshops) {
        if (w.workshopId == workshopId && w.isActive()) {
            workshopOk = true; break;
        }
    }
    if (!workshopOk)
        throw std::runtime_error("Workshop ID " + std::to_string(workshopId) +
                                 " not found or not REGISTERED.");

    auto claims = claimRepo.loadAllRecords();
    for (auto& c : claims) {
        if (c.claimId == claimId) {
            if (c.status != ClaimStatus::APPROVED)
                throw std::runtime_error("Workshop can only be assigned after claim is APPROVED. Current: " +
                                         claimStatusToString(c.status));
            c.workshopId = workshopId;
            c.updateStatus(ClaimStatus::WORKSHOP_ASSIGNED);
            claimRepo.saveAllRecords(claims);
            return true;
        }
    }
    throw std::runtime_error("Claim ID " + std::to_string(claimId) + " not found.");
}

// ── Queries ──
std::vector<Claim> ClaimService::getPendingClaims() const {
    auto all = claimRepo.loadAllRecords();
    std::vector<Claim> result;
    for (const auto& c : all) {
        if (c.status != ClaimStatus::CLOSED && c.status != ClaimStatus::REJECTED &&
            c.status != ClaimStatus::WORKSHOP_ASSIGNED)
            result.push_back(c);
    }
    return result;
}

std::vector<Claim> ClaimService::getClaimsBySurveyor(int surveyorId) const {
    auto all = claimRepo.loadAllRecords();
    std::vector<Claim> result;
    for (const auto& c : all) {
        if (c.surveyorId == surveyorId) result.push_back(c);
    }
    return result;
}

std::optional<Claim> ClaimService::getClaim(int claimId) const {
    auto all = claimRepo.loadAllRecords();
    for (const auto& c : all) {
        if (c.claimId == claimId) return c;
    }
    return std::nullopt;
}

std::optional<InspectionReport> ClaimService::getInspectionByClaim(int claimId) const {
    auto all = inspectionRepo.loadAllRecords();
    for (const auto& r : all) {
        if (r.claimId == claimId) return r;
    }
    return std::nullopt;
}

std::vector<Workshop> ClaimService::getActiveWorkshops() const {
    auto all = workshopRepo.loadAllRecords();
    std::vector<Workshop> result;
    for (const auto& w : all) {
        if (w.isActive()) result.push_back(w);
    }
    return result;
}

std::vector<Claim> ClaimService::getAllClaims() const {
    return claimRepo.loadAllRecords();
}

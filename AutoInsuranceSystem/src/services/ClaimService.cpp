/**
 * @file ClaimService.cpp
 * @brief Implementation of the ClaimService business logic.
 *
 * Each operation enforces strict workflow rules and validates all
 * preconditions before modifying data. State transitions follow the
 * defined claim lifecycle to ensure data integrity.
 */

#include "../include/services/ClaimService.h"
#include "../include/common/Utils.h"
#include <stdexcept>
#include <algorithm>

// Initialize all file repositories using the provided data directory path
ClaimService::ClaimService(const std::string& dataDir)
    : claimRepo(dataDir + "/claims.csv"),
      policyRepo(dataDir + "/policies.csv"),
      inspectionRepo(dataDir + "/inspections.csv"),
      workshopRepo(dataDir + "/workshops.csv"),
      staffRepo(dataDir + "/staff.csv") {}

// ── Create a new claim ──
// Validates: policy exists, policy is ACTIVE, accident date within coverage,
// and vehicle matches the policy. Sets initial status to SUBMITTED.
Claim ClaimService::createClaim(int policyId, int vehicleId,
                                 const std::string& accidentDate,
                                 const std::string& damageDesc) {
    // Load and validate policy exists
    auto policies = policyRepo.loadAllRecords();
    InsurancePolicy* pol = nullptr;
    for (auto& p : policies) {
        if (p.policyId == policyId) { pol = &p; break; }
    }
    if (!pol)
        throw std::runtime_error("Policy ID " + std::to_string(policyId) + " not found.");

    // Check policy is ACTIVE — expired/cancelled policies cannot have new claims
    if (pol->status != PolicyStatus::ACTIVE)
        throw std::runtime_error("Policy is not ACTIVE. Current status: " +
                                 policyStatusToString(pol->status));

    // Check accident date falls within the policy coverage period
    if (accidentDate < pol->startDate || accidentDate > pol->endDate)
        throw std::runtime_error("Accident date is outside policy coverage period (" +
                                 pol->startDate + " to " + pol->endDate + ").");

    // Check vehicle matches the policy's insured vehicle
    if (pol->vehicleId != vehicleId)
        throw std::runtime_error("Vehicle ID does not match the policy's vehicle.");

    // Create and persist the new claim record
    Claim c;
    c.claimId           = claimRepo.getNextId();
    c.policyId          = policyId;
    c.vehicleId         = vehicleId;
    c.accidentDate      = accidentDate;
    c.claimDate         = Utils::getCurrentDate();
    c.status            = ClaimStatus::SUBMITTED;
    c.surveyorId        = 0;   // Not yet assigned
    c.workshopId        = 0;   // Not yet assigned
    c.damageDescription = damageDesc;
    claimRepo.appendRecord(c);
    return c;
}

// ── Assign surveyor to claim ──
// Validates the staff member has SURVEYOR role and claim is in SUBMITTED status.
// Transitions claim to UNDER_REVIEW status after assignment.
bool ClaimService::assignSurveyor(int claimId, int surveyorId) {
    // Validate the staff member is actually a surveyor
    auto staffList = staffRepo.loadAllRecords();
    bool isSurveyor = false;
    for (const auto& s : staffList) {
        if (s.staffId == surveyorId && s.role == StaffRole::SURVEYOR) {
            isSurveyor = true; break;
        }
    }
    if (!isSurveyor)
        throw std::runtime_error("Staff ID " + std::to_string(surveyorId) + " is not a surveyor.");

    // Find the claim and update its status
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
// Only the assigned surveyor can submit a report for a claim under review.
// Creates an InspectionReport record and transitions claim to INSPECTED.
InspectionReport ClaimService::addInspectionReport(int claimId, int surveyorId,
                                                     const std::string& inspDate,
                                                     double estimatedCost,
                                                     const std::string& recommendation) {
    // Validate claim exists and is assigned to this surveyor
    auto claims = claimRepo.loadAllRecords();
    Claim* clm = nullptr;
    for (auto& c : claims) {
        if (c.claimId == claimId) { clm = &c; break; }
    }
    if (!clm)
        throw std::runtime_error("Claim ID " + std::to_string(claimId) + " not found.");

    // Verify the surveyor is the one assigned to this claim
    if (clm->surveyorId != surveyorId)
        throw std::runtime_error("You are not the assigned surveyor for this claim.");

    // Claim must be under review to accept an inspection report
    if (clm->status != ClaimStatus::UNDER_REVIEW)
        throw std::runtime_error("Claim must be UNDER_REVIEW to submit inspection.");

    // Prevent duplicate inspection reports for the same claim
    auto existing = getInspectionByClaim(claimId);
    if (existing.has_value())
        throw std::runtime_error("Inspection report already exists for this claim.");

    // Create and persist the inspection report
    InspectionReport r;
    r.inspectionId  = inspectionRepo.getNextId();
    r.claimId       = claimId;
    r.surveyorId    = surveyorId;
    r.inspectionDate = inspDate;
    r.estimatedCost = estimatedCost;
    r.recommendation = recommendation;
    inspectionRepo.appendRecord(r);

    // Update claim status to INSPECTED — ready for manager review
    clm->updateStatus(ClaimStatus::INSPECTED);
    claimRepo.saveAllRecords(claims);

    return r;
}

// ── Approve claim ──
// Manager approves a claim after reviewing the inspection report.
// Requires the claim to have an inspection report and be in INSPECTED status.
bool ClaimService::approveClaim(int claimId) {
    // Must have an inspection report before approval
    auto insp = getInspectionByClaim(claimId);
    if (!insp.has_value())
        throw std::runtime_error("Cannot approve: no inspection report for claim " +
                                 std::to_string(claimId) + ".");

    // Find claim and transition to APPROVED status
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
// Manager rejects a claim after reviewing the inspection report.
// Similar workflow to approval but transitions to REJECTED terminal state.
bool ClaimService::rejectClaim(int claimId) {
    // Must have an inspection report before rejection decision
    auto insp = getInspectionByClaim(claimId);
    if (!insp.has_value())
        throw std::runtime_error("Cannot reject: no inspection report for claim " +
                                 std::to_string(claimId) + ".");

    // Find claim and transition to REJECTED status
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
// After a claim is approved, the manager assigns a registered workshop
// for vehicle repair. Only active (REGISTERED) workshops can be assigned.
bool ClaimService::assignWorkshop(int claimId, int workshopId) {
    // Validate the workshop exists and is currently REGISTERED
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

    // Find the approved claim and assign the workshop
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

// ── Query: Pending Claims ──
// Returns claims still requiring action (excludes terminal states)
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

// ── Query: Claims by Surveyor ──
// Returns all claims assigned to a specific surveyor for their review
std::vector<Claim> ClaimService::getClaimsBySurveyor(int surveyorId) const {
    auto all = claimRepo.loadAllRecords();
    std::vector<Claim> result;
    for (const auto& c : all) {
        if (c.surveyorId == surveyorId) result.push_back(c);
    }
    return result;
}

// ── Query: Single Claim Lookup ──
// Returns a specific claim by ID, or nullopt if not found
std::optional<Claim> ClaimService::getClaim(int claimId) const {
    auto all = claimRepo.loadAllRecords();
    for (const auto& c : all) {
        if (c.claimId == claimId) return c;
    }
    return std::nullopt;
}

// ── Query: Inspection Report for Claim ──
// Returns the inspection report associated with a claim, or nullopt if none exists
std::optional<InspectionReport> ClaimService::getInspectionByClaim(int claimId) const {
    auto all = inspectionRepo.loadAllRecords();
    for (const auto& r : all) {
        if (r.claimId == claimId) return r;
    }
    return std::nullopt;
}

// ── Query: Active Workshops ──
// Returns only workshops with REGISTERED status (eligible for assignment)
std::vector<Workshop> ClaimService::getActiveWorkshops() const {
    auto all = workshopRepo.loadAllRecords();
    std::vector<Workshop> result;
    for (const auto& w : all) {
        if (w.isActive()) result.push_back(w);
    }
    return result;
}

// ── Query: All Claims ──
// Returns the complete list of all claims regardless of status
std::vector<Claim> ClaimService::getAllClaims() const {
    return claimRepo.loadAllRecords();
}

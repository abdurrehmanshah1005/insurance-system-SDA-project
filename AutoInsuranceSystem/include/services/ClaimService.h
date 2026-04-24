/**
 * @file ClaimService.h
 * @brief Business logic service managing the complete claim lifecycle.
 *
 * Handles all claim-related operations following the workflow:
 * createClaim → assignSurveyor → addInspectionReport → approve/reject → assignWorkshop
 *
 * Each state transition is validated to ensure proper workflow order.
 * The service enforces business rules such as:
 * - Only ACTIVE policies can have claims filed against them
 * - Accident date must fall within the policy coverage period
 * - Only assigned surveyors can submit inspection reports
 * - Claims must be INSPECTED before approval/rejection
 * - Only REGISTERED workshops can be assigned to approved claims
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

// ─────────────────────────────────────────────
// ClaimService  (Business Layer - Claim Processing)
//   Manages the complete lifecycle of damage claims from
//   submission through approval to workshop assignment.
// ─────────────────────────────────────────────
class ClaimService {
private:
    FileRepository<Claim>            claimRepo;       // Claim records persistence
    FileRepository<InsurancePolicy>  policyRepo;      // Policy validation lookups
    FileRepository<InspectionReport> inspectionRepo;  // Inspection report storage
    FileRepository<Workshop>         workshopRepo;    // Workshop validation lookups
    FileRepository<Staff>            staffRepo;       // Staff role validation

public:
    // Constructor: initializes repositories from the data directory
    ClaimService(const std::string& dataDir);

    // Create a new claim against an active policy (Salesman operation)
    Claim createClaim(int policyId, int vehicleId,
                      const std::string& accidentDate,
                      const std::string& damageDesc);

    // Assign a surveyor to inspect a submitted claim (Manager operation)
    bool assignSurveyor(int claimId, int surveyorId);

    // Submit inspection findings for an assigned claim (Surveyor operation)
    InspectionReport addInspectionReport(int claimId, int surveyorId,
                                          const std::string& inspDate,
                                          double estimatedCost,
                                          const std::string& recommendation);

    // Approve a claim after reviewing the inspection report (Manager operation)
    bool approveClaim(int claimId);

    // Reject a claim after reviewing the inspection report (Manager operation)
    bool rejectClaim(int claimId);

    // Assign a registered workshop to repair the vehicle (Manager operation)
    bool assignWorkshop(int claimId, int workshopId);

    // ── Query Methods ──
    // Retrieve all pending claims (not closed, rejected, or workshop-assigned)
    std::vector<Claim> getPendingClaims() const;

    // Retrieve all claims assigned to a specific surveyor
    std::vector<Claim> getClaimsBySurveyor(int surveyorId) const;

    // Look up a single claim by its ID
    std::optional<Claim> getClaim(int claimId) const;

    // Look up the inspection report for a specific claim
    std::optional<InspectionReport> getInspectionByClaim(int claimId) const;

    // Retrieve all workshops with REGISTERED status
    std::vector<Workshop> getActiveWorkshops() const;

    // Retrieve the complete list of all claims
    std::vector<Claim> getAllClaims() const;
};

#endif // CLAIM_SERVICE_H

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
// ClaimService  (matches class diagram)
//   +createClaim()           → Claim
//   +assignSurveyor()        → bool
//   +addInspectionReport()   → InspectionReport
//   +approveClaim()          → bool
//   +rejectClaim()           → bool
//   +assignWorkshop()        → bool
// ─────────────────────────────────────────────
class ClaimService {
private:
    FileRepository<Claim>            claimRepo;
    FileRepository<InsurancePolicy>  policyRepo;
    FileRepository<InspectionReport> inspectionRepo;
    FileRepository<Workshop>         workshopRepo;
    FileRepository<Staff>            staffRepo;

public:
    ClaimService(const std::string& dataDir);

    Claim createClaim(int policyId, int vehicleId,
                      const std::string& accidentDate,
                      const std::string& damageDesc);

    bool assignSurveyor(int claimId, int surveyorId);

    InspectionReport addInspectionReport(int claimId, int surveyorId,
                                          const std::string& inspDate,
                                          double estimatedCost,
                                          const std::string& recommendation);

    bool approveClaim(int claimId);
    bool rejectClaim(int claimId);
    bool assignWorkshop(int claimId, int workshopId);

    // Queries
    std::vector<Claim> getPendingClaims() const;
    std::vector<Claim> getClaimsBySurveyor(int surveyorId) const;
    std::optional<Claim> getClaim(int claimId) const;
    std::optional<InspectionReport> getInspectionByClaim(int claimId) const;
    std::vector<Workshop> getActiveWorkshops() const;
    std::vector<Claim> getAllClaims() const;
};

#endif // CLAIM_SERVICE_H

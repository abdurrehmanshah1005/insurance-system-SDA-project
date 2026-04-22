#ifndef ENUMS_H
#define ENUMS_H

#include <string>
#include <stdexcept>

// ── Policy Status ──
enum class PolicyStatus { ACTIVE, EXPIRED, CANCELLED };

inline std::string policyStatusToString(PolicyStatus s) {
    switch (s) {
        case PolicyStatus::ACTIVE:    return "ACTIVE";
        case PolicyStatus::EXPIRED:   return "EXPIRED";
        case PolicyStatus::CANCELLED: return "CANCELLED";
    }
    return "UNKNOWN";
}

inline PolicyStatus stringToPolicyStatus(const std::string& s) {
    if (s == "ACTIVE")    return PolicyStatus::ACTIVE;
    if (s == "EXPIRED")   return PolicyStatus::EXPIRED;
    if (s == "CANCELLED") return PolicyStatus::CANCELLED;
    throw std::invalid_argument("Invalid PolicyStatus: " + s);
}

// ── Claim Status ──
enum class ClaimStatus {
    SUBMITTED, UNDER_REVIEW, INSPECTED,
    APPROVED, REJECTED, WORKSHOP_ASSIGNED, CLOSED
};

inline std::string claimStatusToString(ClaimStatus s) {
    switch (s) {
        case ClaimStatus::SUBMITTED:          return "SUBMITTED";
        case ClaimStatus::UNDER_REVIEW:       return "UNDER_REVIEW";
        case ClaimStatus::INSPECTED:          return "INSPECTED";
        case ClaimStatus::APPROVED:           return "APPROVED";
        case ClaimStatus::REJECTED:           return "REJECTED";
        case ClaimStatus::WORKSHOP_ASSIGNED:  return "WORKSHOP_ASSIGNED";
        case ClaimStatus::CLOSED:             return "CLOSED";
    }
    return "UNKNOWN";
}

inline ClaimStatus stringToClaimStatus(const std::string& s) {
    if (s == "SUBMITTED")          return ClaimStatus::SUBMITTED;
    if (s == "UNDER_REVIEW")       return ClaimStatus::UNDER_REVIEW;
    if (s == "INSPECTED")          return ClaimStatus::INSPECTED;
    if (s == "APPROVED")           return ClaimStatus::APPROVED;
    if (s == "REJECTED")           return ClaimStatus::REJECTED;
    if (s == "WORKSHOP_ASSIGNED")  return ClaimStatus::WORKSHOP_ASSIGNED;
    if (s == "CLOSED")             return ClaimStatus::CLOSED;
    throw std::invalid_argument("Invalid ClaimStatus: " + s);
}

// ── Workshop Status ──
enum class WorkshopStatus { REGISTERED, SUSPENDED };

inline std::string workshopStatusToString(WorkshopStatus s) {
    switch (s) {
        case WorkshopStatus::REGISTERED: return "REGISTERED";
        case WorkshopStatus::SUSPENDED:  return "SUSPENDED";
    }
    return "UNKNOWN";
}

inline WorkshopStatus stringToWorkshopStatus(const std::string& s) {
    if (s == "REGISTERED") return WorkshopStatus::REGISTERED;
    if (s == "SUSPENDED")  return WorkshopStatus::SUSPENDED;
    throw std::invalid_argument("Invalid WorkshopStatus: " + s);
}

// ── Staff Role ──
enum class StaffRole { SALESMAN, SURVEYOR, MANAGER };

inline std::string staffRoleToString(StaffRole r) {
    switch (r) {
        case StaffRole::SALESMAN: return "SALESMAN";
        case StaffRole::SURVEYOR: return "SURVEYOR";
        case StaffRole::MANAGER:  return "MANAGER";
    }
    return "UNKNOWN";
}

inline StaffRole stringToStaffRole(const std::string& s) {
    if (s == "SALESMAN") return StaffRole::SALESMAN;
    if (s == "SURVEYOR") return StaffRole::SURVEYOR;
    if (s == "MANAGER")  return StaffRole::MANAGER;
    throw std::invalid_argument("Invalid StaffRole: " + s);
}

#endif // ENUMS_H

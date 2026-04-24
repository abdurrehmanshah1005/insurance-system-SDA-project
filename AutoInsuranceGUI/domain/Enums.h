/**
 * @file Enums.h
 * @brief Enumeration definitions for the Automobile Insurance Management System.
 *
 * This header defines all status enumerations used across the system including
 * PolicyStatus, ClaimStatus, WorkshopStatus, and StaffRole. Each enum has
 * corresponding conversion functions for string serialization and deserialization,
 * enabling CSV-based file persistence.
 */

#ifndef ENUMS_H
#define ENUMS_H

#include <string>
#include <stdexcept>

// ── Policy Status ──
// Represents the lifecycle states of an insurance policy.
// ACTIVE: policy is currently valid and can accept claims.
// EXPIRED: policy period has ended.
// CANCELLED: policy was terminated before expiry.
enum class PolicyStatus { ACTIVE, EXPIRED, CANCELLED };

// Convert PolicyStatus enum to its string representation for CSV storage
inline std::string policyStatusToString(PolicyStatus s) {
    switch (s) {
        case PolicyStatus::ACTIVE:    return "ACTIVE";
        case PolicyStatus::EXPIRED:   return "EXPIRED";
        case PolicyStatus::CANCELLED: return "CANCELLED";
    }
    return "UNKNOWN";
}

// Parse a string back into PolicyStatus enum (used when reading from CSV)
inline PolicyStatus stringToPolicyStatus(const std::string& s) {
    if (s == "ACTIVE")    return PolicyStatus::ACTIVE;
    if (s == "EXPIRED")   return PolicyStatus::EXPIRED;
    if (s == "CANCELLED") return PolicyStatus::CANCELLED;
    throw std::invalid_argument("Invalid PolicyStatus: " + s);
}

// ── Claim Status ──
// Represents the workflow stages of a damage claim.
// SUBMITTED → UNDER_REVIEW → INSPECTED → APPROVED/REJECTED → WORKSHOP_ASSIGNED → CLOSED
enum class ClaimStatus {
    SUBMITTED, UNDER_REVIEW, INSPECTED,
    APPROVED, REJECTED, WORKSHOP_ASSIGNED, CLOSED
};

// Convert ClaimStatus enum to string for serialization
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

// Parse string back into ClaimStatus enum (used when loading from CSV files)
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
// Tracks whether a repair workshop is currently authorized to receive vehicles.
// Only REGISTERED workshops can be assigned to approved claims.
enum class WorkshopStatus { REGISTERED, SUSPENDED };

// Convert WorkshopStatus to string for file persistence
inline std::string workshopStatusToString(WorkshopStatus s) {
    switch (s) {
        case WorkshopStatus::REGISTERED: return "REGISTERED";
        case WorkshopStatus::SUSPENDED:  return "SUSPENDED";
    }
    return "UNKNOWN";
}

// Parse string back into WorkshopStatus
inline WorkshopStatus stringToWorkshopStatus(const std::string& s) {
    if (s == "REGISTERED") return WorkshopStatus::REGISTERED;
    if (s == "SUSPENDED")  return WorkshopStatus::SUSPENDED;
    throw std::invalid_argument("Invalid WorkshopStatus: " + s);
}

// ── Staff Role ──
// Defines the three user roles in the system, each with different permissions:
// SALESMAN: registers customers, adds vehicles, creates policies, files claims.
// SURVEYOR: inspects damaged vehicles and submits inspection reports.
// MANAGER: reviews claims, approves/rejects, assigns workshops, views reports.
enum class StaffRole { SALESMAN, SURVEYOR, MANAGER };

// Convert StaffRole to string representation
inline std::string staffRoleToString(StaffRole r) {
    switch (r) {
        case StaffRole::SALESMAN: return "SALESMAN";
        case StaffRole::SURVEYOR: return "SURVEYOR";
        case StaffRole::MANAGER:  return "MANAGER";
    }
    return "UNKNOWN";
}

// Parse string back into StaffRole (used for CSV deserialization)
inline StaffRole stringToStaffRole(const std::string& s) {
    if (s == "SALESMAN") return StaffRole::SALESMAN;
    if (s == "SURVEYOR") return StaffRole::SURVEYOR;
    if (s == "MANAGER")  return StaffRole::MANAGER;
    throw std::invalid_argument("Invalid StaffRole: " + s);
}

#endif // ENUMS_H

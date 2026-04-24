/**
 * @file CustomerService.cpp
 * @brief Implementation of the CustomerService business logic.
 *
 * Implements customer registration, vehicle management, and policy
 * creation with complete input validation and business rule enforcement.
 */

#include "../include/services/CustomerService.h"
#include "../include/common/Utils.h"
#include <stdexcept>
#include <algorithm>

// Initialize repositories with paths derived from the data directory
CustomerService::CustomerService(const std::string& dataDir)
    : customerRepo(dataDir + "/customers.csv"),
      vehicleRepo(dataDir + "/vehicles.csv"),
      policyRepo(dataDir + "/policies.csv") {}

// ── Register a new customer ──
// Creates a customer record with an auto-generated unique ID
// and timestamps the registration with today's date.
Customer CustomerService::registerCustomer(const std::string& name,
                                            const std::string& contact,
                                            const std::string& address) {
    Customer c;
    c.customerId  = customerRepo.getNextId();
    c.name        = name;
    c.contact     = contact;
    c.address     = address;
    c.createdDate = Utils::getCurrentDate();  // Record when customer joined
    customerRepo.appendRecord(c);
    return c;
}

// ── Add a vehicle to a customer ──
// Validates that the customer exists and the registration number is unique
// across all vehicles in the system to prevent duplicate entries.
Vehicle CustomerService::addVehicle(int customerId,
                                     const std::string& regNo,
                                     const std::string& make,
                                     const std::string& model,
                                     int year) {
    // Validate customer exists before adding their vehicle
    auto cust = getCustomer(customerId);
    if (!cust.has_value())
        throw std::runtime_error("Customer ID " + std::to_string(customerId) + " not found.");

    // Validate unique registration number across all vehicles
    auto vehicles = vehicleRepo.loadAllRecords();
    for (const auto& v : vehicles) {
        if (v.registrationNo == regNo)
            throw std::runtime_error("Vehicle with registration '" + regNo + "' already exists.");
    }

    // Create and persist the new vehicle record
    Vehicle v;
    v.vehicleId      = vehicleRepo.getNextId();
    v.customerId     = customerId;
    v.registrationNo = regNo;
    v.make           = make;
    v.model          = model;
    v.year           = year;
    vehicleRepo.appendRecord(v);
    return v;
}

// ── Issue an insurance policy ──
// Creates a policy linking a customer to their vehicle with coverage dates.
// Validates customer exists, vehicle belongs to customer, and dates are valid.
InsurancePolicy CustomerService::createPolicy(int customerId,
                                               int vehicleId,
                                               const std::string& startDate,
                                               const std::string& endDate,
                                               double premium) {
    // Validate customer exists
    auto cust = getCustomer(customerId);
    if (!cust.has_value())
        throw std::runtime_error("Customer ID " + std::to_string(customerId) + " not found.");

    // Validate vehicle exists and belongs to the specified customer
    auto vehicles = vehicleRepo.loadAllRecords();
    bool found = false;
    for (const auto& v : vehicles) {
        if (v.vehicleId == vehicleId && v.customerId == customerId) {
            found = true;
            break;
        }
    }
    if (!found)
        throw std::runtime_error("Vehicle ID " + std::to_string(vehicleId) +
                                 " not found or does not belong to customer " +
                                 std::to_string(customerId) + ".");

    // Validate that start date comes before end date
    if (startDate >= endDate)
        throw std::runtime_error("Start date must be before end date.");

    // Create and persist the new policy with ACTIVE status
    InsurancePolicy p;
    p.policyId   = policyRepo.getNextId();
    p.customerId = customerId;
    p.vehicleId  = vehicleId;
    p.startDate  = startDate;
    p.endDate    = endDate;
    p.premium    = premium;
    p.status     = PolicyStatus::ACTIVE;
    policyRepo.appendRecord(p);
    return p;
}

// ── Query: Get customer by ID ──
// Returns the customer if found, or nullopt otherwise
std::optional<Customer> CustomerService::getCustomer(int id) const {
    auto all = customerRepo.loadAllRecords();
    for (const auto& c : all) {
        if (c.customerId == id) return c;
    }
    return std::nullopt;
}

// ── Query: Get all customers ──
// Returns the complete list of registered customers
std::vector<Customer> CustomerService::getAllCustomers() const {
    return customerRepo.loadAllRecords();
}

// ── Query: Get vehicles by customer ──
// Returns all vehicles owned by a specific customer
std::vector<Vehicle> CustomerService::getVehiclesByCustomer(int customerId) const {
    auto all = vehicleRepo.loadAllRecords();
    std::vector<Vehicle> result;
    for (const auto& v : all) {
        if (v.customerId == customerId) result.push_back(v);
    }
    return result;
}

// ── Query: Get policies by customer ──
// Returns all insurance policies held by a specific customer
std::vector<InsurancePolicy> CustomerService::getPoliciesByCustomer(int customerId) const {
    auto all = policyRepo.loadAllRecords();
    std::vector<InsurancePolicy> result;
    for (const auto& p : all) {
        if (p.customerId == customerId) result.push_back(p);
    }
    return result;
}

// ── Query: Get policy by ID ──
// Returns a specific policy if found, or nullopt otherwise
std::optional<InsurancePolicy> CustomerService::getPolicy(int policyId) const {
    auto all = policyRepo.loadAllRecords();
    for (const auto& p : all) {
        if (p.policyId == policyId) return p;
    }
    return std::nullopt;
}

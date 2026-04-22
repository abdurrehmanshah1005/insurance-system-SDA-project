#include "../include/services/CustomerService.h"
#include "../include/common/Utils.h"
#include <stdexcept>
#include <algorithm>

CustomerService::CustomerService(const std::string& dataDir)
    : customerRepo(dataDir + "/customers.csv"),
      vehicleRepo(dataDir + "/vehicles.csv"),
      policyRepo(dataDir + "/policies.csv") {}

Customer CustomerService::registerCustomer(const std::string& name,
                                            const std::string& contact,
                                            const std::string& address) {
    Customer c;
    c.customerId  = customerRepo.getNextId();
    c.name        = name;
    c.contact     = contact;
    c.address     = address;
    c.createdDate = Utils::getCurrentDate();
    customerRepo.appendRecord(c);
    return c;
}

Vehicle CustomerService::addVehicle(int customerId,
                                     const std::string& regNo,
                                     const std::string& make,
                                     const std::string& model,
                                     int year) {
    // Validate customer exists
    auto cust = getCustomer(customerId);
    if (!cust.has_value())
        throw std::runtime_error("Customer ID " + std::to_string(customerId) + " not found.");

    // Validate unique registration number
    auto vehicles = vehicleRepo.loadAllRecords();
    for (const auto& v : vehicles) {
        if (v.registrationNo == regNo)
            throw std::runtime_error("Vehicle with registration '" + regNo + "' already exists.");
    }

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

InsurancePolicy CustomerService::createPolicy(int customerId,
                                               int vehicleId,
                                               const std::string& startDate,
                                               const std::string& endDate,
                                               double premium) {
    // Validate customer exists
    auto cust = getCustomer(customerId);
    if (!cust.has_value())
        throw std::runtime_error("Customer ID " + std::to_string(customerId) + " not found.");

    // Validate vehicle exists and belongs to customer
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

    // Validate dates
    if (startDate >= endDate)
        throw std::runtime_error("Start date must be before end date.");

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

std::optional<Customer> CustomerService::getCustomer(int id) const {
    auto all = customerRepo.loadAllRecords();
    for (const auto& c : all) {
        if (c.customerId == id) return c;
    }
    return std::nullopt;
}

std::vector<Customer> CustomerService::getAllCustomers() const {
    return customerRepo.loadAllRecords();
}

std::vector<Vehicle> CustomerService::getVehiclesByCustomer(int customerId) const {
    auto all = vehicleRepo.loadAllRecords();
    std::vector<Vehicle> result;
    for (const auto& v : all) {
        if (v.customerId == customerId) result.push_back(v);
    }
    return result;
}

std::vector<InsurancePolicy> CustomerService::getPoliciesByCustomer(int customerId) const {
    auto all = policyRepo.loadAllRecords();
    std::vector<InsurancePolicy> result;
    for (const auto& p : all) {
        if (p.customerId == customerId) result.push_back(p);
    }
    return result;
}

std::optional<InsurancePolicy> CustomerService::getPolicy(int policyId) const {
    auto all = policyRepo.loadAllRecords();
    for (const auto& p : all) {
        if (p.policyId == policyId) return p;
    }
    return std::nullopt;
}

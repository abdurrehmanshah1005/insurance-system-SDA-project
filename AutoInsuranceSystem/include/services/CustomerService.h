/**
 * @file CustomerService.h
 * @brief Business logic service for customer, vehicle, and policy management.
 *
 * Handles all Salesman operations including:
 * - Customer registration with unique ID generation
 * - Vehicle registration with duplicate plate number prevention
 * - Insurance policy issuance with ownership and date validation
 *
 * Also provides query methods for looking up customers, vehicles, and policies.
 */

#ifndef CUSTOMER_SERVICE_H
#define CUSTOMER_SERVICE_H

#include "../domain/Customer.h"
#include "../domain/Vehicle.h"
#include "../domain/InsurancePolicy.h"
#include "../repositories/FileRepository.h"
#include <vector>
#include <optional>

// ─────────────────────────────────────────────
// CustomerService  (Business Layer - Customer Management)
//   Manages customer onboarding, vehicle registration,
//   and insurance policy creation with full validation.
// ─────────────────────────────────────────────
class CustomerService {
private:
    FileRepository<Customer>        customerRepo;   // Customer records persistence
    FileRepository<Vehicle>         vehicleRepo;    // Vehicle records persistence
    FileRepository<InsurancePolicy> policyRepo;     // Policy records persistence

public:
    // Constructor: initializes all repositories from the data directory
    CustomerService(const std::string& dataDir);

    // Register a new customer with auto-generated ID and current date
    Customer         registerCustomer(const std::string& name,
                                      const std::string& contact,
                                      const std::string& address);

    // Add a vehicle to a customer (validates customer exists, unique reg number)
    Vehicle          addVehicle(int customerId,
                                const std::string& regNo,
                                const std::string& make,
                                const std::string& model,
                                int year);

    // Issue a new insurance policy (validates customer, vehicle ownership, dates)
    InsurancePolicy  createPolicy(int customerId,
                                   int vehicleId,
                                   const std::string& startDate,
                                   const std::string& endDate,
                                   double premium);

    // ── Query Methods ──
    // Look up a single customer by ID
    std::optional<Customer> getCustomer(int id) const;

    // Retrieve the complete list of all registered customers
    std::vector<Customer>   getAllCustomers() const;

    // Retrieve all vehicles owned by a specific customer
    std::vector<Vehicle>    getVehiclesByCustomer(int customerId) const;

    // Retrieve all insurance policies held by a specific customer
    std::vector<InsurancePolicy> getPoliciesByCustomer(int customerId) const;

    // Look up a single policy by ID
    std::optional<InsurancePolicy> getPolicy(int policyId) const;
};

#endif // CUSTOMER_SERVICE_H

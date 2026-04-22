#ifndef CUSTOMER_SERVICE_H
#define CUSTOMER_SERVICE_H

#include "../domain/Customer.h"
#include "../domain/Vehicle.h"
#include "../domain/InsurancePolicy.h"
#include "../repositories/FileRepository.h"
#include <vector>
#include <optional>

// ─────────────────────────────────────────────
// CustomerService  (matches class diagram)
//   +registerCustomer() → Customer
//   +addVehicle()       → Vehicle
//   +createPolicy()     → InsurancePolicy
// ─────────────────────────────────────────────
class CustomerService {
private:
    FileRepository<Customer>        customerRepo;
    FileRepository<Vehicle>         vehicleRepo;
    FileRepository<InsurancePolicy> policyRepo;

public:
    CustomerService(const std::string& dataDir);

    Customer         registerCustomer(const std::string& name,
                                      const std::string& contact,
                                      const std::string& address);

    Vehicle          addVehicle(int customerId,
                                const std::string& regNo,
                                const std::string& make,
                                const std::string& model,
                                int year);

    InsurancePolicy  createPolicy(int customerId,
                                   int vehicleId,
                                   const std::string& startDate,
                                   const std::string& endDate,
                                   double premium);

    // Queries
    std::optional<Customer> getCustomer(int id) const;
    std::vector<Customer>   getAllCustomers() const;
    std::vector<Vehicle>    getVehiclesByCustomer(int customerId) const;
    std::vector<InsurancePolicy> getPoliciesByCustomer(int customerId) const;
    std::optional<InsurancePolicy> getPolicy(int policyId) const;
};

#endif // CUSTOMER_SERVICE_H

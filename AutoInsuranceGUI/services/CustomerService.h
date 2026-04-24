/**
 * @file CustomerService.h
 * @brief Business logic for customer, vehicle, and policy management (GUI version).
 *
 * Header-only service handling Salesman operations:
 * - Customer registration with auto-generated IDs
 * - Vehicle registration with duplicate plate prevention
 * - Insurance policy creation with ownership and date validation
 * - Query methods for lookups by ID and customer
 */

#ifndef CUSTOMER_SERVICE_H
#define CUSTOMER_SERVICE_H
#include "../domain/Customer.h"
#include "../domain/Vehicle.h"
#include "../domain/InsurancePolicy.h"
#include "../repositories/FileRepository.h"
#include <vector>
#include <optional>

// Manages customer onboarding, vehicle registration, and policy issuance
class CustomerService {
    FileRepository<Customer> customerRepo;          // Customer data persistence
    FileRepository<Vehicle> vehicleRepo;            // Vehicle data persistence
    FileRepository<InsurancePolicy> policyRepo;     // Policy data persistence
public:
    // Constructor: initialize repositories from the data directory path
    CustomerService(const std::string& d)
        : customerRepo(d+"/customers.csv"), vehicleRepo(d+"/vehicles.csv"), policyRepo(d+"/policies.csv"){}

    // Register a new customer with auto-generated ID and today's date
    Customer registerCustomer(const std::string& name,const std::string& contact,const std::string& address){
        Customer c; c.customerId=customerRepo.getNextId(); c.name=name; c.contact=contact; c.address=address;
        // Generate current date as registration timestamp
        c.createdDate=[]{ std::time_t t=std::time(nullptr); std::tm tm; localtime_s(&tm,&t);
            char b[11]; std::strftime(b,11,"%Y-%m-%d",&tm); return std::string(b); }();
        customerRepo.appendRecord(c); return c;
    }

    // Add a vehicle — validates customer exists and registration is unique
    Vehicle addVehicle(int cid,const std::string& reg,const std::string& mk,const std::string& mdl,int yr){
        // Ensure the customer exists before adding their vehicle
        auto cust=getCustomer(cid); if(!cust) throw std::runtime_error("Customer not found.");
        // Check for duplicate registration numbers across all vehicles
        auto vehs=vehicleRepo.loadAllRecords(); for(auto&v:vehs) if(v.registrationNo==reg) throw std::runtime_error("Duplicate registration.");
        // Create and persist the vehicle record
        Vehicle v; v.vehicleId=vehicleRepo.getNextId(); v.customerId=cid; v.registrationNo=reg; v.make=mk; v.model=mdl; v.year=yr;
        vehicleRepo.appendRecord(v); return v;
    }

    // Create a policy — validates customer, vehicle ownership, and date range
    InsurancePolicy createPolicy(int cid,int vid,const std::string& sd,const std::string& ed,double prem){
        if(!getCustomer(cid)) throw std::runtime_error("Customer not found.");
        // Verify the vehicle belongs to this customer
        auto vehs=vehicleRepo.loadAllRecords(); bool ok=false;
        for(auto&v:vehs) if(v.vehicleId==vid&&v.customerId==cid){ok=true;break;}
        if(!ok) throw std::runtime_error("Vehicle not found or doesn't belong to customer.");
        // Ensure start date is before end date
        if(sd>=ed) throw std::runtime_error("Start date must be before end date.");
        // Create policy with ACTIVE status
        InsurancePolicy p; p.policyId=policyRepo.getNextId(); p.customerId=cid; p.vehicleId=vid;
        p.startDate=sd; p.endDate=ed; p.premium=prem; p.status=PolicyStatus::ACTIVE;
        policyRepo.appendRecord(p); return p;
    }

    // ── Query: Find customer by ID ──
    std::optional<Customer> getCustomer(int id) const {
        auto a=customerRepo.loadAllRecords(); for(auto&c:a) if(c.customerId==id) return c; return std::nullopt;
    }

    // ── Query: Get all registered customers ──
    std::vector<Customer> getAllCustomers() const { return customerRepo.loadAllRecords(); }

    // ── Query: Get vehicles belonging to a customer ──
    std::vector<Vehicle> getVehiclesByCustomer(int cid) const {
        auto a=vehicleRepo.loadAllRecords(); std::vector<Vehicle> r; for(auto&v:a) if(v.customerId==cid) r.push_back(v); return r;
    }

    // ── Query: Get policies held by a customer ──
    std::vector<InsurancePolicy> getPoliciesByCustomer(int cid) const {
        auto a=policyRepo.loadAllRecords(); std::vector<InsurancePolicy> r; for(auto&p:a) if(p.customerId==cid) r.push_back(p); return r;
    }

    // ── Query: Find policy by ID ──
    std::optional<InsurancePolicy> getPolicy(int pid) const {
        auto a=policyRepo.loadAllRecords(); for(auto&p:a) if(p.policyId==pid) return p; return std::nullopt;
    }
};
#endif

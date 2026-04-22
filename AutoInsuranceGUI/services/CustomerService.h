#ifndef CUSTOMER_SERVICE_H
#define CUSTOMER_SERVICE_H
#include "../domain/Customer.h"
#include "../domain/Vehicle.h"
#include "../domain/InsurancePolicy.h"
#include "../repositories/FileRepository.h"
#include <vector>
#include <optional>

class CustomerService {
    FileRepository<Customer> customerRepo;
    FileRepository<Vehicle> vehicleRepo;
    FileRepository<InsurancePolicy> policyRepo;
public:
    CustomerService(const std::string& d)
        : customerRepo(d+"/customers.csv"), vehicleRepo(d+"/vehicles.csv"), policyRepo(d+"/policies.csv"){}

    Customer registerCustomer(const std::string& name,const std::string& contact,const std::string& address){
        Customer c; c.customerId=customerRepo.getNextId(); c.name=name; c.contact=contact; c.address=address;
        c.createdDate=[]{ std::time_t t=std::time(nullptr); std::tm tm; localtime_s(&tm,&t);
            char b[11]; std::strftime(b,11,"%Y-%m-%d",&tm); return std::string(b); }();
        customerRepo.appendRecord(c); return c;
    }
    Vehicle addVehicle(int cid,const std::string& reg,const std::string& mk,const std::string& mdl,int yr){
        auto cust=getCustomer(cid); if(!cust) throw std::runtime_error("Customer not found.");
        auto vehs=vehicleRepo.loadAllRecords(); for(auto&v:vehs) if(v.registrationNo==reg) throw std::runtime_error("Duplicate registration.");
        Vehicle v; v.vehicleId=vehicleRepo.getNextId(); v.customerId=cid; v.registrationNo=reg; v.make=mk; v.model=mdl; v.year=yr;
        vehicleRepo.appendRecord(v); return v;
    }
    InsurancePolicy createPolicy(int cid,int vid,const std::string& sd,const std::string& ed,double prem){
        if(!getCustomer(cid)) throw std::runtime_error("Customer not found.");
        auto vehs=vehicleRepo.loadAllRecords(); bool ok=false;
        for(auto&v:vehs) if(v.vehicleId==vid&&v.customerId==cid){ok=true;break;}
        if(!ok) throw std::runtime_error("Vehicle not found or doesn't belong to customer.");
        if(sd>=ed) throw std::runtime_error("Start date must be before end date.");
        InsurancePolicy p; p.policyId=policyRepo.getNextId(); p.customerId=cid; p.vehicleId=vid;
        p.startDate=sd; p.endDate=ed; p.premium=prem; p.status=PolicyStatus::ACTIVE;
        policyRepo.appendRecord(p); return p;
    }
    std::optional<Customer> getCustomer(int id) const {
        auto a=customerRepo.loadAllRecords(); for(auto&c:a) if(c.customerId==id) return c; return std::nullopt;
    }
    std::vector<Customer> getAllCustomers() const { return customerRepo.loadAllRecords(); }
    std::vector<Vehicle> getVehiclesByCustomer(int cid) const {
        auto a=vehicleRepo.loadAllRecords(); std::vector<Vehicle> r; for(auto&v:a) if(v.customerId==cid) r.push_back(v); return r;
    }
    std::vector<InsurancePolicy> getPoliciesByCustomer(int cid) const {
        auto a=policyRepo.loadAllRecords(); std::vector<InsurancePolicy> r; for(auto&p:a) if(p.customerId==cid) r.push_back(p); return r;
    }
    std::optional<InsurancePolicy> getPolicy(int pid) const {
        auto a=policyRepo.loadAllRecords(); for(auto&p:a) if(p.policyId==pid) return p; return std::nullopt;
    }
};
#endif

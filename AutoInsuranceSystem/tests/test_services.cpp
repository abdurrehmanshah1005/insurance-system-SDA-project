#include <iostream>
#include <cassert>
#include <filesystem>
#include <fstream>
#include "../include/services/CustomerService.h"
#include "../include/services/ClaimService.h"
#include "../include/services/ReportService.h"
#include "../include/common/Utils.h"

namespace fs = std::filesystem;

static const std::string TEST_DATA = "test_data";
static int passed = 0, failed = 0;

// Helper: clean test data directory
void setupTestData() {
    fs::remove_all(TEST_DATA);
    fs::create_directories(TEST_DATA);

    // Create empty files
    std::ofstream(TEST_DATA + "/customers.csv").close();
    std::ofstream(TEST_DATA + "/vehicles.csv").close();
    std::ofstream(TEST_DATA + "/policies.csv").close();
    std::ofstream(TEST_DATA + "/claims.csv").close();
    std::ofstream(TEST_DATA + "/inspections.csv").close();
    std::ofstream(TEST_DATA + "/workshops.csv").close();

    // Staff file with test surveyor and manager
    std::ofstream sf(TEST_DATA + "/staff.csv");
    sf << "1|Test Sales|SALESMAN|pass\n";
    sf << "2|Test Surveyor|SURVEYOR|pass\n";
    sf << "3|Test Manager|MANAGER|pass\n";
    sf.close();

    // Workshops
    std::ofstream wf(TEST_DATA + "/workshops.csv");
    wf << "1|Test Workshop|Addr|Contact|REGISTERED\n";
    wf << "2|Suspended WS|Addr|Contact|SUSPENDED\n";
    wf.close();
}

#define RUN_TEST(name) \
    try { setupTestData(); name(); passed++; std::cout << "  [PASS] " #name "\n"; } \
    catch (const std::exception& e) { failed++; std::cout << "  [FAIL] " #name ": " << e.what() << "\n"; }

// ═══════════════════════════════════
// TEST CASES
// ═══════════════════════════════════

void test_register_customer() {
    CustomerService cs(TEST_DATA);
    auto c = cs.registerCustomer("John", "123", "Addr1");
    assert(c.customerId == 1);
    assert(c.name == "John");
}

void test_add_vehicle_unique_reg() {
    CustomerService cs(TEST_DATA);
    cs.registerCustomer("John", "123", "Addr");
    cs.addVehicle(1, "ABC-123", "Toyota", "Corolla", 2022);
    // Duplicate registration should fail
    try {
        cs.addVehicle(1, "ABC-123", "Honda", "Civic", 2023);
        assert(false && "Should have thrown");
    } catch (const std::runtime_error&) { /* expected */ }
}

void test_add_vehicle_invalid_customer() {
    CustomerService cs(TEST_DATA);
    try {
        cs.addVehicle(999, "XYZ", "Make", "Model", 2020);
        assert(false && "Should have thrown");
    } catch (const std::runtime_error&) { /* expected */ }
}

void test_create_policy_valid() {
    CustomerService cs(TEST_DATA);
    cs.registerCustomer("John", "123", "Addr");
    cs.addVehicle(1, "ABC-123", "Toyota", "Corolla", 2022);
    auto p = cs.createPolicy(1, 1, "2026-01-01", "2027-01-01", 50000);
    assert(p.policyId == 1);
    assert(p.status == PolicyStatus::ACTIVE);
}

void test_create_policy_wrong_vehicle_owner() {
    CustomerService cs(TEST_DATA);
    cs.registerCustomer("John", "123", "Addr");
    cs.registerCustomer("Jane", "456", "Addr2");
    cs.addVehicle(1, "ABC-123", "Toyota", "Corolla", 2022);
    try {
        cs.createPolicy(2, 1, "2026-01-01", "2027-01-01", 50000); // Jane doesn't own vehicle 1
        assert(false && "Should have thrown");
    } catch (const std::runtime_error&) { /* expected */ }
}

void test_file_claim_active_policy() {
    CustomerService cs(TEST_DATA);
    cs.registerCustomer("John", "123", "Addr");
    cs.addVehicle(1, "ABC-123", "Toyota", "Corolla", 2022);
    cs.createPolicy(1, 1, "2026-01-01", "2027-01-01", 50000);

    ClaimService cls(TEST_DATA);
    auto c = cls.createClaim(1, 1, "2026-06-15", "Front bumper damage");
    assert(c.claimId == 1);
    assert(c.status == ClaimStatus::SUBMITTED);
}

void test_file_claim_inactive_policy() {
    CustomerService cs(TEST_DATA);
    cs.registerCustomer("John", "123", "Addr");
    cs.addVehicle(1, "ABC-123", "Toyota", "Corolla", 2022);
    cs.createPolicy(1, 1, "2025-01-01", "2025-12-31", 50000); // expired period

    ClaimService cls(TEST_DATA);
    try {
        cls.createClaim(1, 1, "2026-06-15", "Damage"); // date outside coverage
        assert(false && "Should have thrown");
    } catch (const std::runtime_error&) { /* expected */ }
}

void test_approve_without_inspection() {
    CustomerService cs(TEST_DATA);
    cs.registerCustomer("John", "123", "Addr");
    cs.addVehicle(1, "ABC-123", "Toyota", "Corolla", 2022);
    cs.createPolicy(1, 1, "2026-01-01", "2027-01-01", 50000);

    ClaimService cls(TEST_DATA);
    cls.createClaim(1, 1, "2026-06-15", "Damage");
    try {
        cls.approveClaim(1); // No inspection yet
        assert(false && "Should have thrown");
    } catch (const std::runtime_error&) { /* expected */ }
}

void test_full_claim_lifecycle() {
    CustomerService cs(TEST_DATA);
    cs.registerCustomer("John", "123", "Addr");
    cs.addVehicle(1, "ABC-123", "Toyota", "Corolla", 2022);
    cs.createPolicy(1, 1, "2026-01-01", "2027-01-01", 50000);

    ClaimService cls(TEST_DATA);
    cls.createClaim(1, 1, "2026-06-15", "Damage");
    cls.assignSurveyor(1, 2);       // assign surveyor
    cls.addInspectionReport(1, 2, "2026-06-20", 25000.0, "Repair recommended");
    cls.approveClaim(1);             // approve after inspection
    cls.assignWorkshop(1, 1);        // assign registered workshop

    auto c = cls.getClaim(1);
    assert(c.has_value());
    assert(c->status == ClaimStatus::WORKSHOP_ASSIGNED);
    assert(c->workshopId == 1);
}

void test_assign_suspended_workshop() {
    CustomerService cs(TEST_DATA);
    cs.registerCustomer("John", "123", "Addr");
    cs.addVehicle(1, "ABC-123", "Toyota", "Corolla", 2022);
    cs.createPolicy(1, 1, "2026-01-01", "2027-01-01", 50000);

    ClaimService cls(TEST_DATA);
    cls.createClaim(1, 1, "2026-06-15", "Damage");
    cls.assignSurveyor(1, 2);
    cls.addInspectionReport(1, 2, "2026-06-20", 25000.0, "Repair");
    cls.approveClaim(1);
    try {
        cls.assignWorkshop(1, 2); // Workshop 2 is SUSPENDED
        assert(false && "Should have thrown");
    } catch (const std::runtime_error&) { /* expected */ }
}

void test_report_new_customers_by_month() {
    CustomerService cs(TEST_DATA);
    cs.registerCustomer("John", "123", "Addr");
    cs.registerCustomer("Jane", "456", "Addr2");

    ReportService rs(TEST_DATA);
    std::string today = Utils::getCurrentDate();
    int m, y;
    Utils::parseDateMonthYear(today, m, y);
    auto custs = rs.getNewCustomersByMonth(m, y);
    assert(custs.size() == 2);
}

// ═══════════════════════════════════
// MAIN
// ═══════════════════════════════════
int main() {
    std::cout << "\n=== Running Unit Tests ===\n\n";

    RUN_TEST(test_register_customer);
    RUN_TEST(test_add_vehicle_unique_reg);
    RUN_TEST(test_add_vehicle_invalid_customer);
    RUN_TEST(test_create_policy_valid);
    RUN_TEST(test_create_policy_wrong_vehicle_owner);
    RUN_TEST(test_file_claim_active_policy);
    RUN_TEST(test_file_claim_inactive_policy);
    RUN_TEST(test_approve_without_inspection);
    RUN_TEST(test_full_claim_lifecycle);
    RUN_TEST(test_assign_suspended_workshop);
    RUN_TEST(test_report_new_customers_by_month);

    std::cout << "\n--- Results: " << passed << " passed, " << failed << " failed ---\n";
    fs::remove_all(TEST_DATA); // cleanup

    return failed > 0 ? 1 : 0;
}

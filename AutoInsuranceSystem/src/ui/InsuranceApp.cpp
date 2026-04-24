/**
 * @file InsuranceApp.cpp
 * @brief Implementation of the console-based insurance management UI.
 *
 * Implements all menu screens and user interaction flows for the three
 * staff roles. Each operation collects user input, delegates to the
 * appropriate service, and displays the result or error message.
 */

#include "../include/ui/InsuranceApp.h"
#include "../include/common/Utils.h"
#include <iostream>
#include <iomanip>

// Initialize all services and ensure the data directory exists on disk
InsuranceApp::InsuranceApp(const std::string& dataDir)
    : dataDir(dataDir),
      customerService(dataDir),
      claimService(dataDir),
      reportService(dataDir),
      staffRepo(dataDir + "/staff.csv") {
    Utils::ensureDataDir(dataDir);
}

// Display the welcome banner and start the main menu loop
void InsuranceApp::run() {
    std::cout << "========================================\n";
    std::cout << " Automobile Insurance Management System\n";
    std::cout << "========================================\n\n";
    showMainMenu();
}

// ════════════════════════════════════════════
//  MAIN MENU — Role selection and login
// ════════════════════════════════════════════
void InsuranceApp::showMainMenu() {
    while (true) {
        std::cout << "\n--- Main Menu ---\n";
        std::cout << "1. Login as Salesman\n";
        std::cout << "2. Login as Surveyor\n";
        std::cout << "3. Login as Manager\n";
        std::cout << "0. Exit\n";
        int choice = Utils::readInt("Select: ");
        if (choice == 0) {
            std::cout << "Goodbye!\n";
            return;
        }
        // Map menu choice to staff role
        StaffRole selectedRole;
        switch (choice) {
            case 1: selectedRole = StaffRole::SALESMAN; break;
            case 2: selectedRole = StaffRole::SURVEYOR; break;
            case 3: selectedRole = StaffRole::MANAGER;  break;
            default: std::cout << "  Invalid choice.\n"; continue;
        }
        // Attempt login and route to role-specific menu
        try {
            Staff staff = login(selectedRole);
            std::cout << "\nWelcome, " << staff.name << " ("
                      << staffRoleToString(staff.role) << ")!\n";
            switch (staff.role) {
                case StaffRole::SALESMAN: salesmanMenu(staff); break;
                case StaffRole::SURVEYOR: surveyorMenu(staff); break;
                case StaffRole::MANAGER:  managerMenu(staff);  break;
            }
        } catch (const std::exception& e) {
            std::cout << "  [ERROR] " << e.what() << "\n";
        }
    }
}

// Authenticate staff member by ID, password, and expected role
Staff InsuranceApp::login(StaffRole expectedRole) {
    int id = Utils::readInt("Staff ID: ");
    std::string pwd = Utils::readString("Password: ");
    auto staffList = staffRepo.loadAllRecords();
    for (const auto& s : staffList) {
        if (s.staffId == id && s.authenticate(pwd)) {
            // Verify the staff member has the correct role
            if (s.role != expectedRole)
                throw std::runtime_error("This staff member is not a " + staffRoleToString(expectedRole) + ".");
            return s;
        }
    }
    throw std::runtime_error("Invalid staff ID or password.");
}

// ════════════════════════════════════════════
//  SALESMAN MENU — Customer and policy operations
// ════════════════════════════════════════════
void InsuranceApp::salesmanMenu(const Staff& staff) {
    while (true) {
        std::cout << "\n--- Salesman Menu ---\n";
        std::cout << "1. Register Customer\n";
        std::cout << "2. Add Vehicle\n";
        std::cout << "3. Issue Insurance Policy\n";
        std::cout << "4. File Damage Claim\n";
        std::cout << "0. Logout\n";
        int ch = Utils::readInt("Select: ");
        try {
            switch (ch) {
                case 1: registerCustomer(); break;
                case 2: addVehicle();        break;
                case 3: issuePolicy();       break;
                case 4: fileClaim();         break;
                case 0: return;
                default: std::cout << "  Invalid choice.\n";
            }
        } catch (const std::exception& e) {
            std::cout << "  [ERROR] " << e.what() << "\n";
        }
    }
}

// ════════════════════════════════════════════
//  SURVEYOR MENU — Inspection operations
// ════════════════════════════════════════════
void InsuranceApp::surveyorMenu(const Staff& staff) {
    while (true) {
        std::cout << "\n--- Surveyor Menu ---\n";
        std::cout << "1. View Assigned Claims\n";
        std::cout << "2. Submit Inspection Report\n";
        std::cout << "0. Logout\n";
        int ch = Utils::readInt("Select: ");
        try {
            switch (ch) {
                case 1: viewAssignedClaims(staff.staffId); break;
                case 2: submitInspection(staff.staffId);   break;
                case 0: return;
                default: std::cout << "  Invalid choice.\n";
            }
        } catch (const std::exception& e) {
            std::cout << "  [ERROR] " << e.what() << "\n";
        }
    }
}

// ════════════════════════════════════════════
//  MANAGER MENU — Claim management and reports
// ════════════════════════════════════════════
void InsuranceApp::managerMenu(const Staff& staff) {
    while (true) {
        std::cout << "\n--- Manager Menu ---\n";
        std::cout << "1. View Pending Claims\n";
        std::cout << "2. Assign Surveyor to Claim\n";
        std::cout << "3. Approve Claim\n";
        std::cout << "4. Reject Claim\n";
        std::cout << "5. Assign Workshop\n";
        std::cout << "6. View Customer Claim History\n";
        std::cout << "7. Reports\n";
        std::cout << "0. Logout\n";
        int ch = Utils::readInt("Select: ");
        try {
            switch (ch) {
                case 1: viewPendingClaims();       break;
                case 2: assignSurveyorToClaim();   break;
                case 3: approveClaim();            break;
                case 4: rejectClaim();             break;
                case 5: assignWorkshop();          break;
                case 6: viewCustomerClaimHistory(); break;
                case 7: reportsMenu();             break;
                case 0: return;
                default: std::cout << "  Invalid choice.\n";
            }
        } catch (const std::exception& e) {
            std::cout << "  [ERROR] " << e.what() << "\n";
        }
    }
}

// ════════════════════════════════════════════
//  REPORTS SUB-MENU — Report generation options
// ════════════════════════════════════════════
void InsuranceApp::reportsMenu() {
    while (true) {
        std::cout << "\n--- Reports ---\n";
        std::cout << "1. New Customers by Month\n";
        std::cout << "2. Pending Claims\n";
        std::cout << "3. Inspection Report Logs\n";
        std::cout << "0. Back\n";
        int ch = Utils::readInt("Select: ");
        try {
            switch (ch) {
                case 1: reportNewCustomers();  break;
                case 2: reportPendingClaims(); break;
                case 3: reportInspections();   break;
                case 0: return;
                default: std::cout << "  Invalid choice.\n";
            }
        } catch (const std::exception& e) {
            std::cout << "  [ERROR] " << e.what() << "\n";
        }
    }
}

// ════════════════════════════════════════════
//  SALESMAN OPERATIONS — UI interaction flows
// ════════════════════════════════════════════

// Collect customer details and delegate to CustomerService
void InsuranceApp::registerCustomer() {
    std::cout << "\n-- Register New Customer --\n";
    std::string name    = Utils::readString("Name: ");
    std::string contact = Utils::readString("Contact: ");
    std::string address = Utils::readString("Address: ");
    auto c = customerService.registerCustomer(name, contact, address);
    std::cout << "  [OK] Customer registered with ID: " << c.customerId << "\n";
    c.display();
}

// Collect vehicle details and register under a customer
void InsuranceApp::addVehicle() {
    std::cout << "\n-- Add Vehicle --\n";
    int custId      = Utils::readInt("Customer ID: ");
    std::string reg = Utils::readString("Registration No: ");
    std::string mk  = Utils::readString("Make: ");
    std::string mdl = Utils::readString("Model: ");
    int year        = Utils::readInt("Year: ");
    auto v = customerService.addVehicle(custId, reg, mk, mdl, year);
    std::cout << "  [OK] Vehicle added with ID: " << v.vehicleId << "\n";
    v.display();
}

// Issue a new policy after showing customer's vehicles for reference
void InsuranceApp::issuePolicy() {
    std::cout << "\n-- Issue Insurance Policy --\n";
    int custId  = Utils::readInt("Customer ID: ");

    // Show customer's vehicles so salesman can select the correct one
    auto vehicles = customerService.getVehiclesByCustomer(custId);
    if (vehicles.empty()) {
        std::cout << "  Customer has no vehicles. Please add one first.\n";
        return;
    }
    std::cout << "  Customer's vehicles:\n";
    for (const auto& v : vehicles) v.display();

    int vehId       = Utils::readInt("Vehicle ID: ");
    std::string sd  = Utils::readDate("Start Date (YYYY-MM-DD): ");
    std::string ed  = Utils::readDate("End Date (YYYY-MM-DD): ");
    double premium  = Utils::readDouble("Premium Amount: ");
    auto p = customerService.createPolicy(custId, vehId, sd, ed, premium);
    std::cout << "  [OK] Policy issued with ID: " << p.policyId << "\n";
    p.display();
}

// File a damage claim against an existing policy
void InsuranceApp::fileClaim() {
    std::cout << "\n-- File Damage Claim --\n";
    int polId       = Utils::readInt("Policy ID: ");
    auto pol = customerService.getPolicy(polId);
    if (!pol.has_value()) {
        std::cout << "  Policy not found.\n";
        return;
    }
    // Show policy details for confirmation
    std::cout << "  Policy details:\n";
    pol->display();

    std::string accDate = Utils::readDate("Accident Date (YYYY-MM-DD): ");
    std::string desc    = Utils::readString("Damage Description: ");
    auto c = claimService.createClaim(polId, pol->vehicleId, accDate, desc);
    std::cout << "  [OK] Claim filed with ID: " << c.claimId << "\n";
    c.display();
}

// ════════════════════════════════════════════
//  SURVEYOR OPERATIONS — Inspection workflow
// ════════════════════════════════════════════

// Display all claims currently assigned to this surveyor
void InsuranceApp::viewAssignedClaims(int surveyorId) {
    auto claims = claimService.getClaimsBySurveyor(surveyorId);
    if (claims.empty()) {
        std::cout << "  No claims assigned to you.\n";
        return;
    }
    std::cout << "\n  Your assigned claims:\n";
    Utils::printDivider();
    for (const auto& c : claims) c.display();
}

// Submit an inspection report for one of the surveyor's assigned claims
void InsuranceApp::submitInspection(int surveyorId) {
    std::cout << "\n-- Submit Inspection Report --\n";
    viewAssignedClaims(surveyorId);  // Show claims for reference

    int claimId        = Utils::readInt("Claim ID: ");
    std::string inDate = Utils::readDate("Inspection Date (YYYY-MM-DD): ");
    double cost        = Utils::readDouble("Estimated Repair Cost: ");
    std::string rec    = Utils::readString("Recommendation: ");
    auto r = claimService.addInspectionReport(claimId, surveyorId, inDate, cost, rec);
    std::cout << "  [OK] Inspection report submitted (ID: " << r.inspectionId << ")\n";
    r.display();
}

// ════════════════════════════════════════════
//  MANAGER OPERATIONS — Claim approval workflow
// ════════════════════════════════════════════

// Show all claims that need manager attention
void InsuranceApp::viewPendingClaims() {
    auto claims = claimService.getPendingClaims();
    if (claims.empty()) {
        std::cout << "  No pending claims.\n";
        return;
    }
    std::cout << "\n  Pending Claims:\n";
    Utils::printDivider();
    for (const auto& c : claims) c.display();
}

// Assign a surveyor staff member to inspect a submitted claim
void InsuranceApp::assignSurveyorToClaim() {
    std::cout << "\n-- Assign Surveyor to Claim --\n";
    viewPendingClaims();  // Show pending claims for reference
    int claimId    = Utils::readInt("Claim ID: ");
    int surveyorId = Utils::readInt("Surveyor Staff ID: ");
    claimService.assignSurveyor(claimId, surveyorId);
    std::cout << "  [OK] Surveyor " << surveyorId << " assigned to claim " << claimId << ".\n";
}

// Approve an inspected claim after reviewing the inspection report
void InsuranceApp::approveClaim() {
    std::cout << "\n-- Approve Claim --\n";
    int claimId = Utils::readInt("Claim ID: ");
    // Show inspection report if available for manager review
    auto insp = claimService.getInspectionByClaim(claimId);
    if (insp.has_value()) {
        std::cout << "  Inspection report:\n";
        insp->display();
    }
    claimService.approveClaim(claimId);
    std::cout << "  [OK] Claim " << claimId << " APPROVED.\n";
}

// Reject an inspected claim after reviewing the inspection report
void InsuranceApp::rejectClaim() {
    std::cout << "\n-- Reject Claim --\n";
    int claimId = Utils::readInt("Claim ID: ");
    // Show inspection report for manager reference before rejection
    auto insp = claimService.getInspectionByClaim(claimId);
    if (insp.has_value()) {
        std::cout << "  Inspection report:\n";
        insp->display();
    }
    claimService.rejectClaim(claimId);
    std::cout << "  [OK] Claim " << claimId << " REJECTED.\n";
}

// Assign a registered workshop to repair the vehicle for an approved claim
void InsuranceApp::assignWorkshop() {
    std::cout << "\n-- Assign Workshop --\n";
    // Display available workshops for selection
    auto workshops = claimService.getActiveWorkshops();
    if (workshops.empty()) {
        std::cout << "  No registered workshops available.\n";
        return;
    }
    std::cout << "  Registered Workshops:\n";
    for (const auto& w : workshops) w.display();

    int claimId    = Utils::readInt("Claim ID: ");
    int workshopId = Utils::readInt("Workshop ID: ");
    claimService.assignWorkshop(claimId, workshopId);
    std::cout << "  [OK] Workshop " << workshopId << " assigned to claim " << claimId << ".\n";
}

// View complete claim history for a specific customer
void InsuranceApp::viewCustomerClaimHistory() {
    int custId = Utils::readInt("Customer ID: ");
    auto claims = reportService.getCustomerClaimHistory(custId);
    if (claims.empty()) {
        std::cout << "  No claims found for customer " << custId << ".\n";
        return;
    }
    std::cout << "\n  Claim History for Customer " << custId << ":\n";
    Utils::printDivider();
    for (const auto& c : claims) c.display();
}

// ════════════════════════════════════════════
//  REPORT OPERATIONS — Management reports
// ════════════════════════════════════════════

// Generate report of customers registered in a specific month/year
void InsuranceApp::reportNewCustomers() {
    int month = Utils::readInt("Month (1-12): ");
    int year  = Utils::readInt("Year: ");
    auto custs = reportService.getNewCustomersByMonth(month, year);
    if (custs.empty()) {
        std::cout << "  No new customers in " << month << "/" << year << ".\n";
        return;
    }
    std::cout << "\n  New Customers in " << month << "/" << year << ":\n";
    Utils::printDivider();
    for (const auto& c : custs) c.display();
    std::cout << "  Total: " << custs.size() << "\n";
}

// Generate report of all pending claims requiring attention
void InsuranceApp::reportPendingClaims() {
    auto claims = reportService.getPendingClaims();
    if (claims.empty()) {
        std::cout << "  No pending claims.\n";
        return;
    }
    std::cout << "\n  Pending Claims Report:\n";
    Utils::printDivider();
    for (const auto& c : claims) c.display();
    std::cout << "  Total pending: " << claims.size() << "\n";
}

// Generate report of all inspection reports in the system
void InsuranceApp::reportInspections() {
    auto reports = reportService.getInspectionReports();
    if (reports.empty()) {
        std::cout << "  No inspection reports found.\n";
        return;
    }
    std::cout << "\n  Inspection Report Logs:\n";
    Utils::printDivider();
    for (const auto& r : reports) r.display();
    std::cout << "  Total: " << reports.size() << "\n";
}

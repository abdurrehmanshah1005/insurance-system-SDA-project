#include "../include/ui/InsuranceApp.h"
#include "../include/common/Utils.h"
#include <iostream>
#include <iomanip>

InsuranceApp::InsuranceApp(const std::string& dataDir)
    : dataDir(dataDir),
      customerService(dataDir),
      claimService(dataDir),
      reportService(dataDir),
      staffRepo(dataDir + "/staff.csv") {
    Utils::ensureDataDir(dataDir);
}

void InsuranceApp::run() {
    std::cout << "========================================\n";
    std::cout << " Automobile Insurance Management System\n";
    std::cout << "========================================\n\n";
    showMainMenu();
}

// ════════════════════════════════════════════
//  MAIN MENU
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
        StaffRole selectedRole;
        switch (choice) {
            case 1: selectedRole = StaffRole::SALESMAN; break;
            case 2: selectedRole = StaffRole::SURVEYOR; break;
            case 3: selectedRole = StaffRole::MANAGER;  break;
            default: std::cout << "  Invalid choice.\n"; continue;
        }
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

Staff InsuranceApp::login(StaffRole expectedRole) {
    int id = Utils::readInt("Staff ID: ");
    std::string pwd = Utils::readString("Password: ");
    auto staffList = staffRepo.loadAllRecords();
    for (const auto& s : staffList) {
        if (s.staffId == id && s.authenticate(pwd)) {
            if (s.role != expectedRole)
                throw std::runtime_error("This staff member is not a " + staffRoleToString(expectedRole) + ".");
            return s;
        }
    }
    throw std::runtime_error("Invalid staff ID or password.");
}

// ════════════════════════════════════════════
//  SALESMAN MENU
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
//  SURVEYOR MENU
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
//  MANAGER MENU
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
//  REPORTS SUB-MENU
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
//  SALESMAN OPERATIONS
// ════════════════════════════════════════════
void InsuranceApp::registerCustomer() {
    std::cout << "\n-- Register New Customer --\n";
    std::string name    = Utils::readString("Name: ");
    std::string contact = Utils::readString("Contact: ");
    std::string address = Utils::readString("Address: ");
    auto c = customerService.registerCustomer(name, contact, address);
    std::cout << "  [OK] Customer registered with ID: " << c.customerId << "\n";
    c.display();
}

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

void InsuranceApp::issuePolicy() {
    std::cout << "\n-- Issue Insurance Policy --\n";
    int custId  = Utils::readInt("Customer ID: ");

    // Show customer's vehicles
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

void InsuranceApp::fileClaim() {
    std::cout << "\n-- File Damage Claim --\n";
    int polId       = Utils::readInt("Policy ID: ");
    auto pol = customerService.getPolicy(polId);
    if (!pol.has_value()) {
        std::cout << "  Policy not found.\n";
        return;
    }
    std::cout << "  Policy details:\n";
    pol->display();

    std::string accDate = Utils::readDate("Accident Date (YYYY-MM-DD): ");
    std::string desc    = Utils::readString("Damage Description: ");
    auto c = claimService.createClaim(polId, pol->vehicleId, accDate, desc);
    std::cout << "  [OK] Claim filed with ID: " << c.claimId << "\n";
    c.display();
}

// ════════════════════════════════════════════
//  SURVEYOR OPERATIONS
// ════════════════════════════════════════════
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

void InsuranceApp::submitInspection(int surveyorId) {
    std::cout << "\n-- Submit Inspection Report --\n";
    viewAssignedClaims(surveyorId);

    int claimId        = Utils::readInt("Claim ID: ");
    std::string inDate = Utils::readDate("Inspection Date (YYYY-MM-DD): ");
    double cost        = Utils::readDouble("Estimated Repair Cost: ");
    std::string rec    = Utils::readString("Recommendation: ");
    auto r = claimService.addInspectionReport(claimId, surveyorId, inDate, cost, rec);
    std::cout << "  [OK] Inspection report submitted (ID: " << r.inspectionId << ")\n";
    r.display();
}

// ════════════════════════════════════════════
//  MANAGER OPERATIONS
// ════════════════════════════════════════════
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

void InsuranceApp::assignSurveyorToClaim() {
    std::cout << "\n-- Assign Surveyor to Claim --\n";
    viewPendingClaims();
    int claimId    = Utils::readInt("Claim ID: ");
    int surveyorId = Utils::readInt("Surveyor Staff ID: ");
    claimService.assignSurveyor(claimId, surveyorId);
    std::cout << "  [OK] Surveyor " << surveyorId << " assigned to claim " << claimId << ".\n";
}

void InsuranceApp::approveClaim() {
    std::cout << "\n-- Approve Claim --\n";
    int claimId = Utils::readInt("Claim ID: ");
    auto insp = claimService.getInspectionByClaim(claimId);
    if (insp.has_value()) {
        std::cout << "  Inspection report:\n";
        insp->display();
    }
    claimService.approveClaim(claimId);
    std::cout << "  [OK] Claim " << claimId << " APPROVED.\n";
}

void InsuranceApp::rejectClaim() {
    std::cout << "\n-- Reject Claim --\n";
    int claimId = Utils::readInt("Claim ID: ");
    auto insp = claimService.getInspectionByClaim(claimId);
    if (insp.has_value()) {
        std::cout << "  Inspection report:\n";
        insp->display();
    }
    claimService.rejectClaim(claimId);
    std::cout << "  [OK] Claim " << claimId << " REJECTED.\n";
}

void InsuranceApp::assignWorkshop() {
    std::cout << "\n-- Assign Workshop --\n";
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
//  REPORT OPERATIONS
// ════════════════════════════════════════════
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

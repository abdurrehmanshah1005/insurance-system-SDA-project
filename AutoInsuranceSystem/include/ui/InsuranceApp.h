/**
 * @file InsuranceApp.h
 * @brief Presentation layer for the console-based insurance management system.
 *
 * This class implements the text-based user interface with role-based menus
 * for Salesman, Surveyor, and Manager. It coordinates between user input
 * and the business layer services (CustomerService, ClaimService, ReportService).
 *
 * The UI follows a hierarchical menu structure:
 *   Main Menu → Role Login → Role-specific Menu → Operations/Reports
 */

#ifndef INSURANCE_APP_H
#define INSURANCE_APP_H

#include "../services/CustomerService.h"
#include "../services/ClaimService.h"
#include "../services/ReportService.h"
#include "../domain/Staff.h"
#include "../repositories/FileRepository.h"
#include <string>

// ─────────────────────────────────────────────
// InsuranceApp  (Presentation Layer)
//   Provides the console-based user interface and
//   delegates all business logic to the service classes.
// ─────────────────────────────────────────────
class InsuranceApp {
private:
    std::string dataDir;                    // Path to the CSV data directory
    CustomerService customerService;        // Handles customer/vehicle/policy ops
    ClaimService    claimService;            // Handles claim lifecycle operations
    ReportService   reportService;           // Handles report generation
    FileRepository<Staff> staffRepo;         // Staff authentication lookups

    // Menu navigation methods
    void showMainMenu();
    Staff login(StaffRole expectedRole);

    // Role-specific menu screens
    void salesmanMenu(const Staff& staff);
    void surveyorMenu(const Staff& staff);
    void managerMenu(const Staff& staff);
    void reportsMenu();

    // Salesman operations (UI wrappers around CustomerService/ClaimService)
    void registerCustomer();
    void addVehicle();
    void issuePolicy();
    void fileClaim();

    // Surveyor operations (UI wrappers around ClaimService)
    void viewAssignedClaims(int surveyorId);
    void submitInspection(int surveyorId);

    // Manager operations (UI wrappers around ClaimService)
    void viewPendingClaims();
    void approveClaim();
    void rejectClaim();
    void assignWorkshop();
    void assignSurveyorToClaim();
    void viewCustomerClaimHistory();

    // Report operations (UI wrappers around ReportService)
    void reportNewCustomers();
    void reportPendingClaims();
    void reportInspections();

public:
    // Constructor: initializes services and ensures data directory exists
    explicit InsuranceApp(const std::string& dataDir);

    // Entry point: starts the main menu loop
    void run();
};

#endif

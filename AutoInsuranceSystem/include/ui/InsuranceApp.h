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
//   +run() → void
// ─────────────────────────────────────────────
class InsuranceApp {
private:
    std::string dataDir;
    CustomerService customerService;
    ClaimService    claimService;
    ReportService   reportService;
    FileRepository<Staff> staffRepo;

    void showMainMenu();
    Staff login(StaffRole expectedRole);

    // Role menus
    void salesmanMenu(const Staff& staff);
    void surveyorMenu(const Staff& staff);
    void managerMenu(const Staff& staff);
    void reportsMenu();

    // Salesman operations
    void registerCustomer();
    void addVehicle();
    void issuePolicy();
    void fileClaim();

    // Surveyor operations
    void viewAssignedClaims(int surveyorId);
    void submitInspection(int surveyorId);

    // Manager operations
    void viewPendingClaims();
    void approveClaim();
    void rejectClaim();
    void assignWorkshop();
    void assignSurveyorToClaim();
    void viewCustomerClaimHistory();

    // Reports
    void reportNewCustomers();
    void reportPendingClaims();
    void reportInspections();

public:
    explicit InsuranceApp(const std::string& dataDir);
    void run();
};

#endif

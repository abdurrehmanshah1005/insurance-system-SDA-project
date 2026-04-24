/**
 * @file App.h
 * @brief Main application class for the SFML-based GUI Insurance Management System.
 *
 * This header-only class implements the complete graphical user interface using SFML.
 * It manages three screens (Role Selection, Login, Dashboard) and provides
 * role-based workflows for Salesman, Surveyor, and Manager operations.
 *
 * Architecture: The App class acts as the Presentation Layer, delegating all
 * business logic to CustomerService, ClaimService, and ReportService.
 * Data persistence is handled by FileRepository through CSV files.
 */

#ifndef APP_H
#define APP_H

#include <SFML/Graphics.hpp>
#include "gui/Theme.h"
#include "gui/Widgets.h"
#include "services/CustomerService.h"
#include "services/ClaimService.h"
#include "services/ReportService.h"
#include "domain/Staff.h"
#include "repositories/FileRepository.h"
#include <memory>
#include <functional>

// Application screen states for navigation flow
enum class Screen { ROLE_SELECT, LOGIN, DASHBOARD };

// Dashboard content panels — each represents a different operation view
enum class ContentPanel {
    NONE, REG_CUSTOMER, ADD_VEHICLE, ISSUE_POLICY, FILE_CLAIM,
    VIEW_ASSIGNED, SUBMIT_INSPECTION,
    PENDING_CLAIMS, ASSIGN_SURVEYOR, APPROVE_CLAIM, REJECT_CLAIM,
    ASSIGN_WORKSHOP, CUSTOMER_HISTORY,
    REP_NEW_CUST, REP_PENDING, REP_INSPECTIONS
};

// Main application class — manages SFML window, screens, and business services
class App {
    // ── Core SFML components ──
    sf::RenderWindow window;           // Main rendering window
    sf::Font font, fontBold;           // Regular and bold typefaces
    Screen currentScreen = Screen::ROLE_SELECT;  // Current navigation state
    ContentPanel currentPanel = ContentPanel::NONE;  // Active dashboard panel
    Staff loggedInStaff;               // Currently authenticated staff member
    StaffRole selectedRole = StaffRole::SALESMAN;  // Role selected at login
    std::string dataDir = "data";      // Path to CSV data directory

    // ── Business layer services ──
    CustomerService customerSvc;       // Customer, vehicle, policy operations
    ClaimService claimSvc;             // Claim lifecycle management
    ReportService reportSvc;           // Report generation
    FileRepository<Staff> staffRepo;   // Staff authentication lookups

    // ── Role selection screen widgets ──
    Panel roleCard;
    Label roleTitle, roleSubtitle;
    Button roleSalesmanBtn, roleSurveyorBtn, roleManagerBtn;

    // ── Login screen widgets ──
    Panel loginCard;
    Label loginTitle, loginRoleLabel, loginError;
    TextInput loginId, loginPwd;
    Button loginBtn, loginBackBtn;

    // ── Dashboard layout widgets ──
    Panel sidebar, header, contentBg;  // Main layout panels
    Label headerTitle, headerRole;     // Header text elements
    Button logoutBtn;                  // Logout button in header
    std::vector<Button> navButtons;    // Sidebar navigation buttons
    Toast toast;                       // Notification popup

    // ── Dynamic form widgets (rebuilt for each panel switch) ──
    std::vector<TextInput> formInputs; // Input fields for current form
    std::vector<Button> formButtons;   // Action buttons for current form
    std::vector<Label> formLabels;     // Labels for current form
    DataTable dataTable, secondTable;  // Primary and secondary data tables
    Label panelTitle;                  // Current panel title
    Label statusLabel;                 // Status/info display

public:
    // Initialize SFML window, load fonts, set up services, and show role selection
    App() : window(sf::VideoMode({Theme::WIN_W, Theme::WIN_H}), "Auto Insurance System", sf::Style::Close),
            font(Theme::FONT_PATH), fontBold(Theme::FONT_BOLD_PATH),
            customerSvc(dataDir), claimSvc(dataDir), reportSvc(dataDir),
            staffRepo(dataDir + "/staff.csv"), toast(font)
    {
        window.setFramerateLimit(60);
        setupRoleSelect();  // Start on the role selection screen
    }

    // Main application loop — polls events, updates state, and renders
    void run() {
        while (window.isOpen()) {
            while (auto event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) window.close();
            // Process events based on current screen state
                switch (currentScreen) {
                    case Screen::ROLE_SELECT: handleRoleSelectEvent(*event); break;
                    case Screen::LOGIN: handleLoginEvent(*event); break;
                    case Screen::DASHBOARD: handleDashboardEvent(*event); break;
                }
            }
            // Render the active screen with a cleared background
            window.clear(Theme::BG);
            switch (currentScreen) {
                case Screen::ROLE_SELECT: drawRoleSelect(); break;
                case Screen::LOGIN: drawLogin(); break;
                case Screen::DASHBOARD: drawDashboard(); break;
            }
            toast.draw(window);
            window.display();
        }
    }

private:
    // ═══════════════════════════════════════
    //  ROLE SELECTION SCREEN
    // ═══════════════════════════════════════
    // Set up role selection button callbacks to navigate to login
    void setupRoleSelect() {
        // Calculate centered position for the role selection card
        float cw = 400, ch = 320;
        float cx = (Theme::WIN_W - cw) / 2.f, cy = (Theme::WIN_H - ch) / 2.f;
        roleCard = Panel({cx, cy}, {cw, ch}, Theme::CARD);
        roleTitle = Label(fontBold, "Auto Insurance System", Theme::FONT_XL, Theme::TEXT_PRIMARY);
        roleTitle.setPosition({cx + 28, cy + 25});
        roleSubtitle = Label(font, "Select your role to continue", Theme::FONT_MD, Theme::TEXT_SECONDARY);
        roleSubtitle.setPosition({cx + 80, cy + 68});
        float bw = cw - 60, bh = 48;
        roleSalesmanBtn = Button(fontBold, "Login as Salesman", {cx+30, cy+110}, {bw, bh}, Theme::PRIMARY, Theme::PRIMARY_HOVER);
        roleSalesmanBtn.setCallback([this](){ selectedRole=StaffRole::SALESMAN; currentScreen=Screen::LOGIN; setupLogin(); });
        roleSurveyorBtn = Button(fontBold, "Login as Surveyor", {cx+30, cy+170}, {bw, bh}, Theme::ACCENT, {110,72,210});
        roleSurveyorBtn.setCallback([this](){ selectedRole=StaffRole::SURVEYOR; currentScreen=Screen::LOGIN; setupLogin(); });
        roleManagerBtn = Button(fontBold, "Login as Manager", {cx+30, cy+230}, {bw, bh}, Theme::SUCCESS, Theme::SUCCESS_HOVER);
        roleManagerBtn.setCallback([this](){ selectedRole=StaffRole::MANAGER; currentScreen=Screen::LOGIN; setupLogin(); });
    }
    // Handle click events for role selection buttons
    void handleRoleSelectEvent(const sf::Event& e) {
        roleSalesmanBtn.handleEvent(e); roleSurveyorBtn.handleEvent(e); roleManagerBtn.handleEvent(e);
    }
    // Render the role selection screen with top accent bar
    void drawRoleSelect() {
        sf::RectangleShape topBar({(float)Theme::WIN_W, 4.f});
        topBar.setFillColor(Theme::PRIMARY); window.draw(topBar);
        roleCard.draw(window); roleTitle.draw(window); roleSubtitle.draw(window);
        roleSalesmanBtn.draw(window); roleSurveyorBtn.draw(window); roleManagerBtn.draw(window);
    }

    // ═══════════════════════════════════════
    //  LOGIN SCREEN
    // ═══════════════════════════════════════
    // Build the login form with ID/password fields and back button
    void setupLogin() {
        // Calculate centered position for the login card
        float cw = 380, ch = 380;
        float cx = (Theme::WIN_W - cw) / 2.f, cy = (Theme::WIN_H - ch) / 2.f;
        loginCard = Panel({cx, cy}, {cw, ch}, Theme::CARD);
        loginTitle = Label(fontBold, "Staff Login", Theme::FONT_XL, Theme::TEXT_PRIMARY);
        loginTitle.setPosition({cx + 105, cy + 20});
        // Color-code the role label: blue for salesman, purple for surveyor, green for manager
        sf::Color rc = selectedRole==StaffRole::SALESMAN ? Theme::PRIMARY : selectedRole==StaffRole::SURVEYOR ? Theme::ACCENT : Theme::SUCCESS;
        loginRoleLabel = Label(fontBold, staffRoleToString(selectedRole), Theme::FONT_LG, rc);
        loginRoleLabel.setPosition({cx + 130, cy + 58});
        loginId = TextInput(font, "Enter staff ID", {cx+30, cy+120}, {cw-60, Theme::INPUT_H}, "Staff ID");
        loginPwd = TextInput(font, "Enter password", {cx+30, cy+195}, {cw-60, Theme::INPUT_H}, "Password", true);
        float halfW = (cw - 70) / 2.f;
        loginBtn = Button(font, "Login", {cx+30, cy+270}, {halfW, Theme::BTN_H}, Theme::PRIMARY, Theme::PRIMARY_HOVER);
        loginBtn.setCallback([this](){ doLogin(); });
        loginBackBtn = Button(font, "Back", {cx+40+halfW, cy+270}, {halfW, Theme::BTN_H}, Theme::CARD_HOVER, Theme::SIDEBAR);
        loginBackBtn.setCallback([this](){ currentScreen=Screen::ROLE_SELECT; loginError.setString(""); });
        loginError = Label(font, "", Theme::FONT_SM, Theme::DANGER);
        loginError.setPosition({cx + 30, cy + 330});
    }

    // Authenticate staff against the staff.csv file and validate role match
    void doLogin() {
        try {
            int id = std::stoi(loginId.getValue());
            auto staff = staffRepo.loadAllRecords();
            for (auto& s : staff) {
                if (s.staffId == id && s.authenticate(loginPwd.getValue())) {
                    if (s.role != selectedRole) {
                        loginError.setString("Staff is not a " + staffRoleToString(selectedRole) + "!");
                        return;
                    }
                    loggedInStaff = s;
                    currentScreen = Screen::DASHBOARD;
                    setupDashboard();
                    return;
                }
            }
            loginError.setString("Invalid credentials!");
        } catch (...) { loginError.setString("Enter a valid staff ID!"); }
    }

    // Handle text input, button clicks, and keyboard shortcuts on login
    void handleLoginEvent(const sf::Event& e) {
        loginId.handleEvent(e); loginPwd.handleEvent(e);
        loginBtn.handleEvent(e); loginBackBtn.handleEvent(e);
        if (auto* key = e.getIf<sf::Event::KeyPressed>()) {
            if (key->code == sf::Keyboard::Key::Enter) doLogin();
            if (key->code == sf::Keyboard::Key::Escape) { currentScreen = Screen::ROLE_SELECT; loginError.setString(""); }
        }
    }

    // Render the login card with all form elements
    void drawLogin() {
        sf::RectangleShape topBar({(float)Theme::WIN_W, 4.f});
        topBar.setFillColor(Theme::PRIMARY); window.draw(topBar);
        loginCard.draw(window); loginTitle.draw(window); loginRoleLabel.draw(window);
        loginId.draw(window); loginPwd.draw(window);
        loginBtn.draw(window); loginBackBtn.draw(window); loginError.draw(window);
    }

    // ═══════════════════════════════════════
    //  DASHBOARD
    // ═══════════════════════════════════════
    // Build the dashboard layout: sidebar navigation, header, and content area
    void setupDashboard() {
        sidebar = Panel({0, 0}, {Theme::SIDEBAR_W, (float)Theme::WIN_H}, Theme::SIDEBAR);
        // Header bar spans the width minus sidebar
        header = Panel({Theme::SIDEBAR_W, 0}, {Theme::WIN_W - Theme::SIDEBAR_W, Theme::HEADER_H}, {20, 30, 48});
        // Content background area with padding
        contentBg = Panel({Theme::SIDEBAR_W + 16, Theme::HEADER_H + 16},
                          {Theme::WIN_W - Theme::SIDEBAR_W - 32, Theme::WIN_H - Theme::HEADER_H - 32}, Theme::CARD);

        headerTitle = Label(fontBold, "Welcome, " + loggedInStaff.name, Theme::FONT_LG, Theme::TEXT_PRIMARY);
        headerTitle.setPosition({Theme::SIDEBAR_W + 20, 16});
        headerRole = Label(font, staffRoleToString(loggedInStaff.role), Theme::FONT_SM, Theme::TEXT_SECONDARY);
        headerRole.setPosition({Theme::SIDEBAR_W + 20, 40});

        logoutBtn = Button(font, "Logout", {Theme::WIN_W - 110.f, 12.f}, {90, 36}, Theme::DANGER, Theme::DANGER_HOVER);
        logoutBtn.setCallback([this]() { currentScreen = Screen::ROLE_SELECT; loginId.clear(); loginPwd.clear(); loginError.setString(""); });

        // Navigation buttons
        // Build role-specific navigation buttons in the sidebar
        navButtons.clear();
        float ny = 80;  // Starting Y position for nav buttons
        auto addNav = [&](const std::string& label, ContentPanel panel) {
            Button b(font, label, {10, ny}, {Theme::SIDEBAR_W - 20, 38}, Theme::SIDEBAR, Theme::CARD_HOVER);
            b.setCallback([this, panel]() { switchPanel(panel); });
            navButtons.push_back(b);
            ny += 44;
        };

        // App title in sidebar
        panelTitle = Label(fontBold, "", Theme::FONT_LG, Theme::TEXT_PRIMARY);
        statusLabel = Label(font, "", Theme::FONT_MD, Theme::TEXT_SECONDARY);

        switch (loggedInStaff.role) {
            case StaffRole::SALESMAN:
                addNav("Register Customer", ContentPanel::REG_CUSTOMER);
                addNav("Add Vehicle", ContentPanel::ADD_VEHICLE);
                addNav("Issue Policy", ContentPanel::ISSUE_POLICY);
                addNav("File Claim", ContentPanel::FILE_CLAIM);
                break;
            case StaffRole::SURVEYOR:
                addNav("Assigned Claims", ContentPanel::VIEW_ASSIGNED);
                addNav("Submit Inspection", ContentPanel::SUBMIT_INSPECTION);
                break;
            case StaffRole::MANAGER:
                addNav("Pending Claims", ContentPanel::PENDING_CLAIMS);
                addNav("Assign Surveyor", ContentPanel::ASSIGN_SURVEYOR);
                addNav("Approve Claim", ContentPanel::APPROVE_CLAIM);
                addNav("Reject Claim", ContentPanel::REJECT_CLAIM);
                addNav("Assign Workshop", ContentPanel::ASSIGN_WORKSHOP);
                addNav("Customer History", ContentPanel::CUSTOMER_HISTORY);
                ny += 10;
                addNav("Rpt: New Customers", ContentPanel::REP_NEW_CUST);
                addNav("Rpt: Pending Claims", ContentPanel::REP_PENDING);
                addNav("Rpt: Inspections", ContentPanel::REP_INSPECTIONS);
                break;
        }
        // Auto-select first panel
        switch (loggedInStaff.role) {
            case StaffRole::SALESMAN:  switchPanel(ContentPanel::REG_CUSTOMER); break;
            case StaffRole::SURVEYOR:  switchPanel(ContentPanel::VIEW_ASSIGNED); break;
            case StaffRole::MANAGER:   switchPanel(ContentPanel::PENDING_CLAIMS); break;
        }
    }

    // Switch the dashboard content area to show a different operation panel.
    // Clears all dynamic widgets and rebuilds them for the selected panel.
    void switchPanel(ContentPanel p) {
        currentPanel = p;
        formInputs.clear(); formButtons.clear(); formLabels.clear();
        secondTable = DataTable(); // Reset secondary table
        float cx = Theme::SIDEBAR_W + 36, cy = Theme::HEADER_H + 56;  // Content origin
        float fw = Theme::WIN_W - Theme::SIDEBAR_W - 72;  // Available content width

        auto addInput = [&](const std::string& lbl, const std::string& ph, bool pwd = false) {
            formInputs.emplace_back(font, ph, sf::Vector2f{cx, cy}, sf::Vector2f{fw > 400 ? 350.f : fw, Theme::INPUT_H}, lbl, pwd);
            cy += 70;
        };
        auto addBtn = [&](const std::string& lbl, sf::Color c, sf::Color h, std::function<void()> cb) {
            Button b(font, lbl, {cx, cy}, {160, Theme::BTN_H}, c, h);
            b.setCallback(cb);
            formButtons.push_back(b);
            cy += 55;
        };

        switch (p) {
        case ContentPanel::REG_CUSTOMER:  // Salesman: register a new customer
            panelTitle.setString("Register New Customer"); panelTitle.setPosition({cx, cy - 20}); cy += 30;
            addInput("Name", "Enter customer name");
            addInput("Contact", "Phone number");
            addInput("Address", "Full address");
            addBtn("Register", Theme::SUCCESS, Theme::SUCCESS_HOVER, [this]() { doRegisterCustomer(); });
            break;
        case ContentPanel::ADD_VEHICLE:  // Salesman: register vehicle under customer
            panelTitle.setString("Add Vehicle"); panelTitle.setPosition({cx, cy - 20}); cy += 30;
            addInput("Customer ID", "Enter customer ID");
            addInput("Registration No", "e.g. ABC-123");
            addInput("Make", "e.g. Toyota");
            addInput("Model", "e.g. Corolla");
            addInput("Year", "e.g. 2023");
            addBtn("Add Vehicle", Theme::SUCCESS, Theme::SUCCESS_HOVER, [this]() { doAddVehicle(); });
            break;
        case ContentPanel::ISSUE_POLICY:  // Salesman: create insurance policy
            panelTitle.setString("Issue Insurance Policy"); panelTitle.setPosition({cx, cy - 20}); cy += 30;
            addInput("Customer ID", "Customer ID");
            addInput("Vehicle ID", "Vehicle ID");
            addInput("Start Date", "YYYY-MM-DD");
            addInput("End Date", "YYYY-MM-DD");
            addInput("Premium", "Amount");
            addBtn("Issue Policy", Theme::SUCCESS, Theme::SUCCESS_HOVER, [this]() { doIssuePolicy(); });
            break;
        case ContentPanel::FILE_CLAIM:  // Salesman: file a damage claim
            panelTitle.setString("File Damage Claim"); panelTitle.setPosition({cx, cy - 20}); cy += 30;
            addInput("Policy ID", "Policy ID");
            addInput("Vehicle ID", "Vehicle ID");
            addInput("Accident Date", "YYYY-MM-DD");
            addInput("Damage Description", "Describe damage");
            addBtn("File Claim", Theme::WARNING, {200, 130, 0}, [this]() { doFileClaim(); });
            break;
        case ContentPanel::VIEW_ASSIGNED:  // Surveyor: view claims assigned to them
            panelTitle.setString("Assigned Claims"); panelTitle.setPosition({cx, cy - 20}); cy += 30;
            setupClaimTable(cx, cy, fw); loadAssignedClaims();
            break;
        case ContentPanel::SUBMIT_INSPECTION:  // Surveyor: submit findings for a claim
            panelTitle.setString("Submit Inspection Report"); panelTitle.setPosition({cx, cy - 20}); cy += 30;
            // Show assigned UNDER_REVIEW claims as reference
            dataTable = DataTable(font, {cx, cy}, {fw, 150},
                {"ID", "Policy", "Vehicle", "Accident", "Status", "Description"},
                {50, 60, 65, 100, 120, 250});
            { auto cls = claimSvc.getClaimsBySurveyor(loggedInStaff.staffId);
              std::vector<std::vector<std::string>> rows;
              for (auto& c : cls) if(c.status==ClaimStatus::UNDER_REVIEW)
                  rows.push_back({std::to_string(c.claimId), std::to_string(c.policyId),
                      std::to_string(c.vehicleId), c.accidentDate, claimStatusToString(c.status), c.damageDescription});
              dataTable.setRows(rows); }
            cy += 165;
            addInput("Claim ID", "Claim ID");
            addInput("Inspection Date", "YYYY-MM-DD");
            addInput("Estimated Cost", "Amount");
            addInput("Recommendation", "Your recommendation");
            addBtn("Submit Report", Theme::SUCCESS, Theme::SUCCESS_HOVER, [this]() { doSubmitInspection(); });
            break;
        case ContentPanel::PENDING_CLAIMS:  // Manager: view all active claims
            panelTitle.setString("All Pending Claims"); panelTitle.setPosition({cx, cy - 20}); cy += 30;
            setupClaimTable(cx, cy, fw); loadPendingClaims();
            break;
        case ContentPanel::ASSIGN_SURVEYOR:  // Manager: assign surveyor to inspect
            panelTitle.setString("Assign Surveyor to Claim"); panelTitle.setPosition({cx, cy - 20}); cy += 30;
            // Show SUBMITTED claims
            dataTable = DataTable(font, {cx, cy}, {fw, 150},
                {"Claim ID", "Policy", "Vehicle", "Accident Date", "Status", "Description"},
                {65, 60, 65, 100, 100, 250});
            { auto cls = claimSvc.getClaimsByStatus(ClaimStatus::SUBMITTED);
              std::vector<std::vector<std::string>> rows;
              for (auto& c : cls) rows.push_back({std::to_string(c.claimId), std::to_string(c.policyId),
                  std::to_string(c.vehicleId), c.accidentDate, claimStatusToString(c.status), c.damageDescription});
              dataTable.setRows(rows); }
            cy += 165;
            // Show surveyors info
            { auto survs = claimSvc.getSurveyors(); std::string info = "Surveyors: ";
              for (auto& s : survs) info += "#" + std::to_string(s.staffId) + " " + s.name + "  ";
              statusLabel.setString(info); statusLabel.setPosition({cx, cy}); }
            cy += 25;
            addInput("Claim ID", "Claim ID");
            addInput("Surveyor Staff ID", "Surveyor ID");
            addBtn("Assign", Theme::PRIMARY, Theme::PRIMARY_HOVER, [this]() { doAssignSurveyor(); });
            break;
        case ContentPanel::APPROVE_CLAIM:  // Manager: approve after reviewing inspection
            panelTitle.setString("Approve Claim"); panelTitle.setPosition({cx, cy - 20}); cy += 30;
            // Show INSPECTED claims with inspection info
            dataTable = DataTable(font, {cx, cy}, {fw, 150},
                {"Claim ID", "Policy", "Vehicle", "Accident", "Status", "Surveyor"},
                {65, 60, 65, 100, 100, 75});
            { auto cls = claimSvc.getClaimsByStatus(ClaimStatus::INSPECTED);
              std::vector<std::vector<std::string>> rows;
              for (auto& c : cls) rows.push_back({std::to_string(c.claimId), std::to_string(c.policyId),
                  std::to_string(c.vehicleId), c.accidentDate, claimStatusToString(c.status),
                  c.surveyorId ? std::to_string(c.surveyorId) : "-"});
              dataTable.setRows(rows); }
            cy += 155;
            // Show inspection reports for those claims
            secondTable = DataTable(font, {cx, cy}, {fw, 120},
                {"Claim", "Surveyor", "Date", "Est.Cost", "Recommendation"},
                {65, 75, 110, 90, 300});
            { auto insp = claimSvc.getAllInspections(); auto cls = claimSvc.getClaimsByStatus(ClaimStatus::INSPECTED);
              std::vector<std::vector<std::string>> rows;
              for (auto& c : cls) for (auto& r : insp) if(r.claimId==c.claimId)
                  rows.push_back({std::to_string(r.claimId), std::to_string(r.surveyorId),
                      r.inspectionDate, std::to_string((int)r.estimatedCost), r.recommendation});
              secondTable.setRows(rows); }
            cy += 135;
            addInput("Claim ID", "Claim ID to approve");
            addBtn("Approve", Theme::SUCCESS, Theme::SUCCESS_HOVER, [this]() { doApproveClaim(); });
            break;
        case ContentPanel::REJECT_CLAIM:  // Manager: reject after reviewing inspection
            panelTitle.setString("Reject Claim"); panelTitle.setPosition({cx, cy - 20}); cy += 30;
            // Show INSPECTED claims
            dataTable = DataTable(font, {cx, cy}, {fw, 150},
                {"Claim ID", "Policy", "Vehicle", "Accident", "Status", "Surveyor"},
                {65, 60, 65, 100, 100, 75});
            { auto cls = claimSvc.getClaimsByStatus(ClaimStatus::INSPECTED);
              std::vector<std::vector<std::string>> rows;
              for (auto& c : cls) rows.push_back({std::to_string(c.claimId), std::to_string(c.policyId),
                  std::to_string(c.vehicleId), c.accidentDate, claimStatusToString(c.status),
                  c.surveyorId ? std::to_string(c.surveyorId) : "-"});
              dataTable.setRows(rows); }
            cy += 155;
            // Inspection reports
            secondTable = DataTable(font, {cx, cy}, {fw, 120},
                {"Claim", "Surveyor", "Date", "Est.Cost", "Recommendation"},
                {65, 75, 110, 90, 300});
            { auto insp = claimSvc.getAllInspections(); auto cls = claimSvc.getClaimsByStatus(ClaimStatus::INSPECTED);
              std::vector<std::vector<std::string>> rows;
              for (auto& c : cls) for (auto& r : insp) if(r.claimId==c.claimId)
                  rows.push_back({std::to_string(r.claimId), std::to_string(r.surveyorId),
                      r.inspectionDate, std::to_string((int)r.estimatedCost), r.recommendation});
              secondTable.setRows(rows); }
            cy += 135;
            addInput("Claim ID", "Claim ID to reject");
            addBtn("Reject", Theme::DANGER, Theme::DANGER_HOVER, [this]() { doRejectClaim(); });
            break;
        case ContentPanel::ASSIGN_WORKSHOP:  // Manager: send vehicle to repair shop
            panelTitle.setString("Assign Workshop"); panelTitle.setPosition({cx, cy - 20}); cy += 30;
            // Show APPROVED claims
            dataTable = DataTable(font, {cx, cy}, {fw, 130},
                {"Claim ID", "Policy", "Vehicle", "Accident", "Status"},
                {65, 70, 70, 110, 120});
            { auto cls = claimSvc.getClaimsByStatus(ClaimStatus::APPROVED);
              std::vector<std::vector<std::string>> rows;
              for (auto& c : cls) rows.push_back({std::to_string(c.claimId), std::to_string(c.policyId),
                  std::to_string(c.vehicleId), c.accidentDate, claimStatusToString(c.status)});
              dataTable.setRows(rows); }
            cy += 140;
            // Show workshops table
            secondTable = DataTable(font, {cx, cy}, {fw, 120},
                {"ID", "Name", "Address", "Contact", "Status"},
                {40, 160, 200, 120, 100});
            { auto ws = claimSvc.getAllWorkshops();
              std::vector<std::vector<std::string>> rows;
              for (auto& w : ws) rows.push_back({std::to_string(w.workshopId), w.name,
                  w.address, w.contact, workshopStatusToString(w.status)});
              secondTable.setRows(rows); }
            cy += 135;
            addInput("Claim ID", "Claim ID");
            addInput("Workshop ID", "Workshop ID");
            addBtn("Assign Workshop", Theme::PRIMARY, Theme::PRIMARY_HOVER, [this]() { doAssignWorkshop(); });
            break;
        case ContentPanel::CUSTOMER_HISTORY:  // Manager: look up customer's past claims
            panelTitle.setString("Customer Claim History"); panelTitle.setPosition({cx, cy - 20}); cy += 30;
            addInput("Customer ID", "Customer ID");
            addBtn("Search", Theme::PRIMARY, Theme::PRIMARY_HOVER, [this]() { doCustomerHistory(); });
            cy += 10;
            setupClaimTable(cx, cy, fw);
            break;
        case ContentPanel::REP_NEW_CUST:  // Manager report: customer acquisition
            panelTitle.setString("Report: New Customers by Month"); panelTitle.setPosition({cx, cy - 20}); cy += 30;
            addInput("Month", "1-12");
            addInput("Year", "e.g. 2026");
            addBtn("Generate", Theme::ACCENT, {110, 72, 210}, [this]() { doReportNewCustomers(); });
            cy += 10;
            dataTable = DataTable(font, {cx, cy}, {fw, 300},
                {"ID", "Name", "Contact", "Address", "Registered"}, {60, 150, 130, 180, 120});
            break;
        case ContentPanel::REP_PENDING:  // Manager report: outstanding claims
            panelTitle.setString("Report: Pending Claims"); panelTitle.setPosition({cx, cy - 20}); cy += 30;
            setupClaimTable(cx, cy, fw); loadPendingClaims();
            break;
        case ContentPanel::REP_INSPECTIONS:  // Manager report: all inspections
            panelTitle.setString("Report: Inspection Logs"); panelTitle.setPosition({cx, cy - 20}); cy += 30;
            dataTable = DataTable(font, {cx, cy}, {fw, 400},
                {"ID", "Claim", "Surveyor", "Date", "Est.Cost", "Recommendation"}, {50, 70, 80, 110, 100, 200});
            { auto reps = reportSvc.getInspectionReports(); std::vector<std::vector<std::string>> rows;
              for (auto& r : reps) rows.push_back({std::to_string(r.inspectionId), std::to_string(r.claimId),
                  std::to_string(r.surveyorId), r.inspectionDate, std::to_string((int)r.estimatedCost), r.recommendation});
              dataTable.setRows(rows); }
            break;
        default: break;
        }
    }

    // Helper: set up a standard 7-column claim table at the given position
    void setupClaimTable(float cx, float cy, float fw) {
        dataTable = DataTable(font, {cx, cy}, {fw, 350},
            {"ID", "Policy", "Vehicle", "Accident", "Status", "Surveyor", "Workshop"},
            {50, 60, 65, 100, 130, 75, 80});
    }

    // Helper: populate the claim table with all pending (non-terminal) claims
    void loadPendingClaims() {
        auto cls = claimSvc.getPendingClaims(); std::vector<std::vector<std::string>> rows;
        for (auto& c : cls) rows.push_back({std::to_string(c.claimId), std::to_string(c.policyId),
            std::to_string(c.vehicleId), c.accidentDate, claimStatusToString(c.status),
            c.surveyorId ? std::to_string(c.surveyorId) : "-", c.workshopId ? std::to_string(c.workshopId) : "-"});
        dataTable.setRows(rows);
    }

    // Helper: populate the claim table with claims assigned to the logged-in surveyor
    void loadAssignedClaims() {
        auto cls = claimSvc.getClaimsBySurveyor(loggedInStaff.staffId); std::vector<std::vector<std::string>> rows;
        for (auto& c : cls) rows.push_back({std::to_string(c.claimId), std::to_string(c.policyId),
            std::to_string(c.vehicleId), c.accidentDate, claimStatusToString(c.status),
            std::to_string(c.surveyorId), c.workshopId ? std::to_string(c.workshopId) : "-"});
        dataTable.setRows(rows);
    }

    // ═══════════════════════════════════════
    //  ACTION HANDLERS — Execute business operations via service layer
    //  Each method: reads form inputs → calls service → shows toast → refreshes
    // ═══════════════════════════════════════
    // Register a new customer using form input values
    void doRegisterCustomer() {
        try {
            auto c = customerSvc.registerCustomer(formInputs[0].getValue(), formInputs[1].getValue(), formInputs[2].getValue());
            toast.show("Customer registered! ID: " + std::to_string(c.customerId), Theme::SUCCESS);
            for (auto& i : formInputs) i.clear();
        } catch (std::exception& e) { toast.show(e.what(), Theme::DANGER); }
    }
    // Add a vehicle under a customer with registration details
    void doAddVehicle() {
        try {
            auto v = customerSvc.addVehicle(std::stoi(formInputs[0].getValue()), formInputs[1].getValue(),
                formInputs[2].getValue(), formInputs[3].getValue(), std::stoi(formInputs[4].getValue()));
            toast.show("Vehicle added! ID: " + std::to_string(v.vehicleId), Theme::SUCCESS);
            for (auto& i : formInputs) i.clear();
        } catch (std::exception& e) { toast.show(e.what(), Theme::DANGER); }
    }
    // Issue a new insurance policy with coverage dates and premium
    void doIssuePolicy() {
        try {
            auto p = customerSvc.createPolicy(std::stoi(formInputs[0].getValue()), std::stoi(formInputs[1].getValue()),
                formInputs[2].getValue(), formInputs[3].getValue(), std::stod(formInputs[4].getValue()));
            toast.show("Policy issued! ID: " + std::to_string(p.policyId), Theme::SUCCESS);
            for (auto& i : formInputs) i.clear();
        } catch (std::exception& e) { toast.show(e.what(), Theme::DANGER); }
    }
    // File a damage claim against a policy
    void doFileClaim() {
        try {
            auto c = claimSvc.createClaim(std::stoi(formInputs[0].getValue()), std::stoi(formInputs[1].getValue()),
                formInputs[2].getValue(), formInputs[3].getValue());
            toast.show("Claim filed! ID: " + std::to_string(c.claimId), Theme::SUCCESS);
            for (auto& i : formInputs) i.clear();
        } catch (std::exception& e) { toast.show(e.what(), Theme::DANGER); }
    }
    // Submit an inspection report for an assigned claim (surveyor operation)
    void doSubmitInspection() {
        try {
            auto r = claimSvc.addInspectionReport(std::stoi(formInputs[0].getValue()), loggedInStaff.staffId,
                formInputs[1].getValue(), std::stod(formInputs[2].getValue()), formInputs[3].getValue());
            toast.show("Inspection submitted! ID: " + std::to_string(r.inspectionId), Theme::SUCCESS);
            switchPanel(ContentPanel::SUBMIT_INSPECTION); // refresh table
        } catch (std::exception& e) { toast.show(e.what(), Theme::DANGER); }
    }
    // Assign a surveyor to inspect a submitted claim (manager operation)
    void doAssignSurveyor() {
        try {
            claimSvc.assignSurveyor(std::stoi(formInputs[0].getValue()), std::stoi(formInputs[1].getValue()));
            toast.show("Surveyor assigned!", Theme::SUCCESS);
            switchPanel(ContentPanel::ASSIGN_SURVEYOR); // refresh table
        } catch (std::exception& e) { toast.show(e.what(), Theme::DANGER); }
    }
    // Approve an inspected claim (manager operation)
    void doApproveClaim() {
        try {
            claimSvc.approveClaim(std::stoi(formInputs[0].getValue()));
            toast.show("Claim APPROVED!", Theme::SUCCESS);
            switchPanel(ContentPanel::APPROVE_CLAIM); // refresh table
        } catch (std::exception& e) { toast.show(e.what(), Theme::DANGER); }
    }
    // Reject an inspected claim (manager operation)
    void doRejectClaim() {
        try {
            claimSvc.rejectClaim(std::stoi(formInputs[0].getValue()));
            toast.show("Claim REJECTED.", Theme::WARNING);
            switchPanel(ContentPanel::REJECT_CLAIM); // refresh table
        } catch (std::exception& e) { toast.show(e.what(), Theme::DANGER); }
    }
    // Assign a registered workshop to an approved claim
    void doAssignWorkshop() {
        try {
            claimSvc.assignWorkshop(std::stoi(formInputs[0].getValue()), std::stoi(formInputs[1].getValue()));
            toast.show("Workshop assigned!", Theme::SUCCESS);
            switchPanel(ContentPanel::ASSIGN_WORKSHOP); // refresh table
        } catch (std::exception& e) { toast.show(e.what(), Theme::DANGER); }
    }
    // Look up and display claim history for a specific customer
    void doCustomerHistory() {
        try {
            auto cls = reportSvc.getCustomerClaimHistory(std::stoi(formInputs[0].getValue()));
            std::vector<std::vector<std::string>> rows;
            for (auto& c : cls) rows.push_back({std::to_string(c.claimId), std::to_string(c.policyId),
                std::to_string(c.vehicleId), c.accidentDate, claimStatusToString(c.status),
                c.surveyorId ? std::to_string(c.surveyorId) : "-", c.workshopId ? std::to_string(c.workshopId) : "-"});
            dataTable.setRows(rows);
            toast.show("Found " + std::to_string(rows.size()) + " claims", Theme::PRIMARY);
        } catch (std::exception& e) { toast.show(e.what(), Theme::DANGER); }
    }
    // Generate report of new customers registered in a specific month/year
    void doReportNewCustomers() {
        try {
            auto custs = reportSvc.getNewCustomersByMonth(std::stoi(formInputs[0].getValue()), std::stoi(formInputs[1].getValue()));
            std::vector<std::vector<std::string>> rows;
            for (auto& c : custs) rows.push_back({std::to_string(c.customerId), c.name, c.contact, c.address, c.createdDate});
            dataTable.setRows(rows);
            toast.show("Found " + std::to_string(rows.size()) + " customers", Theme::PRIMARY);
        } catch (std::exception& e) { toast.show(e.what(), Theme::DANGER); }
    }

    // ═══════════════════════════════════════
    //  DASHBOARD EVENT HANDLING — Delegates to all active widgets
    // ═══════════════════════════════════════
    // Forward events to all dashboard widgets (buttons, inputs, tables)
    void handleDashboardEvent(const sf::Event& e) {
        logoutBtn.handleEvent(e);
        for (auto& b : navButtons) b.handleEvent(e);
        for (auto& i : formInputs) i.handleEvent(e);
        for (auto& b : formButtons) b.handleEvent(e);
        dataTable.handleEvent(e);
        secondTable.handleEvent(e);
    }

    // Render the complete dashboard: sidebar, header, content area, and widgets
    void drawDashboard() {
        // Accent bar
        sf::RectangleShape accent({(float)Theme::WIN_W, 3.f});
        accent.setFillColor(Theme::PRIMARY); window.draw(accent);

        sidebar.draw(window); header.draw(window); contentBg.draw(window);
        headerTitle.draw(window); headerRole.draw(window); logoutBtn.draw(window);

        // Sidebar title
        Label sideTitle(fontBold, "MENU", Theme::FONT_SM, Theme::TEXT_MUTED);
        sideTitle.setPosition({20, 55}); sideTitle.draw(window);
        for (auto& b : navButtons) b.draw(window);

        // Sidebar logo area
        sf::CircleShape logo(18.f);
        logo.setFillColor(Theme::PRIMARY); logo.setPosition({15, 12});
        window.draw(logo);
        Label logoText(fontBold, "AIS", Theme::FONT_MD, Theme::TEXT_PRIMARY);
        logoText.setPosition({22, 16}); logoText.draw(window);

        // Content
        panelTitle.draw(window);
        statusLabel.draw(window);
        for (auto& l : formLabels) l.draw(window);
        for (auto& i : formInputs) i.draw(window);
        for (auto& b : formButtons) b.draw(window);
        dataTable.draw(window);
        secondTable.draw(window);
    }
};

#endif

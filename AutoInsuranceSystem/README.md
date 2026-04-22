# Automobile Insurance Management System

## Overview
A C++ console-based information system for an automobile insurance company. Manages customers, vehicles, insurance policies, damage claims, inspections, staff (salesmen, surveyors, managers), and registered workshops.

## Architecture

```
┌─────────────────────────────────┐
│   Presentation Layer (UI)       │  InsuranceApp — text menus
├─────────────────────────────────┤
│   Business Layer (Services)     │  CustomerService, ClaimService, ReportService
├─────────────────────────────────┤
│   Data Access Layer             │  FileRepository<T> — CSV persistence
├─────────────────────────────────┤
│   File Storage                  │  data/*.csv
└─────────────────────────────────┘
```

- **UI never accesses files directly** — all data flows through services.
- **Services enforce all business rules** — validation, status transitions, access control.
- **FileRepository** is a generic template handling load/save/append for any entity type.

## Project Structure

```
AutoInsuranceSystem/
├── CMakeLists.txt
├── README.md
├── include/
│   ├── domain/         (Entity headers: Customer, Vehicle, Policy, Claim, etc.)
│   ├── repositories/   (FileRepository template)
│   ├── services/       (CustomerService, ClaimService, ReportService)
│   ├── ui/             (InsuranceApp)
│   └── common/         (Utils)
├── src/
│   ├── main.cpp
│   ├── ui/             (InsuranceApp.cpp)
│   └── services/       (Service implementations)
├── data/               (CSV data files)
├── docs/               (Diagrams, design rationale, code quality report)
└── tests/              (Unit tests)
```

## Build & Run

### Using g++ directly (Windows with MSYS2):
```bash
# Build main application
g++ -std=c++17 -Wall -o insurance_app.exe src/main.cpp src/ui/InsuranceApp.cpp src/services/CustomerService.cpp src/services/ClaimService.cpp src/services/ReportService.cpp -I include

# Build tests
g++ -std=c++17 -Wall -o test_services.exe tests/test_services.cpp src/services/CustomerService.cpp src/services/ClaimService.cpp src/services/ReportService.cpp -I include

# Run
./insurance_app.exe
./test_services.exe
```

### Using CMake:
```bash
cmake -B build
cmake --build build
./build/insurance_app.exe
```

## Login Credentials (Seed Data)

| Staff ID | Name             | Role     | Password |
|----------|------------------|----------|----------|
| 1        | Ahmed Salesman   | SALESMAN | pass123  |
| 2        | Bilal Surveyor   | SURVEYOR | pass123  |
| 3        | Kamran Manager   | MANAGER  | pass123  |

## Menu Walkthrough

### Main Menu
1. **Login** → Enter Staff ID and password

### Salesman Menu
1. Register Customer
2. Add Vehicle
3. Issue Insurance Policy
4. File Damage Claim

### Surveyor Menu
1. View Assigned Claims
2. Submit Inspection Report

### Manager Menu
1. View Pending Claims
2. Assign Surveyor to Claim
3. Approve Claim
4. Reject Claim
5. Assign Workshop
6. View Customer Claim History
7. Reports (New Customers by Month, Pending Claims, Inspection Logs)

## Business Rules Enforced

1. **Claim filing** requires an ACTIVE policy and accident date within coverage period
2. **Claim approval/rejection** requires an existing inspection report
3. **Workshop assignment** only after claim is APPROVED
4. **Workshop** must be REGISTERED (not SUSPENDED)
5. **Vehicle registration** numbers must be unique
6. **Policy creation** requires valid customer-vehicle ownership

## Assumptions & Limitations

- Single-user console application (no concurrency)
- Passwords stored in plain text (academic project)
- Date validation is basic (YYYY-MM-DD format check)
- Data files must be in `data/` directory relative to the executable
- No policy expiration auto-check (status remains as set)

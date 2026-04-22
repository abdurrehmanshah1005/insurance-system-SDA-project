# Code Quality Report

## Analysis Tool
**Compiler**: g++ (UCRT64 / MSYS2) with `-Wall` flag  
**Standard**: C++17  
**Date**: 2026-04-22

## Build Results

```
g++ -std=c++17 -Wall -o insurance_app.exe src/main.cpp src/ui/InsuranceApp.cpp 
    src/services/CustomerService.cpp src/services/ClaimService.cpp 
    src/services/ReportService.cpp -I include
```

**Result**: Clean compilation — **0 warnings, 0 errors**

## Static Analysis Summary

### Methodology
Since cppcheck/SonarQube was not available in the build environment, the following manual code quality review was conducted:

### Findings

| # | Category | Severity | Description | Status |
|---|----------|----------|-------------|--------|
| 1 | Architecture | ✅ Satisfied | 3-layer separation enforced — UI never accesses files | Clean |
| 2 | Type Safety | ✅ Satisfied | All statuses use `enum class` types | Clean |
| 3 | Input Validation | ✅ Satisfied | All user inputs validated with retry loops | Clean |
| 4 | Exception Safety | ✅ Satisfied | Business rule violations throw `std::runtime_error` with clear messages | Clean |
| 5 | Const Correctness | ✅ Satisfied | Query methods marked `const` | Clean |
| 6 | File I/O Safety | ✅ Satisfied | Atomic writes (temp file + rename) | Clean |
| 7 | Memory Safety | ✅ Satisfied | No raw pointers owned, RAII throughout | Clean |
| 8 | Code Duplication | ⚠️ Minor | FileRepository template eliminates most duplication | Acceptable |

### Code Metrics

| Metric | Value |
|--------|-------|
| Source Files | 8 (.h headers + .cpp implementations) |
| Total Lines of Code | ~900 |
| Number of Classes/Structs | 11 (7 domain + 3 services + 1 UI) |
| Unit Tests | 11 |
| Test Pass Rate | 100% (11/11) |

### Recommendations
1. Consider adding `noexcept` specifications where applicable
2. Could add `[[nodiscard]]` attributes to service methods returning values
3. Password storage is plain-text (acceptable for academic project, not production)

## Test Results

```
=== Running Unit Tests ===

  [PASS] test_register_customer
  [PASS] test_add_vehicle_unique_reg
  [PASS] test_add_vehicle_invalid_customer
  [PASS] test_create_policy_valid
  [PASS] test_create_policy_wrong_vehicle_owner
  [PASS] test_file_claim_active_policy
  [PASS] test_file_claim_inactive_policy
  [PASS] test_approve_without_inspection
  [PASS] test_full_claim_lifecycle
  [PASS] test_assign_suspended_workshop
  [PASS] test_report_new_customers_by_month

--- Results: 11 passed, 0 failed ---
```

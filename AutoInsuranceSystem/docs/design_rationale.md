# Design Rationale

## 1. Layered Architecture

The system uses a strict 3-layer architecture:

- **Presentation Layer** (`InsuranceApp`): Handles all user interaction via text menus. Never accesses files directly.
- **Business Layer** (`CustomerService`, `ClaimService`, `ReportService`): Contains all domain logic, validations, and workflow enforcement.
- **Data Access Layer** (`FileRepository<T>`): Generic template class providing CSV-based persistence for all entity types.

**Rationale**: This separation ensures the UI can change without affecting business logic, and the storage mechanism can change without affecting either.

## 2. Three Consolidated Services

Rather than having a separate service per entity, we grouped related operations:

- **CustomerService**: Manages customers, vehicles, and policies (customer onboarding workflow).
- **ClaimService**: Manages the complete claim lifecycle including surveyor assignment, inspection, approval, and workshop assignment.
- **ReportService**: Read-only service for generating management reports.

**Rationale**: Keeps the service layer simple while still maintaining clear separation of concerns.

## 3. Generic FileRepository Template

A single `FileRepository<T>` template replaces the need for separate repository classes per entity. Each entity type provides `toCSV()`, `fromCSV()`, and `getId()` methods.

**Rationale**: Eliminates code duplication across 7 entity types. The template provides type-safe operations while the CSV interface is consistent.

## 4. Pipe-Delimited CSV Format

Uses `|` as the CSV delimiter instead of comma.

**Rationale**: Damage descriptions and addresses may contain commas. Pipe delimiter avoids the need for complex CSV escaping.

## 5. Atomic File Writes

The `saveAllRecords()` method writes to a `.tmp` file first, then replaces the original.

**Rationale**: Prevents data corruption if the application crashes mid-write.

## 6. Enum-Based Status Types

All statuses (PolicyStatus, ClaimStatus, WorkshopStatus, StaffRole) use C++ `enum class` types.

**Rationale**: Provides compile-time type safety and prevents invalid status values.

## 7. Business Rule Enforcement

All business rules are enforced in the service layer:
- Claims require ACTIVE policy with valid date range
- Manager approval/rejection requires existing inspection report  
- Workshop assignment requires APPROVED claim and REGISTERED workshop
- Vehicle registration numbers must be unique

**Rationale**: Centralizing validation in services means the UI cannot bypass rules, regardless of how the user interface evolves.

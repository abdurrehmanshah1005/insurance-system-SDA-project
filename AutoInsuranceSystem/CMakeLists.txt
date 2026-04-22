cmake_minimum_required(VERSION 3.16)
project(AutoInsuranceSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Main executable ──
add_executable(insurance_app
    src/main.cpp
    src/ui/InsuranceApp.cpp
    src/services/CustomerService.cpp
    src/services/ClaimService.cpp
    src/services/ReportService.cpp
)

target_include_directories(insurance_app PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# ── Test executable ──
add_executable(tests
    tests/test_services.cpp
    src/services/CustomerService.cpp
    src/services/ClaimService.cpp
    src/services/ReportService.cpp
)

target_include_directories(tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Copy data folder to build directory
file(COPY ${CMAKE_SOURCE_DIR}/data DESTINATION ${CMAKE_BINARY_DIR})

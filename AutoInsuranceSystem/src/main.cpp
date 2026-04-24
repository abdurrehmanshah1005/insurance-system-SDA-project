/**
 * @file main.cpp
 * @brief Entry point for the Automobile Insurance Management System (CLI version).
 *
 * Creates the InsuranceApp instance with the "data" directory for CSV file
 * storage and starts the main menu loop. Any unhandled exceptions are caught
 * and reported to stderr before exiting with an error code.
 */

#include "../include/ui/InsuranceApp.h"
#include <iostream>

// Application entry point — initializes and runs the insurance system
int main() {
    try {
        InsuranceApp app("data");  // Initialize with data directory path
        app.run();                 // Start the main menu loop
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

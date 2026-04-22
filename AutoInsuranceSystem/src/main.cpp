// ─────────────────────────────────────────────
// Automobile Insurance Management System
// Entry Point
// ─────────────────────────────────────────────
#include "../include/ui/InsuranceApp.h"
#include <iostream>

int main() {
    try {
        InsuranceApp app("data");
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

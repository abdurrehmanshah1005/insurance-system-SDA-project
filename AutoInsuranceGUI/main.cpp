/**
 * @file main.cpp
 * @brief Entry point for the SFML-based GUI Insurance Management System.
 *
 * Creates the App instance which initializes the SFML window, loads fonts,
 * and sets up the service layer. The application starts on the role selection
 * screen. Any unhandled exceptions are caught and reported to stderr.
 */

#include "App.h"

// Application entry point — initializes the GUI and starts the event loop
int main() {
    try {
        App app;    // Initialize SFML window, services, and UI components
        app.run();  // Start the main rendering and event loop
    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

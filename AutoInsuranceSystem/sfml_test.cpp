/**
 * @file sfml_test.cpp
 * @brief Simple SFML rendering test to verify library linkage.
 *
 * This utility program creates a minimal SFML window with a green circle
 * to validate that SFML is correctly installed and linked. It is not part
 * of the main application — used only during development setup.
 */

#include <SFML/Graphics.hpp>

// Test entry point — creates a window and renders a green circle
int main()
{
    // Create a 400x400 window for the render test
    sf::RenderWindow window(sf::VideoMode({400, 400}), "SFML Circle Test");

    // Create a green circle shape centered in the window
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);
    shape.setPosition({100.f, 100.f});

    // Main render loop — runs until the window is closed
    while (window.isOpen())
    {
        // Process all pending events (close, resize, etc.)
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // Clear, draw, and present the frame
        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}

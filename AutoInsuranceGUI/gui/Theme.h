/**
 * @file Theme.h
 * @brief Centralized design system constants for the SFML GUI application.
 *
 * Defines all visual constants including the modern dark color palette,
 * typography sizes, layout dimensions, and font paths. All UI components
 * reference these constants to maintain visual consistency throughout
 * the application.
 */

#ifndef THEME_H
#define THEME_H
#include <SFML/Graphics.hpp>
#include <string>

namespace Theme {
    // ── Window dimensions ──
    const unsigned int WIN_W = 1280, WIN_H = 720;

    // ── Color Palette (Modern Dark Theme) ──
    // Background and surface colors
    const sf::Color BG        {15, 23, 42};       // Deep navy background
    const sf::Color SIDEBAR   {30, 41, 59};       // Sidebar panel color
    const sf::Color CARD      {51, 65, 85};       // Card/content area background
    const sf::Color CARD_HOVER{71, 85,105};       // Card hover state
    const sf::Color INPUT_BG  {30, 41, 59};       // Text input background
    const sf::Color INPUT_BORDER{100,116,139};    // Input border (unfocused)
    const sf::Color INPUT_FOCUS {59,130,246};     // Input border (focused - blue)

    // Action colors for buttons and status indicators
    const sf::Color PRIMARY   {59,130,246};       // Primary action (blue)
    const sf::Color PRIMARY_HOVER{37,99,235};     // Primary hover state
    const sf::Color SUCCESS   {34,197, 94};       // Success/confirm (green)
    const sf::Color SUCCESS_HOVER{22,163, 74};    // Success hover state
    const sf::Color DANGER    {239, 68, 68};      // Danger/error (red)
    const sf::Color DANGER_HOVER{220, 38, 38};    // Danger hover state
    const sf::Color WARNING   {245,158, 11};      // Warning/caution (amber)
    const sf::Color ACCENT    {139, 92,246};      // Accent highlight (purple)

    // Text colors for different emphasis levels
    const sf::Color TEXT_PRIMARY  {248,250,252};  // Primary text (near-white)
    const sf::Color TEXT_SECONDARY{148,163,184};  // Secondary/subtle text
    const sf::Color TEXT_DARK     {30, 41, 59};   // Dark text on light backgrounds
    const sf::Color TEXT_MUTED    {100,116,139};  // Muted/placeholder text

    // Table and UI element colors
    const sf::Color TABLE_ROW_EVEN{30, 41, 59};  // Even row background
    const sf::Color TABLE_ROW_ODD {41, 55, 75};  // Odd row background (zebra stripe)
    const sf::Color TABLE_HEADER  {15, 23, 42};  // Table header background
    const sf::Color DIVIDER       {51, 65, 85};  // Divider line color
    const sf::Color SCROLLBAR     {71, 85,105};  // Scrollbar thumb color
    const sf::Color TOAST_BG      {30, 41, 59, 230};  // Toast notification background

    // ── Layout sizes ──
    const float SIDEBAR_W  = 220.f;    // Sidebar width in pixels
    const float HEADER_H   = 60.f;     // Top header height
    const float BTN_H      = 42.f;     // Standard button height
    const float INPUT_H    = 40.f;     // Text input field height
    const float PADDING    = 16.f;     // Standard padding
    const float RADIUS     = 8.f;      // Border radius for rounded elements

    // ── Typography sizes ──
    const unsigned FONT_SM = 14;       // Small text (labels, captions)
    const unsigned FONT_MD = 16;       // Medium text (body, inputs)
    const unsigned FONT_LG = 20;       // Large text (section titles)
    const unsigned FONT_XL = 28;       // Extra large (page titles)
    const unsigned FONT_TITLE = 36;    // Title size (unused currently)

    // ── Font paths (Windows system fonts) ──
    const std::string FONT_PATH      = "C:/Windows/Fonts/segoeui.ttf";
    const std::string FONT_BOLD_PATH = "C:/Windows/Fonts/segoeuib.ttf";
}

#endif

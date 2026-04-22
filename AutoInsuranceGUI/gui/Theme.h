#ifndef THEME_H
#define THEME_H
#include <SFML/Graphics.hpp>
#include <string>

namespace Theme {
    // ── Window ──
    const unsigned int WIN_W = 1280, WIN_H = 720;

    // ── Color Palette (Modern Dark) ──
    const sf::Color BG        {15, 23, 42};       // #0f172a
    const sf::Color SIDEBAR   {30, 41, 59};       // #1e293b
    const sf::Color CARD      {51, 65, 85};       // #334155
    const sf::Color CARD_HOVER{71, 85,105};       // #475569
    const sf::Color INPUT_BG  {30, 41, 59};
    const sf::Color INPUT_BORDER{100,116,139};    // #64748b
    const sf::Color INPUT_FOCUS {59,130,246};     // #3b82f6

    const sf::Color PRIMARY   {59,130,246};       // blue
    const sf::Color PRIMARY_HOVER{37,99,235};
    const sf::Color SUCCESS   {34,197, 94};       // green
    const sf::Color SUCCESS_HOVER{22,163, 74};
    const sf::Color DANGER    {239, 68, 68};      // red
    const sf::Color DANGER_HOVER{220, 38, 38};
    const sf::Color WARNING   {245,158, 11};      // amber
    const sf::Color ACCENT    {139, 92,246};      // purple

    const sf::Color TEXT_PRIMARY  {248,250,252};  // white-ish
    const sf::Color TEXT_SECONDARY{148,163,184};  // #94a3b8
    const sf::Color TEXT_DARK     {30, 41, 59};
    const sf::Color TEXT_MUTED    {100,116,139};

    const sf::Color TABLE_ROW_EVEN{30, 41, 59};
    const sf::Color TABLE_ROW_ODD {41, 55, 75};
    const sf::Color TABLE_HEADER  {15, 23, 42};
    const sf::Color DIVIDER       {51, 65, 85};
    const sf::Color SCROLLBAR     {71, 85,105};
    const sf::Color TOAST_BG      {30, 41, 59, 230};

    // ── Sizes ──
    const float SIDEBAR_W  = 220.f;
    const float HEADER_H   = 60.f;
    const float BTN_H      = 42.f;
    const float INPUT_H    = 40.f;
    const float PADDING    = 16.f;
    const float RADIUS     = 8.f;
    const unsigned FONT_SM = 14;
    const unsigned FONT_MD = 16;
    const unsigned FONT_LG = 20;
    const unsigned FONT_XL = 28;
    const unsigned FONT_TITLE = 36;

    // ── Font path ──
    const std::string FONT_PATH      = "C:/Windows/Fonts/segoeui.ttf";
    const std::string FONT_BOLD_PATH = "C:/Windows/Fonts/segoeuib.ttf";
}

#endif

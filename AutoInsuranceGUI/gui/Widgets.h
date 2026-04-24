/**
 * @file Widgets.h
 * @brief Reusable SFML-based UI components for the insurance GUI application.
 *
 * Contains the following widget classes:
 * - RoundedRect: Custom SFML Shape with rounded corners
 * - Label: Text display with optional styling
 * - Button: Clickable button with hover effects and callback support
 * - TextInput: Editable text field with focus, placeholder, and password mode
 * - Toast: Temporary notification popup with fade-out animation
 * - Panel: Background panel with rounded corners
 * - DataTable: Scrollable data table with headers, zebra striping, and clipping
 */

#ifndef WIDGETS_H
#define WIDGETS_H

#include <SFML/Graphics.hpp>
#include "Theme.h"
#include <functional>
#include <string>
#include <vector>
#include <optional>
#include <cmath>
#include <algorithm>
#include <cstdint>
#include <memory>

// ═══════════════════════════════════════════════
//  ROUNDED RECTANGLE — Custom Shape for modern UI elements
//  Generates corner points along circular arcs for smooth corners
// ═══════════════════════════════════════════════
class RoundedRect : public sf::Shape {
    sf::Vector2f m_size; float m_radius; unsigned m_corners;
public:
    // Constructor: size of rectangle, corner radius, points per corner arc
    RoundedRect(sf::Vector2f size={0,0}, float radius=8.f, unsigned corners=8)
        : m_size(size), m_radius(radius), m_corners(corners) { update(); }
    void setSize(sf::Vector2f s){ m_size=s; update(); }
    sf::Vector2f getSize() const { return m_size; }
    // Total points = 4 corners * points per corner
    std::size_t getPointCount() const override { return m_corners*4; }
    // Calculate point position along the rounded corner arc
    sf::Vector2f getPoint(std::size_t i) const override {
        unsigned corner = static_cast<unsigned>(i) / m_corners;
        float angle = static_cast<float>(i % m_corners) / static_cast<float>(m_corners) * 3.14159f/2.f;
        float cx, cy;
        // Select center point based on which corner (top-right, top-left, etc.)
        switch(corner){
            case 0: cx=m_size.x-m_radius; cy=m_radius; break;
            case 1: cx=m_radius; cy=m_radius; angle+=3.14159f/2.f; break;
            case 2: cx=m_radius; cy=m_size.y-m_radius; angle+=3.14159f; break;
            default:cx=m_size.x-m_radius; cy=m_size.y-m_radius; angle+=3.f*3.14159f/2.f; break;
        }
        return {cx + m_radius*std::cos(angle), cy - m_radius*std::sin(angle)};
    }
};

// ═══════════════════════════════════════════════
//  LABEL — Simple styled text display widget
//  Wraps sf::Text with optional lazy initialization
// ═══════════════════════════════════════════════
class Label {
    std::optional<sf::Text> m_text;  // Optional to allow default construction
public:
    Label()=default;
    // Create a label with font, content, size, and color
    Label(const sf::Font& font, const std::string& str, unsigned size=Theme::FONT_MD, sf::Color col=Theme::TEXT_PRIMARY){
        m_text.emplace(font, str, size); m_text->setFillColor(col);
    }
    void setString(const std::string& s){ if(m_text) m_text->setString(s); }
    void setPosition(sf::Vector2f p){ if(m_text) m_text->setPosition(p); }
    void setColor(sf::Color c){ if(m_text) m_text->setFillColor(c); }
    sf::FloatRect getBounds() const { return m_text ? m_text->getGlobalBounds() : sf::FloatRect(); }
    void draw(sf::RenderWindow& w) const { if(m_text) w.draw(*m_text); }
};

// ═══════════════════════════════════════════════
//  BUTTON — Interactive clickable button with hover effect
//  Supports callback functions and automatic text centering
// ═══════════════════════════════════════════════
class Button {
    RoundedRect m_bg;                          // Button background shape
    std::optional<sf::Text> m_text;            // Button label text
    sf::Color m_color{Theme::PRIMARY}, m_hoverColor{Theme::PRIMARY_HOVER};
    bool m_hovered=false, m_visible=true;
    std::function<void()> m_callback;          // Click handler function
    sf::Vector2f m_pos, m_size;
    // Center the text label within the button bounds
    void centerText(){
        if(!m_text) return;
        auto tb=m_text->getGlobalBounds();
        m_text->setPosition({m_pos.x+(m_size.x-tb.size.x)/2.f, m_pos.y+(m_size.y-tb.size.y)/2.f-4.f});
    }
public:
    Button()=default;
    // Create a button with label, position, size, and color scheme
    Button(const sf::Font& font, const std::string& label, sf::Vector2f pos, sf::Vector2f size,
           sf::Color color=Theme::PRIMARY, sf::Color hover=Theme::PRIMARY_HOVER)
        : m_bg(size, Theme::RADIUS), m_color(color), m_hoverColor(hover), m_pos(pos), m_size(size)
    {
        m_text.emplace(font, label, Theme::FONT_MD);
        m_text->setFillColor(Theme::TEXT_PRIMARY);
        m_bg.setPosition(pos); m_bg.setFillColor(color); centerText();
    }
    void setCallback(std::function<void()> cb){ m_callback=cb; }
    void setLabel(const std::string& s){ if(m_text){ m_text->setString(s); centerText(); } }
    void setPosition(sf::Vector2f p){ m_pos=p; m_bg.setPosition(p); centerText(); }
    // Check if a point is within the button's clickable area
    bool contains(sf::Vector2f mp) const { return mp.x>=m_pos.x&&mp.x<=m_pos.x+m_size.x&&mp.y>=m_pos.y&&mp.y<=m_pos.y+m_size.y; }
    // Handle mouse move (hover) and click events
    bool handleEvent(const sf::Event& event){
        if(!m_text) return false;
        if(auto* moved=event.getIf<sf::Event::MouseMoved>()){
            sf::Vector2f mp(static_cast<float>(moved->position.x), static_cast<float>(moved->position.y));
            m_hovered=contains(mp); m_bg.setFillColor(m_hovered?m_hoverColor:m_color); return false;
        }
        if(auto* click=event.getIf<sf::Event::MouseButtonPressed>()){
            if(click->button==sf::Mouse::Button::Left){
                sf::Vector2f mp(static_cast<float>(click->position.x), static_cast<float>(click->position.y));
                if(contains(mp)){ if(m_callback) m_callback(); return true; }
            }
        }
        return false;
    }
    void draw(sf::RenderWindow& w) const { if(!m_text) return; w.draw(m_bg); w.draw(*m_text); }
};

// ═══════════════════════════════════════════════
//  TEXT INPUT — Editable text field with focus management
//  Supports placeholder text, labels, password masking, and cursor blinking
// ═══════════════════════════════════════════════
class TextInput {
    RoundedRect m_bg, m_border;                // Background and border shapes
    std::optional<sf::Text> m_text, m_placeholder, m_labelText;
    std::string m_value;                       // Current input value
    sf::Vector2f m_pos, m_size;
    bool m_focused=false, m_isPassword=false;  // Focus state and password mode
    sf::RectangleShape m_cursor;               // Blinking cursor indicator
    sf::Clock m_cursorClock;                   // Timer for cursor blink animation
    // Update the displayed text (masks characters in password mode)
    void updateDisplay(){
        if(!m_text) return;
        m_text->setString(m_isPassword ? std::string(m_value.size(),'*') : m_value);
    }
public:
    TextInput()=default;
    // Create input with font, placeholder, position, size, label, and password flag
    TextInput(const sf::Font& font, const std::string& ph, sf::Vector2f pos, sf::Vector2f size,
              const std::string& label="", bool pwd=false)
        : m_bg(size,6.f), m_border({size.x+2,size.y+2},6.f), m_pos(pos), m_size(size), m_isPassword(pwd)
    {
        m_text.emplace(font,"",Theme::FONT_MD); m_text->setFillColor(Theme::TEXT_PRIMARY);
        m_text->setPosition({pos.x+10, pos.y+(size.y-Theme::FONT_MD)/2.f-4.f});
        m_placeholder.emplace(font,ph,Theme::FONT_MD); m_placeholder->setFillColor(Theme::TEXT_MUTED);
        m_placeholder->setPosition({pos.x+10, pos.y+(size.y-Theme::FONT_MD)/2.f-4.f});
        if(!label.empty()){ m_labelText.emplace(font,label,Theme::FONT_SM); m_labelText->setFillColor(Theme::TEXT_SECONDARY); m_labelText->setPosition({pos.x,pos.y-22.f}); }
        m_border.setPosition({pos.x-1,pos.y-1}); m_border.setFillColor(Theme::INPUT_BORDER);
        m_bg.setPosition(pos); m_bg.setFillColor(Theme::INPUT_BG);
        m_cursor.setSize({2.f,(float)(Theme::FONT_MD+4)}); m_cursor.setFillColor(Theme::TEXT_PRIMARY);
    }
    const std::string& getValue() const { return m_value; }
    void clear(){ m_value.clear(); updateDisplay(); }
    // Handle click (focus) and keyboard (typing) events
    void handleEvent(const sf::Event& event){
        if(!m_text) return;
        // Toggle focus on mouse click inside/outside input area
        if(auto* click=event.getIf<sf::Event::MouseButtonPressed>()){
            sf::Vector2f mp(static_cast<float>(click->position.x),static_cast<float>(click->position.y));
            m_focused=(mp.x>=m_pos.x&&mp.x<=m_pos.x+m_size.x&&mp.y>=m_pos.y&&mp.y<=m_pos.y+m_size.y);
            m_border.setFillColor(m_focused?Theme::INPUT_FOCUS:Theme::INPUT_BORDER);
        }
        // Process keyboard input when focused
        if(m_focused){
            if(auto* te=event.getIf<sf::Event::TextEntered>()){
                if(te->unicode=='\b'){ if(!m_value.empty()) m_value.pop_back(); }
                else if(te->unicode>=32&&te->unicode<127) m_value+=static_cast<char>(te->unicode);
                updateDisplay();
            }
        }
    }
    // Draw the input field with background, border, text/placeholder, and cursor
    void draw(sf::RenderWindow& w){
        if(!m_text) return;
        w.draw(m_border); w.draw(m_bg);
        if(m_labelText) w.draw(*m_labelText);
        if(m_value.empty()&&!m_focused) w.draw(*m_placeholder); else w.draw(*m_text);
        // Draw blinking cursor when focused (blinks every 0.5 seconds)
        if(m_focused && std::fmod(m_cursorClock.getElapsedTime().asSeconds(),1.f)<0.5f){
            auto tb=m_text->getGlobalBounds();
            m_cursor.setPosition({m_text->getPosition().x+tb.size.x+2.f, m_pos.y+(m_size.y-Theme::FONT_MD-4)/2.f});
            w.draw(m_cursor);
        }
    }
};

// ═══════════════════════════════════════════════
//  TOAST — Temporary notification popup with fade-out
//  Shows colored messages that auto-dismiss after a duration
// ═══════════════════════════════════════════════
class Toast {
    std::optional<sf::Text> m_text;
    RoundedRect m_bg;
    sf::Clock m_clock; float m_duration=3.f; bool m_active=false;
    sf::Color m_color;
    const sf::Font* m_font;
public:
    Toast()=default;
    Toast(const sf::Font& font): m_font(&font){ m_bg = RoundedRect({0,0}, 8.f); }
    // Display a toast notification with message, color, and duration
    void show(const std::string& msg, sf::Color color=Theme::SUCCESS, float dur=3.f){
        m_text.emplace(*m_font, msg, Theme::FONT_MD); m_text->setFillColor(Theme::TEXT_PRIMARY);
        m_color=color; m_duration=dur; m_active=true; m_clock.restart();
        auto tb=m_text->getGlobalBounds(); float w=tb.size.x+40.f;
        m_bg.setSize({w,44.f}); m_bg.setFillColor(color);
        m_bg.setPosition({(Theme::WIN_W-w)/2.f,20.f}); m_text->setPosition({(Theme::WIN_W-w)/2.f+20.f,30.f});
    }
    // Draw the toast with fade-out effect in the last 0.5 seconds
    void draw(sf::RenderWindow& w){
        if(!m_active||!m_text) return;
        if(m_clock.getElapsedTime().asSeconds()>m_duration){ m_active=false; return; }
        float rem=m_duration-m_clock.getElapsedTime().asSeconds();
        // Apply alpha fade in the last 0.5 seconds before dismissal
        std::uint8_t a=rem<0.5f?static_cast<std::uint8_t>(rem/0.5f*255):255;
        auto c=m_color; c.a=a; m_bg.setFillColor(c);
        auto tc=Theme::TEXT_PRIMARY; tc.a=a; m_text->setFillColor(tc);
        w.draw(m_bg); w.draw(*m_text);
    }
};

// ═══════════════════════════════════════════════
//  PANEL — Simple background rectangle with rounded corners
//  Used for sidebar, header, cards, and content areas
// ═══════════════════════════════════════════════
class Panel {
    RoundedRect m_bg;
public:
    Panel()=default;
    // Create a panel with position, size, and background color
    Panel(sf::Vector2f pos, sf::Vector2f size, sf::Color col=Theme::CARD)
        : m_bg(size, Theme::RADIUS) { m_bg.setPosition(pos); m_bg.setFillColor(col); }
    void draw(sf::RenderWindow& w) const { w.draw(m_bg); }
};

// ═══════════════════════════════════════════════
//  DATA TABLE — Scrollable table for displaying records
//  Features: column headers, zebra-striped rows, mouse wheel scrolling,
//  view clipping, and an auto-calculated scrollbar
// ═══════════════════════════════════════════════
class DataTable {
    const sf::Font* m_font=nullptr;
    sf::Vector2f m_pos, m_size;
    std::vector<std::string> m_headers;                    // Column header labels
    std::vector<std::vector<std::string>> m_rows;          // Row data
    float m_scrollOffset=0.f, m_rowH=36.f;                 // Scroll state and row height
    std::vector<float> m_colWidths;                        // Column width configuration
public:
    DataTable()=default;
    // Create a table with font, position, size, headers, and column widths
    DataTable(const sf::Font& font, sf::Vector2f pos, sf::Vector2f size,
              const std::vector<std::string>& headers, const std::vector<float>& colWidths)
        : m_font(&font), m_pos(pos), m_size(size), m_headers(headers), m_colWidths(colWidths){}
    // Replace all row data and reset scroll position
    void setRows(const std::vector<std::vector<std::string>>& rows){ m_rows=rows; m_scrollOffset=0; }
    // Handle mouse wheel scrolling within the table bounds
    void handleEvent(const sf::Event& event){
        if(auto* scroll=event.getIf<sf::Event::MouseWheelScrolled>()){
            sf::Vector2f mp(static_cast<float>(scroll->position.x),static_cast<float>(scroll->position.y));
            if(mp.x>=m_pos.x&&mp.x<=m_pos.x+m_size.x&&mp.y>=m_pos.y&&mp.y<=m_pos.y+m_size.y){
                m_scrollOffset-=scroll->delta*30.f;
                float maxS=std::max(0.f,(float)m_rows.size()*m_rowH-m_size.y+m_rowH+10.f);
                m_scrollOffset=std::clamp(m_scrollOffset,0.f,maxS);
            }
        }
    }
    // Render the table with header, rows, and scrollbar using view clipping
    void draw(sf::RenderWindow& w){
        if(!m_font) return;
        // Draw background
        RoundedRect bg(m_size,6.f); bg.setPosition(m_pos); bg.setFillColor(Theme::CARD); w.draw(bg);
        // Set up clipping view to prevent rows from overflowing
        sf::View oldView=w.getView();
        sf::FloatRect clipRect(m_pos,m_size);
        sf::View clipView(clipRect);
        clipView.setViewport({{m_pos.x/Theme::WIN_W,m_pos.y/Theme::WIN_H},{m_size.x/Theme::WIN_W,m_size.y/Theme::WIN_H}});
        w.setView(clipView);
        float y=m_pos.y-m_scrollOffset;
        // Draw header row with bold column names
        sf::RectangleShape hbg({m_size.x,m_rowH}); hbg.setPosition({m_pos.x,y}); hbg.setFillColor(Theme::TABLE_HEADER); w.draw(hbg);
        float x=m_pos.x+8.f;
        for(size_t i=0;i<m_headers.size();i++){
            sf::Text ht(*m_font,m_headers[i],Theme::FONT_SM); ht.setFillColor(Theme::TEXT_SECONDARY);
            ht.setPosition({x,y+10.f}); ht.setStyle(sf::Text::Bold); w.draw(ht);
            x+=(i<m_colWidths.size()?m_colWidths[i]:100.f);
        }
        y+=m_rowH;
        // Draw data rows with alternating background colors (zebra striping)
        for(size_t r=0;r<m_rows.size();r++){
            sf::RectangleShape rbg({m_size.x,m_rowH}); rbg.setPosition({m_pos.x,y});
            rbg.setFillColor(r%2==0?Theme::TABLE_ROW_EVEN:Theme::TABLE_ROW_ODD); w.draw(rbg);
            x=m_pos.x+8.f;
            for(size_t c=0;c<m_rows[r].size();c++){
                sf::Text ct(*m_font,m_rows[r][c],Theme::FONT_SM); ct.setFillColor(Theme::TEXT_PRIMARY);
                ct.setPosition({x,y+10.f}); w.draw(ct);
                x+=(c<m_colWidths.size()?m_colWidths[c]:100.f);
            }
            y+=m_rowH;
        }
        // Draw scrollbar if content exceeds visible area
        if((float)(m_rows.size()+1)*m_rowH>m_size.y){
            float total=(float)(m_rows.size()+1)*m_rowH;
            float barH=std::max(20.f,m_size.y*m_size.y/total);
            float barY=m_pos.y+m_scrollOffset/total*m_size.y;
            sf::RectangleShape sb({6.f,barH}); sb.setPosition({m_pos.x+m_size.x-8.f,barY}); sb.setFillColor(Theme::SCROLLBAR); w.draw(sb);
        }
        // Restore the original view after clipping
        w.setView(oldView);
    }
};

#endif

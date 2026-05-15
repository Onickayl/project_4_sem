#ifndef VIEW_HPP
#define VIEW_HPP

#include <SFML/Graphics.hpp>
#include "model.hpp"

// игра
class Game
{
private:
    sf::RenderWindow &window;

    const sf::Font &font;
    sf::Text weatherText;

    sf::RectangleShape trunk;
    sf::RectangleShape ground;

    static const int numBars = 4;
    sf::RectangleShape barFills[numBars];
    sf::RectangleShape barFrames[numBars];

public:
    Game(sf::RenderWindow &window, const sf::Font &font);

    void draw(const Tree &tree, const Weather &weather, bool manual, const std::string &climateName);
};

// меню и настроки
class MenuView
{
private:
    sf::RenderWindow &window;
    const sf::Font &font;

    // меню
    sf::Text title;
    sf::Text startBtn;
    sf::Text settingsBtn;
    sf::Text exitBtn;
    sf::Text *menuItems[3];

    // настройки
    sf::Text settingsTitle;
    sf::Text climateText;
    sf::Text expText;
    sf::Text speedText;
    sf::Text manualText;
    sf::Text backBtn;
    sf::Text *settingsItems[5];

public:
    MenuView(sf::RenderWindow &window, const sf::Font &font);

    void showMenu();
    void showSettings(const SettingsModel &settings);
    void showPause();

    // подсветка выбранного
    void highlightMenuItem(int index);
    void highlightSettingsItem(int index);
};

#endif
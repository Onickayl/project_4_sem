#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "model.hpp"
#include "view.hpp"
#include "data.hpp"
#include <SFML/Graphics.hpp>

enum class GameState
{
    Menu,    // глав меню
    Running, // симуляция
    Settings // настройки
};

class Controller
{
private:
    sf::RenderWindow window;
    sf::Clock deltaClock;
    sf::Font font;

    Data data;

    GameState state = GameState::Menu;

    // мрдели
    Tree tree;
    Weather weather;
    SettingsModel settings;

    // отрисовка
    Game gameView;
    MenuView menuView;

    // временные настройки для меню
    SettingsModel tempSettings;
    int selectedMenuItem = 0;
    int selectedSettingsItem = 0;

    // пауза
    bool paused = false;
    float pauseDeltaTime = 0.0f;

    bool manual = false;
    bool simWasActive = false;

    // методы
    void processEvents(); // обрабатывает события
    void render();        // отрисовка текущ сост

    void MenuInput(const sf::Event::KeyPressed &keyEvent);     // обработка нажатий в глав меню
    void SettingsInput(const sf::Event::KeyPressed &keyEvent); // обработка нажатий в настройках
    void GameInput(const sf::Event::KeyPressed &keyEvent);     // обработка нажатий в симуляции

    void updateGame(float dt);
    void applySettings(); // применение настроек
    void restartSim();

public:
    Controller();
    void run();

    // примение настроек из командной строки
    void applyCommandLineSettings(ClimateType climate, ExperimentType exp, bool manual, float simSpeed);
};

#endif
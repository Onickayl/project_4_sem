#include "view.hpp"
#include "branch.hpp"
#include "leaf.hpp"
#include "weather.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>

// игра
Game::Game(sf::RenderWindow &win, const sf::Font &fnt)
    : window(win), font(fnt), weatherText(fnt, "", 18)
{
    // ствол
    trunk.setSize({50, 500});
    trunk.setPosition({400, 200});
    trunk.setFillColor(sf::Color(139, 69, 19));

    // земля
    ground.setSize({800.0f, 50.0f});
    ground.setPosition({0, 650});
    ground.setFillColor(sf::Color(101, 67, 33));

    // текст погоды
    weatherText.setFillColor(sf::Color::White);
    weatherText.setPosition({10, 10});

    // прямоугольники прогресса сезона
    float barStartX = 10;
    float barStartY = 40;
    float barWidth = 10;
    float barHeight = 15;
    float barSpacing = 5;

    for (int i = 0; i < numBars; i++)
    {
        barFrames[i].setSize({barWidth, barHeight});
        barFrames[i].setPosition({barStartX + i * (barWidth + barSpacing), barStartY});
        barFrames[i].setFillColor(sf::Color::Transparent);
        barFrames[i].setOutlineColor(sf::Color(150, 150, 150));
        barFrames[i].setOutlineThickness(1);

        barFills[i].setSize({0, barHeight});
        barFills[i].setPosition({barStartX + i * (barWidth + barSpacing), barStartY});
    }
}

void Game::draw(const Tree &tree, const Weather &weather, bool manual, const std::string &climateName)
{
    window.draw(trunk);
    window.draw(ground);

    draw_Branch(window, const_cast<std::vector<Branch> &>(tree.getBranches()));
    draw_Leaves(window, tree.getLeaves());

    if (weather.getTemp() <= 0)
    {
        drawSnow(window, weather.getSnowflakes(), const_cast<std::vector<Precipitation> &>(weather.getGroundSnow()), 200, weather.getRain());
    }
    else
    {
        drawRain(window, weather.getRaindrops(), 200, weather.getRain());
    }

    // текст погоды
    std::ostringstream oss;
    if (!manual)
    {
        oss << weather.getSeason() << " | " << climateName << " | "
            << "Sun: " << static_cast<int>(weather.getSun()) << "%  "
            << "Temp: " << static_cast<int>(weather.getTemp()) << "C  "
            << "Rain: " << static_cast<int>(weather.getRain()) << "%  "
            << "Wind: " << static_cast<int>(weather.getWind()) << "%";
    }
    else
    {
        oss << "Sun: " << static_cast<int>(weather.getSun()) << "%  "
            << "Temp: " << static_cast<int>(weather.getTemp()) << "C  "
            << "Rain: " << static_cast<int>(weather.getRain()) << "%  "
            << "Wind: " << static_cast<int>(weather.getWind()) << "%";
    }
    weatherText.setString(oss.str());
    window.draw(weatherText);

    // прямоугольники прогресса
    if (!manual && !weather.getSeason().empty())
    {
        float phase = tree.getYearTime() / 365.0f;
        float seasonProgress = (phase * 4.0f) - std::floor(phase * 4.0f);

        int filledBars = static_cast<int>(seasonProgress * numBars);
        float partialBar = seasonProgress * numBars - filledBars;

        sf::Color seasonColor;

        const std::string &season = weather.getSeason();
        if (season == "Spring")
        {
            seasonColor = sf::Color::Green;
        }
        else if (season == "Summer")
        {
            seasonColor = sf::Color::Yellow;
        }
        else if (season == "Autumn")
        {
            seasonColor = sf::Color::Red;
        }
        else
        {
            seasonColor = sf::Color::Cyan;
        }

        for (int i = 0; i < numBars; i++)
        {
            if (i < filledBars)
            {
                barFills[i].setSize({10, 15});
            }
            else if (i == filledBars)
            {
                barFills[i].setSize({10 * partialBar, 15});
            }
            else
            {
                barFills[i].setSize({0, 15});
            }
            
            barFills[i].setFillColor(seasonColor);

            window.draw(barFrames[i]);
            window.draw(barFills[i]);
        }
    }
}

// меню
MenuView::MenuView(sf::RenderWindow &win, const sf::Font &fnt)
    : window(win), font(fnt), title(fnt, "Tree Simulation", 52), startBtn(fnt, "Start", 40), settingsBtn(fnt, "Settings", 40), exitBtn(fnt, "Exit", 40), settingsTitle(fnt, "Settings", 44), climateText(fnt, "Climate: Default", 28), expText(fnt, "Experiment: Normal", 28), speedText(fnt, "Speed: 1.00x", 28), manualText(fnt, "Manual mode: Off", 28), backBtn(fnt, "Back", 36)
{
    // заголовок
    title.setFillColor(sf::Color(237, 147, 0));
    title.setOutlineColor(sf::Color::Black);
    title.setOutlineThickness(2);
    title.setPosition({400 - 175, 80});

    // кнопки меню
    startBtn.setFillColor(sf::Color::White);
    startBtn.setPosition({400 - 45, 220});

    settingsBtn.setFillColor(sf::Color::White);
    settingsBtn.setPosition({400 - 70, 300});

    exitBtn.setFillColor(sf::Color::White);
    exitBtn.setPosition({400 - 35, 380});

    menuItems[0] = &startBtn;
    menuItems[1] = &settingsBtn;
    menuItems[2] = &exitBtn;

    // заголовок настроек
    settingsTitle.setFillColor(sf::Color(237, 147, 0));
    settingsTitle.setPosition({400 - 80, 60});

    climateText.setFillColor(sf::Color::White);
    climateText.setPosition({200, 160});

    expText.setFillColor(sf::Color::White);
    expText.setPosition({200, 230});

    speedText.setFillColor(sf::Color::White);
    speedText.setPosition({200, 300});

    manualText.setFillColor(sf::Color::White);
    manualText.setPosition({200, 370});

    backBtn.setFillColor(sf::Color::White);
    backBtn.setPosition({400 - 50, 500});

    settingsItems[0] = &climateText;
    settingsItems[1] = &expText;
    settingsItems[2] = &speedText;
    settingsItems[3] = &manualText;
    settingsItems[4] = &backBtn;
}

void MenuView::showMenu()
{
    window.draw(title);
    window.draw(startBtn);
    window.draw(settingsBtn);
    window.draw(exitBtn);
}

void MenuView::showSettings(const SettingsModel &settings)
{
    climateText.setString("Climate: " + settings.getClimateName());
    expText.setString("Experiment: " + settings.getExpName());

    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2) << settings.getSimSpeed();
    speedText.setString("Speed: " + ss.str() + "x");

    manualText.setString("Manual mode: " + std::string(settings.isManual() ? "On" : "Off"));

    window.draw(settingsTitle);
    window.draw(climateText);
    window.draw(expText);
    window.draw(speedText);
    window.draw(manualText);
    window.draw(backBtn);
}

void MenuView::showPause()
{
    sf::RectangleShape pauseBg(sf::Vector2f(300, 80));
    pauseBg.setFillColor(sf::Color(100, 150, 200, 220));
    pauseBg.setOutlineColor(sf::Color::White);
    pauseBg.setOutlineThickness(2);
    pauseBg.setPosition({400 - 150, 300 - 40});

    sf::Text pauseText(font, "PAUSED", 40);
    pauseText.setFillColor(sf::Color::Yellow);
    pauseText.setOutlineColor(sf::Color::Black);
    pauseText.setOutlineThickness(2);

    sf::FloatRect pb = pauseText.getLocalBounds();
    pauseText.setPosition({400 - pb.size.x / 2, 300 - pb.size.y / 2});

    window.draw(pauseBg);
    window.draw(pauseText);
}

void MenuView::highlightMenuItem(int index)
{
    for (int i = 0; i < 3; i++)
    {
        menuItems[i]->setFillColor(sf::Color::White);
    }

    if (index >= 0 && index < 3)
    {
        menuItems[index]->setFillColor(sf::Color(237, 147, 0));
    }
}

void MenuView::highlightSettingsItem(int index)
{
    for (int i = 0; i < 5; i++)
    {
        settingsItems[i]->setFillColor(sf::Color::White);
    }

    if (index >= 0 && index < 5)
    {
        settingsItems[index]->setFillColor(sf::Color(237, 147, 0));
    }
}
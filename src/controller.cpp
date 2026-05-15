#include "controller.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

Controller::Controller()
    : window(sf::VideoMode({800, 700}), "Tree Simulation"), gameView(window, font), menuView(window, font)
{
    // ограничиваем количество кадров в секунду (FPS) до 60
    window.setFramerateLimit(60);

    // загружаем шрифт
    if (!font.openFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf"))
    {
        std::cerr << "Failed to load font!" << std::endl;
    }

    tempSettings = settings;

    data.open(settings.getClimate(), settings.getExperiment(), settings.isManual());
}

void Controller::run()
{
    while (window.isOpen())
    {
        processEvents();

        // получаем время с прошлого кадра
        float dt = deltaClock.restart().asSeconds();

        updateGame(dt);
        render();

        window.display();
    }

    if (simWasActive)
    {
        data.close();
        std::cout << "Data saved to " << data.getWeatherFilename() << std::endl;
        std::cout << "Data saved to " << data.getLeafFilename() << std::endl;
    }
}

void Controller::processEvents()
{
    while (const auto event = window.pollEvent())
    {
        // клавиши
        if (const auto *keyEvent = event->getIf<sf::Event::KeyPressed>())
        {
            // esc - выход
            if (keyEvent->scancode == sf::Keyboard::Scan::Escape)
            {
                if (state == GameState::Running)
                {
                    state = GameState::Menu;
                }
                else if (state == GameState::Settings)
                {
                    // отменяем изменения и выходим
                    tempSettings = settings;
                    state = GameState::Menu;
                }
                else
                {
                    window.close();
                }

                continue; // обработали — дальше не идём
            }

            // передаём событие в зависимости от состояния
            switch (state)
            {
            case GameState::Menu:
                MenuInput(*keyEvent);
                break;
            case GameState::Settings:
                SettingsInput(*keyEvent);
                break;
            case GameState::Running:
                GameInput(*keyEvent);
                break;
            }
        }
    }
}

void Controller::MenuInput(const sf::Event::KeyPressed &keyEvent)
{
    if (keyEvent.scancode == sf::Keyboard::Scan::Up)
    {
        selectedMenuItem = (selectedMenuItem - 1 + 3) % 3;
    }
    else if (keyEvent.scancode == sf::Keyboard::Scan::Down)
    {
        selectedMenuItem = (selectedMenuItem + 1) % 3;
    }
    else if (keyEvent.scancode == sf::Keyboard::Scan::Enter)
    {
        switch (selectedMenuItem)
        {
        case 0:
            // Start
            restartSim();
            state = GameState::Running;
            break;
        case 1:
            // Settings
            tempSettings = settings;  // копируем текущие настройки
            selectedSettingsItem = 4; // выделяем Back
            state = GameState::Settings;
            break;
        case 2:
            // Exit
            window.close();
            break;
        }
    }
}

void Controller::SettingsInput(const sf::Event::KeyPressed &keyEvent)
{
    if (keyEvent.scancode == sf::Keyboard::Scan::Up)
    {
        selectedSettingsItem = (selectedSettingsItem - 1 + 5) % 5;
    }
    else if (keyEvent.scancode == sf::Keyboard::Scan::Down)
    {
        selectedSettingsItem = (selectedSettingsItem + 1) % 5;
    }
    else if (keyEvent.scancode == sf::Keyboard::Scan::Left)
    {
        switch (selectedSettingsItem)
        {
        case 0:
        {
            // climate
            int c = static_cast<int>(tempSettings.getClimate());
            c = (c - 1 + 7) % 7;
            tempSettings.setClimate(static_cast<ClimateType>(c));
            break;
        }
        case 1:
        {
            // experiment
            int e = static_cast<int>(tempSettings.getExperiment());
            e = (e - 1 + 3) % 3;
            tempSettings.setExperiment(static_cast<ExperimentType>(e));
            break;
        }
        case 2:
            // speed
            tempSettings.setSimSpeed(std::max(0.25f, tempSettings.getSimSpeed() - 0.25f));
            break;
        case 3:
            // manual
            tempSettings.setManual(!tempSettings.isManual());
            break;
        }
    }
    else if (keyEvent.scancode == sf::Keyboard::Scan::Right)
    {
        switch (selectedSettingsItem)
        {
        case 0:
        {
            // climate
            int c = static_cast<int>(tempSettings.getClimate());
            c = (c + 1) % 7;
            tempSettings.setClimate(static_cast<ClimateType>(c));
            break;
        }
        case 1:
        {
            // experiment
            int e = static_cast<int>(tempSettings.getExperiment());
            e = (e + 1) % 3;
            tempSettings.setExperiment(static_cast<ExperimentType>(e));
            break;
        }
        case 2:
            // speed
            tempSettings.setSimSpeed(std::min(3.0f, tempSettings.getSimSpeed() + 0.25f));
            break;
        case 3:
            // manual mode
            tempSettings.setManual(!tempSettings.isManual());
            break;
        }
    }
    else if (keyEvent.scancode == sf::Keyboard::Scan::Enter && selectedSettingsItem == 4)
    {
        // back — применяем настройки
        applySettings();
        state = GameState::Menu;
    }
}

void Controller::GameInput(const sf::Event::KeyPressed &keyEvent)
{
    if (keyEvent.scancode == sf::Keyboard::Scan::P)
    {
        paused = !paused;
        tree.setPaused(paused);
    }
    else if (keyEvent.scancode == sf::Keyboard::Scan::R)
    {
        restartSim();
    }
    else if (settings.isManual())
    {

        switch (keyEvent.scancode)
        {
        case sf::Keyboard::Scan::Num1:
            weather.setSun(weather.getSun() + 10);
            break;
        case sf::Keyboard::Scan::Num2:
            weather.setSun(weather.getSun() - 10);
            break;
        case sf::Keyboard::Scan::Num3:
            weather.setTemp(weather.getTemp() + 5);
            break;
        case sf::Keyboard::Scan::Num4:
            weather.setTemp(weather.getTemp() - 5);
            break;
        case sf::Keyboard::Scan::Num5:
            weather.setRain(weather.getRain() + 10);
            break;
        case sf::Keyboard::Scan::Num6:
            weather.setRain(weather.getRain() - 10);
            break;
        case sf::Keyboard::Scan::Num7:
            weather.setWind(weather.getWind() + 10);
            break;
        case sf::Keyboard::Scan::Num8:
            weather.setWind(weather.getWind() - 10);
            break;
        default:
            break;
        }
    }
}

void Controller::updateGame(float dt)
{
    if (state == GameState::Running)
    {
        if (paused || !tree.getIsRunning())
        {
            return;
        }

        float simSpeed = settings.getSimSpeed();
        float deltaTime = dt * simSpeed;

        if (deltaTime > 0.033f)
        {
            deltaTime = 0.033f;
        }

        if (!settings.isManual())
        {
            // автопогода
            weather.update(deltaTime, tree.getYearTime(), settings.getClimate(), settings.getExperiment());
        }
        else
        {

            tree.setYearTime(tree.getYearTime() + deltaTime * 10.0f);

            float phase = tree.getYearTime() / 365.0f;

            std::string season;

            if (phase < 0.25f)
            {
                season = "Spring";
            }
            else if (phase < 0.5f)
            {
                season = "Summer";
            }
            else if (phase < 0.75f)
            {
                season = "Autumn";
            }
            else
            {
                season = "Winter";
            }

            if (weather.getTemp() <= 0)
            {
                updateSnow(const_cast<std::vector<Precipitation> &>(weather.getSnowflakes()),
                           const_cast<std::vector<Precipitation> &>(weather.getGroundSnow()),
                           weather.getRain(), deltaTime);
            }
            else
            {
                updateRain(const_cast<std::vector<Precipitation> &>(weather.getRaindrops()),
                           weather.getRain(), deltaTime);
            }

            // обновляем почву
            float soilWater = weather.getSoilWater();
            soilWater += weather.getRain() * 0.01f * deltaTime * 10.0f;
            soilWater -= (weather.getSun() * 0.3f + weather.getTemp() * 0.5f) * 0.005f * deltaTime * 10.0f;
            soilWater = std::max(0.0f, std::min(100.0f, soilWater));
        }

        tree.update(deltaTime, weather, settings.isManual());

        // Запись CSV
        if (tree.getIsRunning())
        {
            data.recordDay(tree.getYearTime(), weather.getSeason(), weather.getTemp(), weather.getSun(), weather.getRain(),
                           weather.getWind(), weather.getSoilWater(), tree.getLeaves());
        }
    }
}

void Controller::render()
{
    window.clear(sf::Color(20, 30, 50));

    switch (state)
    {
    case GameState::Menu:

        menuView.showMenu();
        menuView.highlightMenuItem(selectedMenuItem);
        break;

    case GameState::Settings:

        menuView.showSettings(tempSettings);
        menuView.highlightSettingsItem(selectedSettingsItem);
        break;

    case GameState::Running:

        gameView.draw(tree, weather, settings.isManual(), settings.getClimateName());

        if (paused)
        {
            menuView.showPause();
        }
        break;
    }
}

void Controller::applySettings()
{
    settings = tempSettings;

    // пересоздаём CSV с новыми настройками
    data.open(settings.getClimate(), settings.getExperiment(), settings.isManual());
    simWasActive = false;
    restartSim();
}

void Controller::restartSim()
{
    tree.restart();
    weather = Weather(200, 200); // пересоздаём погоду
    paused = false;
    pauseDeltaTime = 0.0f;
    data.reset(); // сброс счётчика дней
    simWasActive = true;
}

void Controller::applyCommandLineSettings(ClimateType climate, ExperimentType exp, bool manual, float simSpeed)
{
    settings.setClimate(climate);
    settings.setExperiment(exp);
    settings.setManual(manual);
    settings.setSimSpeed(simSpeed);

    tempSettings = settings;

    // переоткрываем CSV с новыми настройками
    data.open(climate, exp, manual);
}
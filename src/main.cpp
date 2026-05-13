
#include <SFML/Graphics.hpp>
#include <math.h>
#include "branch.hpp"
#include "leaf.hpp"
#include "weather.hpp"
#include <iostream>
#include <filesystem>
#include <iomanip>   
#include <fstream>   
#include <sstream>

std::string getWeatherFilename(ClimateType climate, ExperimentType exp, bool manual);
std::string getLeafFilename(ClimateType climate, ExperimentType exp, bool manual);
std::string climateToString(ClimateType climate);


enum GameState 
{
    Menu,       // глав меню
    Running,    // симуляция
    Settings    // настройки
};

int main(int argc, char* argv[])
{

// ручной режим - да/нет
    bool manual = false;

// для паузы 
    bool paused = false;      // флаг паузы
    float pauseDeltaTime = 0; // накапливаем время в паузе

// режим эксперимента
    ExperimentType currentExp = ExperimentType::Normal;

// скорость симуляции
    float simSpeed = 1.0f;

// состояние игры 
    GameState gameState = Menu;
    
// аргументы командной строки
    if (argc >= 2)
    {
        for (int i = 1; i < argc; i++)
        {
            std::string arg = argv[i];
            if (arg == "--man")             manual = true;
            else if (arg == "--sim")        manual = false;
            else if (arg == "--eq")         currentClimate = ClimateType::Equatorial;
            else if (arg == "--trop")       currentClimate = ClimateType::Tropical;
            else if (arg == "--subtrop")    currentClimate = ClimateType::Subtropical;
            else if (arg == "--tempOc")     currentClimate = ClimateType::TemperateOceanic;
            else if (arg == "--tempCon")    currentClimate = ClimateType::TemperateContinental;
            else if (arg == "--subar")      currentClimate = ClimateType::Subarctic;
            else if (arg == "--drought")    currentExp = ExperimentType::Drought;
            else if (arg == "--warming")    currentExp = ExperimentType::GlobalWarming;
            else if (arg == "--normal")     currentExp = ExperimentType::Normal;
            else if (arg == "--speed")
            {
                simSpeed = std::atof(argv[i + 1]);
                if (simSpeed <= 0.0f) simSpeed = 1.0f;
                i++;
            }
            else
            {
                std::cout << "Usage: ./autumn [--eq|--trop] [--drought|--warming]" << std::endl;
                return 1;
            }
        }
    }
    
    srand(time(nullptr));
    
// окно
    sf::RenderWindow window(sf::VideoMode({800, 700}), "Tree Simulation");

// ограничиваем количество кадров в секунду (FPS) до 60
    window.setFramerateLimit(60);

    // секундомер, который измеряет время между кадрами, чтобы движение объектов не зависело от скорости компьютер
    sf::Clock deltaClock;
    //без него скорость падения листьев зависит от FPS
    
// про текст
    // создаём объект "шрифт"
    sf::Font font;
    font.openFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf");
    

// текст для меню
    sf::Text title(font, "Tree Simulation", 52);                // переменная(шрифт, сам текст, высота букв)
    title.setFillColor(sf::Color(237, 147, 0));                 // цвет текста (оранжевый)
    title.setOutlineColor(sf::Color::Black);                    // цвет обводки букв
    title.setOutlineThickness(2);                               // толщина обводки
    sf::FloatRect titleBounds = title.getLocalBounds();         // длина и высота текста в пикс
    title.setPosition({400 - titleBounds.size.x / 2, 80});      // позиция на экране
    
    sf::Text startBtn(font, "Start", 40);
    startBtn.setFillColor(sf::Color::White);
    sf::FloatRect startBounds = startBtn.getLocalBounds();
    startBtn.setPosition({400 - startBounds.size.x / 2, 220});
    
    sf::Text settingsBtn(font, "Settings", 40);
    settingsBtn.setFillColor(sf::Color::White);
    sf::FloatRect settingsBounds = settingsBtn.getLocalBounds();
    settingsBtn.setPosition({400 - settingsBounds.size.x / 2, 300});
    
    sf::Text exitBtn(font, "Exit", 40);
    exitBtn.setFillColor(sf::Color::White);
    sf::FloatRect exitBounds = exitBtn.getLocalBounds();
    exitBtn.setPosition({400 - exitBounds.size.x / 2, 380});
    
// номер выбранного пункта меню
    int selectedItem = 2;

// массив адресов кнопок меню
    sf::Text* MenuItems[3] = {&startBtn, &settingsBtn, &exitBtn};
    
// текст для настроек
    sf::Text settingsTitle(font, "Settings", 44);
    settingsTitle.setFillColor(sf::Color(237, 147, 0));
    sf::FloatRect settingsTitleBounds = settingsTitle.getLocalBounds();
    settingsTitle.setPosition({400 - settingsTitleBounds.size.x / 2, 60});
    
    sf::Text climateText(font, "Climate: Default", 28);
    climateText.setFillColor(sf::Color::White);
    climateText.setPosition({200, 160});
    
    sf::Text expText(font, "Experiment: Normal", 28);
    expText.setFillColor(sf::Color::White);
    expText.setPosition({200, 230});
    
    sf::Text speedText(font, "Speed: 1.00x", 28);
    speedText.setFillColor(sf::Color::White);
    speedText.setPosition({200, 300});
    
    sf::Text manualText(font, "Manual mode: Off", 28);
    manualText.setFillColor(sf::Color::White);
    manualText.setPosition({200, 370});
    
    sf::Text backBtn(font, "Back", 36);
    backBtn.setFillColor(sf::Color::White);
    sf::FloatRect backBounds = backBtn.getLocalBounds();
    backBtn.setPosition({400 - backBounds.size.x / 2, 500});
    
// номер выбранного пункта в меню настроек
    int settingsSelected = 0;

// массив адресов кнопок настройки
    sf::Text* settingsItems[5] = {&climateText, &expText, &speedText, &manualText, &backBtn};
    
// текст погоды
    sf::Text weatherText(font);
    weatherText.setCharacterSize(18);
    weatherText.setFillColor(sf::Color::White);
    weatherText.setPosition({10, 10});


// ствол
    sf::RectangleShape trunk({50, 500});        // 50 вправо, 500 пикселей вниз
    trunk.setPosition({400, 200});
    trunk.setFillColor(sf::Color(139, 69, 19));
    
// земля
    sf::RectangleShape ground({800.0f, 50.0f});
    ground.setPosition({0, 650});
    ground.setFillColor(sf::Color(101, 67, 33));
    
// ветки
    size_t num_branch = 16;
    std::vector<Branch> branches;
    init_Branches(branches, num_branch);
    
// листья
    size_t num_leaf = 400;
    std::vector<Leaf> leaves;
    init_Leaves(leaves, branches, num_leaf);
    
// дождь
    size_t num_drops = 200;
    std::vector<Precipitation> raindrops;
    init_Raindrops(raindrops, num_drops);
    
// снег
    size_t num_flakes = 200;
    std::vector<Precipitation> snowflakes;
    std::vector<Precipitation> groundSnow;
    init_Snowdrops(snowflakes, num_flakes);
    

// прямоугольники прогресса сезона
    const int numBars = 4;                  // кол-во прямоугольников в сезоне
    sf::RectangleShape barFills[numBars];   // заполнение
    sf::RectangleShape barFrames[numBars];  // рамка
    float barStartX = 10;
    float barStartY = 40;
    float barWidth = 10;
    float barHeight = 15;
    float barSpacing = 5;   // расстояние между прямоугольниками
    
    for (int i = 0; i < numBars; i++)
    {
        // рамка
        barFrames[i].setSize({barWidth, barHeight});
        barFrames[i].setPosition({barStartX + i * (barWidth + barSpacing), barStartY});
        barFrames[i].setFillColor(sf::Color::Transparent);
        barFrames[i].setOutlineColor(sf::Color(150, 150, 150));
        barFrames[i].setOutlineThickness(1);

        // заполнение
        barFills[i].setSize({0, barHeight});
        barFills[i].setPosition({barStartX + i * (barWidth + barSpacing), barStartY});
    }


// временные настройки
    ClimateType tempClimate = currentClimate;
    ExperimentType tempExp = currentExp;
    float tempSpeed = simSpeed;
    bool tempManual = !manual;


// игровые дни
    float year_time = 0; // 0 - начало весны, 365 - конец зимы

// симуляция - только год длится
    bool isRunning = true;


// CSV файлы
    std::string weatherFilename = getWeatherFilename(currentClimate, currentExp, manual);
    std::ofstream weatherFile(weatherFilename);

    if (!weatherFile.is_open())
    {
        std::cerr << "Cannot open CSV file" << std::endl;
        return 1;
    }

    weatherFile << "Day,Season,Temp,Sun,Rain,Wind,SoilWater\n";
    
    std::string leafFilename = getLeafFilename(currentClimate, currentExp, manual);
    std::ofstream leafFile(leafFilename);

    if (!leafFile.is_open())
    {
        std::cerr << "Cannot open CSV file" << std::endl;
        return 1;
    }

    leafFile << "Day,Season,AvgWater,AvgSugar,AvgChlorophyll,AvgAnthocyanin\n";

    // последний день, кот записали
    float lastRecordedDay = -1.0f;
    


// главный цикл, программа крутится здесь, пока окно открыто
    while (window.isOpen())
    {

        // создаём переменную event, кот будет хранить информацию о том, что сделал пользователь (нажал на крестик, клавишу и т.д.)
        // Проверяем, есть ли какие-то события
        while (const auto event = window.pollEvent())
        {
            // проверка нажатия клавиши
            if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyEvent->scancode == sf::Keyboard::Scan::Escape)
                {
                    if (gameState == Running) 
                    {
                        gameState = Menu;
                    }
                    else if (gameState == Settings) 
                    {
                        gameState = Menu;
                    }
                    else 
                    {
                        window.close();
                    }
                }
                
                // меню
                if (gameState == Menu)
                {
                    if (keyEvent->scancode == sf::Keyboard::Scan::Up)
                    {
                        selectedItem = (selectedItem - 1 + 3) % 3;
                    }

                    if (keyEvent->scancode == sf::Keyboard::Scan::Down)
                    {
                        selectedItem = (selectedItem + 1) % 3;
                    }

                    if (keyEvent->scancode == sf::Keyboard::Scan::Enter)
                    {
                        if (selectedItem == 0)
                        {
                            gameState = Running;
                            year_time = 0.0f;
                            isRunning = true;
                            paused = false;
                            init_Leaves(leaves, branches, num_leaf);
                        }
                        else if (selectedItem == 1)
                        {
                            gameState = Settings;
                            tempClimate = currentClimate;
                            tempExp = currentExp;
                            tempSpeed = simSpeed;
                            tempManual = manual;
                            settingsSelected = 4;
                        }
                        else if (selectedItem == 2)
                        {
                            window.close();
                        }
                    }
                    
                    for (int i = 0; i < 3; i++)
                    {
                        MenuItems[i]->setFillColor(sf::Color::White);
                    }

                    MenuItems[selectedItem]->setFillColor(sf::Color(237, 147, 0));
                }
                
                // настройки
                else if (gameState == Settings)
                {
                    if (keyEvent->scancode == sf::Keyboard::Scan::Up)
                    {
                        settingsSelected = (settingsSelected - 1 + 5) % 5;
                    }
                    if (keyEvent->scancode == sf::Keyboard::Scan::Down)
                    {
                        settingsSelected = (settingsSelected + 1) % 5;
                    }
                    if (keyEvent->scancode == sf::Keyboard::Scan::Left)
                    {

                        switch (settingsSelected)
                        {
                            case 0:
                                tempClimate = static_cast<ClimateType>((static_cast<int>(tempClimate) - 1 + 7) % 7);
                                break;
                            case 1:
                                tempExp = static_cast<ExperimentType>((static_cast<int>(tempExp) - 1 + 3) % 3);
                                break;
                            case 2:
                                tempSpeed = std::max(0.25f, tempSpeed - 0.25f);
                                break;
                            case 3:
                                tempManual = !tempManual;
                                break;
                        }
                    }
                    if (keyEvent->scancode == sf::Keyboard::Scan::Right)
                    {
                        switch (settingsSelected)
                        {
                            case 0:
                                tempClimate = static_cast<ClimateType>((static_cast<int>(tempClimate) + 1) % 7);
                                break;
                            case 1:
                                tempExp = static_cast<ExperimentType>((static_cast<int>(tempExp) + 1) % 3);
                                break;
                            case 2:
                                tempSpeed = std::min(3.0f, tempSpeed + 0.25f);
                                break;
                            case 3:
                                tempManual = !tempManual;
                                break;
                        }
                    }

                    if (keyEvent->scancode == sf::Keyboard::Scan::Enter && settingsSelected == 4)
                    {
                        // применяе настройки перед выходом
                        currentClimate = tempClimate;
                        currentExp = tempExp;
                        simSpeed = tempSpeed;
                        manual = tempManual;

                        year_time = 0.0f;
                        isRunning = true;
                        lastRecordedDay = -1.0f;
                        init_Leaves(leaves, branches, num_leaf);

                        //std::cout << "Applied climate: " << climateToString(currentClimate) << std::endl;

                        // обновляем csv файлы с новыми настройками
                        weatherFile.close();
                        leafFile.close();

                        weatherFilename = getWeatherFilename(currentClimate, currentExp, manual);
                        leafFilename = getLeafFilename(currentClimate, currentExp, manual);

                        weatherFile.open(weatherFilename);
                        leafFile.open(leafFilename);

                        weatherFile << "Day,Season,Temp,Sun,Rain,Wind,SoilWater\n";
                        leafFile << "Day,Season,AvgWater,AvgSugar,AvgChlorophyll,AvgAnthocyanin\n";

                        lastRecordedDay = -1.0f; // сбросываем день записи

                        gameState = Menu;
                    }
                    
                    //текст настроек
                    const char* climateNames[] = {"Default", "Equatorial", "Tropical", "Subtropical", "Temperate Oceanic", "Temperate Continental", "Subarctic"};
                    climateText.setString(std::string("Climate: ") + climateNames[static_cast<int>(tempClimate)]);
                    
                    const char* expNames[] = {"Normal", "Drought", "Global Warming"};
                    expText.setString(std::string("Experiment: ") + expNames[static_cast<int>(tempExp)]);
 
                    std::stringstream ss;
                    ss << std::fixed << std::setprecision(2) << tempSpeed;
                    speedText.setString("Speed: " + ss.str() + "x");

                    manualText.setString(std::string("Manual mode: ") + (tempManual ? "On" : "Off"));
                    
                    for (int i = 0; i < 5; i++)
                    {
                        settingsItems[i]->setFillColor(sf::Color::White);
                    }

                    settingsItems[settingsSelected]->setFillColor(sf::Color(237, 147, 0));
                }
                
                // игра
                else if (gameState == Running)
                {
                
                    if (keyEvent->scancode == sf::Keyboard::Scan::P)
                    {
                        paused = !paused;
                    }
                    if (keyEvent->scancode == sf::Keyboard::Scan::R)
                    {
                        year_time = 0.0f;
                        isRunning = true;
                        paused = false;
                        lastRecordedDay = -1.0f;

                        // пересоздаём листья
                        init_Leaves(leaves, branches, num_leaf);               

                        // очищаем CSV и пишем заголовки заново
                        weatherFile.close();
                        weatherFile.open(weatherFilename);  
                        weatherFile << "Day,Season,Temp,Sun,Rain,Wind,SoilWater\n";

                        leafFile.close();
                        leafFile.open(leafFilename);  
                        leafFile << "Day,Season,AvgWater,AvgSugar,AvgChlorophyll,AvgAnthocyanin\n";


                    }
                    if (manual)
                    {
                        if (keyEvent->scancode == sf::Keyboard::Scan::Num1) sun = std::min(100.0f, sun + 10);
                        if (keyEvent->scancode == sf::Keyboard::Scan::Num2) sun = std::max(0.0f, sun - 10);
                        if (keyEvent->scancode == sf::Keyboard::Scan::Num3) temp = std::min(30.0f, temp + 5);
                        if (keyEvent->scancode == sf::Keyboard::Scan::Num4) temp = std::max(-10.0f, temp - 5);
                        if (keyEvent->scancode == sf::Keyboard::Scan::Num5) rain = std::min(100.0f, rain + 10);
                        if (keyEvent->scancode == sf::Keyboard::Scan::Num6) rain = std::max(0.0f, rain - 10);
                        if (keyEvent->scancode == sf::Keyboard::Scan::Num7) wind = std::min(100.0f, wind + 10);
                        if (keyEvent->scancode == sf::Keyboard::Scan::Num8) wind = std::max(0.0f, wind - 10);
                    }
                }
            }
        }

        
        // получаем время с прошлого кадра
        float dt = deltaClock.restart().asSeconds();
        /*
        .restart() - сбрасывает секундомер в 0 и возвращает, сколько времени прошло с прошлого сброса
        .asSeconds() - возвращает это время в секундах (как число с плавающей точкой)
        */

        if (gameState == Running)
        {

            float currentSpeed = paused ? 0.0f : simSpeed;

            // масштабируем время
            float deltaTime = dt * currentSpeed;

            // jграничиваем максимальный шаг (на случай зависаний, когда может быть резкий скачок листа)
            if (deltaTime > 0.033f) deltaTime = 0.033f;
            
            // если пауза — копим время, но не обновляем
            if (paused) 
            {
                pauseDeltaTime += deltaTime;
                deltaTime = 0.0f;               // зануляем, чтобы mature не копил изменения
            }

// погода
            std::string season;
            
            if (isRunning)
            {
                if (!manual)
                {
                    auto_weather(year_time, deltaTime, season, currentExp);
                }
                else
                {
                    year_time += deltaTime * 10.0f;
                }
                
                distributeWater(leaves, branches, soilWater, deltaTime);
                
                if (temp <= 0)
                {
                    updateSnow(snowflakes, groundSnow, rain, deltaTime);
                }
                else
                {
                    updateRain(raindrops, rain, deltaTime);
                }
                
                update_leaf(leaves, branches, deltaTime, season);
                
         // Записываем CSV каждый игровой день
                int currentDay = (int)year_time;

                if (currentDay > lastRecordedDay && currentDay <= 365)
                {
                    lastRecordedDay = currentDay;
                    
                    float totalWater = 0.0f;
                    float totalSuger = 0.0f;
                    float totalChloro = 0.0f;
                    float totalAnth = 0.0f;
                    int matureCount = 0;

                    for (const auto &leaf : leaves)
                    {
                        if (leaf.state == LeafState::Mature)
                        {
                            totalWater += leaf.water;
                            totalSuger += leaf.sugar;
                            totalChloro += leaf.chlorophyll;
                            totalAnth += leaf.anthocyanin;
                            matureCount++;
                        }
                    }
                    
                    float avgWater = matureCount > 0 ? totalWater / matureCount : 0;
                    float avgSuger = matureCount > 0 ? totalSuger / matureCount : 0;
                    float avgChloro = matureCount > 0 ? totalChloro / matureCount : 0;
                    float avgAnth = matureCount > 0 ? totalAnth / matureCount : 0;
                    
                    weatherFile << currentDay << "," << season << ","
                                << std::fixed << std::setprecision(1) << temp << ","
                                << std::fixed << std::setprecision(0) << sun << ","
                                << std::fixed << std::setprecision(0) << rain << ","
                                << std::fixed << std::setprecision(0) << wind << ","
                                << std::fixed << std::setprecision(1) << soilWater << "\n";
                    
                    leafFile << currentDay << "," << season << ","
                             << std::fixed << std::setprecision(1) << avgWater << ","
                             << std::fixed << std::setprecision(1) << avgSuger << ","
                             << std::fixed << std::setprecision(1) << avgChloro << ","
                             << std::fixed << std::setprecision(1) << avgAnth << "\n";
                }
                
                if (year_time >= 365.0f && !manual)
                {
                    isRunning = false;
                    std::cout << "The end." << std::endl;
                }
            }


        // прямоугольники прогресса сезона
            if (!manual && !season.empty())
            {
                sf::Color seasonColor;

                if (season == "Spring")         seasonColor = sf::Color::Green;
                else if (season == "Summer")    seasonColor = sf::Color::Yellow;
                else if (season == "Autumn")    seasonColor = sf::Color::Red;
                else                            seasonColor = sf::Color::Cyan;

                // прогресс внутри сезона
                float phase = year_time / 365.0f;
                float seasonProgress = (phase * 4.0f) - std::floor(phase * 4.0f);
                // floor() отбрасывает дробную часть - только целая часть

                // заполняем прямоугольники
                int filledBars = (int)(seasonProgress * numBars);         // сколько целых делений заполнено
                float partialBar = seasonProgress * numBars - filledBars; // дробная часть
                
                for (int i = 0; i < numBars; i++)
                {
                    if (i < filledBars)
                    {
                        // полностью заполнен
                        barFills[i].setSize({barWidth, barHeight});
                        barFills[i].setFillColor(seasonColor);
                    }
                    else if (i == filledBars)
                    {
                        // частично заполнен
                        barFills[i].setSize({barWidth * partialBar, barHeight});
                        barFills[i].setFillColor(seasonColor);
                    }
                    else
                    {
                        // пусто
                        barFills[i].setSize({0, barHeight});
                    }
                }
            }
            
        // текст погоды
            std::string text;

            if (!manual)
            {
                text = season + " | " + climateToString(currentClimate) + " | " +
                       "Sun: " + std::to_string((int)sun) + "%  " +
                       "Temp: " + std::to_string((int)temp) + "C  " +
                       "Rain: " + std::to_string((int)rain) + "%  " +
                       "Wind: " + std::to_string((int)wind) + "%";
            }
            else
            {
                text = "Sun: " + std::to_string((int)sun) + "%  " +
                       "Temp: " + std::to_string((int)temp) + "C  " +
                       "Rain: " + std::to_string((int)rain) + "%  " +
                       "Wind: " + std::to_string((int)wind) + "%";
            }
            weatherText.setString(text);
        }

// отрисовка

        // заливаем всё окно тёмно-синим цветом
        window.clear(sf::Color(20, 30, 50));
        
        if (gameState == Menu)
        {
            window.draw(title);
            window.draw(startBtn);
            window.draw(settingsBtn);
            window.draw(exitBtn);
        }
        else if (gameState == Settings)
        {
            window.draw(settingsTitle);
            window.draw(climateText);
            window.draw(expText);
            window.draw(speedText);
            window.draw(manualText);
            window.draw(backBtn);
        }
        else if (gameState == Running)
        {
            window.draw(trunk);
            window.draw(ground);
            draw_Branch(window, branches);
            draw_Leaves(window, leaves);
            
            if (temp <= 0)
            {
                drawSnow(window, snowflakes, groundSnow, num_flakes);
            }
            else
            {
                drawRain(window, raindrops, num_drops);
            }
            
            // рисуем текст
            window.draw(weatherText);
            
            // рисуем прямоугольники прогресса
            if (!manual)
            {
                for (int i = 0; i < numBars; i++)
                {
                    window.draw(barFrames[i]);
                    window.draw(barFills[i]);
                }
            }
            
            if (paused)
            {
                // голубой прямоугольник
                sf::RectangleShape pauseBg(sf::Vector2f(300, 80));
                pauseBg.setFillColor(sf::Color(100, 150, 200, 220));  // голубой, полупрозрачный
                pauseBg.setOutlineColor(sf::Color::White);
                pauseBg.setOutlineThickness(2);
                pauseBg.setPosition({400 - 150, 300 - 40});  // центрируем прямоугольник
                
                // текст паузы
                sf::Text pauseText(font, "PAUSED", 40);
                pauseText.setFillColor(sf::Color::Yellow);
                pauseText.setOutlineColor(sf::Color::Black);
                pauseText.setOutlineThickness(2);
                
                // центрируем текст внутри прямоугольника
                sf::FloatRect pauseBounds = pauseText.getLocalBounds();
                pauseText.setPosition({400 - pauseBounds.size.x / 2, 300 - pauseBounds.size.y / 2});
                
                // рисуем сначала прямоугольник, потом текст
                window.draw(pauseBg);
                window.draw(pauseText);
            }
        }
        
        // показываем всё, что нарисовали, на экране
        window.display();
    }
    
    weatherFile.close();
    leafFile.close();
    
    std::cout << "Data saved to " << weatherFilename << std::endl;
    std::cout << "Data saved to " << leafFilename << std::endl;
    
    return 0;
}


std::string getWeatherFilename(ClimateType climate, ExperimentType exp, bool manual)
{
    std::string mName = manual ? "man" : "sim";


    std::string cName;

    switch (climate) 
    {
        case ClimateType::Equatorial:               cName = "eq"; break;
        case ClimateType::Tropical:                 cName = "trop"; break;
        case ClimateType::Subtropical:              cName = "subtrop"; break;
        case ClimateType::TemperateOceanic:         cName = "tempOc"; break;
        case ClimateType::TemperateContinental:     cName = "tempCon"; break;
        case ClimateType::Subarctic:                cName = "subar"; break;
        default:                                    cName = "default";
    }


    std::string eName;

    switch (exp)
    {
        case ExperimentType::Normal:        eName = "normal"; break;
        case ExperimentType::Drought:       eName = "drought"; break;
        case ExperimentType::GlobalWarming: eName = "warming"; break;
        default:                            eName = "default";
    }


    return "weather_" + mName + "_" + cName + "_" + eName + ".txt";
}

std::string getLeafFilename(ClimateType climate, ExperimentType exp, bool manual)
{
    std::string mName = manual ? "man" : "sim";


    std::string cName;

    switch (climate) 
    {
        case ClimateType::Equatorial:               cName = "eq"; break;
        case ClimateType::Tropical:                 cName = "trop"; break;
        case ClimateType::Subtropical:              cName = "subtrop"; break;
        case ClimateType::TemperateOceanic:         cName = "tempOc"; break;
        case ClimateType::TemperateContinental:     cName = "tempCon"; break;
        case ClimateType::Subarctic:                cName = "subar"; break;
        default:                                    cName = "default";
    }


    std::string eName;

    switch (exp)
    {
        case ExperimentType::Normal:        eName = "normal"; break;
        case ExperimentType::Drought:       eName = "drought"; break;
        case ExperimentType::GlobalWarming: eName = "warming"; break;
        default:                            eName = "default";
    }


    return "leaf_" + mName + "_" + cName + "_" + eName + ".csv";
}

std::string climateToString(ClimateType climate)
{
    switch (climate)
    {
        case ClimateType::Equatorial:           return "Equatorial";
        case ClimateType::Tropical:             return "Tropical";
        case ClimateType::Subtropical:          return "Subtropical";
        case ClimateType::TemperateOceanic:     return "Temperate Oceanic";
        case ClimateType::TemperateContinental: return "Temperate Continental";
        case ClimateType::Subarctic:            return "Subarctic";
        case ClimateType::Default:              return "Default";
        default:                                return "Unknown";
    }
}
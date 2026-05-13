#include <SFML/Graphics.hpp>
#include <math.h>
#include "branch.hpp"
#include "leaf.hpp"
#include "weather.hpp"
#include <iostream>
#include <filesystem>
#include <iomanip>   
#include <fstream>   

std::string getFilename(ClimateType climate, ExperimentType exp, bool manual);
std::string climateToString(ClimateType climate);

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

                if (simSpeed <= 0.0f)
                {
                    simSpeed = 1.0f;
                }

                i++;
            }
            else
            {
                std::cout << "Please write: ./autumn [--eq|--trop] [--drought|--warming]" << std::endl;
                return 1;
            }
        }
    }
    
    srand(time(nullptr));

// Создаём окно 800x600 с названием "autumn". Переменная window — это окно.
    sf::RenderWindow window(sf::VideoMode(800, 700), "autumn");

    // ограничиваем количество кадров в секунду (FPS) до 60
    window.setFramerateLimit(60);

    // секундомер, который измеряет время между кадрами, чтобы движение объектов не зависело от скорости компьютер
    sf::Clock deltaClock;
    //без него скорость падения листьев зависит от FPS

// про текст
    // создаём объект "шрифт"
    sf::Font font;
    // загружаем конкретный файл со шрифтом
    font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf"); // это стандартный шрифт в Linux

    // создаёт сам текст, который будет отображаться на экране
    sf::Text weatherText;

    // привязывает шрифт к тексту - этот текст должен быть написан вот таким шрифтом font
    // без этого текст не отобразится
    weatherText.setFont(font);

    // размер букв в пикселях
    weatherText.setCharacterSize(20);

    // цвет текста - белый
    weatherText.setFillColor(sf::Color::White);

    // где будет текст на экране
    weatherText.setPosition(10, 10);

// прямоугольники прогресса сезона

    const int numBars = 4; // 4 деления на сезон
    sf::RectangleShape barFills[numBars];   // заполнение
    sf::RectangleShape barFrames[numBars];  // рамки

    float barStartX = 10;
    float barStartY = 40; // под текстом
    float barWidth = 10;
    float barHeight = 15;
    float barSpacing = 5;   // расстояние между прямоугольниками

    for (int i = 0; i < numBars; i++)
    {
        // рамка
        barFrames[i].setSize(sf::Vector2f(barWidth, barHeight));
        barFrames[i].setPosition(barStartX + i * (barWidth + barSpacing), barStartY);
        barFrames[i].setFillColor(sf::Color::Transparent);
        barFrames[i].setOutlineColor(sf::Color(150, 150, 150));     // цвет контура - серый
        barFrames[i].setOutlineThickness(1);                        // толщина контура в пикселях

        // заполнение
        barFills[i].setSize(sf::Vector2f(0, barHeight));            // ширина 0 - пустой
        barFills[i].setPosition(barStartX + i * (barWidth + barSpacing), barStartY);
    }


// ствол дерева
    sf::RectangleShape trunk(sf::Vector2f(50, 500)); // 50 вправо, 500 пикселей вниз
    trunk.setPosition(400, 200);                     // Начинаем с Y=200, чтобы 500 пикселей вниз закончились на Y=700
    trunk.setFillColor(sf::Color(139, 69, 19));

// земля
    sf::RectangleShape ground(sf::Vector2f(800.0f, 50.0f));
    ground.setPosition(0, 650);
    ground.setFillColor(sf::Color(101, 67, 33));

// инициализация веток
    size_t num_branch = 16;
    std::vector<Branch> branches;
    init_Branches(branches, num_branch);

// инициализация листьев
    size_t num_leaf = 400;
    std::vector<Leaf> leaves;
    init_Leaves(leaves, branches, num_leaf);

// инициализация капель
    size_t num_drops = 200;
    std::vector<Precipitation> raindrops;
    init_Raindrops(raindrops, num_drops);

// инициализация снежинок
    size_t num_flakes = 200;
    std::vector<Precipitation> snowflakes;
    std::vector<Precipitation> groundSnow;
    init_Snowdrops(snowflakes, num_flakes);

// игровые дни
    float year_time = 0; // 0 - начало весны, 365 - конец зимы

// симуляция - только год длится
    bool isRunning = true;


// для записи в CSV
    // Открываем CSV файл
    std::string csvFilename = getFilename(currentClimate, currentExp, manual);
    std::ofstream csvFile(csvFilename);

    if (!csvFile.is_open())
    {
        std::cerr << "Cannot open CSV file" << std::endl;
        return 1;
    }

    // Записываем заголовки
    csvFile << "Day,Season,Temp,Sun,Rain,Wind,SoilWater,AvgChlorophyll\n";
    float lastRecordedDay = -1.0f;


// Главный цикл. Программа крутится здесь, пока окно открыто
    while (window.isOpen())
    {
        sf::Event event; // Создаём переменную event, которая будет хранить информацию о том, что сделал пользователь (нажал на крестик, клавишу и т.д.).

        // Проверяем, есть ли какие-то события (например, пользователь нажал на крестик).
        while (window.pollEvent(event))
        {

            // Если событие — это нажатие клавиши Esc, то закрываем окно.
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
            {
                window.close();
            }

            // пауза - если нажать клавишу P
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P)
            {
                paused = !paused;
            }

            // рестарт симуляции - если нажать клавишу R
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R)
            {
                year_time = 0.0f;
                isRunning = true;
                paused = false;
                lastRecordedDay = -1.0f;

                // пересоздаём листья
                init_Leaves(leaves, branches, num_leaf);

               // очищаем CSV и пишем заголовки заново
                csvFile.close();
                csvFile.open(csvFilename);  
                csvFile << "Day,Season,Temp,Sun,Rain,Wind,SoilWater,AvgChlorophyll\n";

            }         

            // ручные клавиши
            if (event.type == sf::Event::KeyPressed && manual)
            {

                // солнце - клавиши 1 и 2
                if (event.key.code == sf::Keyboard::Num1)
                {
                    sun += 10;
                    if (sun > 100)
                    {
                        sun = 100;
                    }
                }
                if (event.key.code == sf::Keyboard::Num2)
                {
                    sun -= 10;
                    if (sun < 0)
                    {
                        sun = 0;
                    }
                }

                // темп - клавиши 3 и 4
                if (event.key.code == sf::Keyboard::Num3)
                {
                    temp += 5;
                    if (temp > 30)
                    {
                        temp = 30;
                    }
                }
                if (event.key.code == sf::Keyboard::Num4)
                {
                    temp -= 5;
                    if (temp < -10)
                    {
                        temp = -10;
                    }
                }

                // дождь - клавиши 5 и 6
                if (event.key.code == sf::Keyboard::Num5)
                {
                    rain += 10;
                    if (rain > 100)
                    {
                        rain = 100;
                    }
                }
                if (event.key.code == sf::Keyboard::Num6)
                {
                    rain -= 10;
                    if (rain < 0)
                    {
                        rain = 0;
                    }
                }

                // ветер - клавиши 7 и 8
                if (event.key.code == sf::Keyboard::Num7)
                {
                    wind += 10;
                    if (wind > 100)
                    {
                        wind = 100;
                    }
                }
                if (event.key.code == sf::Keyboard::Num8)
                {
                    wind -= 10;
                    if (wind < 0)
                    {
                        wind = 0;
                    }
                }
            }
        }

                
        // получаем время с прошлого кадра
        float rawDeltaTime = deltaClock.restart().asSeconds();
        /*
        .restart() - сбрасывает секундомер в 0 и возвращает, сколько времени прошло с прошлого сброса
        .asSeconds() - возвращает это время в секундах (как число с плавающей точкой)
        */


        float currentSpeed;

        if (paused)
        {
            currentSpeed = 0.0f;
        }
        else 
        {
            currentSpeed = simSpeed;
        }
        
        // масштабируем время
        float deltaTime = rawDeltaTime * currentSpeed;

        // Ограничиваем максимальный шаг (на случай зависаний, когда может быть резкий скачок листа)
        if (deltaTime > 0.033f)
        {
            deltaTime = 0.033f;
        }


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

            // обновление листьев
            update_leaf(leaves, branches, deltaTime, season);


    // Записываем CSV каждый игровой день
            int currentDay = (int)year_time;

            if (currentDay > lastRecordedDay && currentDay <= 365)
            {
                lastRecordedDay = currentDay;

                // Считаем листья по состояниям
                float totalChloro = 0.0f;
                int matureCount = 0;

                for (const auto &leaf : leaves)
                {
                    if (leaf.state == LeafState::Mature)
                    {
                        totalChloro += leaf.chlorophyll; 
                        matureCount++; 
                    }
                }

                float avgChloro = matureCount > 0 ? totalChloro / matureCount : 0.0f;


                // Записываем строку
                csvFile << currentDay << ","
                        << season << ","
                        << std::fixed << std::setprecision(1) << temp << ","
                        << std::fixed << std::setprecision(0) << sun << ","
                        << std::fixed << std::setprecision(0) << rain << ","
                        << std::fixed << std::setprecision(0) << wind << ","
                        << std::fixed << std::setprecision(1) << soilWater << ","
                        << std::fixed << std::setprecision(1) << avgChloro << "\n";
            }

            // Если прошел год (365 дней)
            if (year_time >= 365.0f && !manual)
            {
                isRunning = false;
                std::cout << "the end." << std::endl;
            }
        }

// прямоугольники прогресса сезона
        if (!manual)
        {
            sf::Color seasonColor;

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

            // прогресс внутри сезона
            float phase = year_time / 365.0f;
            float seasonProgress = (phase * 4.0f) - floor(phase * 4.0f);
            // floor() отбрасывает дробную часть - только целая часть

            // заполняем прямоугольники
            int filledBars = (int)(seasonProgress * numBars);         // сколько целых делений заполнено
            float partialBar = seasonProgress * numBars - filledBars; // дробная часть

            for (int i = 0; i < numBars; i++)
            {
                if (i < filledBars)
                {
                    // полностью заполнен
                    barFills[i].setSize(sf::Vector2f(barWidth, barHeight));
                    barFills[i].setFillColor(seasonColor);
                }
                else if (i == filledBars)
                {
                    // частично заполнен
                    barFills[i].setSize(sf::Vector2f(barWidth * partialBar, barHeight));
                    barFills[i].setFillColor(seasonColor);
                }
                else
                {
                    // пусто
                    barFills[i].setSize(sf::Vector2f(0, barHeight));
                }
            }
        }

        // Заливаем всё окно тёмно-синим цветом.
        window.clear(sf::Color(20, 30, 50)); // RGB: 20,30,50

        // рисуем
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


        std::string text;

        if (!manual)
        {
            // собираем строку
            text = season + " | " +
                   climateToString(currentClimate) + " | " +
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

        // устанавливаем текст, который мы собрали, в объект weatherText                   
        weatherText.setString(text);
        
        // рисуем текст
        window.draw(weatherText);

        if (!manual)
        {
            // Рисуем прямоугольники
            for (int i = 0; i < numBars; i++)
            {
                window.draw(barFrames[i]); // рамки
                window.draw(barFills[i]);  // заполнение
            }
        }

        // Показываем всё, что нарисовали, на экране
        window.display();
    }

    csvFile.close();
    std::cout << "Data saved to " << csvFilename << std::endl;

    return 0;
}


std::string getFilename(ClimateType climate, ExperimentType exp, bool manual)
{
    std::string mName;
    if (manual) 
    {
        mName = "man";
    }
    else 
    {
        mName = "sim";
    }

    std::string cName;
    switch (climate) 
    {
        case ClimateType::Equatorial:               cName = "equatorial"; break;
        case ClimateType::Tropical:                 cName = "tropical"; break;
        case ClimateType::Subtropical:              cName = "tropical"; break;
        case ClimateType::TemperateOceanic:         cName = "tropical"; break;
        case ClimateType::TemperateContinental:     cName = "tropical"; break;
        case ClimateType::Subarctic:                cName = "tropical"; break;
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
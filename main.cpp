#include <SFML/Graphics.hpp>
#include <math.h>
#include "branch.hpp"
#include "leaf.hpp"
#include "weather.hpp"
#include <iostream>
#include <filesystem>
#include <iomanip>   
#include <fstream>   

std::string getFilename(ExperimentType exp);

int main(int argc, char* argv[])
{

// ручной режим - да/нет
    bool manual = false;

// для паузы 
    bool paused = false;      // флаг паузы
    float pauseDeltaTime = 0; // накапливаем время в паузе

// режим
    ExperimentType currentExp = ExperimentType::Normal;


// аргументы командной строки
    if (argc >= 2)
    {
        std::string arg = argv[1]; 

        if (arg == "--normal")
        {
            currentExp = ExperimentType::Normal;
        }
        else if (arg == "--warming")
        {
            currentExp = ExperimentType::GlobalWarming;
        }
        else if (arg == "--drought")
        {
            currentExp = ExperimentType::Drought;
        }
        else
        {
            std::cout << "Please write: ./autumn [--normal|--drought|--warming]" << std::endl;
            return 1;
        }
    }
    
    srand(time(nullptr));

// Создаём окно 800x600 с названием "autumn". Переменная window — это окно.
    sf::RenderWindow window(sf::VideoMode(800, 600), "autumn");

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
    trunk.setPosition(400, 100);                     // Начинаем с Y=100, чтобы 500 пикселей вниз закончились на Y=600
    trunk.setFillColor(sf::Color(139, 69, 19));

// земля
    sf::RectangleShape ground(sf::Vector2f(800.0f, 50.0f));
    ground.setPosition(0, 550); 

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
    std::string csvFilename = getFilename(currentExp);
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

            if (event.type == sf::Event::KeyPressed && manual == true)
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
        float deltaTime = deltaClock.restart().asSeconds();
        /*
        .restart() - сбрасывает секундомер в 0 и возвращает, сколько времени прошло с прошлого сброса
        .asSeconds() - возвращает это время в секундах (как число с плавающей точкой)
        */

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

// авто-погода

        std::string season;

        if (isRunning)
        {
            auto_weather(year_time, deltaTime, season, currentExp);

            distributeWater(leaves, branches, soilWater, deltaTime);

            if (season == "Winter")
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
            if (year_time >= 365.0f)
            {
                isRunning = false;
                std::cout << "the end." << std::endl;
            }
        }

// прямоугольники прогресса сезона
        
        sf::Color seasonColor;

        if (season == "Spring")
        {
            seasonColor = sf::Color::Green; 
            ground.setFillColor(sf::Color(101, 67, 33)); 
        }
        else if (season == "Summer")
        {
            seasonColor = sf::Color::Yellow; 
            ground.setFillColor(sf::Color(34, 139, 34)); 
        }
        else if (season == "Autumn")
        {
            seasonColor = sf::Color::Red;
            ground.setFillColor(sf::Color(101, 67, 33)); 
        }
        else
        {
            seasonColor = sf::Color::Cyan; 
            ground.setFillColor(sf::Color(240, 248, 255)); 
        }

        // прогресс внутри сезона 
        float phase = year_time / 365.0f;
        float seasonProgress = (phase * 4.0f) - floor(phase * 4.0f); 
        // floor() отбрасывает дробную часть - только целая часть

        // заполняем прямоугольники
        int filledBars = (int)(seasonProgress * numBars);           // сколько целых делений заполнено
        float partialBar = seasonProgress * numBars - filledBars;   // дробная часть

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



        // Заливаем всё окно тёмно-синим цветом.
        window.clear(sf::Color(20, 30, 50)); // RGB: 20,30,50

        // рисуем
        window.draw(trunk);
        window.draw(ground);
        draw_Branch(window, branches);
        draw_Leaves(window, leaves);

        if (season == "Winter")
        {
            drawSnow(window, snowflakes, groundSnow, num_flakes);
        }
        else
        {
            drawRain(window, raindrops, num_drops);
        }
        

        // собираем строку
        std::string text = season + " | " + 
                            "Sun: " + std::to_string((int)sun) + "%  " +
                           "Temp: " + std::to_string((int)temp) + "C  " +
                           "Rain: " + std::to_string((int)rain) + "%  " +
                           "Wind: " + std::to_string((int)wind) + "%";

        // устанавливаем текст, который мы собрали, в объект weatherText                   
        weatherText.setString(text);
        
        // рисуем текст
        window.draw(weatherText);

        // Рисуем прямоугольники
        for (int i = 0; i < numBars; i++)
        {
            window.draw(barFrames[i]);      // рамки
            window.draw(barFills[i]);       // заполнение
        }


        // Показываем всё, что нарисовали, на экране
        window.display();
    }

    csvFile.close();
    std::cout << "Data saved to" << csvFilename << std::endl;

    return 0;
}


std::string getFilename(ExperimentType exp)
{
    switch (exp)
    {
        case ExperimentType::Normal:        return "weather_normal.txt";
        case ExperimentType::Drought:       return "weather_drought.txt";
        case ExperimentType::GlobalWarming: return "weather_warming.txt";
        default:                            return "weather_unknown.txt";
    }
}
#include <SFML/Graphics.hpp>
#include <math.h>
#include "branch.hpp"
#include "leaf.hpp"
#include "weather.hpp"
#include <iostream>
#include <filesystem>

int main()
{
    
    srand(time(nullptr));

    size_t num_leaf = 160;
    std::vector<Leaf> leaves;

    size_t num_branch = 16;
    std::vector<Branch> branches;

    // Создаём окно 800x600 с названием "autumn". Переменная window — это окно.
    sf::RenderWindow window(sf::VideoMode(800, 600), "autumn");

    // ограничиваем количество кадров в секунду (FPS) до 60
    window.setFramerateLimit(60);

    // секундомер, который измеряет время между кадрами, чтобы движение объектов не зависело от скорости компьютер
    sf::Clock deltaClock;
    //без него скорость падения листьев зависит от FPS

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



    // ствол дерева
    sf::RectangleShape trunk(sf::Vector2f(50, 500)); // 50 вправо, 500 пикселей вниз
    trunk.setPosition(400, 100);                     // Начинаем с Y=100, чтобы 500 пикселей вниз закончились на Y=600
    trunk.setFillColor(sf::Color(139, 69, 19));

    // инициализация веток
    init_Branches(branches, num_branch);

    // инициализация листьев
    init_Leaves(leaves, branches, num_leaf);

    float year_time = 0; // 0 - начало весны, 365 - конец зимы

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

            if (event.type == sf::Event::KeyPressed)
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

// авто-погода - начальная версия

        year_time += deltaTime * 10.0f; // 1 секунда реальности = 10 дней в игре

        std::string season;
        float t = 0;

        if (year_time > 365)
        {
            year_time = 0; // по новой
        }

// весна (0-90 дней)
        if (year_time < 90)
        {
            season = "Spring";
            t = year_time / 90.0f;
            temp = 0 + t * 15.0f;  // 0C -> 15C
            sun = 20 + t * 30.0f;  // 20% -> 50%
            rain = 30 + t * 40.0f; // 30% -> 70%
            wind = 10 + t * 30.0f; // 10% -> 40%
        }
// лето (90-180 дней)
        else if (year_time < 180)
        {
            season = "Summer";
            t = (year_time - 90) / 90.0f;
            temp = 15 + t * 10.0f; // 15C -> 25C
            sun = 50 + t * 30.0f;  // 50% -> 80%
            rain = 70 - t * 40.0f; // 70% -> 40%
            wind = 40 - t * 15.0f;  // 40% -> 25%
        }
// осень (180-270 дней)
        else if (year_time < 270)
        { 
            season = "Autumn";
            t = (year_time - 180) / 90.0f;
            temp = 25 - t * 20.0f; // 25C -> 5C
            sun = 80 - t * 30.0f;  // 80% -> 50%
            rain = 40 + t * 45.0f; // 40% -> 85%
            wind = 25 + t * 40.0f; // 25% -> 65%
        }
// зима (270-365 дней)
        else
        {
            season = "Winter";
            t = (year_time - 270) / 95.0f;
            temp = 5 - t * 15.0f; // 5C -> -10C
            sun = 50 - t * 30.0f;  // 50% -> 20%
            rain = 85 - t * 85.0f; // 85% -> 0%
            wind = 65 - t * 30.0f; // 65% -> 35%
        }


        // обновление листьев
        update_leaf(leaves, deltaTime);


        // Заливаем всё окно тёмно-синим цветом.
        window.clear(sf::Color(20, 30, 50)); // RGB: 20,30,50

        // рисуем
        window.draw(trunk);
        draw_Branch(window, branches);
        draw_Leaves(window, leaves);

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

        // Показываем всё, что нарисовали, на экране
        window.display();
    }

    return 0;
}

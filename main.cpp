#include <SFML/Graphics.hpp>
#include <math.h>
#include "branch.hpp"
#include "leaf.hpp"
#include "weather.hpp"
#include <iostream>
#include <filesystem>

int main()
{
    // g++ -std=c++17 main.cpp branch.cpp leaf.cpp weather.cpp -o autumn -lsfml-graphics -lsfml-window -lsfml-system && ./autumn

    srand(time(nullptr));

    size_t leaves_count = 160;
    Leaf leaves[160];

    size_t branch_count = 4;
    Branch branches[4];

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
    init_Branches(branches, branch_count);

    // инициализация листьев
    init_Leaves(leaves, leaves_count, branches, branch_count);


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

        // обновление листьев
        update_leaf(leaves, leaves_count);
        

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

        // листопад
        update_falling_leaves(leaves, leaves_count, deltaTime);

        // Заливаем всё окно тёмно-синим цветом.
        window.clear(sf::Color(20, 30, 50)); // RGB: 20,30,50

        // рисуем
        window.draw(trunk);
        draw_Branch(window, branches, branch_count);
        draw_Leaves(window, leaves, leaves_count);

        // собираем строку
        std::string text = "Sun: " + std::to_string((int)sun) + "%  " +
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

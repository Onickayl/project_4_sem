#include "leaf.hpp"
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <iostream>


// инициализация листьев
void init_Leaves(Leaf leaves[], size_t leaves_count, Branch branches[], size_t branch_count) 
{
    for (int i = 0; i < leaves_count; i++) 
    {
        // Выбираем случайную ветку
        int branchNum = rand() % branch_count;
        
        // Вычисляем позицию на ветке
        float t = (rand() % 100) / 100.0; // от 0 до 1

        // Находим направление ветки
        float dx = branches[branchNum].endX - branches[branchNum].startX;
        float dy = branches[branchNum].endY - branches[branchNum].startY;

        leaves[i].x = branches[branchNum].startX + t * dx;
        leaves[i].y = branches[branchNum].startY + t * dy;

        // Маленькое случайное смещение от -4 до 4
        leaves[i].x += (rand() % 9) - 4;
        leaves[i].y += (rand() % 9) - 4;

        leaves[i].chlorophyll = 100;
        leaves[i].carotenoids = 50;
        leaves[i].anthocyanin = 0;
        leaves[i].water = 100;
        leaves[i].sugar = 50;
        leaves[i].stickiness = 60 + (rand() % 40);
        leaves[i].is_alive = 1;

        // Запоминаем, на какой ветке висит
        leaves[i].branchIndex = branchNum;
        leaves[i].positionOnBranch = t;
        
        branches[branchNum].leafCount++;
    }
}

// Рисование листьев
void draw_Leaves(sf::RenderWindow &window, Leaf leaves[], size_t leaves_count)
{
    for (int i = 0; i < leaves_count; i++)
    {

        // Создаём кружок с радиусом 5 пикселей
        sf::CircleShape leafShape(5.0f);

        // Ставим в нужное место (например, x=400, y=300)
        leafShape.setPosition(leaves[i].x, leaves[i].y);

        /*
        leaf.setScale(1.8f, 0.6f);        // растянуть в овал
        leaf.setRotation(rand() % 360);   // случайный поворот
        */

        if (leaves[i].is_alive)
        {
            // Цвет для живых - меняется
            if (leaves[i].chlorophyll > 70)
            {
                leafShape.setFillColor(sf::Color::Green);
            }
            else if (leaves[i].chlorophyll > 40)
            {
                leafShape.setFillColor(sf::Color::Yellow);
            }
            else if (leaves[i].anthocyanin > 15 && sun > 40)
            {
                leafShape.setFillColor(sf::Color::Red);
            }
            else
            {
                leafShape.setFillColor(sf::Color::Yellow);
            }
        }
        else
        {
            // Цвет для мёртвых листьев - не меняется
            if (leaves[i].chlorophyll > 70)
            {
                leafShape.setFillColor(sf::Color::Green);
            }
            else if (leaves[i].chlorophyll > 40)
            {
                leafShape.setFillColor(sf::Color::Yellow);
            }
            else if (leaves[i].anthocyanin > 15 && sun > 40)
            {
                leafShape.setFillColor(sf::Color::Red);
            }
            else
            {
                leafShape.setFillColor(sf::Color::Yellow);
            }
        }

        window.draw(leafShape);
    }
}

void update_leaf(Leaf leaves[], size_t leaves_count)
{
/*
Уменьшает хлорофилл от солнца и холода
Увеличивает антоцианы при наличии сахара, холода и солнца
Каротиноиды всегда постоянны
Результат: листья меняют цвет в зависимости от погоды

Дождь увеличивает прилипчивость, ветер уменьшает
Если stickiness < 20% — лист падает
Результат: Листья отрываются и падают
*/

    for (int i = 0; i < leaves_count; i++)
    {
        if (leaves[i].is_alive)
        {
            // нормализация (0-1)
            float S = sun / 100.0f;          
            float T = (temp + 10.0f) / 40.0f; 
            if (T < 0) 
            {
                T = 0;
            }
            if (T > 1) 
            {
                T = 1;
            }
            float W = leaves[i].water / 100.0f;
            float Sugar = leaves[i].sugar / 100.0f;

// Хлорофилл (зеленый)
            
            float optimal_sun = 1.0f - 2.0f * (S - 0.5f) * (S - 0.5f);      // пик при 50%
            float optimal_temp = 1.0f - (T - 0.75f) * (T - 0.75f) * 3.0f;   // пик при 20°C

            leaves[i].chlorophyll = 100.0f * optimal_sun * optimal_temp * W;

// Антоцианы (красный)

            float cold_stress = exp(- (T * T) / 0.15f);
            float antho = 2.0f * S * cold_stress * Sugar * W;
            leaves[i].anthocyanin += antho;


// Прилипчивость
            if (wind < 80) 
            {
                // при слабом ветре прилипчивость почти не меняется
                leaves[i].stickiness -= wind * 0.002f;
            } else 
            {
                // при сильном ветре листья начинают срываться
                leaves[i].stickiness -= wind * 0.05f;
            }
            
            // дождь увеличивает прилипчивость
            leaves[i].stickiness += rain * 0.03f;
            
// Отрыв 
            if (leaves[i].stickiness < 20) 
            {
                leaves[i].is_alive = 0;
            }


// ограничиваем значения
            leaves[i].chlorophyll = std::max(0.0f, std::min(100.0f, leaves[i].chlorophyll));
            leaves[i].anthocyanin = std::max(0.0f, std::min(100.0f, leaves[i].anthocyanin));
            leaves[i].stickiness = std::max(0.0f, std::min(100.0f, leaves[i].stickiness));
                
        }
    }
}

void update_falling_leaves(Leaf leaves[], size_t leaves_count, float deltaTime)
{
    for (int i = 0; i < leaves_count; i++)
    {
        if (!leaves[i].is_alive && leaves[i].y < 590)
        {
            // увеличиваем координату 'y' на 15*deltaTime пикселя за кадр
            leaves[i].y += 15.0f * deltaTime;
            // медленнное падение 15 -> 8
            // быстрое падение 15 -> 50

            // Покачивание (20 пикселей в секунду)
            leaves[i].x += ((rand() % 5) - 2) * 20.0f * deltaTime;
            // случайно двигается -2 0 +2

            // Не выходим за границы
            if (leaves[i].x < 0)
            {
                leaves[i].x = 0;
            }
            if (leaves[i].x > 800)
            {
                leaves[i].x = 800;
            }
        }

        if (!leaves[i].is_alive && leaves[i].y >= 600)
        {
            leaves[i].y = 590;
        }
    }
}

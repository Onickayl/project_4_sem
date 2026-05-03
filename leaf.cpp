#include "leaf.hpp"
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <iostream>


// инициализация листьев
void init_Leaves(std::vector<Leaf>& leaves, Branch branches[], size_t branch_count, size_t num_leaf) 
{

    leaves.clear();  // очищаем, если там что-то было
    leaves.reserve(num_leaf); // выделяем память

    for (int i = 0; i < num_leaf; i++) 
    {
        Leaf leaf;
        // Выбираем случайную ветку
        int branchNum = rand() % branch_count;
        
        // Вычисляем позицию на ветке
        float t = (rand() % 100) / 100.0; // от 0 до 1

        // Находим направление ветки
        float dx = branches[branchNum].endX - branches[branchNum].startX;
        float dy = branches[branchNum].endY - branches[branchNum].startY;

        leaf.x = branches[branchNum].startX + t * dx;
        leaf.y = branches[branchNum].startY + t * dy;

        // Маленькое случайное смещение от -4 до 4
        leaf.x += (rand() % 9) - 4;
        leaf.y += (rand() % 9) - 4;

        leaf.chlorophyll = 100;
        leaf.carotenoids = 50;
        leaf.anthocyanin = 0;
        leaf.water = 100;
        leaf.sugar = 50;
        leaf.stickiness = 60 + (rand() % 40);
        leaf.is_alive = 1;

        // Запоминаем, на какой ветке висит
        leaf.branchIndex = branchNum;
        leaf.positionOnBranch = t;
        
        branches[branchNum].leafCount++;
        
        leaves.push_back(leaf);
    }

    
}

// Рисование листьев
void draw_Leaves(sf::RenderWindow &window, const std::vector<Leaf>& leaves)
{
    // Создаём кружок с радиусом 5 пикселей
    sf::CircleShape leafShape(5.0f);

    for (const auto& leaf : leaves) 
    {
        // Ставим в нужное место (например, x=400, y=300)
        leafShape.setPosition(leaf.x, leaf.y);

        /*
        leaf.setScale(1.8f, 0.6f);        // растянуть в овал
        leaf.setRotation(rand() % 360);   // случайный поворот
        */

        if (leaf.is_alive)
        {
            // Цвет для живых - меняется
            if (leaf.chlorophyll > 70)
            {
                leafShape.setFillColor(sf::Color::Green);
            }
            else if (leaf.chlorophyll > 40)
            {
                leafShape.setFillColor(sf::Color::Yellow);
            }
            else if (leaf.anthocyanin > 15 && sun > 40)
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
            if (leaf.chlorophyll > 70)
            {
                leafShape.setFillColor(sf::Color::Green);
            }
            else if (leaf.chlorophyll > 40)
            {
                leafShape.setFillColor(sf::Color::Yellow);
            }
            else if (leaf.anthocyanin > 15 && sun > 40)
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

void update_leaf(std::vector<Leaf>& leaves)
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

    for (auto& leaf : leaves) 
    {
        if (leaf.is_alive)
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
            float W = leaf.water / 100.0f;
            float Sugar = leaf.sugar / 100.0f;

// Хлорофилл (зеленый)
            
            float optimal_sun = 1.0f - 2.0f * (S - 0.5f) * (S - 0.5f);      // пик при 50%
            float optimal_temp = 1.0f - (T - 0.75f) * (T - 0.75f) * 3.0f;   // пик при 20°C

            leaf.chlorophyll = 100.0f * optimal_sun * optimal_temp * W;

// Антоцианы (красный)

            float cold_stress = exp(- (T * T) / 0.15f);
            float antho = 2.0f * S * cold_stress * Sugar * W;
            leaf.anthocyanin += antho;


// Прилипчивость
            if (wind < 80) 
            {
                // при слабом ветре прилипчивость почти не меняется
                leaf.stickiness -= wind * 0.002f;
            } else 
            {
                // при сильном ветре листья начинают срываться
                leaf.stickiness -= wind * 0.05f;
            }
            
            // дождь увеличивает прилипчивость
            leaf.stickiness += rain * 0.03f;
            
// Отрыв 
            if (leaf.stickiness < 20) 
            {
                leaf.is_alive = 0;
            }


// ограничиваем значения
            leaf.chlorophyll = std::max(0.0f, std::min(100.0f, leaf.chlorophyll));
            leaf.anthocyanin = std::max(0.0f, std::min(100.0f, leaf.anthocyanin));
            leaf.stickiness = std::max(0.0f, std::min(100.0f, leaf.stickiness));
                
        }
    }
}

void update_falling_leaves(std::vector<Leaf>& leaves, float deltaTime)
{
    for (auto& leaf : leaves) 
    {
        if (!leaf.is_alive && leaf.y < 590)
        {
            // увеличиваем координату 'y' на 15*deltaTime пикселя за кадр
            leaf.y += 15.0f * deltaTime;
            // медленнное падение 15 -> 8
            // быстрое падение 15 -> 50

            // Покачивание (20 пикселей в секунду)
            leaf.x += ((rand() % 5) - 2) * 20.0f * deltaTime;      // leaf.pos.x += sin(leaf.pos.y * 0.1f) * 20.0f * deltaTime; // Покачивание
            // случайно двигается -2 0 +2

            // Не выходим за границы
            if (leaf.x < 0)
            {
                leaf.x = 0;
            }
            if (leaf.x > 800)
            {
                leaf.x = 800;
            }
        }

        if (!leaf.is_alive && leaf.y >= 600)
        {
           // leaf.y = 590;
        }
    }
}

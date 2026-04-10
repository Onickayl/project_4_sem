#include "leaf.hpp"
#include <cstdlib>
#include <cmath>


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
        leaves[i].stickiness = 100;
        leaves[i].is_alive = 1;

        // Запоминаем, на какой ветке висит
        leaves[i].branchIndex = branchNum;
        leaves[i].positionOnBranch = t;
        
        branches[branchNum].leafCount++;
    }
}

// Рисование листьев
void draw_Leaves(sf::RenderWindow& window, Leaf leaves[], size_t leaves_count) 
{
    for (int i = 0; i < leaves_count; i++) 
    {
        if (leaves[i].is_alive) 
        {
            // Создаём кружок с радиусом 5 пикселей
            sf::CircleShape leafShape(5.0f);
            
            // Ставим в нужное место (например, x=400, y=300)
            leafShape.setPosition(leaves[i].x, leaves[i].y);

            /*
            leaf.setScale(1.8f, 0.6f);        // растянуть в овал
            leaf.setRotation(rand() % 360);   // случайный поворот
            */
            
            // Цвет зависит от хлорофилла
            if (leaves[i].chlorophyll > 70)
            {
                leafShape.setFillColor(sf::Color::Green);
            }
            else if (leaves[i].chlorophyll > 40)
            {
                leafShape.setFillColor(sf::Color::Yellow);
            }
            else
            {
                leafShape.setFillColor(sf::Color::Red);
            }
                
            window.draw(leafShape);
        }
    }
}

void update_leaf(Leaf leaves[], size_t leaves_count)
{
    /*
Уменьшает хлорофилл от солнца и холода
Увеличивает антоцианы при наличии сахара, холода и солнца
Каротиноиды всегда постоянны

Результат: листья меняют цвет в зависимости от погоды
    */

    for (int i = 0; i < leaves_count; i++)
    {
        if (leaves[i].is_alive)
        {
// Влияние солнца на хлорофилл
            // Яркое солнце разрушает хлорофилл
            if (sun > 70)
            {
                leaves[i].chlorophyll -= 0.5f; // медленное уменьшение
            }
            // Умеренное солнце + вода восстанавливают хлорофилл
            else if (sun > 30 && leaves[i].water > 50)
            {
                leaves[i].chlorophyll += 0.3f;
            }

// Влияние температуры
            // Холод ускоряет разрушение хлорофилла
            if (temp < 10)
            {
                leaves[i].chlorophyll -= 0.4f;
            }
            // Тепло замедляет разрушение
            else if (temp > 20)
            {
                leaves[i].chlorophyll += 0.2f;
            }

// Образование антоцианов (красный цвет)
            // Условия: холод + солнце + много сахара
            if (temp < 5 && sun > 50 && leaves[i].sugar > 60)
            {
                leaves[i].anthocyanin += 0.8f;
            }
            // Медленное накопление в обычных условиях
            else if (temp < 15 && sun > 40)
            {
                leaves[i].anthocyanin += 0.2f;
            }

// ограничиваем значения
            if (leaves[i].chlorophyll > 100)
            {
                leaves[i].chlorophyll = 100;
            }

            if (leaves[i].chlorophyll < 0)
            {
                leaves[i].chlorophyll = 0;
            }

            if (leaves[i].anthocyanin > 100)
            {
                leaves[i].anthocyanin = 100;
            }
                
            if (leaves[i].anthocyanin < 0)
            {
                leaves[i].anthocyanin = 0;
            }
                
        }
    }
}

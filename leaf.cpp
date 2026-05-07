#include "leaf.hpp"
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <iostream>


// инициализация листьев
void init_Leaves(std::vector<Leaf>& leaves, std::vector<Branch>& branches, size_t num_leaf) 
{

    leaves.clear();             // очищаем, если там что-то было
    leaves.reserve(num_leaf);   // выделяем память

    for (int i = 0; i < num_leaf; i++) 
    {
        Leaf leaf;
        // Выбираем случайную ветку
        int branchNum = rand() % branches.size();
        
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

    /*!!!!!!!!!поменять значения для почки!!!!!!!!!!!!!!!!!!!*/
        leaf.chlorophyll = 100;
        leaf.carotenoids = 50;
        leaf.anthocyanin = 0;
        leaf.water = 100;
        leaf.sugar = 50;
        leaf.stickiness = 60 + (rand() % 40);
        leaf.state = LeafState::Bud;

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
    // Создаём кружок с радиусом 1 пиксель - пустышка
    sf::CircleShape leafShape(1.0f);

    for (const auto& leaf : leaves) 
    {
        // устанавливаем нужный размер
        leafShape.setRadius(leaf.size);
        
        // Ставим в нужное место (например, x=400, y=300)
        leafShape.setPosition(leaf.x, leaf.y);


        if (leaf.state == LeafState::Dead || leaf.state == LeafState::Falling) 
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
        else 
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


        window.draw(leafShape);
    }
}



void update_leaf(std::vector<Leaf> &leaves, float deltaTime)
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

    for (auto &leaf : leaves)
    {
        switch (leaf.state)
        {
        case LeafState::Bud:

            // если тепло, почка начинает расти

            if (temp > 10.0f)
                leaf.state = LeafState::Growing;

            break;

        case LeafState::Growing:

            leaf.size += 2.0f * deltaTime; // лист увеличивается

            if (leaf.size >= 5.0f)
                leaf.state = LeafState::Mature;

            break;

        case LeafState::Mature:

            mature(leaf);

            break;

        case LeafState::Falling:

            leaf.y += 30.0f * deltaTime; // падает вниз

            leaf.x += sin(leaf.y * 0.1f) * 20.0f * deltaTime; // покачивание

            if (leaf.y >= 590.0f)
                leaf.state = LeafState::Dead;

            break;

        case LeafState::Dead:

            // ничего не делаем, просто лежит

            break;
        }
    }
}

void mature(Leaf &leaf)
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

    float optimal_sun = 1.0f - 2.0f * (S - 0.5f) * (S - 0.5f);    // пик при 50%
    float optimal_temp = 1.0f - (T - 0.75f) * (T - 0.75f) * 3.0f; // пик при 20°C

    leaf.chlorophyll = 100.0f * optimal_sun * optimal_temp * W;

// Антоцианы (красный)

    float cold_stress = exp(-(T * T) / 0.15f);
    float antho = 2.0f * S * cold_stress * Sugar * W;
    leaf.anthocyanin += antho;

// Прилипчивость
    if (wind < 80)
    {
        // при слабом ветре прилипчивость почти не меняется
        leaf.stickiness -= wind * 0.002f;
    }
    else
    {
        // при сильном ветре листья начинают срываться
        leaf.stickiness -= wind * 0.05f;
    }

    // дождь увеличивает прилипчивость
    leaf.stickiness += rain * 0.03f;

// Отрыв
    if (leaf.stickiness < 20.0f)
    {

        leaf.state = LeafState::Falling;

        // Фиксируем цвет, чтобы он не менялся в полете

        // (нужна функция GetCurrentColor на основе хлорофилла)
    }

// ограничиваем значения
    leaf.chlorophyll = std::max(0.0f, std::min(100.0f, leaf.chlorophyll));
    leaf.anthocyanin = std::max(0.0f, std::min(100.0f, leaf.anthocyanin));
    leaf.stickiness = std::max(0.0f, std::min(100.0f, leaf.stickiness));
}

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
        if (branches.empty()) return;
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
        leaf.water = 70 + (rand() % 30);        // 70-100
        leaf.sugar = 30 + (rand() % 40);        // 30-70
        leaf.stickiness = 60 + (rand() % 40);
        leaf.state = LeafState::Bud;

        leaf.current_color = sf::Color::Green;  // начальный цвет
        leaf.current_color.a = 255;             // полностью непрозрачный

        leaf.changeSpeed = 0.5f + (rand() % 500) / 100.0f;  // 0.5-5.5

        float normalizedY = (leaf.y - 100.0f) / 400.0f;
        normalizedY = std::max(0.0f, std::min(1.0f, normalizedY));
        leaf.lightModifier = exp(-normalizedY * 2.0f);

        leaf.wakeupTemp = 3.0f + (rand() % 5);  // 3-8°C

        leaf.growSpeed = 1.5f + (rand() % 200) / 100.0f;  // 1.5-3.5

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
    // создаём кружок с радиусом 1 пиксель - пустышка
    sf::CircleShape leafShape(1.0f);

    for (const auto& leaf : leaves) 
    {
        if (leaf.state == LeafState::Bud && leaf.size < 0.5f) 
        {
            // пропускаем маленького размера почки, чтобы сначала дерево было полностью голым
            continue; 
        }

        // устанавливаем нужный размер
        leafShape.setRadius(leaf.size);
        
        // Ставим в нужное место (например, x=400, y=300)
        leafShape.setPosition(leaf.x, leaf.y);


        if (leaf.state == LeafState::Dead || leaf.state == LeafState::Falling) 
        {
// Цвет для мёртвых листьев - не меняется
            leafShape.setFillColor(leaf.current_color);

        } 
        else 
        {
// Цвет для живых - меняется

            float greenLevel = leaf.chlorophyll / 100.0f;
            float redLevel = leaf.anthocyanin / 100.0f;

            sf::Color color;

            if (redLevel < 0.15f)
            {
                // нет красного — плавно зелёный -> жёлтый
                float t = 1.0f - greenLevel;
                color = lerpColor(sf::Color::Green, sf::Color::Yellow, t);
            }
            else
            {
                // Есть красный — смешиваем жёлтый и красный
                sf::Color baseColor;
                if (greenLevel > 0.3f)
                {
                    float t = (greenLevel - 0.3f) / 0.7f;
                    baseColor = lerpColor(sf::Color::Yellow, sf::Color::Green, t);
                }
                else
                {
                    baseColor = sf::Color::Yellow;
                }

                float t = redLevel;
                color = lerpColor(baseColor, sf::Color::Red, t);
            }

            leafShape.setFillColor(color);
        }

        window.draw(leafShape);
    }
}



void update_leaf(std::vector<Leaf> &leaves, std::vector<Branch>& branches, float deltaTime, std::string& season)
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

            if (temp > leaf.wakeupTemp) 
                leaf.state = LeafState::Growing;

            break;

        case LeafState::Growing:

            leaf.size += leaf.growSpeed * deltaTime; // лист увеличивается

            if (leaf.size >= 7.0f)
                leaf.state = LeafState::Mature;

            break;

        case LeafState::Mature:

            mature(leaf, branches, deltaTime);

            break;

        case LeafState::Falling:

            leaf.y += 100.0f * deltaTime; // падает вниз

            leaf.x += sin(leaf.y * 0.1f) * 20.0f * deltaTime; // покачивание

            if (leaf.y >= 590.0f)
                leaf.state = LeafState::Dead;

            break;

        case LeafState::Dead:

            // листья на земле постепенно исчезают зимой
            if (season == "Winter")
            {
                leaf.current_color.a -= 20.0f * deltaTime; // уменьшаем прозрачность
                if (leaf.current_color.a < 10)
                {
                    leaf.current_color.a = 0; // полностью прозрачный
                }
            }

            break;
        }
    }
}


void mature(Leaf &leaf, std::vector<Branch>& branches, float deltaTime)
{

// нормализация (0-1)
    float S = (sun / 100.0f) * leaf.lightModifier;
    float T = (temp + 10.0f) / 40.0f;
    if (T < 0) T = 0;
    if (T > 1) T = 1;

    float evaporation = 0.5f;
    leaf.water -= (S * leaf.lightModifier + T) * evaporation * deltaTime;
    
    float W = leaf.water / 100.0f;
    float Sugar = leaf.sugar / 100.0f;

/*


5.1 Лист потребляет воду из ветки
waterNeeded = leaf.maxWater - leaf.water (сколько нужно для восполнения)

waterReceived = branch.water * коэффициент_передачи * deltaTime / branch.leafCount

Вычесть waterReceived из branch.water

Прибавить к leaf.water

*/

// Хлорофилл (зеленый)


    float optimal_sun;
    if (S > 0.4f)
    {
        optimal_sun = 0.9f;
    }        
    else
    {
        float t = S / 0.4f;                     
        optimal_sun = t * t * (3.0f - 2.0f * t); 
    }

    float optimal_temp = 1.0f - (T - 0.75f) * (T - 0.75f) * 3.0f; // пик при 20°C

    float targetChlorophyll = 100.0f * std::max(0.0f, optimal_sun * optimal_temp * W);    
    leaf.chlorophyll += (targetChlorophyll - leaf.chlorophyll) * deltaTime * leaf.changeSpeed;

// Антоцианы (красный)

    float cold_stress;
    if (T < 0.4f) // ниже 6C
    {
        cold_stress = 1.0f + (0.4f - T) * 4.0f; // до 2.6 при -10C
    }
    else
    {
        cold_stress = 0.0f; // выше 6C — красный не нужен
    }

    float targetAntho = 100.0f * cold_stress * Sugar * 2.0f;
    targetAntho = std::min(100.0f, targetAntho);
    leaf.anthocyanin += (targetAntho - leaf.anthocyanin) * deltaTime * 1.5f; 

// Прилипчивость

/*
В mature():

Вода увеличивает прилипчивость:

leaf.stickiness += leaf.water * коэффициент_влажности

Влажный лист гибкий и крепкий

Но сильный дождь сбивает листья:

if (rain > 70) leaf.stickiness -= (rain - 70) * коэффициент_сбивания

Ливень механически отрывает листья

Ветер сушит лист:

leaf.water -= wind * коэффициент_сушки * deltaTime

Сухой лист становится хрупким

leaf.stickiness -= wind * коэффициент_ветра * (1.0 - leaf.water / 100.0f)

Сухой лист на ветру отрывается быстрее
*/

    // Ветер влияет всегда, но нелинейно
    if (wind > 10)
    {
        float windFactor = (wind - 10) / 90.0f;         // 0 - 1
        leaf.stickiness -= wind * 0.015f * windFactor;
    }
    else
    {
        leaf.stickiness -= wind * 0.002f; // очень слабое влияние
    }

    // дождь увеличивает прилипчивость
    leaf.stickiness += rain * 0.01f;

// Отрыв
    if (leaf.stickiness < 20.0f)
    {

        leaf.state = LeafState::Falling;

        // Сохраняем цвет в момент отрыва

            float greenLevel = leaf.chlorophyll / 100.0f;
            float redLevel = leaf.anthocyanin / 100.0f;


            if (redLevel < 0.15f)
            {
                // нет красного — плавно зелёный -> жёлтый
                float t = 1.0f - greenLevel;
                leaf.current_color = lerpColor(sf::Color::Green, sf::Color::Yellow, t);
            }
            else
            {
                // есть красный — смешиваем жёлтый и красный
                sf::Color baseColor;
                if (greenLevel > 0.3f)
                {
                    float t = (greenLevel - 0.3f) / 0.7f;
                    baseColor = lerpColor(sf::Color::Yellow, sf::Color::Green, t);
                }
                else
                {
                    baseColor = sf::Color::Yellow;
                }

                float t = redLevel;
                leaf.current_color = lerpColor(baseColor, sf::Color::Red, t);
            }
    }

// ограничиваем значения
    leaf.chlorophyll = std::max(0.0f, std::min(100.0f, leaf.chlorophyll));
    leaf.anthocyanin = std::max(0.0f, std::min(100.0f, leaf.anthocyanin));
    leaf.stickiness = std::max(0.0f, std::min(100.0f, leaf.stickiness));
}

void distributeWater(std::vector<Leaf>& leaves, std::vector<Branch>& branches, float& soilWater, float deltaTime)
{

    float totalWaterIntake = soilWater * deltaTime * 0.5f;
    totalWaterIntake = std::min(totalWaterIntake, soilWater);
    float totalPriority = 0.0f;

// приоритет
    for (auto &branch : branches)
    {
        float normalizedHeight = (branch.startY - 100.0f) / 400.0f;
        normalizedHeight = std::max(0.0f, std::min(1.0f, normalizedHeight));

        float priority = branch.leafCount * normalizedHeight * (1.0f - branch.water / 100.0f);
        totalPriority += priority;
    }

// вода веткам
    for (auto &branch : branches)
    {
        float normalizedHeight = (branch.startY - 100.0f) / 400.0f;
        normalizedHeight = std::max(0.0f, std::min(1.0f, normalizedHeight));

        float priority = branch.leafCount * normalizedHeight * (1.0f - branch.water / 100.0f);
        branch.water += totalWaterIntake * priority / totalPriority;

        if (branch.water > 100.0f)
            branch.water = 100.0f;
    }

    soilWater -= totalWaterIntake;

// вода листьям 
    for (auto& leaf : leaves)
    {
        //if (leaf.state != LeafState::Mature) continue;  
        
        float waterNeeded = 100.0f - leaf.water;
        float waterFromBranch = branches[leaf.branchIndex].water * 0.1f * deltaTime;

        waterFromBranch = std::min(waterFromBranch, waterNeeded);
        waterFromBranch = std::min(waterFromBranch, branches[leaf.branchIndex].water);
        
        leaf.water += waterFromBranch;
        branches[leaf.branchIndex].water -= waterFromBranch;
    }

}

// LERP для цветов
sf::Color lerpColor(const sf::Color& a, const sf::Color& b, float t)
{
    t = std::max(0.0f, std::min(1.0f, t));
    
    return sf::Color(
                        a.r + (b.r - a.r) * t,
                        a.g + (b.g - a.g) * t,
                        a.b + (b.b - a.b) * t
                    );
}


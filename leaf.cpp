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

        
        leaf.chlorophyll = 80 + (rand() % 20);      // 80-100
        leaf.carotenoids = 50;
        leaf.anthocyanin = 0 + (rand() % 3);        // 0-3
        leaf.water = 70 + (rand() % 30);            // 70-100
        leaf.sugar = 30 + (rand() % 40);            // 30-70
        leaf.stickiness = 90 + (rand() % 10);       // 90-100
        leaf.state = LeafState::Bud;

        leaf.current_color = sf::Color::Green;  // начальный цвет
        leaf.current_color.a = 255;             // полностью непрозрачный

        float normalizedY = (leaf.y - 100.0f) / 400.0f;
        normalizedY = std::max(0.0f, std::min(1.0f, normalizedY));
        leaf.lightModifier = exp(-normalizedY * 2.0f);

        leaf.wakeupTemp = 3.0f + (rand() % 5);  // 3-8°C

        leaf.growSpeed = 2.5f + (rand() % 200) / 100.0f;  // 2.5-3.5

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

            leafShape.setFillColor(getLeafColor(leaf)); 
        }

        window.draw(leafShape);
    }
}


void update_leaf(std::vector<Leaf> &leaves, std::vector<Branch>& branches, float deltaTime, std::string& season)
{

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

            leaf.size += leaf.growSpeed * deltaTime * 10.0f; // лист увеличивается

            if (leaf.size >= 7.0f)
                leaf.state = LeafState::Mature;

            break;

        case LeafState::Mature:

            mature(leaf, branches, deltaTime);

            break;

        case LeafState::Falling:

            leaf.y += 200.0f * deltaTime; // падает вниз

            leaf.x += sin(leaf.y * 0.1f) * 20.0f * deltaTime; // покачивание

            if (leaf.y >= 690.0f)
                leaf.state = LeafState::Dead;

            break;

        case LeafState::Dead:

            // листья на земле постепенно исчезают зимой
            if (season == "Winter")
            {
                leaf.current_color.a -= 20.0f * deltaTime; // уменьшаем прозрачность

                if (leaf.current_color.a < 30)
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

    float dt = deltaTime * 10.0f;      
    float lightLeaf = sun * leaf.lightModifier;
    
// испарение волы в листе
    if (temp > 25.0f)  
    {
        leaf.water -= 0.1f * (temp - 25.0f) * dt; 
    }
    else if (temp > 20.0f)
    {
        leaf.water -= 0.05f * (temp - 20.0f) * dt; 
    }


// прилипчивость   
    if (leaf.water < 25.0f)
    {
        leaf.stickiness -= 0.3f * dt;  
    }
    

// фотосинтез: нужна вода, свет и хлорофилл
    if (leaf.water > 1.0f && lightLeaf > 10.0f && leaf.chlorophyll > 1.0f)
    {
        leaf.water -= 0.1f * dt;
        leaf.sugar += 0.5f * dt;
        leaf.stickiness += 0.5f * dt;  
    }
    
    leaf.sugar -= 0.2f * dt;  // базовый метаболизм
    


// хлорофилл
    // холод — плохо
    if (temp < 10.0f)
    {
        leaf.chlorophyll -= 0.2f * (10.0f - temp) * dt;
    }
    
    // экстремальная жара — плохо
    if (lightLeaf > 85.0f && temp > 25.0f)
    {
        leaf.chlorophyll -= 0.2f * (lightLeaf - 75.0f) * dt;
    }
    
    // ид условия — хлорофилл восстанавливается
    if (temp > 10.0f && temp < 28.0f && lightLeaf > 15.0f && lightLeaf < 85.0f)
    {
        //leaf.chlorophyll += 0.2f * dt;
        if (leaf.anthocyanin < 10.0f)  // если красного мало 
        {
            leaf.chlorophyll += 0.2f * dt;
        }
    }

    
// антоциан
    if (temp < 12.0f && temp > -5.0f && leaf.sugar > 10.0f && leaf.water > 10.0f)
    {
        leaf.sugar -= 0.5f * dt;
        leaf.water -= 0.5f * dt;
        leaf.anthocyanin += 0.5f * dt;
    }
    
// прилипчивость
    if (temp < 5.0f)
    {
        leaf.stickiness -= 0.3f * (5.0f - temp) * dt;
    }
    leaf.stickiness -= wind * 0.003f * dt;
    leaf.stickiness += rain * 0.01f * dt;
    
// отрыв
    if (leaf.stickiness < 10.0f)
    {
        leaf.state = LeafState::Falling;
        leaf.current_color = getLeafColor(leaf);  
        leaf.current_color.a = 255;
    }
    
// ограничения
    leaf.chlorophyll  = std::max(0.0f, std::min(100.0f, leaf.chlorophyll));
    leaf.anthocyanin  = std::max(0.0f, std::min(100.0f, leaf.anthocyanin));
    leaf.carotenoids  = std::max(0.0f, std::min(100.0f, leaf.carotenoids));
    leaf.water        = std::max(0.0f, std::min(100.0f, leaf.water));
    leaf.sugar        = std::max(0.0f, std::min(100.0f, leaf.sugar));
    leaf.stickiness   = std::max(0.0f, std::min(100.0f, leaf.stickiness));
}

void distributeWater(std::vector<Leaf> &leaves, std::vector<Branch> &branches, float &soilWater, float deltaTime)
{
    float dt = deltaTime * 10.0f;

    float totalWaterIntake = soilWater * dt * 0.01f;            // это сколько пойдёт в дерево
    totalWaterIntake = std::min(totalWaterIntake, soilWater);
    soilWater -= totalWaterIntake;
    float totalPriority = 0.0f;    

// приоритет
    for (auto &branch : branches)
    {
        float normalizedHeight = (branch.startY - 200.0f) / 400.0f;
        normalizedHeight = std::max(0.0f, std::min(1.0f, normalizedHeight));

        float priority = branch.leafCount * normalizedHeight * (1.0f - branch.water / 100.0f);
        totalPriority += priority;
    }

// вода веткам
    for (auto &branch : branches)
    {
        float normalizedHeight = (branch.startY - 200.0f) / 400.0f;
        normalizedHeight = std::max(0.0f, std::min(1.0f, normalizedHeight));

        float priority = branch.leafCount * normalizedHeight * (1.0f - branch.water / 100.0f);
        branch.water += totalWaterIntake * priority / totalPriority;

        branch.water  = std::max(0.0f, std::min(100.0f, branch.water));
    }


// вода по листьям 
    for (auto& leaf : leaves)
    {
        if (leaf.state == LeafState::Falling) continue;  
        if (leaf.state == LeafState::Dead) continue;
        
        float waterNeeded = 100.0f - leaf.water;
        float waterFromBranch = (branches[leaf.branchIndex].water * 0.5f * dt) / branches[leaf.branchIndex].leafCount;

        waterFromBranch = std::min(waterFromBranch, waterNeeded);
        
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

sf::Color getLeafColor(const Leaf& leaf)
{
    float greenLevel    = leaf.chlorophyll / 100.0f;
    float redLevel      = leaf.anthocyanin / 100.0f;
    
    if (redLevel < 0.15f)
    {
        // зелёный -> жёлтый
        float t = 1.0f - greenLevel;
        return lerpColor(sf::Color::Green, sf::Color::Yellow, t);
    }
    else
    {
        if (greenLevel > 0.3f)
        {
            // много красного и зелёного
            float t = (greenLevel - 0.3f) / 0.7f;
            sf::Color base = lerpColor(sf::Color::Yellow, sf::Color::Green, t);
            return lerpColor(base, sf::Color::Red, redLevel);
        }
        else
        {
            // жёлтый -> красный
            return lerpColor(sf::Color::Yellow, sf::Color::Red, redLevel);
        }
    }
}
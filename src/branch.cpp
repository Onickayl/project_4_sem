#include "branch.hpp"
#include <cstdlib>
#include <cmath>


void init_Branches(std::vector<Branch>& branches, size_t num_branch)
{
    branches.clear();               // очищаем, если там что-то было
    branches.reserve(num_branch);   // выделяем память

    size_t leftCount = num_branch / 2;
    size_t rightCount = num_branch - leftCount;

    // левые ветки
    for (int i = 0; i < leftCount; i++) 
    {
        Branch branch;

        branch.startX = 400;
        branch.startY = 220 + rand() % 281;
        branch.endX = 250 + rand() % 101;
        branch.endY = branch.startY - 20 - rand() % 81;
        branch.width = 10 + rand() % 5;
        branch.leafCount = 0;
        branch.water = 50 + rand() % 30;

        branches.push_back(branch);
    }

    // правые ветки
    for (int i = 0; i < rightCount; i++) 
    {
        Branch branch;

        branch.startX = 450;
        branch.startY = 220 + rand() % 281;
        branch.endX = 500 + rand() % 101;
        branch.endY = branch.startY - 20 - rand() % 81;
        branch.width = 10 + rand() % 5;
        branch.leafCount = 0;
        branch.water = 50 + rand() % 30;

        branches.push_back(branch);
    }
}



void draw_Branch(sf::RenderWindow& window, std::vector<Branch>& branches)
{               
    for (const auto& branch : branches) 
    {
        float dx = branch.endX - branch.startX;
        float dy = branch.endY - branch.startY;
        float length = std::sqrt(dx*dx + dy*dy);
        

        // Вычисляем угол в градусах
        float angleRad = std::atan2(dy, dx);
        float angleDeg = angleRad * 180.0f / 3.1415926535f;

        float angle = 270.0f + angleDeg;


        sf::RectangleShape branch_shape({branch.width, length});

        branch_shape.setOrigin({branch.width / 2, 0});              // центр по ширине, верх по длине - точка привязки
        branch_shape.setPosition({branch.startX, branch.startY});
        
        branch_shape.setRotation(sf::degrees(angle));
        
        branch_shape.setFillColor(sf::Color(139, 69, 19));
        
        window.draw(branch_shape);
    }
}
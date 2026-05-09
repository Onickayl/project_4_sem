#include "branch.hpp"
#include <cstdlib>
#include <cmath>


void init_Branches(std::vector<Branch>& branches, size_t num_branch)
{

    // попробовала рандом

    branches.clear();               // очищаем, если там что-то было
    branches.reserve(num_branch);   // выделяем память

    size_t leftCount = num_branch / 2;
    size_t rightCount = num_branch - leftCount;

    // левые ветки
    for (int i = 0; i < leftCount; i++) 
    {
        Branch branch;

        branch.startX = 400;
        branch.startY = 120 + rand() % 281;
        branch.endX = 250 + rand() % 101;
        branch.endY = branch.startY - 20 - rand() % 81;
        branch.width = 10 + rand() % 5;
        branch.leafCount = 0;

        branches.push_back(branch);
    }

    // правые ветки
    for (int i = 0; i < rightCount; i++) 
    {
        Branch branch;

        branch.startX = 450;
        branch.startY = 120 + rand() % 281;
        branch.endX = 500 + rand() % 101;
        branch.endY = branch.startY - 20 - rand() % 81;
        branch.width = 10 + rand() % 5;
        branch.leafCount = 0;

        branches.push_back(branch);
    }

}


// Рисование ветки, растущей из точки на стволе
void draw_Branch(sf::RenderWindow& window, std::vector<Branch>& branches)
{               
    for (const auto& branch : branches) 
    {
        float dx = branch.endX - branch.startX;
        float dy = branch.endY - branch.startY;
        float length = sqrt(dx*dx + dy*dy);
        float angle = -1 * (atan2(dx, dy) * 180 / 3.14159);

        sf::RectangleShape branch_shape(sf::Vector2f(branch.width, length));


        // Важно! Сдвигаем точку привязки к тому месту, где ветка крепится к стволу
        // По умолчанию точка привязки (origin) — верхний левый угол.
        // Нам нужно, чтобы ветка росла из своего "корня", а не из середины.
        // Ставим origin в середину левого края (по толщине) и в самый верх (0 по длине)
        branch_shape.setOrigin(branch.width/2, 0);  // центр по ширине, верх по длине. Точка привязки


        branch_shape.setPosition(branch.startX, branch.startY);
        branch_shape.setRotation(angle);                   // от оси Y, что направлена вниз, и по часовой стрелке
        branch_shape.setFillColor(sf::Color(139, 69, 19));
        
        window.draw(branch_shape);

    }
    
}
#include "branch.hpp"
#include <cstdlib>
#include <cmath>


void init_Branches(Branch branches[], size_t branch_count)
{
    /*for (int i = 0; i < branch_count; i++) 
    {
        branches[i].startX = 400 + rand() % 50;
        branches[i].startY = 200 + rand() % 300;
        branches[i].endX = 380 + rand() % 90;
        branches[i].endY = 200 + rand() % 300;
        branches[i].width = 10 + rand() % 5;
        branches[i].angle = rand() % 360;
        branches[i].leafCount = 10;
        
    }*/ // ПОДУМАТЬ, МОЖЕТ РАЗДЕЛИТЬ ИХ НА ПРАВО ЛЕВО?

    // Левая нижняя ветка
    branches[0].startX = 400;
    branches[0].startY = 400;
    branches[0].endX = 300;
    branches[0].endY = 300;
    branches[0].width = 12;
    branches[0].leafCount = 0;
    
    // Правая нижняя ветка
    branches[1].startX = 450;
    branches[1].startY = 380;
    branches[1].endX = 500;
    branches[1].endY = 300;
    branches[1].width = 12;
    branches[1].leafCount = 0;
    
    // Левая верхняя ветка
    branches[2].startX = 400;
    branches[2].startY = 300;
    branches[2].endX = 280;
    branches[2].endY = 180;
    branches[2].width = 10;
    branches[2].leafCount = 0;
    
    // Правая верхняя ветка
    branches[3].startX = 450;
    branches[3].startY = 280;
    branches[3].endX = 520;
    branches[3].endY = 180;
    branches[3].width = 10;
    branches[3].leafCount = 0;
    
}


// Рисование ветки, растущей из точки на стволе
void draw_Branch(sf::RenderWindow& window, Branch branches[], size_t branch_count)
{               
    for (int i = 0; i < branch_count; i++) 
    {
        float dx = branches[i].endX - branches[i].startX;
        float dy = branches[i].endY - branches[i].startY;
        float length = sqrt(dx*dx + dy*dy);
        float angle = -1 * (atan2(dx, dy) * 180 / 3.14159);

        sf::RectangleShape branch_shape(sf::Vector2f(branches[i].width, length));


        // Важно! Сдвигаем точку привязки к тому месту, где ветка крепится к стволу
        // По умолчанию точка привязки (origin) — верхний левый угол.
        // Нам нужно, чтобы ветка росла из своего "корня", а не из середины.
        // Ставим origin в середину левого края (по толщине) и в самый верх (0 по длине)
        branch_shape.setOrigin(branches[i].width/2, 0);  // центр по ширине, верх по длине. Точка привязки


        branch_shape.setPosition(branches[i].startX, branches[i].startY);
        branch_shape.setRotation(angle);                   // от оси Y, что направлена вниз, и по часовой стрелке
        branch_shape.setFillColor(sf::Color(139, 69, 19));
        
        window.draw(branch_shape);

    }
    
}
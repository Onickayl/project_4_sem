#ifndef BRANCH_HPP
#define BRANCH_HPP

#include <SFML/Graphics.hpp>
#include "weather.hpp" 


struct Branch
{
    float startX, startY;   // откуда растёт (координаты начала)
    float endX, endY;       // куда растёт (координаты конца)
    float width;            // толщина ветки (для отрисовки)
    float angle;            // угол наклона
    int leafCount;          // сколько листьев на этой ветке
};

void init_Branches(Branch branches[], size_t branch_count);
void draw_Branch(sf::RenderWindow& window, Branch branches[], size_t branch_count);


#endif
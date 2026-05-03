#ifndef LEAF_HPP
#define LEAF_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include "branch.hpp"
#include "weather.hpp" 


struct Leaf
{
    float x;                // координата x
    float y;                // координата y

    float water;            // кол-во воды          0-100%
    float sugar;            // кол-во сахара        0-100%

    float chlorophyll;      // кол-во хлорофилла    0-100% (зелёный)
    float carotenoids;      // кол-во каротиноидов  0-100% (жёлтый)
    float anthocyanin;      // кол-во антоциан      0-100% (красный)
    float stickiness;       // прилипчивость        0-100%

    int is_alive;           // жив - 1, мёртв - 0

    int branchIndex;        // номер ветки, на которой висит лист
    float positionOnBranch; // 0-1, где именно на ветке
};

void init_Leaves(std::vector<Leaf>& leaves, Branch branches[], size_t branch_count, size_t num_leaf);
void draw_Leaves(sf::RenderWindow& window, const std::vector<Leaf>& leaves);
void update_leaf(std::vector<Leaf>& leaves);
void update_falling_leaves(std::vector<Leaf>& leaves, float deltaTime);

#endif
#ifndef LEAF_HPP
#define LEAF_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include "branch.hpp"
#include "weather.hpp" 

enum class LeafState 
{
   Bud,      // почка (весна)
   Growing,  // растет
   Mature,   // взрослый лист (лето)
   Falling,  // падает (осень)
   Dead      // лежит на земле
};


struct Leaf
{
    float x;                // координата x
    float y;                // координата y

    float size = 0.0f;      // текущий размер (будет расти весной)
    LeafState state;        // статус листа (в каком состоянии сейчас находится)

    float water;            // кол-во воды          0-100%
    float sugar;            // кол-во сахара        0-100%

    float chlorophyll;      // кол-во хлорофилла    0-100% (зелёный)
    float carotenoids;      // кол-во каротиноидов  0-100% (жёлтый)
    float anthocyanin;      // кол-во антоциан      0-100% (красный)
    float stickiness;       // прилипчивость        0-100%

    sf::Color current_color;

    float lightModifier;    // индивидуальный коэффициент освещенности (зависит от того, как высоко висит лист).
    float wakeupTemp;       // индивидуальная температура появления почки
    float growSpeed;        // индивидуальная скорость роста почки в лист

    int branchIndex;        // номер ветки, на которой висит лист
    float positionOnBranch; // 0-1, где именно на ветке
};

void init_Leaves(std::vector<Leaf>& leaves, std::vector<Branch>& branches, size_t num_leaf);
void draw_Leaves(sf::RenderWindow& window, const std::vector<Leaf>& leaves);
void update_leaf(std::vector<Leaf>& leaves, std::vector<Branch>& branches, float deltaTime, std::string& season);
void mature(Leaf &leaf, std::vector<Branch>& branches, float deltaTime);
void distributeWater(std::vector<Leaf>& leaves, std::vector<Branch>& branches, float& soilWater, float deltaTime);
sf::Color lerpColor(const sf::Color& a, const sf::Color& b, float t);
sf::Color getLeafColor(const Leaf& leaf);

#endif
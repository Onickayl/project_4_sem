#ifndef WEATHER_HPP
#define WEATHER_HPP

#include <string>
#include <iostream>
#include <cmath>
#include <vector>
#include <SFML/Graphics.hpp>


// объявление глобальных переменных (extern — значит "определены в другом месте")
extern float sun;
extern float temp;
extern float rain;
extern float wind;
extern float soilWater;

struct Raindrop
{
    float x, y;       // позиция
    float speed;      // скорость падения
    float length;     // длина капли (чёрточка, а не точка)

};

void auto_weather(float& year_time, float deltaTime, std::string& season);

void init_Raindrops(std::vector<Raindrop> raindrops, size_t num_drops);
void updateRain(std::vector<Raindrop>& drops, float rainIntensity, float deltaTime);
void drawRain(sf::RenderWindow& window, const std::vector<Raindrop>& drops, size_t num_drops);



#endif
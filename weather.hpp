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

enum class ExperimentType
{
    Normal,             // нормальный климат
    Drought,            // засуха
    GlobalWarming       // глобальное потепление
};

struct Precipitation
{
    float x, y;       // позиция
    float speed;      // скорость падения
    float length;     // размер

};

void auto_weather(float& year_time, float deltaTime, std::string& season, ExperimentType& currentExp);

void init_Raindrops(std::vector<Precipitation> raindrops, size_t num_drops);
void updateRain(std::vector<Precipitation>& raindrops, float rainIntensity, float deltaTime);
void drawRain(sf::RenderWindow& window, const std::vector<Precipitation>& raindrops, size_t num_drops);

void init_Snowdrops(std::vector<Precipitation> &snowflakes, size_t num_flakes);
void updateSnow(std::vector<Precipitation>& snowflakes, std::vector<Precipitation> &groundSnow, float snowIntensity, float deltaTime);
void drawSnow(sf::RenderWindow &window, const std::vector<Precipitation> &snowflakes, std::vector<Precipitation> &groundSnow, size_t num_flakes);



#endif
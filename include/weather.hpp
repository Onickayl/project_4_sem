#ifndef WEATHER_HPP
#define WEATHER_HPP

#define _USE_MATH_DEFINES // для M_PI

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

enum class ExperimentType
{
    Normal,
    Drought,
    GlobalWarming
};

enum class ClimateType
{
    Default,
    Equatorial,
    Tropical,
    Subtropical,
    TemperateOceanic,
    TemperateContinental,
    Subarctic
};

struct Experiment
{
    std::string name;
    float sunCoef;
    float tempCoef;
    float rainCoef;
    float windCoef;
};

struct Precipitation
{
    float x, y;
    float speed;
    float length;
};

float lerpMonth(const float arr[12], float phase);

void auto_weather(float &year_time, float deltaTime, float &sun, float &temp, float &rain, float &wind,
                  float &soilWater, std::string &season, ClimateType climate, ExperimentType currentExp);

void init_Raindrops(std::vector<Precipitation> &raindrops, size_t num_drops);
void updateRain(std::vector<Precipitation> &raindrops, float rainIntensity, float deltaTime);
void drawRain(sf::RenderWindow &window, const std::vector<Precipitation> &raindrops, size_t num_drops, float rainIntensity);

void init_Snowdrops(std::vector<Precipitation> &snowflakes, size_t num_flakes);
void updateSnow(std::vector<Precipitation> &snowflakes, std::vector<Precipitation> &groundSnow,
                float snowIntensity, float deltaTime);
void drawSnow(sf::RenderWindow &window, const std::vector<Precipitation> &snowflakes,
              std::vector<Precipitation> &groundSnow, size_t num_flakes, float snowIntensity);

#endif

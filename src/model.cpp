#include "model.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>

// настроки
SettingsModel::SettingsModel(bool man, float speed, ExperimentType exp)
    : manual(man), simSpeed(speed), currentExp(exp)
{
}

std::string SettingsModel::getClimateName() const
{
    switch (currentClimate)
    {
    case ClimateType::Equatorial:
        return "Equatorial";
    case ClimateType::Tropical:
        return "Tropical";
    case ClimateType::Subtropical:
        return "Subtropical";
    case ClimateType::TemperateOceanic:
        return "Temperate Oceanic";
    case ClimateType::TemperateContinental:
        return "Temperate Continental";
    case ClimateType::Subarctic:
        return "Subarctic";
    case ClimateType::Default:
        return "Default";
    default:
        return "Unknown";
    }
}

std::string SettingsModel::getExpName() const
{
    switch (currentExp)
    {
    case ExperimentType::Normal:
        return "Normal";
    case ExperimentType::Drought:
        return "Drought";
    case ExperimentType::GlobalWarming:
        return "Global Warming";
    default:
        return "Unknown";
    }
}

// дерево
Tree::Tree(float timeYear, bool run, bool pause, float lastDay, size_t num_branch, size_t num_leaf)
    : yearTime(timeYear), isRunning(run), paused(pause), lastRecordedDay(lastDay), numLeaves(num_leaf)
{
    srand(time(nullptr));

    // ветки
    init_Branches(branches, num_branch);

    // листья
    init_Leaves(leaves, branches, num_leaf);
}

void Tree::update(float deltaTime, const Weather &weather, bool manual)
{
    if (!isRunning || paused)
    {
        return;
    }

    float currentSoilWater = weather.getSoilWater();
    std::string currentSeason = weather.getSeason();
    float sun = weather.getSun();
    float temp = weather.getTemp();
    float rain = weather.getRain();
    float wind = weather.getWind();

    distributeWater(leaves, branches, currentSoilWater, deltaTime);
    update_leaf(leaves, branches, deltaTime, currentSeason, sun, temp, rain, wind);

    // 10 дней/сек
    yearTime += deltaTime * 10.0f;

    if (yearTime >= 365.0f && !manual)
    {
        isRunning = false;
    }
}

void Tree::restart()
{
    yearTime = 0.0f;
    isRunning = true;
    paused = false;
    lastRecordedDay = -1.0f;

    // пересоздаём листья
    leaves.clear();
    init_Leaves(leaves, branches, numLeaves);
}

// погода
Weather::Weather(size_t num_drops, size_t num_flakes)
{
    // дождь
    init_Raindrops(raindrops, num_drops);

    // снег
    init_Snowdrops(snowflakes, num_flakes);
}

void Weather::update(float deltaTime, float year_time,
                     ClimateType climate, ExperimentType currentExp)
{

    auto_weather(year_time, deltaTime, sun, temp, rain, wind, soilWater, season, climate, currentExp);

    if (temp <= 0)
    {
        updateSnow(snowflakes, groundSnow, rain, deltaTime);
    }
    else
    {
        updateRain(raindrops, rain, deltaTime);
    }
}
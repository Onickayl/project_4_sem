#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include <string>
#include "branch.hpp"
#include "leaf.hpp"
#include "weather.hpp"

// погода
class Weather
{
private:
    std::vector<Precipitation> raindrops;
    std::vector<Precipitation> snowflakes;
    std::vector<Precipitation> groundSnow;
    std::string season;

    float sun = 50.0f;
    float temp = 15.0f;
    float rain = 30.0f;
    float wind = 20.0f;
    float soilWater = 50.0f;

public:
    Weather(size_t num_drops = 200, size_t num_flakes = 200);

    void update(float deltaTime, float year_time, ClimateType climate, ExperimentType currentExp);

    float getSun() const { return sun; }
    float getTemp() const { return temp; }
    float getRain() const { return rain; }
    float getWind() const { return wind; }
    float getSoilWater() const { return soilWater; }
    std::string getSeason() const { return season; }

    const std::vector<Precipitation> &getRaindrops() const { return raindrops; }
    const std::vector<Precipitation> &getSnowflakes() const { return snowflakes; }
    const std::vector<Precipitation> &getGroundSnow() const { return groundSnow; }

    // ручной режим
    void setSun(float s) { sun = std::max(0.0f, std::min(100.0f, s)); }
    void setTemp(float t) { temp = std::max(-30.0f, std::min(30.0f, t)); }
    void setRain(float r) { rain = std::max(0.0f, std::min(100.0f, r)); }
    void setWind(float w) { wind = std::max(0.0f, std::min(100.0f, w)); }
};

// дерево
class Tree
{
private:
    std::vector<Branch> branches;
    std::vector<Leaf> leaves;
    float yearTime;
    bool isRunning;
    bool paused;
    float lastRecordedDay;
    size_t numLeaves;

public:
    Tree(float timeYear = 0.0f, bool run = true, bool pause = false,
         float lastDay = -1.0f, size_t num_branch = 16, size_t num_leaf = 400);

    void update(float deltaTime, const Weather &weather, bool manual);
    void restart();

    const std::vector<Branch> &getBranches() const { return branches; }
    const std::vector<Leaf> &getLeaves() const { return leaves; }
    float getYearTime() const { return yearTime; }
    bool getIsRunning() const { return isRunning; }

    void setYearTime(float time) { yearTime = time; }
    void setRunning(bool running) { isRunning = running; }
    void setPaused(bool p) { paused = p; }
    bool isPaused() const { return paused; }
};

// настройки
class SettingsModel
{
private:
    ClimateType currentClimate = ClimateType::Default;
    bool manual = false;
    float simSpeed = 1.0f;
    ExperimentType currentExp = ExperimentType::Normal;

public:
    SettingsModel() = default;                                // по умолчанию
    SettingsModel(bool man, float speed, ExperimentType exp); // с параметрами

    ClimateType getClimate() const { return currentClimate; }
    ExperimentType getExperiment() const { return currentExp; }
    float getSimSpeed() const { return simSpeed; }
    bool isManual() const { return manual; }

    // для отображения в меню
    std::string getClimateName() const;
    std::string getExpName() const;

    void setClimate(ClimateType climate) { currentClimate = climate; }
    void setExperiment(ExperimentType exp) { currentExp = exp; }
    void setSimSpeed(float speed) { simSpeed = speed; }
    void setManual(bool m) { manual = m; }
};

#endif
#ifndef DATA_HPP
#define DATA_HPP

#include <string>
#include <fstream>
#include <vector>
#include "weather.hpp"

class Leaf;  // предварительное объявление, чтобы избежать циклические завасимости

class Data
{
private:
    std::ofstream weatherFile;
    std::ofstream leafFile;

    std::string weatherFilename;
    std::string leafFilename;

    float lastRecordedDay = -1.0f;

    std::string getWeatherFilename(ClimateType climate, ExperimentType exp, bool manual) const;
    std::string getLeafFilename(ClimateType climate, ExperimentType exp, bool manual) const;

public:
    Data() = default;
    ~Data();

    // открыть/пересоздать файлы с новыми настройками
    void open(ClimateType climate, ExperimentType exp, bool manual);

    // закрыть
    void close();

    // записать данные текущ игрового дня
    void recordDay(float yearTime, const std::string &season,
                   float temp, float sun, float rain, float wind, float soilWater,
                   const std::vector<Leaf> &leaves);

    // рестарт
    void reset();

    std::string getWeatherFilename() const { return weatherFilename; }
    std::string getLeafFilename() const { return leafFilename; }
};

#endif
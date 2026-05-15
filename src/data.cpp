#include "data.hpp"
#include "leaf.hpp"
#include <iomanip>
#include <iostream>

Data::~Data()
{
    close();
}

std::string Data::getWeatherFilename(ClimateType climate, ExperimentType exp, bool manual) const
{
    std::string mName = manual ? "man" : "sim";
    std::string cName;

    switch (climate)
    {
    case ClimateType::Equatorial:
        cName = "eq";
        break;
    case ClimateType::Tropical:
        cName = "trop";
        break;
    case ClimateType::Subtropical:
        cName = "subtrop";
        break;
    case ClimateType::TemperateOceanic:
        cName = "tempOc";
        break;
    case ClimateType::TemperateContinental:
        cName = "tempCon";
        break;
    case ClimateType::Subarctic:
        cName = "subar";
        break;
    default:
        cName = "default";
    }

    std::string eName;
    switch (exp)
    {
    case ExperimentType::Normal:
        eName = "normal";
        break;
    case ExperimentType::Drought:
        eName = "drought";
        break;
    case ExperimentType::GlobalWarming:
        eName = "warming";
        break;
    default:
        eName = "default";
    }

    return "weather_" + mName + "_" + cName + "_" + eName + ".txt";
}

std::string Data::getLeafFilename(ClimateType climate, ExperimentType exp, bool manual) const
{
    std::string mName = manual ? "man" : "sim";
    std::string cName;

    switch (climate)
    {
    case ClimateType::Equatorial:
        cName = "eq";
        break;
    case ClimateType::Tropical:
        cName = "trop";
        break;
    case ClimateType::Subtropical:
        cName = "subtrop";
        break;
    case ClimateType::TemperateOceanic:
        cName = "tempOc";
        break;
    case ClimateType::TemperateContinental:
        cName = "tempCon";
        break;
    case ClimateType::Subarctic:
        cName = "subar";
        break;
    default:
        cName = "default";
    }

    std::string eName;
    switch (exp)
    {
    case ExperimentType::Normal:
        eName = "normal";
        break;
    case ExperimentType::Drought:
        eName = "drought";
        break;
    case ExperimentType::GlobalWarming:
        eName = "warming";
        break;
    default:
        eName = "default";
    }

    return "leaf_" + mName + "_" + cName + "_" + eName + ".csv";
}

void Data::open(ClimateType climate, ExperimentType exp, bool manual)
{
    close();

    weatherFilename = getWeatherFilename(climate, exp, manual);
    leafFilename = getLeafFilename(climate, exp, manual);

    weatherFile.open(weatherFilename);

    if (!weatherFile.is_open())
    {
        std::cerr << "Cannot open " << weatherFilename << std::endl;
        return;
    }
    weatherFile << "Day,Season,Temp,Sun,Rain,Wind,SoilWater\n";

    leafFile.open(leafFilename);

    if (!leafFile.is_open())
    {
        std::cerr << "Cannot open " << leafFilename << std::endl;
        return;
    }
    leafFile << "Day,Season,AvgWater,AvgSugar,AvgChlorophyll,AvgAnthocyanin\n";

    lastRecordedDay = -1.0f;
}

void Data::close()
{
    if (weatherFile.is_open())
    {
        weatherFile.close();
    }

    if (leafFile.is_open())
    {
        leafFile.close();
    }
}

void Data::recordDay(float yearTime, const std::string &season, float temp, float sun, float rain, float wind, float soilWater, const std::vector<Leaf> &leaves)
{
    if (!weatherFile.is_open() || !leafFile.is_open())
    {
        return;
    }

    int currentDay = static_cast<int>(yearTime);

    // записываем только если день сменился и не больше 365
    if (currentDay <= lastRecordedDay || currentDay > 365)
    {
        return;
    }

    lastRecordedDay = currentDay;

    weatherFile << currentDay << "," << season << ","
                << std::fixed << std::setprecision(1) << temp << ","
                << std::fixed << std::setprecision(0) << sun << ","
                << std::fixed << std::setprecision(0) << rain << ","
                << std::fixed << std::setprecision(0) << wind << ","
                << std::fixed << std::setprecision(1) << soilWater << "\n";

    float totalWater = 0.0f;
    float totalSugar = 0.0f;
    float totalChloro = 0.0f;
    float totalAnth = 0.0f;
    int matureCount = 0;

    for (const auto &leaf : leaves)
    {
        if (leaf.state == LeafState::Mature)
        {
            totalWater += leaf.water;
            totalSugar += leaf.sugar;
            totalChloro += leaf.chlorophyll;
            totalAnth += leaf.anthocyanin;
            matureCount++;
        }
    }

    float avgWater = matureCount > 0 ? totalWater / matureCount : 0.0f;
    float avgSugar = matureCount > 0 ? totalSugar / matureCount : 0.0f;
    float avgChloro = matureCount > 0 ? totalChloro / matureCount : 0.0f;
    float avgAnth = matureCount > 0 ? totalAnth / matureCount : 0.0f;

    leafFile << currentDay << "," << season << ","
             << std::fixed << std::setprecision(1) << avgWater << ","
             << std::fixed << std::setprecision(1) << avgSugar << ","
             << std::fixed << std::setprecision(1) << avgChloro << ","
             << std::fixed << std::setprecision(1) << avgAnth << "\n";
}

void Data::reset()
{
    lastRecordedDay = -1.0f;
}
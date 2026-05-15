#include <SFML/Graphics.hpp>
#include <math.h>
#include "branch.hpp"
#include "leaf.hpp"
#include "weather.hpp"
#include "model.hpp"
#include "view.hpp"
#include "controller.hpp"
#include <iostream>
#include <filesystem>
#include <iomanip>
#include <fstream>
#include <sstream>

int main(int argc, char *argv[])
{

    // знач по умолчанию
    ClimateType currentClimate = ClimateType::Default;
    ExperimentType currentExp = ExperimentType::Normal;
    bool manual = false;
    float simSpeed = 1.0f;

    // аргументы командной строки
    if (argc >= 2)
    {
        for (int i = 1; i < argc; i++)
        {
            std::string arg = argv[i];
            if (arg == "--man")
                manual = true;
            else if (arg == "--sim")
                manual = false;
            else if (arg == "--eq")
                currentClimate = ClimateType::Equatorial;
            else if (arg == "--trop")
                currentClimate = ClimateType::Tropical;
            else if (arg == "--subtrop")
                currentClimate = ClimateType::Subtropical;
            else if (arg == "--tempOc")
                currentClimate = ClimateType::TemperateOceanic;
            else if (arg == "--tempCon")
                currentClimate = ClimateType::TemperateContinental;
            else if (arg == "--subar")
                currentClimate = ClimateType::Subarctic;
            else if (arg == "--drought")
                currentExp = ExperimentType::Drought;
            else if (arg == "--warming")
                currentExp = ExperimentType::GlobalWarming;
            else if (arg == "--normal")
                currentExp = ExperimentType::Normal;
            else if (arg == "--speed")
            {
                if (i + 1 < argc)
                {
                    simSpeed = std::atof(argv[i + 1]);
                    if (simSpeed <= 0.0f)
                    {
                        simSpeed = 1.0f;
                    }

                    i++; // пропускаем следующий аргумент (число)
                }
                else
                {
                    std::cout << "Usage: ./autumn [--eq|--trop] [--drought|--warming]" << std::endl;
                    return 1;
                }
            }
            else
            {
                std::cout << "Usage: ./autumn [--eq|--trop] [--drought|--warming]" << std::endl;
                return 1;
            }
        }
    }
    
    // инициализация рандома
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // создаём контроллер с настройками из аргументов
    Controller controller;

    // применяем настройки из командной строки к контроллеру
    controller.applyCommandLineSettings(currentClimate, currentExp, manual, simSpeed);

    // запускаем главный цикл
    controller.run();

    return 0;
}

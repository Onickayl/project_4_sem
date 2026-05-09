#ifndef WEATHER_HPP
#define WEATHER_HPP

#include <string>
#include <iostream>
#include <cmath>

// объявление глобальных переменных (extern — значит "определены в другом месте")
extern float sun;
extern float temp;
extern float rain;
extern float wind;

void auto_weather(float& year_time, float deltaTime, std::string& season);

#endif
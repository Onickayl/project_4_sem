#include "weather.hpp"

// глобальные переменные (здесь они реально существуют)
float sun = 70.0f;
float temp = 15.0f;
float rain = 0.0f;
float wind = 0.0f;


void auto_weather(float& year_time, float deltaTime, std::string& season)
{
    year_time += deltaTime * 10.0f; // 10


    // фаза года 0 - 1
    float phase = year_time / 365.0f;
    if (phase > 1.0f) phase = 1.0f; 
    



// температура
    float radTemp = (phase - 0.375f) * 2.0f * M_PI;  
    temp = 5.0f + 15.0f * cos(radTemp);            // от -10С до 20С

// солнце
    float radSun = (phase - 0.375) * 2.0f * M_PI;  
    sun = 50.0f + 30.0f * cos(radSun);             // от 20 до 80
    if (sun < 0.0f) sun = 0.0f;
    if (sun > 100.0f) sun = 100.0f;

// дождь 
    float radRain = phase * 4.0f * M_PI;
    rain = 50.0f + 25.0f * cos(radRain);           // от 25 до 75
    if (rain < 0.0f) rain = 0.0f;
    if (rain > 100.0f) rain = 100.0f;

// ветер
    float radWind = (phase - 0.625f) * 2.0f * M_PI;
    wind = 50.0f + 30.0f * cos(radWind);           // от 20 до 80
    if (wind < 0.0f) wind = 0.0f;
    if (wind > 100.0f) wind = 100.0f;

// Сезон 
    if (phase < 0.25f)
    {
        season = "Spring";
    }
    else if (phase < 0.5f)
    {
        season = "Summer";
    }
    else if (phase < 0.75f)
    {
        season = "Autumn";
    }
    else
    {
        season = "Winter";
    }

}
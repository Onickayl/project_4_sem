#include "weather.hpp"

// глобальные переменные (здесь они реально существуют)
float sun = 70.0f;
float temp = 15.0f;
float rain = 0.0f;
float wind = 0.0f;
float soilWater = 50.0f;

ClimateType currentClimate = ClimateType::Default;  // значение по умолчанию


std::vector<Experiment> experiments = {
    {"Normal",         1.0f,  0.0f, 1.0f, 1.0f}, 
    {"Drought",        1.5f,  7.0f, 0.05f, 1.3f}, 
    {"GlobalWarming",  1.0f,  5.0f, 0.6f,  1.0f},  
};



float lerpMonth(const float arr[12], float phase)
{
    float monthPhase = phase * 12.0f;
    int m0 = ((int)monthPhase) % 12;                // текущий месяц
    int m1 = (m0 + 1) % 12;                         // следующий месяц
    float frac = monthPhase - floor(monthPhase);    // прогресс между ними
    return arr[m0] + (arr[m1] - arr[m0]) * frac;
}

void auto_weather(float& year_time, float deltaTime, std::string& season, ExperimentType& currentExp)
{

    // 10 дней/сек
    year_time += deltaTime * 10.0f;
    float dt = deltaTime * 10.0f;

    // фаза
    float phase = year_time / 365.0f;
    if (phase > 1.0f) phase = 1.0f;
    
    int idx = static_cast<int>(currentExp);
    Experiment exp = experiments[idx];
    float x = phase * 2.0f * M_PI;


    // общий делитель для перевода осадков мм в %, т.к. максимум осадков 348 мм
    const float max_rain = 3.5f;

    switch (currentClimate)
    {
    case ClimateType::Equatorial:
    {
        const float tempArr[12] = {28.4f, 28.6f, 28.9f, 28.6f, 28.3f, 28.2f, 28.3f, 28.3f, 27.8f, 27.4f, 27.3f, 27.9f};
        const float rainArr[12] = {172.5f, 171.9f, 165.2f, 161.4f, 145.3f, 153.2f, 137.9f, 172.6f, 252.4f, 310.5f, 240.1f, 107.2f};
        
        temp = lerpMonth(tempArr, phase);
        rain = std::min(100.0f, lerpMonth(rainArr, phase) / max_rain);
        sun  = 85.0f + 10.0f * sin(phase * 4.0f * M_PI);
        wind = 10.0f + 5.0f * sin(x);
        break;
    }

    case ClimateType::Tropical:
    {
        const float tempArr[12] = {30.8f, 31.8f, 31.2f, 30.7f, 30.2f, 29.9f, 29.4f, 29.4f, 29.4f, 28.2f, 28.5f, 29.7f};
        const float rainArr[12] = {48.8f, 97.8f, 222.8f, 184.2f, 198.0f, 240.4f, 348.3f, 275.1f, 57.5f, 11.8f, 23.8f, 27.7f};
     
        temp = lerpMonth(tempArr, phase);
        rain = std::min(100.0f, lerpMonth(rainArr, phase) / max_rain);
        sun  = 75.0f + 15.0f * cos(x - 0.2f);
        wind = 20.0f + 10.0f * sin(x - 1.0f);
        break;
    }

    case ClimateType::Subtropical:
    {

        const float tempArr[12] = {10.2f, 12.6f, 16.9f, 22.4f, 25.6f, 25.1f, 20.3f, 15.2f, 9.4f, 6.4f, 5.6f, 7.3f};
        const float rainArr[12] = {33.7f, 41.3f, 40.1f, 19.3f, 8.3f, 9.7f, 27.5f, 56.0f, 48.1f, 38.6f, 29.8f, 28.7f};
        
        temp = lerpMonth(tempArr, phase);
        rain = std::min(100.0f, lerpMonth(rainArr, phase) / max_rain);
        sun  = 55.0f + 35.0f * cos(x - 0.1f);
        wind = 30.0f + 15.0f * sin(x + 0.5f);
        break;
    }

    case ClimateType::TemperateOceanic:
    {

        const float tempArr[12] = {7.1f, 9.5f, 12.5f, 15.8f, 17.8f, 17.6f, 14.8f, 11.7f, 7.7f, 5.2f, 4.9f, 5.1f};
        const float rainArr[12] = {43.2f, 45.9f, 53.1f, 56.0f, 51.9f, 58.1f, 55.1f, 74.6f, 66.4f, 69.4f, 62.5f, 48.5f};
        
        temp = lerpMonth(tempArr, phase);
        rain = std::min(100.0f, lerpMonth(rainArr, phase) / max_rain);
        sun  = 40.0f + 30.0f * cos(x);
        wind = 50.0f + 20.0f * sin(x + 0.3f);
        break;
    }

    case ClimateType::TemperateContinental:
    {

        const float tempArr[12] = {-4.5f, 5.8f, 13.8f, 18.0f, 19.9f, 18.2f, 12.0f, 5.1f, -3.6f, -10.3f, -12.4f, -11.5f};
        const float rainArr[12] = {38.0f, 33.2f, 51.6f, 65.0f, 48.4f, 49.6f, 49.3f, 55.7f, 49.1f, 52.5f, 45.5f, 40.0f};
        
        temp = lerpMonth(tempArr, phase);
        rain = std::min(100.0f, lerpMonth(rainArr, phase) / max_rain);
        sun  = 45.0f + 45.0f * cos(x - 0.15f);
        wind = 35.0f + 25.0f * sin(x - 0.6f);
        break;
    }

    case ClimateType::Subarctic:
    {

        const float tempArr[12] = {-18.7f, -4.0f, 7.7f, 16.8f, 19.6f, 15.6f, 6.6f, -6.7f, -25.6f, -37.1f, -37.2f, -32.4f};
        const float rainArr[12] = {6.4f, 8.8f, 19.2f, 28.2f, 39.9f, 39.3f, 31.3f, 19.3f, 16.8f, 8.8f, 9.6f, 8.7f};
        
        temp = lerpMonth(tempArr, phase);
        rain = std::min(100.0f, lerpMonth(rainArr, phase) / max_rain);

        // полярный день/ночь
        float s = cos(x - 1.0f);
        if (s < -0.5f) 
        {
            sun = 2.0f + 3.0f * (s + 0.5f) / 0.5f;

        } else 
        {
            sun = 50.0f + 50.0f * s;
        }

        if (sun < 0.0f) sun = 0.0f;

        wind = 30.0f + 30.0f * fabs(sin(2.0f * (x - 1.0f)));
        break;
    }

    case ClimateType::Default:
    {

        float radTemp = (phase - 0.375f) * 2.0f * M_PI;
        temp = 5.0f + 15.0f * cos(radTemp);

        float radSun = (phase - 0.375f) * 2.0f * M_PI;
        sun = 50.0f + 20.0f * cos(radSun);

        float radRain = phase * 4.0f * M_PI;
        rain = 50.0f + 25.0f * cos(radRain);

        float radWind = (phase - 0.625f) * 2.0f * M_PI;
        wind = 50.0f + 30.0f * cos(radWind);
        break;
    }
    }


// если есть режим эксперимента
    sun  *= exp.sunCoef;
    temp += exp.tempCoef;
    rain *= exp.rainCoef;
    wind *= exp.windCoef;

// ограничения
    sun = std::max(0.0f, std::min(100.0f, sun));
    rain = std::max(0.0f, std::min(100.0f, rain));
    wind = std::max(0.0f, std::min(100.0f, wind));

    float moisture = 0.01f;     // больше от осадков - увеличить    
    float evaporation = 0.005f;  // сильнее испаряется - увеличить

// вода в почве
    soilWater += rain * moisture * dt;                   
    soilWater -= (sun * 0.3f + temp * 0.5f) * evaporation * dt;
    soilWater = std::max(0.0f, std::min(100.0f, soilWater));


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

void init_Raindrops(std::vector<Precipitation> &raindrops, size_t num_drops)
{

    for (int i = 0; i < num_drops; i++)
    {
        Precipitation drop;

        drop.x = rand() % 800;              // случайный X по ширине окна
        drop.y = rand() % 700;                       
        drop.speed = 300 + rand() % 200;    // 300-500 пикселей в секунду
        drop.length = 10 + rand() % 10;     // длина 10-20 пикселей

        raindrops.push_back(drop);
    }
}

void updateRain(std::vector<Precipitation>& raindrops, float rainIntensity, float deltaTime)
{

    float speed = 300 + rain * 2;
    float length = 5 + rain / 100.0f * 15.0f;

    for (auto& drop : raindrops)
    {
        drop.speed = speed;

        // Падают вниз
        drop.y += drop.speed * deltaTime;
        
        // Упали за экран — вернуть наверх
        if (drop.y > 690)
        {
            drop.y = -10; 
            drop.x = rand() % 800;
        }
    }
    
    
    int targetDrops = (int)(rainIntensity * 2.0f);  // 0-200 капель

// Если дождь слабый — убираем лишние капли
    while (raindrops.size() > targetDrops && !raindrops.empty())
    {
        raindrops.pop_back();
    }

// Если дождь сильный — добавляем
    while (raindrops.size() < targetDrops)
    {
        Precipitation drop;

        drop.x = rand() % 800;
        drop.y = rand() % 700; 
        drop.speed = speed;
        drop.length = length;

        raindrops.push_back(drop);
    }
}

void drawRain(sf::RenderWindow& window, const std::vector<Precipitation>& raindrops, size_t num_drops)
{
    float N = num_drops * rain / 100.0f;
    float speed = 300 + rain * 2;
    float length = 5 + rain / 100.0f * 15.0f;

    for (int i = 0; i < N; i++)
    {
        // Капля — это узкий прямоугольник (чёрточка)
        sf::RectangleShape raindrop(sf::Vector2f(1.5f, length));

        raindrop.setPosition(raindrops[i].x, raindrops[i].y);
        
        raindrop.setFillColor(sf::Color(150, 180, 220, 180));  // голубой, полупрозрачный
        
        // Наклоняем каплю по диагонали (как настоящий дождь)
        raindrop.setRotation(15.0f);
        
        window.draw(raindrop);
    }
}

void init_Snowdrops(std::vector<Precipitation> &snowflakes, size_t num_flakes)
{

    for (int i = 0; i < num_flakes; i++)
    {
        Precipitation drop;

        drop.x = rand() % 800;         
        drop.y = rand() % 700;                       
        drop.speed = 30 + rand() % 50;    // 30-80 пикселей в секунду
        drop.length = 2 + rand() % 2;     // длина 2-4 пикселей

        snowflakes.push_back(drop);
    }
}

void updateSnow(std::vector<Precipitation>& snowflakes, std::vector<Precipitation> &groundSnow, float snowIntensity, float deltaTime)
{

    float speed = 30 + rain / 2.0f;
    float length = 1 + rain / 100.0f * 6.0f;

    for (auto &drop : snowflakes)
    {
        drop.speed = speed;

        // падают
        drop.y += drop.speed * deltaTime;
        drop.x += sin(drop.y * 0.1f) * 50.0f * deltaTime; // покачивание 

        // Упали за экран — вернуть наверх + покров
        if (drop.y > 690)
        {
            Precipitation flake;
            flake.x = drop.x;
            flake.y = 690 + rand() % 5; 
            flake.length = length;
            groundSnow.push_back(flake);

            // ограничим количество 
            if (groundSnow.size() > 5000) groundSnow.erase(groundSnow.begin());

            drop.y = -10;
            drop.x = rand() % 800;
        }
    }

    int targetDrops = (int)(snowIntensity * 2.0f);  // 0-200 снежинок

// Если снег слабый — убираем лишние капли
    while (snowflakes.size() > targetDrops && !snowflakes.empty())
    {
        snowflakes.pop_back();
    }

// Если снег сильный — добавляем
    while (snowflakes.size() < targetDrops)
    {
        Precipitation drop;

        drop.x = rand() % 800;
        drop.y = rand() % 700; 
        drop.speed = speed;
        drop.length = length;

        snowflakes.push_back(drop);
    }
}

void drawSnow(sf::RenderWindow &window, const std::vector<Precipitation> &snowflakes, std::vector<Precipitation> &groundSnow, size_t num_flakes)
{
    float N = num_flakes * rain / 100.0f;
    float speed = 30 + rain / 2.0f;
    float length = 3 + rain / 100.0f * 3.0f;

    for (int i = 0; i < N; i++)
    {
        
        sf::CircleShape flakes(length);

        flakes.setPosition(snowflakes[i].x, snowflakes[i].y);

        flakes.setFillColor(sf::Color(255, 255, 255, 200)); // белый, полупрозрачный

        window.draw(flakes);
    }

    for (auto &flake : groundSnow)
    {
        sf::CircleShape shape(flake.length);
        shape.setPosition(flake.x, flake.y);
        shape.setFillColor(sf::Color(255, 255, 255, 200));
        window.draw(shape);
    }
}
#include "weather.hpp"

// глобальные переменные (здесь они реально существуют)
float sun = 70.0f;
float temp = 15.0f;
float rain = 0.0f;
float wind = 0.0f;
float soilWater = 50.0f;



void auto_weather(float& year_time, float deltaTime, std::string& season, ExperimentType& currentExp)
{
    year_time += deltaTime * 10.0f; // 10


    // фаза года 0 - 1
    float phase = year_time / 365.0f;
    if (phase > 1.0f) phase = 1.0f; 
    
    float moisture = 1.0f;
    float evaporation = 1.0f;
    float waterSpeed = 1.0f;

    float rainCoef = 1.0f;
    float tempCoef = 0.0f;

    if (currentExp == ExperimentType::Drought)
    {
        rainCoef = 0.2f;
    }

    if (currentExp == ExperimentType::GlobalWarming)
    {
        tempCoef = 7.0f;
    }


// температура
    float radTemp = (phase - 0.375f) * 2.0f * M_PI;  
    temp = 5.0f + 15.0f * cos(radTemp);            // от -10С до 20С
    temp += tempCoef;

// солнце
    float radSun = (phase - 0.375) * 2.0f * M_PI;  
    sun = 50.0f + 30.0f * cos(radSun);             // от 20 до 80
    if (sun < 0.0f) sun = 0.0f;
    if (sun > 100.0f) sun = 100.0f;

// дождь 
    float radRain = phase * 4.0f * M_PI;
    rain = 50.0f + 25.0f * cos(radRain);           // от 25 до 75
    rain = rain * rainCoef;
    if (rain < 0.0f) rain = 0.0f;
    if (rain > 100.0f) rain = 100.0f;

// ветер
    float radWind = (phase - 0.625f) * 2.0f * M_PI;
    wind = 50.0f + 30.0f * cos(radWind);           // от 20 до 80
    if (wind < 0.0f) wind = 0.0f;
    if (wind > 100.0f) wind = 100.0f;

// вода в почве
    soilWater += rain * moisture;
    soilWater -= (sun + temp) * evaporation;
    soilWater -= waterSpeed * deltaTime;   // уходит в дерево
    if (soilWater < 0.0f) soilWater = 0.0f;
    if (soilWater > 100.0f) soilWater = 100.0f;

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
        drop.y = rand() % 600;                       
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
        if (drop.y > 590)
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
        drop.y = rand() % 600; 
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
        drop.y = rand() % 600;                       
        drop.speed = 30 + rand() % 50;    // 30-80 пикселей в секунду
        drop.length = 1 + rand() % 2;     // длина 1-3 пикселей

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
        if (drop.y > 590)
        {
            Precipitation flake;
            flake.x = drop.x;
            flake.y = 590 + rand() % 5; 
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
        drop.y = rand() % 600; 
        drop.speed = speed;
        drop.length = length;

        snowflakes.push_back(drop);
    }
}

void drawSnow(sf::RenderWindow &window, const std::vector<Precipitation> &snowflakes, std::vector<Precipitation> &groundSnow, size_t num_flakes)
{
    float N = num_flakes * rain / 100.0f;
    float speed = 30 + rain / 2.0f;
    float length = 1 + rain / 100.0f * 6.0f;

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
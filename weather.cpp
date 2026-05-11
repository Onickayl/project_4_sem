#include "weather.hpp"

// глобальные переменные (здесь они реально существуют)
float sun = 70.0f;
float temp = 15.0f;
float rain = 0.0f;
float wind = 0.0f;
float soilWater = 50.0f;


void auto_weather(float& year_time, float deltaTime, std::string& season)
{
    year_time += deltaTime * 10.0f; // 10


    // фаза года 0 - 1
    float phase = year_time / 365.0f;
    if (phase > 1.0f) phase = 1.0f; 
    
    float moisture = 1.0f;
    float evaporation = 1.0f;
    float waterSpeed = 1.0f;


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

// вода в почве
    soilWater += rain * moisture;
    soilWater -= (sun + temp) * evaporation;
    soilWater -= waterSpeed * deltaTime;   // уходит в дерево - может не через скорость, а тоже как коэффициент не знаю
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


void init_Raindrops(std::vector<Raindrop> raindrops, size_t num_drops)
{


    for (int i = 0; i < num_drops; i++)
    {
        Raindrop drop;

        drop.x = rand() % 800;              // случайный X по ширине окна
        drop.y = rand() % 600;                       
        drop.speed = 300 + rand() % 200;    // 300-500 пикселей в секунду
        drop.length = 10 + rand() % 10;     // длина 10-20 пикселей

        raindrops.push_back(drop);
    }
}

void updateRain(std::vector<Raindrop>& drops, float rainIntensity, float deltaTime)
{

    float speed = 300 + rain * 2;
    float length = 5 + rain / 100.0f * 15.0f;

    for (auto& drop : drops)
    {
        drop.speed = speed;

        // Падают вниз
        drop.y += drop.speed * deltaTime;
        
        // Упали за экран — вернуть наверх
        if (drop.y > 590)
        {
            drop.y = -10;//rand() % 600; 
            drop.x = rand() % 800;
        }
    }
    
    
    int targetDrops = (int)(rainIntensity * 2.0f);  // 0-200 капель

// Если дождь слабый — убираем лишние капли
    while (drops.size() > targetDrops && !drops.empty())
    {
        drops.pop_back();
    }

// Если дождь сильный — добавляем
    while (drops.size() < targetDrops)
    {
        Raindrop drop;

        drop.x = rand() % 800;
        drop.y = rand() % 600; 
        drop.speed = speed;
        drop.length = length;

        drops.push_back(drop);
    }
}


void drawRain(sf::RenderWindow& window, const std::vector<Raindrop>& drops, size_t num_drops)
{
    float N = num_drops * rain / 100.0f;
    float speed = 300 + rain * 2;
    float length = 5 + rain / 100.0f * 15.0f;
    //speed = базовая_скорость + rain * множитель
    //length = минимальная_длина + rain * множитель

    for (int i = 0; i < N; i++)
    {
        // Капля — это узкий прямоугольник (чёрточка)
        sf::RectangleShape raindrop(sf::Vector2f(1.5f, length));

        raindrop.setPosition(drops[i].x, drops[i].y);
        
        raindrop.setFillColor(sf::Color(150, 180, 220, 180));  // голубой, полупрозрачный
        
        // Наклоняем каплю по диагонали (как настоящий дождь)
        raindrop.setRotation(15.0f);
        
        window.draw(raindrop);
    }
}
#include <SFML/Graphics.hpp>



struct Leaf
{
    int x;              // координата x
    int y;              // координата y
    float water;        // кол-во воды          0-100%
    float sugar;        // кол-во сахара        0-100%
    float chlorophyll;  // кол-во хлорофилла    0-100% (зелёный)
    float carotenoids;  // кол-во каротиноидов  0-100% (жёлтый)
    float anthocyanin;  //кол-во антоциан       0-100% (красный)
    float stickiness;   // прилипчивость        0-100%
    int is_alive;       // жив - 1, мёртв - 0
};



void drawBranch(sf::RenderWindow& window, float startX, float startY, float length, float width, float angle);
void initLeaves(Leaf leaves[], int count);
void drawLeaves(sf::RenderWindow& window, Leaf leaves[], int count);


int main() 
{
    int count = 100;
    Leaf leaves[count];
    

    // Создаём окно 800x600 с названием "autumn". Переменная window — это окно.
    sf::RenderWindow window(sf::VideoMode(800, 600), "autumn");


// ствол дерева
    sf::RectangleShape trunk(sf::Vector2f(50, 500));    // 50 вправо, 500 пикселей вниз
    trunk.setPosition(400, 100);                        // Начинаем с Y=100, чтобы 500 пикселей вниз закончились на Y=600
    trunk.setFillColor(sf::Color(139, 69, 19));
    
// инициализация листьев 
    initLeaves(leaves, count);

    // Главный цикл. Программа крутится здесь, пока окно открыто
    while (window.isOpen()) 
    {
        sf::Event event;    //Создаём переменную event, которая будет хранить информацию о том, что сделал пользователь (нажал на крестик, клавишу и т.д.).
        
        //Проверяем, есть ли какие-то события (например, пользователь нажал на крестик).
        while (window.pollEvent(event)) 
        {

            //Если событие — это нажатие клавиши Esc, то закрываем окно.
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
            {
                window.close();
            }
        }
        
        // Заливаем всё окно тёмно-синим цветом.
        window.clear(sf::Color(20, 30, 50));  // RGB: 20,30,50
        
        // рисуем 
        window.draw(trunk);
        drawBranch(window, 400, 250, 120, 15, 125);
        drawBranch(window, 450, 250, 120, 15, 215);
        drawLeaves(window, leaves, count);
        

        // Показываем всё, что нарисовали, на экране.
        window.display();
    }
    
    return 0;
}


// Рисование ветки, растущей из точки на стволе
void drawBranch(sf::RenderWindow& window, 
                float startX, float startY,     // откуда растёт
                float length, float width,      // размеры
                float angle)                    // угол наклона
{                  
    
    sf::RectangleShape branch(sf::Vector2f(width, length));
    
    // Важно! Сдвигаем точку привязки к тому месту, где ветка крепится к стволу
    // По умолчанию точка привязки (origin) — верхний левый угол.
    // Нам нужно, чтобы ветка росла из своего "корня", а не из середины.
    // Ставим origin в середину левого края (по толщине) и в самый верх (0 по длине)
    branch.setOrigin(width/2, 0);  // центр по ширине, верх по длине. Точка привязки
    
    branch.setPosition(startX, startY);
    branch.setRotation(angle);                   // от оси Y, что направлена вниз, и по часовой стрелке
    branch.setFillColor(sf::Color(139, 69, 19));
   
    
    window.draw(branch);
}

// инициализация листьев
void initLeaves(Leaf leaves[], int count) 
{
    for (int i = 0; i < count; i++) 
    {
        leaves[i].x = 300 + rand() % 200;
        leaves[i].y = 200 + rand() % 200;
        leaves[i].chlorophyll = 100;
        leaves[i].carotenoids = 50;
        leaves[i].anthocyanin = 0;
        leaves[i].water = 100;
        leaves[i].sugar = 50;
        leaves[i].stickiness = 100;
        leaves[i].is_alive = 1;
    }
}

// Рисование листьев
void drawLeaves(sf::RenderWindow& window, Leaf leaves[], int count) 
{
    for (int i = 0; i < count; i++) 
    {

        if (leaves[i].is_alive) 
        {
            // Создаём кружок с радиусом 5 пикселей
            sf::CircleShape leafShape(5.0f);
            // Ставим в нужное место (например, x=400, y=300)
            leafShape.setPosition(leaves[i].x, leaves[i].y);

            /*
            leaf.setScale(1.8f, 0.6f);        // растянуть в овал
            leaf.setRotation(rand() % 360);   // случайный поворот
            */
            
            // Цвет зависит от хлорофилла
            if (leaves[i].chlorophyll > 70)
            {
                leafShape.setFillColor(sf::Color::Green);
            }
            else if (leaves[i].chlorophyll > 40)
            {
                leafShape.setFillColor(sf::Color::Yellow);
            }
            else
            {
                leafShape.setFillColor(sf::Color::Red);
            }
                
            window.draw(leafShape);
        }
    }
}

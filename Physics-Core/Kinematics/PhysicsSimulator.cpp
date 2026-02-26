#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <cstdlib>

enum class SimulationType {
    None,
    UniformMotion,
    UniformAcceleration,
    FreeFall
};

struct Ball {
    sf::CircleShape shape;
    float velocityX = 0.f;
    float velocityY = 0.f;
};

int main()
{
    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u(1000, 600)),
        "Physics Simulator"
    );

    window.setFramerateLimit(60);

    const float ground = 550.f;
    const float startX = 200.f;
    const float startY = 100.f;
    const float gravity = 980.f;
    const float airResistance = 0.999f;

    SimulationType currentSim = SimulationType::None;
    std::vector<Ball> balls;

    // Initial ball
    Ball ball;
    ball.shape.setRadius(20.f);
    ball.shape.setFillColor(sf::Color::Red);
    ball.shape.setPosition(sf::Vector2f(startX, startY));
    balls.push_back(ball);

    sf::Font font;
    if (!font.openFromFile("arial.ttf"))
        return -1;

    sf::Text infoText(font);
    infoText.setCharacterSize(18);
    infoText.setFillColor(sf::Color::White);
    infoText.setPosition(sf::Vector2f(20.f, 20.f));

    sf::RectangleShape groundLine(sf::Vector2f(1000.f, 5.f));
    groundLine.setFillColor(sf::Color::Green);
    groundLine.setPosition(sf::Vector2f(0.f, ground + 20.f));

    sf::Clock clock;

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();

        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (event->is<sf::Event::KeyPressed>())
            {
                auto key = event->getIf<sf::Event::KeyPressed>()->code;

                if (key == sf::Keyboard::Key::Escape)
                    window.close();

                if (key == sf::Keyboard::Key::Num1)
                {
                    currentSim = SimulationType::UniformMotion;
                    for (auto& b : balls)
                        b.velocityX = 250.f;
                }

                if (key == sf::Keyboard::Key::Num2)
                {
                    currentSim = SimulationType::UniformAcceleration;
                    for (auto& b : balls)
                        b.velocityX = 0.f;
                }

                if (key == sf::Keyboard::Key::Num3)
                {
                    currentSim = SimulationType::FreeFall;
                    for (auto& b : balls)
                        b.velocityY = 0.f;
                }

                if (key == sf::Keyboard::Key::B)
                {
                    Ball newBall;
                    newBall.shape.setRadius(20.f);
                    newBall.shape.setFillColor(sf::Color::Yellow);
                    newBall.shape.setPosition(sf::Vector2f(
                        200.f + rand() % 300,
                        100.f
                    ));
                    balls.push_back(newBall);
                }

                if (key == sf::Keyboard::Key::R)
                {
                    balls.clear();
                    Ball newBall;
                    newBall.shape.setRadius(20.f);
                    newBall.shape.setFillColor(sf::Color::Red);
                    newBall.shape.setPosition(sf::Vector2f(startX, startY));
                    balls.push_back(newBall);
                    currentSim = SimulationType::None;
                }
            }
        }

        // ===== PHYSICS UPDATE =====
        for (auto& b : balls)
        {
            if (currentSim == SimulationType::UniformMotion)
                b.shape.move(sf::Vector2f(b.velocityX * dt, 0.f));

            if (currentSim == SimulationType::UniformAcceleration)
            {
                b.velocityX += 400.f * dt;
                b.shape.move(sf::Vector2f(b.velocityX * dt, 0.f));
            }

            if (currentSim == SimulationType::FreeFall)
            {
                b.velocityY += gravity * dt;
                b.shape.move(sf::Vector2f(0.f, b.velocityY * dt));

                if (b.shape.getPosition().y >= ground)
                {
                    auto pos = b.shape.getPosition();
                    pos.y = ground;
                    b.shape.setPosition(pos);
                    b.velocityY = -b.velocityY * 0.8f;
                }
            }

            // Air resistance
            b.velocityX *= airResistance;
            b.velocityY *= airResistance;
        }

        // ===== BALL-TO-BALL COLLISION =====
        for (size_t i = 0; i < balls.size(); ++i)
        {
            for (size_t j = i + 1; j < balls.size(); ++j)
            {
                auto pos1 = balls[i].shape.getPosition();
                auto pos2 = balls[j].shape.getPosition();

                float dx = pos2.x - pos1.x;
                float dy = pos2.y - pos1.y;
                float distance = std::sqrt(dx * dx + dy * dy);
                float minDist = balls[i].shape.getRadius() + balls[j].shape.getRadius();

                if (distance < minDist && distance != 0.f)
                {
                    float overlap = minDist - distance;
                    float nx = dx / distance;
                    float ny = dy / distance;

                    balls[i].shape.move(sf::Vector2f(-nx * overlap / 2.f, -ny * overlap / 2.f));
                    balls[j].shape.move(sf::Vector2f(nx * overlap / 2.f, ny * overlap / 2.f));

                    std::swap(balls[i].velocityX, balls[j].velocityX);
                    std::swap(balls[i].velocityY, balls[j].velocityY);
                }
            }
        }

        // ===== DRAW =====
        window.clear(sf::Color(20, 20, 20));

        // === GRID (SFML 3 FIXED) ===
        const int gridSpacing = 50;

        for (int x = 0; x <= 1000; x += gridSpacing)
        {
            sf::Vertex line[2];
            line[0].position = sf::Vector2f((float)x, 0.f);
            line[0].color = sf::Color(50, 50, 50);
            line[1].position = sf::Vector2f((float)x, 600.f);
            line[1].color = sf::Color(50, 50, 50);

            window.draw(line, 2, sf::PrimitiveType::Lines);
        }

        for (int y = 0; y <= 600; y += gridSpacing)
        {
            sf::Vertex line[2];
            line[0].position = sf::Vector2f(0.f, (float)y);
            line[0].color = sf::Color(50, 50, 50);
            line[1].position = sf::Vector2f(1000.f, (float)y);
            line[1].color = sf::Color(50, 50, 50);

            window.draw(line, 2, sf::PrimitiveType::Lines);
        }

        for (auto& b : balls)
            window.draw(b.shape);

        window.draw(groundLine);

        std::string mode = "Mode: ";
        if (currentSim == SimulationType::None) mode += "None";
        if (currentSim == SimulationType::UniformMotion) mode += "Uniform Motion";
        if (currentSim == SimulationType::UniformAcceleration) mode += "Uniform Acceleration";
        if (currentSim == SimulationType::FreeFall) mode += "Free Fall";

        infoText.setString(mode + "\nBalls: " + std::to_string(balls.size()));
        window.draw(infoText);

        window.display();
    }

    return 0;
}

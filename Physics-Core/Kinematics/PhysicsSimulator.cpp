#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <cmath>
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
    float mass = 1.f;
};

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({1000, 600}),
        "Advanced Physics Simulator"
    );

    window.setFramerateLimit(60);

    const float ground = 550.f;
    const float gravity = 980.f;
    const float airResistance = 0.999f;
    float accelerationX = 400.f;

    bool elasticMode = true;

    SimulationType currentSim = SimulationType::None;
    std::vector<Ball> balls;
    int selectedBallIndex = -1;

    // Initial ball
    Ball ball;
    ball.mass = 2.f;
    ball.shape.setRadius(15.f + ball.mass * 5.f);
    ball.shape.setFillColor(sf::Color::Red);
    ball.shape.setPosition({200.f, 100.f});
    balls.push_back(ball);
    selectedBallIndex = 0;

    sf::Font font;
    if (!font.openFromFile("arial.ttf"))
        return -1;

    sf::Text infoText(font);
    infoText.setCharacterSize(16);
    infoText.setFillColor(sf::Color::White);
    infoText.setPosition({20.f, 20.f});

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
                    newBall.mass = 1.f + rand() % 3;
                    newBall.shape.setRadius(15.f + newBall.mass * 5.f);
                    newBall.shape.setFillColor(sf::Color::Yellow);
                    newBall.shape.setPosition({
                        200.f + float(rand() % 300),
                        100.f
                    });

                    balls.push_back(newBall);
                    selectedBallIndex = balls.size() - 1;
                }

                if (key == sf::Keyboard::Key::E)
                    elasticMode = !elasticMode;

                if (key == sf::Keyboard::Key::R)
                {
                    balls.clear();
                    selectedBallIndex = -1;
                    currentSim = SimulationType::None;
                }

                if (!balls.empty())
                {
                    if (key == sf::Keyboard::Key::Right)
                        selectedBallIndex = (selectedBallIndex + 1) % balls.size();

                    if (key == sf::Keyboard::Key::Left)
                    {
                        selectedBallIndex--;
                        if (selectedBallIndex < 0)
                            selectedBallIndex = balls.size() - 1;
                    }
                }
            }
        }

        // PHYSICS
        for (auto& b : balls)
        {
            if (currentSim == SimulationType::UniformMotion)
                b.shape.move({b.velocityX * dt, 0.f});

            if (currentSim == SimulationType::UniformAcceleration)
            {
                b.velocityX += accelerationX * dt;
                b.shape.move({b.velocityX * dt, 0.f});
            }

            if (currentSim == SimulationType::FreeFall)
            {
                b.velocityY += gravity * dt;
                b.shape.move({0.f, b.velocityY * dt});

                if (b.shape.getPosition().y >= ground)
                {
                    auto pos = b.shape.getPosition();
                    pos.y = ground;
                    b.shape.setPosition(pos);
                    b.velocityY = -b.velocityY * 0.8f;
                }
            }

            b.velocityX *= airResistance;
            b.velocityY *= airResistance;
        }

        // COLLISION
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

                if (distance < minDist && distance > 0.f)
                {
                    float nx = dx / distance;
                    float ny = dy / distance;
                    float overlap = minDist - distance;

                    balls[i].shape.move({-nx * overlap / 2.f, -ny * overlap / 2.f});
                    balls[j].shape.move({nx * overlap / 2.f, ny * overlap / 2.f});
                }
            }
        }

        // DRAW
        window.clear(sf::Color(20, 20, 20));

        // GRID (SFML 3 SAFE)
        const int gridSpacing = 50;

        for (int x = 0; x <= 1000; x += gridSpacing)
        {
            sf::Vertex line[2];
            line[0].position = {float(x), 0.f};
            line[0].color = sf::Color(50,50,50);
            line[1].position = {float(x), 600.f};
            line[1].color = sf::Color(50,50,50);
            window.draw(line, 2, sf::PrimitiveType::Lines);
        }

        for (int y = 0; y <= 600; y += gridSpacing)
        {
            sf::Vertex line[2];
            line[0].position = {0.f, float(y)};
            line[0].color = sf::Color(50,50,50);
            line[1].position = {1000.f, float(y)};
            line[1].color = sf::Color(50,50,50);
            window.draw(line, 2, sf::PrimitiveType::Lines);
        }

        for (size_t i = 0; i < balls.size(); i++)
        {
            if ((int)i == selectedBallIndex)
            {
                balls[i].shape.setOutlineThickness(3.f);
                balls[i].shape.setOutlineColor(sf::Color::Yellow);
            }
            else
                balls[i].shape.setOutlineThickness(0.f);

            window.draw(balls[i].shape);
        }

        std::string info = "Balls: " + std::to_string(balls.size());

        if (!balls.empty() && selectedBallIndex >= 0)
        {
            auto& b = balls[selectedBallIndex];
            float speed = std::sqrt(b.velocityX*b.velocityX + b.velocityY*b.velocityY);
            float ke = 0.5f * b.mass * speed * speed;

            info += "\nSelected Ball: " + std::to_string(selectedBallIndex + 1);
            info += "\nVelocity X: " + std::to_string(b.velocityX);
            info += "\nVelocity Y: " + std::to_string(b.velocityY);
            info += "\nMass: " + std::to_string(b.mass);
            info += "\nKinetic Energy: " + std::to_string(ke);
        }

        infoText.setString(info);
        window.draw(infoText);

        window.display();
    }

    return 0;
}

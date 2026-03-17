#include <SFML/Graphics.hpp>
#include <cmath>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "SHM Simulator");

    // Clock for time
    sf::Clock clock;

    // SHM parameters
    float amplitude = 200.0f;   // pixels
    float omega = 2.0f;         // angular frequency
    float time = 0.0f;

    // Center position
    float centerX = 400.0f;
    float centerY = 300.0f;

    // Mass (circle)
    sf::CircleShape mass(20.0f);
    mass.setFillColor(sf::Color::Cyan);
    mass.setOrigin({20.0f, 20.0f});

    // Spring (line)
    sf::Vertex line[] =
    {
        sf::Vertex(sf::Vector2f(centerX, centerY)),
        sf::Vertex(sf::Vector2f(centerX, centerY))
    };

    while (window.isOpen())
    {
        // Event handling
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // Time update
        float dt = clock.restart().asSeconds();
        time += dt;

        // SHM position
        float x = amplitude * cos(omega * time);

        // Update mass position
        float massX = centerX + x;
        float massY = centerY;

        mass.setPosition({massX, massY});

        // Update spring
        line[0].position = sf::Vector2f(centerX, centerY);
        line[1].position = sf::Vector2f(massX, massY);

        // Render
        window.clear(sf::Color::Black);
        window.draw(line, 2, sf::PrimitiveType::Lines);
        window.draw(mass);
        window.display();
    }

    return 0;
}

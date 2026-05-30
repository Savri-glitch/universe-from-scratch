#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <random>

constexpr float PI = 3.14159265359f;

class Satellite
{
private:
    float orbitRadius;
    float angularSpeed;
    float angle;

    sf::CircleShape body;

public:
    Satellite(float radius, float speed)
        : orbitRadius(radius),
          angularSpeed(speed),
          angle(0.f)
    {
        body.setRadius(4.f);
        body.setFillColor(sf::Color::White);
        body.setOrigin({4.f, 4.f});
    }

    void update(float dt)
    {
        angle += angularSpeed * dt;
    }

    void draw(sf::RenderWindow& window,
              const sf::Vector2f& earthCenter)
    {
        float x = earthCenter.x + orbitRadius * std::cos(angle);
        float y = earthCenter.y + orbitRadius * std::sin(angle);

        body.setPosition({x, y});

        window.draw(body);
    }

    void drawOrbit(sf::RenderWindow& window,
                   const sf::Vector2f& earthCenter)
    {
        sf::CircleShape orbit(orbitRadius);

        orbit.setFillColor(sf::Color::Transparent);
        orbit.setOutlineThickness(1.f);
        orbit.setOutlineColor(sf::Color(70,70,70));

        orbit.setOrigin({orbitRadius, orbitRadius});
        orbit.setPosition(earthCenter);

        window.draw(orbit);
    }
};

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({1400, 900}),
        "Satellite Tracker"
    );

    window.setFramerateLimit(144);

    sf::View camera(window.getDefaultView());
    // EARTH

    sf::CircleShape earth(60.f);

    earth.setFillColor(
        sf::Color(30, 120, 255));

    earth.setOrigin({60.f, 60.f});

    sf::Vector2f earthCenter(700.f, 450.f);
    // STARS
    std::vector<sf::CircleShape> stars;

    std::mt19937 rng(std::random_device{}());

    std::uniform_real_distribution<float>
        xDist(-3000.f, 3000.f);

    std::uniform_real_distribution<float>
        yDist(-3000.f, 3000.f);

    for(int i = 0; i < 2000; i++)
    {
        sf::CircleShape star(1.f);

        star.setFillColor(sf::Color::White);

        star.setPosition(
            {
                xDist(rng),
                yDist(rng)
            });

        stars.push_back(star);
    }
    // SATELLITES
    std::vector<Satellite> satellites;

    satellites.emplace_back(120.f, 1.2f);
    satellites.emplace_back(180.f, 0.8f);
    satellites.emplace_back(260.f, 0.55f);
    satellites.emplace_back(340.f, 0.35f);
    // CAMERA

    bool dragging = false;

    sf::Vector2i previousMouse;

    sf::Clock clock;
    // LOOP
    while(window.isOpen())
    {
        float dt = clock.restart().asSeconds();

        while(const auto event = window.pollEvent())
        {
            if(event->is<sf::Event::Closed>())
            {
                window.close();
            }
            // ZOOM
            if(const auto* wheel =
                event->getIf<sf::Event::MouseWheelScrolled>())
            {
                if(wheel->delta > 0)
                {
                    camera.zoom(0.9f);
                }
                else
                {
                    camera.zoom(1.1f);
                }
            }
            // PAN
            if(event->is<sf::Event::MouseButtonPressed>())
            {
                dragging = true;
                previousMouse =
                    sf::Mouse::getPosition(window);
            }

            if(event->is<sf::Event::MouseButtonReleased>())
            {
                dragging = false;
            }
        }
        // CAMERA DRAGGING

        if(dragging)
        {
            auto current =
                sf::Mouse::getPosition(window);

            sf::Vector2f delta =
                window.mapPixelToCoords(previousMouse)
                -
                window.mapPixelToCoords(current);

            camera.move(delta);

            previousMouse = current;
        }
        // UPDATE

        for(auto& sat : satellites)
        {
            sat.update(dt);
        }
        // DRAW

        window.clear(
            sf::Color(5, 5, 20));

        window.setView(camera);

        for(auto& star : stars)
        {
            window.draw(star);
        }

        for(auto& sat : satellites)
        {
            sat.drawOrbit(window, earthCenter);
        }

        earth.setPosition(earthCenter);

        window.draw(earth);

        for(auto& sat : satellites)
        {
            sat.draw(window, earthCenter);
        }

        window.display();
    }

    return 0;
}

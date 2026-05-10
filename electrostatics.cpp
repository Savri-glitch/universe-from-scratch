#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>

const int WIDTH = 1200;
const int HEIGHT = 800;

const float GRID_SPACING = 40.f;
const float K = 9000.f; // this will work as scaled Coulomb constant

struct Charge {
    sf::Vector2f position;
    float q; // charge here 
};

float magnitude(sf::Vector2f v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

sf::Vector2f normalize(sf::Vector2f v) {
    float mag = magnitude(v);

    if (mag == 0)
        return sf::Vector2f(0.f, 0.f);

    return sf::Vector2f(v.x / mag, v.y / mag);
}

int main() {
    sf::RenderWindow window(
        sf::VideoMode({WIDTH, HEIGHT}),
        "Electric Field Simulator - SFML 3"
    );

    window.setFramerateLimit(60);

    std::vector<Charge> charges;

    // hereee goes the initial charges
    charges.push_back({sf::Vector2f(400.f, 400.f), 1.f});
    charges.push_back({sf::Vector2f(800.f, 400.f), -1.f});

    bool dragging = false;
    int selectedCharge = -1;

    while (window.isOpen()) {

        while (const std::optional event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            // Mouse pressed stuff
            if (const auto* mousePressed =
                    event->getIf<sf::Event::MouseButtonPressed>()) {

                if (mousePressed->button == sf::Mouse::Button::Left) {

                    sf::Vector2f mousePos =
                        window.mapPixelToCoords(mousePressed->position);

                    for (int i = 0; i < charges.size(); i++) {

                        float dx = mousePos.x - charges[i].position.x;
                        float dy = mousePos.y - charges[i].position.y;

                        if (std::sqrt(dx * dx + dy * dy) < 20.f) {
                            dragging = true;
                            selectedCharge = i;
                        }
                    }
                }

                // Right click should add positive charge
                if (mousePressed->button == sf::Mouse::Button::Right) {

                    sf::Vector2f mousePos =
                        window.mapPixelToCoords(mousePressed->position);

                    charges.push_back({mousePos, 1.f});
                }

                // Middle click should add negative charge
                if (mousePressed->button == sf::Mouse::Button::Middle) {

                    sf::Vector2f mousePos =
                        window.mapPixelToCoords(mousePressed->position);

                    charges.push_back({mousePos, -1.f});
                }
            }

            // when mouse is released
            if (event->is<sf::Event::MouseButtonReleased>()) {
                dragging = false;
                selectedCharge = -1;
            }

            // Mouse moved
            if (const auto* mouseMoved =
                    event->getIf<sf::Event::MouseMoved>()) {

                if (dragging && selectedCharge != -1) {

                    sf::Vector2f mousePos =
                        window.mapPixelToCoords(mouseMoved->position);

                    charges[selectedCharge].position = mousePos;
                }
            }
        }

        window.clear(sf::Color(20, 20, 30));

        // Drawing the electric field vectors 
        for (float x = 0; x < WIDTH; x += GRID_SPACING) {

            for (float y = 0; y < HEIGHT; y += GRID_SPACING) {

                sf::Vector2f field(0.f, 0.f);

                for (const auto& charge : charges) {

                    sf::Vector2f r = {
                        x - charge.position.x,
                        y - charge.position.y
                    };

                    float dist = magnitude(r);

                    if (dist < 15.f)
                        continue;

                    sf::Vector2f dir = normalize(r);

                    float strength = (K * charge.q) / (dist * dist);

                    field += dir * strength;
                }

                float fieldMag = magnitude(field);

                if (fieldMag > 0.f) {

                    sf::Vector2f dir = normalize(field);

                    float arrowLength = 15.f;

                    sf::Vertex line[] = {
                        sf::Vertex(
                            sf::Vector2f(x, y),
                            sf::Color::White
                        ),
                        sf::Vertex(
                            sf::Vector2f(
                                x + dir.x * arrowLength,
                                y + dir.y * arrowLength
                            ),
                            sf::Color::Cyan
                        )
                    };

                    window.draw(line, 2, sf::PrimitiveType::Lines);
                }
            }
        }

        // Draw charges here
        for (const auto& charge : charges) {

            sf::CircleShape circle(15.f);
            circle.setOrigin({15.f, 15.f});
            circle.setPosition(charge.position);

            if (charge.q > 0)
                circle.setFillColor(sf::Color::Red);
            else
                circle.setFillColor(sf::Color::Blue);

            window.draw(circle);
        }

        window.display();
    }

    return 0;
}

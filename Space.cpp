#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

using namespace std;

struct Particle {
    sf::Vector2f pos;
    sf::Vector2f vel;
};

float length(sf::Vector2f v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

sf::Vector2f normalize(sf::Vector2f v) {
    float len = length(v);
    if (len == 0) return {0, 0};
    return v / len;
}

int main() {
    sf::RenderWindow window(sf::VideoMode({1000, 1000}), "Spacetime Grid");

    // Mass (center)
    sf::Vector2f massPos(500, 500);
    float massStrength = 20000.0f;

    // Particles
    vector<Particle> particles;

    for (int i = 0; i < 50; i++) {
        particles.push_back({
            sf::Vector2f(rand() % 1000, rand() % 1000),
            sf::Vector2f(0, 0)
        });
    }

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            // Click to move mass
            if (event->is<sf::Event::MouseButtonPressed>()) {
                massPos = (sf::Vector2f)sf::Mouse::getPosition(window);
            }
        }

        window.clear(sf::Color::Black);

        // GRID DRAW
        int spacing = 40;

        for (int x = 0; x <= 1000; x += spacing) {
            sf::VertexArray line(sf::PrimitiveType::LineStrip);

            for (int y = 0; y <= 1000; y += 10) {
                sf::Vector2f p(x, y);

                // displacement based on gravity well
                sf::Vector2f dir = p - massPos;
                float dist = length(dir) + 1;

                float factor = -massStrength / (dist * dist);

                sf::Vector2f offset = normalize(dir) * factor;

                line.append(sf::Vertex(p + offset, sf::Color(100, 100, 255)));
            }

            window.draw(line);
        }

        for (int y = 0; y <= 1000; y += spacing) {
            sf::VertexArray line(sf::PrimitiveType::LineStrip);

            for (int x = 0; x <= 1000; x += 10) {
                sf::Vector2f p(x, y);

                sf::Vector2f dir = p - massPos;
                float dist = length(dir) + 1;

                float factor = -massStrength / (dist * dist);
                sf::Vector2f offset = normalize(dir) * factor;

                line.append(sf::Vertex(p + offset, sf::Color(100, 100, 255)));
            }

            window.draw(line);
        }

        // PARTICLES
        for (auto &p : particles) {
            sf::Vector2f dir = massPos - p.pos;
            float dist = length(dir) + 1;

            // gravitational acceleration
            float force = massStrength / (dist * dist);
            sf::Vector2f acc = normalize(dir) * force;

            p.vel += acc * 0.01f;
            p.pos += p.vel;

            // draw particle
            sf::CircleShape dot(2);
            dot.setPosition(p.pos);
            dot.setFillColor(sf::Color::White);
            window.draw(dot);
        }

        // MASS VISUAL 
        sf::CircleShape mass(8);
        mass.setOrigin({8, 8});
        mass.setPosition(massPos);
        mass.setFillColor(sf::Color::Red);
        window.draw(mass);

        window.display();
    }

    return 0;
}

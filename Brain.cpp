#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <random>

using namespace std;

// Random generator
std::mt19937 rng(std::random_device{}());
float randf(float a, float b) {
    return std::uniform_real_distribution<float>(a, b)(rng);
}

struct Thought {
    sf::Vector2f pos;
    sf::Vector2f vel;
    float radius;
    float chaos; // how unstable the thought is

    Thought(float x, float y) {
        pos = {x, y};
        vel = {randf(-50, 50), randf(-50, 50)};
        radius = randf(3, 6);
        chaos = randf(0.5f, 2.0f);
    }

    void update(float dt, sf::Vector2u windowSize) {
        // Random jitter (chaos)
        vel.x += randf(-10, 10) * chaos * dt;
        vel.y += randf(-10, 10) * chaos * dt;

        pos += vel * dt;

        // Bounce off walls
        if (pos.x < 0 || pos.x > windowSize.x) vel.x *= -1;
        if (pos.y < 0 || pos.y > windowSize.y) vel.y *= -1;
    }
};

float distance(sf::Vector2f a, sf::Vector2f b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return sqrt(dx * dx + dy * dy);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(900, 700), "Mental Chaos Engine");
    window.setFramerateLimit(60);

    vector<Thought> thoughts;

    // Initial thoughts
    for (int i = 0; i < 80; i++) {
        thoughts.emplace_back(randf(0, 900), randf(0, 700));
    }

    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        // Events
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            // Click = chaos burst
            if (event->is<sf::Event::MouseButtonPressed>()) {
                auto mouse = sf::Mouse::getPosition(window);

                for (auto &t : thoughts) {
                    float d = distance(t.pos, (sf::Vector2f)mouse);
                    if (d < 150) {
                        // push away from click
                        sf::Vector2f dir = t.pos - (sf::Vector2f)mouse;
                        float len = sqrt(dir.x*dir.x + dir.y*dir.y);
                        if (len != 0) dir /= len;

                        t.vel += dir * randf(100, 300);
                        t.chaos += 0.5f; // increase instability
                    }
                }
            }
        }

        // Update
        for (auto &t : thoughts) {
            t.update(dt, window.getSize());
        }

        // Render
        window.clear(sf::Color::Black);

        // Draw connections
        for (size_t i = 0; i < thoughts.size(); i++) {
            for (size_t j = i + 1; j < thoughts.size(); j++) {
                float d = distance(thoughts[i].pos, thoughts[j].pos);

                if (d < 120) {
                    sf::Vertex line[] = {
                        sf::Vertex(thoughts[i].pos, sf::Color(100, 100, 255, 100)),
                        sf::Vertex(thoughts[j].pos, sf::Color(100, 100, 255, 100))
                    };
                    window.draw(line, 2, sf::PrimitiveType::Lines);
                }
            }
        }

        // Draw thoughts
        for (auto &t : thoughts) {
            sf::CircleShape circle(t.radius);
            circle.setPosition(t.pos - sf::Vector2f(t.radius, t.radius));
            circle.setFillColor(sf::Color(200, 200, 255));
            window.draw(circle);
        }

        window.display();
    }

    return 0;
}

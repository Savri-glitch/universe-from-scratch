#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>

using namespace std;

std::mt19937 rng(std::random_device{}());
float randf(float a, float b) { return std::uniform_real_distribution<float>(a, b)(rng); }

struct Pulse {
    sf::Vector2f start, end;
    float progress = 0.0f;
    float speed = 2.0f;
};

struct Thought {
    sf::Vector2f pos;
    sf::Vector2f vel;
    float radius;
    float signal = 0.0f;
    vector<size_t> neighbors;

    Thought(float x, float y) {
        pos = {x, y};
        vel = {randf(-20, 20), randf(-20, 20)};
        radius = randf(2, 4);
    }

    void update(float dt, sf::Vector2u windowSize) {
        pos += vel * dt;
        if (pos.x < 0 || pos.x > (float)windowSize.x) vel.x *= -1;
        if (pos.y < 0 || pos.y > (float)windowSize.y) vel.y *= -1;
        signal *= 0.92f; // Fade signal
        vel *= 0.995f;  // Natural friction
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode({900, 700}), "Neural Pulse Engine");
    window.setFramerateLimit(60);

    vector<Thought> thoughts;
    vector<Pulse> pulses;
    for (int i = 0; i < 120; i++) thoughts.emplace_back(randf(0, 900), randf(0, 700));

    sf::RectangleShape fadeRect;
    fadeRect.setSize({900, 700});
    fadeRect.setFillColor(sf::Color(0, 0, 0, 50));

    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            
            if (event->is<sf::Event::MouseButtonPressed>()) {
                auto mouse = (sf::Vector2f)sf::Mouse::getPosition(window);
                for (size_t i = 0; i < thoughts.size(); i++) {
                    float dx = thoughts[i].pos.x - mouse.x;
                    float dy = thoughts[i].pos.y - mouse.y;
                    if (dx*dx + dy*dy < 100*100) {
                        thoughts[i].signal = 1.0f;
                        // Fire pulses to neighbors
                        for (auto nIdx : thoughts[i].neighbors) {
                            pulses.push_back({thoughts[i].pos, thoughts[nIdx].pos, 0.0f, randf(1.5, 3.0)});
                        }
                    }
                }
            }
        }

        // 1. Refresh Connections (The "Plasticity")
        for (auto& t : thoughts) t.neighbors.clear();
        for (size_t i = 0; i < thoughts.size(); i++) {
            for (size_t j = i + 1; j < thoughts.size(); j++) {
                sf::Vector2f diff = thoughts[j].pos - thoughts[i].pos;
                float d2 = diff.x*diff.x + diff.y*diff.y;
                if (d2 < 90 * 90) {
                    thoughts[i].neighbors.push_back(j);
                    thoughts[j].neighbors.push_back(i);
                    // Attraction physics
                    float dist = sqrt(d2);
                    sf::Vector2f dir = diff / dist;
                    float force = (dist - 60.0f) * 0.2f;
                    thoughts[i].vel += dir * force * dt;
                    thoughts[j].vel -= dir * force * dt;
                }
            }
        }

        for (auto& t : thoughts) t.update(dt, window.getSize());

        // 2. Update Pulses
        for (auto it = pulses.begin(); it != pulses.end();) {
            it->progress += it->speed * dt;
            if (it->progress >= 1.0f) it = pulses.erase(it);
            else ++it;
        }

        // --- DRAW ---
        window.draw(fadeRect);
        sf::RenderStates stateAdd; stateAdd.blendMode = sf::BlendAdd;

        // Draw Lines
        for (size_t i = 0; i < thoughts.size(); i++) {
            for (auto nIdx : thoughts[i].neighbors) {
                if (nIdx > i) {
                    sf::Vertex line[] = { 
                        {thoughts[i].pos, sf::Color(30, 50, 100, 100)}, 
                        {thoughts[nIdx].pos, sf::Color(30, 50, 100, 100)} 
                    };
                    window.draw(line, 2, sf::PrimitiveType::Lines, stateAdd);
                }
            }
        }

        // Draw Pulse Particles
        for (const auto& p : pulses) {
            sf::CircleShape pDot(2.0f);
            pDot.setFillColor(sf::Color(100, 200, 255));
            pDot.setPosition(p.start + (p.end - p.start) * p.progress);
            window.draw(pDot, stateAdd);
        }

        // Draw Nodes
        for (const auto& t : thoughts) {
            sf::CircleShape c(t.radius + t.signal * 3);
            c.setOrigin({c.getRadius(), c.getRadius()});
            c.setPosition(t.pos);
            c.setFillColor(sf::Color(50 + (int)(t.signal * 205), 100 + (int)(t.signal * 100), 255));
            window.draw(c, stateAdd);
        }

        window.display();
    }
    return 0;
}

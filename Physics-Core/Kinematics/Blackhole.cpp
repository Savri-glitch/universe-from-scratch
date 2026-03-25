#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>

using namespace std;

const float G = 500.0f;          // boosted gravity
const float BH_mass = 10000.0f;
const float EVENT_HORIZON = 10.0f;

struct Particle {
    sf::Vector2f pos;
    sf::Vector2f vel;
    bool alive = true;
};

float length(sf::Vector2f v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 800), "Black Hole Simulation");

    vector<Particle> particles;

    // create particles
    for(int i = 0; i < 300; i++) {
        Particle p;
        p.pos = sf::Vector2f(rand() % 800, rand() % 800);
        p.vel = sf::Vector2f((rand()%10 - 5) * 0.2f, (rand()%10 - 5) * 0.2f);
        particles.push_back(p);
    }

    sf::Vector2f bh_pos(400, 400);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);

        // draw black hole
        sf::CircleShape bh(10);
        bh.setFillColor(sf::Color::White);
        bh.setOrigin(10,10);
        bh.setPosition(bh_pos);
        window.draw(bh);

        for(auto &p : particles) {
            if(!p.alive) continue;

            sf::Vector2f dir = bh_pos - p.pos;
            float r = length(dir);

            if(r < EVENT_HORIZON) {
                p.alive = false;
                continue;
            }

            // normalize direction
            dir /= r;

            float force = G * BH_mass / (r * r);

            // update velocity
            p.vel += dir * force * 0.0001f;

            // update position
            p.pos += p.vel;

            // draw particle
            sf::CircleShape dot(2);
            dot.setFillColor(sf::Color::Cyan);
            dot.setPosition(p.pos);
            window.draw(dot);
        }

        window.display();
    }

    return 0;
}

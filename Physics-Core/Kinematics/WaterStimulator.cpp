#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>

struct Particle
{
    sf::Vector2f pos;
    sf::Vector2f vel;
};

float length(sf::Vector2f v)
{
    return std::sqrt(v.x*v.x + v.y*v.y);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode({900,700}), "Particle Water Simulator");
    window.setFramerateLimit(60);

    const int particleCount = 500;

    const float gravity = 900.f;
    const float interactionRadius = 20.f;
    const float stiffness = 0.4f;
    const float damping = 0.995f;

    std::vector<Particle> particles;

    // Spawn water blob
    for(int i=0;i<particleCount;i++)
    {
        Particle p;

        p.pos =
        {
            350 + float(rand()%200),
            100 + float(rand()%200)
        };

        p.vel = {0,0};

        particles.push_back(p);
    }

    sf::CircleShape shape(3.f);
    shape.setFillColor(sf::Color(80,150,255));

    sf::Clock clock;

    while(window.isOpen())
    {
        float dt = clock.restart().asSeconds();

        while(auto event = window.pollEvent())
        {
            if(event->is<sf::Event::Closed>())
                window.close();
        }

        // Apply gravity
        for(auto &p : particles)
        {
            p.vel.y += gravity * dt;
        }

        // Particle interaction (fluid-like behavior)
        for(size_t i=0;i<particles.size();i++)
        {
            for(size_t j=i+1;j<particles.size();j++)
            {
                sf::Vector2f diff = particles[j].pos - particles[i].pos;
                float dist = length(diff);

                if(dist < interactionRadius && dist > 0)
                {
                    sf::Vector2f dir = diff / dist;

                    float force = (interactionRadius - dist) * stiffness;

                    particles[i].vel -= dir * force;
                    particles[j].vel += dir * force;
                }
            }
        }

        // Integrate motion
        for(auto &p : particles)
        {
            p.vel *= damping;
            p.pos += p.vel * dt;

            // Walls
            if(p.pos.x < 10)
            {
                p.pos.x = 10;
                p.vel.x *= -0.5f;
            }

            if(p.pos.x > 890)
            {
                p.pos.x = 890;
                p.vel.x *= -0.5f;
            }

            if(p.pos.y > 690)
            {
                p.pos.y = 690;
                p.vel.y *= -0.5f;
            }

            if(p.pos.y < 10)
            {
                p.pos.y = 10;
                p.vel.y *= -0.5f;
            }
        }

        window.clear(sf::Color(15,15,30));

        for(auto &p : particles)
        {
            shape.setPosition(p.pos);
            window.draw(shape);
        }

        window.display();
    }

    return 0;
}

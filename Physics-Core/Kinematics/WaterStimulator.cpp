#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

struct Particle
{
    sf::Vector2f pos;
    sf::Vector2f vel;
    float density = 0;
    float pressure = 0;
};

float length(sf::Vector2f v)
{
    return std::sqrt(v.x*v.x + v.y*v.y);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode({900,700}), "Particle Water Simulator");
    window.setFramerateLimit(60);

    const int particleCount = 600;

    const float restDensity = 1000.f;
    const float gasConstant = 2000.f;
    const float viscosity = 0.1f;

    const float smoothingRadius = 16.f;
    const float mass = 1.f;

    const float gravity = 1200.f;

    std::vector<Particle> particles;

    for(int i=0;i<particleCount;i++)
    {
        Particle p;

        p.pos =
        {
            200 + float(rand()%300),
            100 + float(rand()%300)
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

        // Density calculation
        for(auto &pi : particles)
        {
            pi.density = 0;

            for(auto &pj : particles)
            {
                float r = length(pi.pos - pj.pos);

                if(r < smoothingRadius)
                {
                    float term = (smoothingRadius*smoothingRadius - r*r);
                    pi.density += mass * term * term * term;
                }
            }

            pi.pressure = gasConstant * (pi.density - restDensity);
        }

        // Forces
        for(auto &pi : particles)
        {
            sf::Vector2f pressureForce(0,0);
            sf::Vector2f viscosityForce(0,0);

            for(auto &pj : particles)
            {
                if(&pi == &pj) continue;

                sf::Vector2f rij = pj.pos - pi.pos;
                float r = length(rij);

                if(r < smoothingRadius && r > 0)
                {
                    sf::Vector2f dir = rij / r;

                    pressureForce += -dir * mass *
                        (pi.pressure + pj.pressure) / (2 * pj.density);

                    viscosityForce += viscosity *
                        (pj.vel - pi.vel);
                }
            }

            sf::Vector2f gravityForce(0, gravity * pi.density);

            sf::Vector2f force =
                pressureForce +
                viscosityForce +
                gravityForce;

            pi.vel += force / pi.density * dt;
        }

        // Integrate motion
        for(auto &p : particles)
        {
            p.pos += p.vel * dt;

            // Container walls
            if(p.pos.x < 10)
            {
                p.pos.x = 10;
                p.vel.x *= -0.5;
            }

            if(p.pos.x > 890)
            {
                p.pos.x = 890;
                p.vel.x *= -0.5;
            }

            if(p.pos.y > 690)
            {
                p.pos.y = 690;
                p.vel.y *= -0.5;
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

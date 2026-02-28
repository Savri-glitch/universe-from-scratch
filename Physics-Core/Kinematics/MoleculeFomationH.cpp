#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

struct Atom {
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float mass = 1.f;
    float radius = 8.f;
    int valence = 1;
    int bonds = 0;
};

struct Bond {
    int a;
    int b;
};

float distance(sf::Vector2f a, sf::Vector2f b)
{
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return std::sqrt(dx * dx + dy * dy);
}

int main()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    sf::RenderWindow window(sf::VideoMode({1000, 700}), "Molecule Formation Simulator");
    window.setFramerateLimit(60);

    const int atomCount = 25;
    const float bondingDistance = 25.f;
    const float bondLength = 20.f;
    const float temperature = 20.f;

    std::vector<Atom> atoms;
    std::vector<Bond> bonds;

    // Create atoms
    for (int i = 0; i < atomCount; i++)
    {
        Atom atom;
        atom.shape.setRadius(atom.radius);
        atom.shape.setOrigin({atom.radius, atom.radius});
        atom.shape.setFillColor(sf::Color::Cyan);

        atom.shape.setPosition({
            float(100 + rand() % 800),
            float(100 + rand() % 500)
        });

        atom.velocity = {
            float(rand() % 100 - 50),
            float(rand() % 100 - 50)
        };

        atoms.push_back(atom);
    }

    sf::Clock clock;

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();

        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // GAS-LIKE RANDOM MOTION
        for (auto& atom : atoms)
        {
            atom.velocity.x += (rand() % 3 - 1) * temperature * dt;
            atom.velocity.y += (rand() % 3 - 1) * temperature * dt;

            atom.shape.move(atom.velocity * dt);

            // Wall bounce
            auto pos = atom.shape.getPosition();
            if (pos.x < 0 || pos.x > 1000)
                atom.velocity.x *= -1;
            if (pos.y < 0 || pos.y > 700)
                atom.velocity.y *= -1;
        }

        // CHECK BONDING
        for (size_t i = 0; i < atoms.size(); i++)
        {
            for (size_t j = i + 1; j < atoms.size(); j++)
            {
                if (atoms[i].bonds >= atoms[i].valence) continue;
                if (atoms[j].bonds >= atoms[j].valence) continue;

                float d = distance(atoms[i].shape.getPosition(),
                                   atoms[j].shape.getPosition());

                if (d < bondingDistance)
                {
                    bonds.push_back({(int)i, (int)j});
                    atoms[i].bonds++;
                    atoms[j].bonds++;
                }
            }
        }

        // MAINTAIN BOND LENGTH
        for (auto& bond : bonds)
        {
            auto& a = atoms[bond.a];
            auto& b = atoms[bond.b];

            sf::Vector2f posA = a.shape.getPosition();
            sf::Vector2f posB = b.shape.getPosition();

            float dx = posB.x - posA.x;
            float dy = posB.y - posA.y;
            float d = std::sqrt(dx * dx + dy * dy);

            if (d == 0) continue;

            float diff = d - bondLength;
            float nx = dx / d;
            float ny = dy / d;

            a.shape.move({ nx * diff * 0.5f, ny * diff * 0.5f });
            b.shape.move({-nx * diff * 0.5f, -ny * diff * 0.5f });
        }

        // DRAW
        window.clear(sf::Color(15, 15, 30));

        // Draw bonds
        for (auto& bond : bonds)
        {
            sf::Vertex line[2];
            line[0].position = atoms[bond.a].shape.getPosition();
            line[0].color = sf::Color::White;
            line[1].position = atoms[bond.b].shape.getPosition();
            line[1].color = sf::Color::White;

            window.draw(line, 2, sf::PrimitiveType::Lines);
        }

        // Draw atoms
        for (auto& atom : atoms)
            window.draw(atom.shape);

        window.display();
    }

    return 0;
}

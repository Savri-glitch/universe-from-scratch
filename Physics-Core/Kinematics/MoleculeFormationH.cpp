#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

struct Atom
{
    sf::CircleShape shape;
    sf::Vector3f pos;
    sf::Vector3f velocity;

    float radius = 8.f;
    int valence = 1;
    int bonds = 0;

    std::vector<sf::Vector3f> trail;
};

struct Bond
{
    int a;
    int b;
};

float distance3D(sf::Vector3f a, sf::Vector3f b)
{
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    float dz = b.z - a.z;
    return std::sqrt(dx*dx + dy*dy + dz*dz);
}

int main()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    sf::RenderWindow window(
        sf::VideoMode({1000,700}),
        "Hydrogen Molecule Formation Simulator"
    );

    window.setFramerateLimit(60);

    const int atomCount = 25;

    float bondingDistance = 25.f;
    float bondLength = 20.f;
    float temperature = 20.f;

    float epsilon = 50.f;
    float sigma = 20.f;

    float cameraDistance = 400.f;
    float cameraAngle = 0.f;

    const float minDistance = 10.f;
    const float damping = 0.995f;
    const float maxSpeed = 200.f;

    std::vector<Atom> atoms;
    std::vector<Bond> bonds;
    std::vector<float> energyHistory;

    for(int i=0;i<atomCount;i++)
    {
        Atom atom;

        atom.shape.setRadius(atom.radius);
        atom.shape.setOrigin({atom.radius,atom.radius});

        atom.pos =
        {
            float(rand()%800 - 400),
            float(rand()%500 - 250),
            float(rand()%200 - 100)
        };

        atom.velocity =
        {
            float(rand()%100 - 50),
            float(rand()%100 - 50),
            float(rand()%100 - 50)
        };

        atoms.push_back(atom);
    }

    sf::Clock clock;

    while(window.isOpen())
    {
        float dt = clock.restart().asSeconds();

        while(auto event = window.pollEvent())
        {
            if(event->is<sf::Event::Closed>())
                window.close();
        }

        // CAMERA CONTROLS
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            cameraAngle -= 1.5f * dt;

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            cameraAngle += 1.5f * dt;

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
            cameraDistance -= 200 * dt;

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E))
            cameraDistance += 200 * dt;

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
            temperature += 5 * dt;

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
            temperature -= 5 * dt;

        float totalEnergy = 0;

        // Lennard-Jones forces
        for(size_t i=0;i<atoms.size();i++)
        {
            for(size_t j=i+1;j<atoms.size();j++)
            {
                float d = distance3D(atoms[i].pos, atoms[j].pos);

                if(d < minDistance)
                    d = minDistance;

                float sr = sigma/d;
                float sr6 = pow(sr,6);
                float sr12 = sr6*sr6;

                float potential = 4*epsilon*(sr12 - sr6);
                totalEnergy += potential;

                float force = 24*epsilon*(2*sr12 - sr6)/(d*d);

                sf::Vector3f dir =
                {
                    (atoms[j].pos.x - atoms[i].pos.x)/d,
                    (atoms[j].pos.y - atoms[i].pos.y)/d,
                    (atoms[j].pos.z - atoms[i].pos.z)/d
                };

                atoms[i].velocity += dir * force * dt;
                atoms[j].velocity -= dir * force * dt;
            }
        }

        energyHistory.push_back(totalEnergy);
        if(energyHistory.size() > 800)
            energyHistory.erase(energyHistory.begin());

        // MOTION
        for(auto& atom:atoms)
        {
            atom.velocity.x += (rand()%3 -1)*temperature*dt;
            atom.velocity.y += (rand()%3 -1)*temperature*dt;
            atom.velocity.z += (rand()%3 -1)*temperature*dt;

            atom.velocity *= damping;

            float speed = std::sqrt(
                atom.velocity.x*atom.velocity.x +
                atom.velocity.y*atom.velocity.y +
                atom.velocity.z*atom.velocity.z
            );

            if(speed > maxSpeed)
                atom.velocity *= maxSpeed/speed;

            atom.pos += atom.velocity * dt;

            // TRAIL
            atom.trail.push_back(atom.pos);
            if(atom.trail.size() > 20)
                atom.trail.erase(atom.trail.begin());
        }

        // BOND FORMATION
        for(size_t i=0;i<atoms.size();i++)
        {
            for(size_t j=i+1;j<atoms.size();j++)
            {
                if(atoms[i].bonds >= atoms[i].valence) continue;
                if(atoms[j].bonds >= atoms[j].valence) continue;

                float d = distance3D(atoms[i].pos,atoms[j].pos);

                if(d < bondingDistance)
                {
                    bonds.push_back({(int)i,(int)j});
                    atoms[i].bonds++;
                    atoms[j].bonds++;
                }
            }
        }

        // MAINTAIN BOND LENGTH
        for(auto& bond:bonds)
        {
            auto& a = atoms[bond.a];
            auto& b = atoms[bond.b];

            float d = distance3D(a.pos,b.pos);
            if(d == 0) continue;

            float diff = d - bondLength;

            sf::Vector3f dir =
            {
                (b.pos.x - a.pos.x)/d,
                (b.pos.y - a.pos.y)/d,
                (b.pos.z - a.pos.z)/d
            };

            a.pos += dir*diff*0.5f;
            b.pos -= dir*diff*0.5f;
        }

        window.clear(sf::Color(15,15,30));

        // ENERGY GRAPH BACKGROUND
        sf::RectangleShape graphBG({1000,120});
        graphBG.setPosition({0,580});
        graphBG.setFillColor(sf::Color(25,25,40));
        window.draw(graphBG);

        float maxEnergy = -1e9;
        float minEnergy = 1e9;

        for(float e : energyHistory)
        {
            if(e > maxEnergy) maxEnergy = e;
            if(e < minEnergy) minEnergy = e;
        }

        float range = maxEnergy - minEnergy;
        if(range == 0) range = 1;

        for(size_t i=1;i<energyHistory.size();i++)
        {
            float x1 = (i-1)*(1000.f/energyHistory.size());
            float x2 = i*(1000.f/energyHistory.size());

            float y1 = 690 - ((energyHistory[i-1]-minEnergy)/range)*110;
            float y2 = 690 - ((energyHistory[i]-minEnergy)/range)*110;

            sf::Vertex line[2];

            line[0].position = {x1,y1};
            line[1].position = {x2,y2};

            line[0].color = sf::Color::Green;
            line[1].color = sf::Color::Green;

            window.draw(line,2,sf::PrimitiveType::Lines);
        }

        float cosA = cos(cameraAngle);
        float sinA = sin(cameraAngle);

        // DRAW BONDS (GLOW)
        for(auto& bond:bonds)
        {
            sf::Vector3f pa = atoms[bond.a].pos;
            sf::Vector3f pb = atoms[bond.b].pos;

            float ax = pa.x*cosA - pa.z*sinA;
            float az = pa.x*sinA + pa.z*cosA;

            float bx = pb.x*cosA - pb.z*sinA;
            float bz = pb.x*sinA + pb.z*cosA;

            float scaleA = cameraDistance/(cameraDistance + az);
            float scaleB = cameraDistance/(cameraDistance + bz);

            sf::Vector2f A = {500 + ax*scaleA, 350 + pa.y*scaleA};
            sf::Vector2f B = {500 + bx*scaleB, 350 + pb.y*scaleB};

            for(int i=3;i>=1;i--)
            {
                sf::Vertex line[2];

                line[0].position = A;
                line[1].position = B;

                int alpha = 60 + i*60;

                line[0].color = sf::Color(120,200,255,alpha);
                line[1].color = sf::Color(120,200,255,alpha);

                window.draw(line,2,sf::PrimitiveType::Lines);
            }
        }

        // DRAW ATOMS
        for(auto& atom:atoms)
        {
            float rx = atom.pos.x*cosA - atom.pos.z*sinA;
            float rz = atom.pos.x*sinA + atom.pos.z*cosA;

            float scale = cameraDistance/(cameraDistance + rz);

            float screenX = 500 + rx*scale;
            float screenY = 350 + atom.pos.y*scale;

            // ENERGY COLOR
            float speed = std::sqrt(
                atom.velocity.x*atom.velocity.x +
                atom.velocity.y*atom.velocity.y +
                atom.velocity.z*atom.velocity.z
            );

            int r = std::min(255,(int)(speed*2));
            int b = 255-r;

            atom.shape.setFillColor(sf::Color(r,120,b));

            // ELECTRON CLOUD
            for(int i=3;i>=1;i--)
            {
                float r = atom.radius*(i*1.8f);

                sf::CircleShape cloud(r);
                cloud.setOrigin({r,r});
                cloud.setPosition({screenX,screenY});
                cloud.setFillColor(sf::Color(120,120,255,30+i*20));
                cloud.setScale({scale,scale});

                window.draw(cloud);
            }

            atom.shape.setPosition({screenX,screenY});
            atom.shape.setScale({scale,scale});

            window.draw(atom.shape);
        }

        window.display();
    }

    return 0;
}

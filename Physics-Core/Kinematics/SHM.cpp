#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <sstream>

int main()
{
    sf::RenderWindow window(sf::VideoMode({1200, 700}), "SHM Dashboard");

    sf::Clock clock;

    float A = 150.0f;
    float omega = 2.0f;
    float time = 0.0f;

    float centerX = 300.0f;
    float centerY = 350.0f;

    // FONT
    sf::Font font;
    if (!font.openFromFile("arial.ttf"))
        return -1;

    // TEXT SETUP
    sf::Text text(font);
    text.setCharacterSize(14);
    text.setFillColor(sf::Color::White);

    sf::CircleShape mass(12.0f);
    mass.setFillColor(sf::Color::Cyan);
    mass.setOrigin({12.0f, 12.0f});

    sf::Vertex line[2];
    line[0].position = {centerX, centerY};

    std::vector<float> KE, PE, pos;
    std::vector<sf::Vector2f> phase;

    const int maxPoints = 500;

    while (window.isOpen())
    {
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        float dt = clock.restart().asSeconds();
        time += dt;

        float x = A * cos(omega * time);
        float v = -A * omega * sin(omega * time);

        float KE_val = 0.5f * v * v;
        float PE_val = 0.5f * omega * omega * x * x;
        float totalE = 0.5f * omega * omega * A * A;

        KE.push_back(KE_val / totalE);
        PE.push_back(PE_val / totalE);
        pos.push_back(x / A);
        phase.push_back({x / A, v / (A * omega)});

        if (KE.size() > maxPoints)
        {
            KE.erase(KE.begin());
            PE.erase(PE.begin());
            pos.erase(pos.begin());
        }

        if (phase.size() > maxPoints)
            phase.erase(phase.begin());

        float massX = centerX + x;
        mass.setPosition({massX, centerY});
        line[1].position = {massX, centerY};

        window.clear(sf::Color(20,20,20));

        // ================= LEFT: MOTION =================
        window.draw(line, 2, sf::PrimitiveType::Lines);
        window.draw(mass);

        text.setString("SHM Motion");
        text.setPosition({200, 50});
        window.draw(text);

        // Live values
        std::stringstream ss;
        ss << "x: " << x << "\nv: " << v;
        text.setString(ss.str());
        text.setPosition({150, 550});
        window.draw(text);

        // ================= ENERGY GRAPH =================
        float gx = 650, gy = 50, gw = 500, gh = 150;

        sf::RectangleShape box1({gw, gh});
        box1.setPosition({gx, gy});
        box1.setOutlineThickness(1);
        box1.setOutlineColor(sf::Color::White);
        box1.setFillColor(sf::Color::Transparent);
        window.draw(box1);

        text.setString("Energy (KE Red, PE Green)");
        text.setPosition({gx, gy - 20});
        window.draw(text);

        // Y axis labels
        text.setString("1");
        text.setPosition({gx - 20, gy});
        window.draw(text);

        text.setString("0");
        text.setPosition({gx - 20, gy + gh - 10});
        window.draw(text);

        sf::VertexArray KEline(sf::PrimitiveType::LineStrip);
        sf::VertexArray PEline(sf::PrimitiveType::LineStrip);

        for (int i = 0; i < KE.size(); i++)
        {
            float px = gx + (i * gw / maxPoints);
            float KEy = gy + gh - KE[i] * gh;
            float PEy = gy + gh - PE[i] * gh;

            KEline.append(sf::Vertex{{px, KEy}, sf::Color::Red});
            PEline.append(sf::Vertex{{px, PEy}, sf::Color::Green});
        }

        window.draw(KEline);
        window.draw(PEline);

        // ================= SINE GRAPH =================
        float sy = 250;

        sf::RectangleShape box2({gw, gh});
        box2.setPosition({gx, sy});
        box2.setOutlineThickness(1);
        box2.setOutlineColor(sf::Color::White);
        box2.setFillColor(sf::Color::Transparent);
        window.draw(box2);

        text.setString("Position vs Time");
        text.setPosition({gx, sy - 20});
        window.draw(text);

        text.setString("1");
        text.setPosition({gx - 20, sy});
        window.draw(text);

        text.setString("-1");
        text.setPosition({gx - 25, sy + gh - 10});
        window.draw(text);

        sf::VertexArray sineLine(sf::PrimitiveType::LineStrip);

        for (int i = 0; i < pos.size(); i++)
        {
            float px = gx + (i * gw / maxPoints);
            float py = sy + gh/2 - pos[i] * (gh/2);

            sineLine.append(sf::Vertex{{px, py}, sf::Color::Cyan});
        }

        window.draw(sineLine);

        // ================= PHASE SPACE =================
        float px0 = 650, py0 = 450, pw = 500, ph = 200;

        sf::RectangleShape box3({pw, ph});
        box3.setPosition({px0, py0});
        box3.setOutlineThickness(1);
        box3.setOutlineColor(sf::Color::White);
        box3.setFillColor(sf::Color::Transparent);
        window.draw(box3);

        text.setString("Phase Space (x vs v)");
        text.setPosition({px0, py0 - 20});
        window.draw(text);

        text.setString("x");
        text.setPosition({px0 + pw - 20, py0 + ph/2});
        window.draw(text);

        text.setString("v");
        text.setPosition({px0 + pw/2, py0});
        window.draw(text);

        sf::VertexArray phasePlot(sf::PrimitiveType::Points);

        for (auto &p : phase)
        {
            float px = px0 + pw/2 + p.x * (pw/2 - 10);
            float py = py0 + ph/2 - p.y * (ph/2 - 10);

            phasePlot.append(sf::Vertex{{px, py}, sf::Color::Yellow});
        }

        window.draw(phasePlot);

        window.display();
    }

    return 0;
}

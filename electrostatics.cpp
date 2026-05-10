#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <optional>
#include <algorithm>

const int WIDTH = 1400;
const int HEIGHT = 900;

const float K = 5000.f;
const float GRID = 45.f;

struct Charge {
    sf::Vector2f pos;
    float q;
};

std::vector<Charge> charges;

float magnitude(const sf::Vector2f& v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

sf::Vector2f normalize(const sf::Vector2f& v) {

    float mag = magnitude(v);

    if (mag == 0.f)
        return {0.f, 0.f};

    return {v.x / mag, v.y / mag};
}

sf::Vector2f electricField(sf::Vector2f p) {

    sf::Vector2f E(0.f, 0.f);

    for (const auto& c : charges) {

        sf::Vector2f r = p - c.pos;

        float dist = magnitude(r);

        if (dist < 12.f)
            continue;

        sf::Vector2f dir = normalize(r);

        float strength =
            (K * c.q) / (dist * dist);

        E += dir * strength;
    }

    return E;
}

void drawGrid(
    sf::RenderWindow& window,
    sf::View& camera
) {

    sf::Vector2f center = camera.getCenter();
    sf::Vector2f size = camera.getSize();

    float left = center.x - size.x / 2.f;
    float right = center.x + size.x / 2.f;

    float top = center.y - size.y / 2.f;
    float bottom = center.y + size.y / 2.f;

    sf::VertexArray lines(sf::PrimitiveType::Lines);

    for (float x = std::floor(left / GRID) * GRID;
         x < right;
         x += GRID) {

        lines.append({
            {x, top},
            sf::Color(30, 30, 40)
        });

        lines.append({
            {x, bottom},
            sf::Color(30, 30, 40)
        });
    }

    for (float y = std::floor(top / GRID) * GRID;
         y < bottom;
         y += GRID) {

        lines.append({
            {left, y},
            sf::Color(30, 30, 40)
        });

        lines.append({
            {right, y},
            sf::Color(30, 30, 40)
        });
    }

    window.draw(lines);
}

void drawField(
    sf::RenderWindow& window,
    sf::View& camera
) {

    sf::Vector2f center = camera.getCenter();
    sf::Vector2f size = camera.getSize();

    float left = center.x - size.x / 2.f;
    float right = center.x + size.x / 2.f;

    float top = center.y - size.y / 2.f;
    float bottom = center.y + size.y / 2.f;

    for (float x = left; x < right; x += GRID) {

        for (float y = top; y < bottom; y += GRID) {

            sf::Vector2f point(x, y);

            sf::Vector2f E =
                electricField(point);

            float mag = magnitude(E);

            if (mag < 0.01f)
                continue;

            sf::Vector2f dir =
                normalize(E);

            float len =
                std::min(18.f, mag * 0.08f);

            sf::Vertex arrow[] = {

                {
                    point,
                    sf::Color::Cyan
                },

                {
                    point + dir * len,
                    sf::Color::White
                }
            };

            window.draw(
                arrow,
                2,
                sf::PrimitiveType::Lines
            );
        }
    }
}

void traceFieldLine(
    sf::RenderWindow& window,
    sf::Vector2f start
) {

    sf::VertexArray line(
        sf::PrimitiveType::LineStrip
    );

    sf::Vector2f current = start;

    for (int i = 0; i < 350; i++) {

        sf::Vector2f E =
            electricField(current);

        float mag = magnitude(E);

        if (mag < 0.001f)
            break;

        sf::Vector2f dir =
            normalize(E);

        line.append({
            current,
            sf::Color::Yellow
        });

        float step =
            std::clamp(
                mag * 0.02f,
                2.f,
                8.f
            );

        current += dir * step;

        for (const auto& c : charges) {

            if (
                magnitude(
                    current - c.pos
                ) < 10.f
            ) {
                window.draw(line);
                return;
            }
        }
    }

    window.draw(line);
}

void drawFieldLines(
    sf::RenderWindow& window
) {

    for (const auto& c : charges) {

        if (c.q <= 0)
            continue;

        int seeds = 18;

        for (int i = 0; i < seeds; i++) {

            float angle =
                (2.f * 3.1415926f * i)
                / seeds;

            sf::Vector2f offset(
                std::cos(angle) * 16.f,
                std::sin(angle) * 16.f
            );

            traceFieldLine(
                window,
                c.pos + offset
            );
        }
    }
}

void drawCharges(
    sf::RenderWindow& window
) {

    for (const auto& c : charges) {

        float radius =
            10.f + std::abs(c.q) * 2.f;

        sf::CircleShape shape(radius);

        shape.setOrigin({
            radius,
            radius
        });

        shape.setPosition(c.pos);

        if (c.q > 0)
            shape.setFillColor(
                sf::Color::Red
            );
        else
            shape.setFillColor(
                sf::Color::Blue
            );

        window.draw(shape);
    }
}

void drawGaussianSurface(
    sf::RenderWindow& window
) {

    sf::CircleShape surface(140.f);

    surface.setOrigin({
        140.f,
        140.f
    });

    surface.setPosition({
        0.f,
        0.f
    });

    surface.setFillColor(
        sf::Color::Transparent
    );

    surface.setOutlineThickness(2.f);

    surface.setOutlineColor(
        sf::Color(255,255,255,120)
    );

    window.draw(surface);
}

void addDipole(
    sf::Vector2f center
) {

    charges.push_back({
        center +
        sf::Vector2f(-70.f, 0.f),
        2.f
    });

    charges.push_back({
        center +
        sf::Vector2f(70.f, 0.f),
        -2.f
    });
}

int main() {

    sf::RenderWindow window(
        sf::VideoMode({
            WIDTH,
            HEIGHT
        }),
        "Electrostatics Sandbox"
    );

    window.setFramerateLimit(60);

    sf::View camera(
        sf::FloatRect(
            {-700.f, -450.f},
            {1400.f, 900.f}
        )
    );

    charges.push_back({
        {-120.f, 0.f},
        2.f
    });

    charges.push_back({
        {120.f, 0.f},
        -2.f
    });

    bool dragging = false;
    int selected = -1;

    while (window.isOpen()) {

        while (const std::optional event =
               window.pollEvent()) {

            if (
                event->is<
                sf::Event::Closed>()
            ) {
                window.close();
            }

            if (const auto* wheel =
                event->getIf<
                sf::Event::MouseWheelScrolled>()
            ) {

                if (wheel->delta > 0)
                    camera.zoom(0.9f);
                else
                    camera.zoom(1.1f);
            }

            if (const auto* key =
                event->getIf<
                sf::Event::KeyPressed>()
            ) {

                if (
                    key->code ==
                    sf::Keyboard::Key::P
                ) {
                    addDipole({0.f,0.f});
                }
            }

            if (const auto* mouse =
                event->getIf<
                sf::Event::MouseButtonPressed>()
            ) {

                sf::Vector2f world =
                    window.mapPixelToCoords(
                        mouse->position,
                        camera
                    );

                if (
                    mouse->button ==
                    sf::Mouse::Button::Left
                ) {

                    for (int i = 0;
                         i < charges.size();
                         i++) {

                        if (
                            magnitude(
                                world -
                                charges[i].pos
                            ) < 20.f
                        ) {

                            dragging = true;
                            selected = i;
                        }
                    }
                }

                if (
                    mouse->button ==
                    sf::Mouse::Button::Right
                ) {

                    charges.push_back({
                        world,
                        1.f
                    });
                }

                if (
                    mouse->button ==
                    sf::Mouse::Button::Middle
                ) {

                    charges.push_back({
                        world,
                        -1.f
                    });
                }
            }

            if (
                event->is<
                sf::Event::MouseButtonReleased>()
            ) {

                dragging = false;
                selected = -1;
            }

            if (const auto* moved =
                event->getIf<
                sf::Event::MouseMoved>())
            {

                if (
                    dragging &&
                    selected != -1
                ) {

                    sf::Vector2f world =
                        window.mapPixelToCoords(
                            moved->position,
                            camera
                        );

                    charges[selected].pos =
                        world;
                }
            }
        }

        float move = 10.f;

        if (
            sf::Keyboard::isKeyPressed(
            sf::Keyboard::Key::W)
        )
            camera.move({0.f, -move});

        if (
            sf::Keyboard::isKeyPressed(
            sf::Keyboard::Key::S)
        )
            camera.move({0.f, move});

        if (
            sf::Keyboard::isKeyPressed(
            sf::Keyboard::Key::A)
        )
            camera.move({-move, 0.f});

        if (
            sf::Keyboard::isKeyPressed(
            sf::Keyboard::Key::D)
        )
            camera.move({move, 0.f});

        window.setView(camera);

        window.clear(
            sf::Color(10,10,18)
        );

        drawGrid(window, camera);

        drawField(window, camera);

        drawFieldLines(window);

        drawGaussianSurface(window);

        drawCharges(window);

        window.display();
    }

    return 0;
}

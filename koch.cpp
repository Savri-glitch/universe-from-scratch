// main.cpp
// Atmospheric procedural-space observation deck using SFML 2.x
// C++17, no external assets.

#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <random>
#include <string>
#include <sstream>
#include <iomanip>

constexpr float PI = 3.14159265359f;
constexpr int W = 1280;
constexpr int H = 720;

float clampf(float v, float a, float b) {
    return std::max(a, std::min(v, b));
}

float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

float smoothstep(float t) {
    t = clampf(t, 0.f, 1.f);
    return t * t * (3.f - 2.f * t);
}

sf::Color mixColor(sf::Color a, sf::Color b, float t) {
    t = clampf(t, 0.f, 1.f);
    return sf::Color(
        static_cast<sf::Uint8>(lerp(a.r, b.r, t)),
        static_cast<sf::Uint8>(lerp(a.g, b.g, t)),
        static_cast<sf::Uint8>(lerp(a.b, b.b, t)),
        static_cast<sf::Uint8>(lerp(a.a, b.a, t))
    );
}

float randf(std::mt19937& rng, float a, float b) {
    std::uniform_real_distribution<float> d(a, b);
    return d(rng);
}

int randi(std::mt19937& rng, int a, int b) {
    std::uniform_int_distribution<int> d(a, b);
    return d(rng);
}

sf::Color randomCosmicColor(std::mt19937& rng, int alpha = 180) {
    std::vector<sf::Color> palette = {
        sf::Color(120, 170, 255, alpha),
        sf::Color(255, 190, 120, alpha),
        sf::Color(170, 120, 255, alpha),
        sf::Color(120, 255, 210, alpha),
        sf::Color(255, 130, 170, alpha),
        sf::Color(210, 230, 255, alpha)
    };
    return palette[randi(rng, 0, static_cast<int>(palette.size()) - 1)];
}

struct Star {
    sf::Vector2f base;
    float z;
    float size;
    float twinkle;
    sf::Color color;
};

struct Particle {
    sf::Vector2f pos;
    sf::Vector2f vel;
    float size;
    float phase;
};

struct FloatObject {
    int type;
    sf::Vector2f pos;
    sf::Vector2f vel;
    float angle;
    float spin;
    float scale;
    float life;
    float maxLife;
};

enum PhenomenonType {
    Nebula,
    StarCluster,
    BinaryStars,
    Pulsar,
    RingedPlanet,
    IceWorld,
    GasGiant,
    AsteroidField,
    Comet,
    BlackHole,
    Quasar,
    DysonSphere,
    OneillCylinder,
    DerelictShip,
    AlienStructure,
    CosmicStorm,
    LensingEvent
};

std::string phenomenonName(PhenomenonType t) {
    switch (t) {
        case Nebula: return "nebula";
        case StarCluster: return "star cluster";
        case BinaryStars: return "binary stars";
        case Pulsar: return "pulsar";
        case RingedPlanet: return "ringed planet";
        case IceWorld: return "ice world";
        case GasGiant: return "gas giant";
        case AsteroidField: return "asteroid field";
        case Comet: return "comet";
        case BlackHole: return "black hole";
        case Quasar: return "quasar";
        case DysonSphere: return "dyson sphere";
        case OneillCylinder: return "o'neill cylinder";
        case DerelictShip: return "derelict spacecraft";
        case AlienStructure: return "unknown structure";
        case CosmicStorm: return "cosmic storm";
        case LensingEvent: return "lensing event";
    }
    return "unknown";
}

struct Phenomenon {
    PhenomenonType type;
    sf::Vector2f pos;
    float scale;
    float phase;
    sf::Color primary;
    sf::Color secondary;
    unsigned seed;
};

struct WorldLayer {
    std::vector<Star> stars;
    std::vector<Phenomenon> phenomena;
    sf::Color bgA;
    sf::Color bgB;
    unsigned seed;
};

WorldLayer generateLayer(unsigned seed) {
    std::mt19937 rng(seed);
    WorldLayer layer;
    layer.seed = seed;

    layer.bgA = sf::Color(randi(rng, 1, 8), randi(rng, 4, 12), randi(rng, 12, 28));
    layer.bgB = sf::Color(randi(rng, 0, 12), randi(rng, 0, 14), randi(rng, 10, 35));

    int starCount = randi(rng, 520, 980);
    layer.stars.reserve(starCount);

    for (int i = 0; i < starCount; ++i) {
        Star s;
        s.base = sf::Vector2f(randf(rng, -80.f, W + 80.f), randf(rng, 20.f, H - 120.f));
        s.z = randf(rng, 0.15f, 1.f);
        s.size = randf(rng, 0.6f, 2.4f) * s.z;
        s.twinkle = randf(rng, 0.f, 30.f);
        int warmth = randi(rng, 0, 3);
        if (warmth == 0) s.color = sf::Color(180, 210, 255, randi(rng, 80, 220));
        else if (warmth == 1) s.color = sf::Color(255, 230, 190, randi(rng, 80, 220));
        else s.color = sf::Color(225, 245, 255, randi(rng, 80, 240));
        layer.stars.push_back(s);
    }

    int count = randi(rng, 2, 5);
    for (int i = 0; i < count; ++i) {
        Phenomenon p;
        p.type = static_cast<PhenomenonType>(randi(rng, 0, 16));
        p.pos = sf::Vector2f(randf(rng, 250.f, W - 250.f), randf(rng, 130.f, H - 230.f));
        p.scale = randf(rng, 0.7f, 1.55f);
        p.phase = randf(rng, 0.f, 1000.f);
        p.primary = randomCosmicColor(rng, randi(rng, 100, 190));
        p.secondary = randomCosmicColor(rng, randi(rng, 60, 150));
        p.seed = rng();
        layer.phenomena.push_back(p);
    }

    if (randi(rng, 0, 10) == 0) {
        Phenomenon anomaly;
        anomaly.type = AlienStructure;
        anomaly.pos = sf::Vector2f(randf(rng, 360.f, W - 360.f), randf(rng, 150.f, 410.f));
        anomaly.scale = randf(rng, 0.9f, 1.8f);
        anomaly.phase = randf(rng, 0.f, 999.f);
        anomaly.primary = sf::Color(120, 255, 210, 120);
        anomaly.secondary = sf::Color(255, 255, 255, 90);
        anomaly.seed = rng();
        layer.phenomena.push_back(anomaly);
    }

    return layer;
}

void drawGradient(sf::RenderTarget& target, sf::Color top, sf::Color bottom) {
    sf::VertexArray q(sf::Quads, 4);
    q[0].position = sf::Vector2f(0, 0);
    q[1].position = sf::Vector2f(W, 0);
    q[2].position = sf::Vector2f(W, H);
    q[3].position = sf::Vector2f(0, H);
    q[0].color = top;
    q[1].color = top;
    q[2].color = bottom;
    q[3].color = bottom;
    target.draw(q);
}

void drawSoftCircle(sf::RenderTarget& target, sf::Vector2f pos, float radius, sf::Color color, int rings = 16) {
    for (int i = rings; i >= 1; --i) {
        float t = static_cast<float>(i) / rings;
        sf::CircleShape c(radius * t);
        c.setOrigin(radius * t, radius * t);
        c.setPosition(pos);
        sf::Color col = color;
        col.a = static_cast<sf::Uint8>(color.a * (1.f - t) * 0.32f);
        c.setFillColor(col);
        target.draw(c, sf::BlendAdd);
    }
}

void drawRing(sf::RenderTarget& target, sf::Vector2f pos, float rx, float ry, float angle, sf::Color color, float thickness = 2.f) {
    sf::VertexArray strip(sf::TriangleStrip);
    int n = 160;

    for (int i = 0; i <= n; ++i) {
        float a = i / static_cast<float>(n) * PI * 2.f;
        float ca = std::cos(a);
        float sa = std::sin(a);

        for (int j = 0; j < 2; ++j) {
            float rrx = rx + (j == 0 ? -thickness : thickness);
            float rry = ry + (j == 0 ? -thickness : thickness);

            sf::Vector2f p(ca * rrx, sa * rry);
            float x = p.x * std::cos(angle) - p.y * std::sin(angle);
            float y = p.x * std::sin(angle) + p.y * std::cos(angle);

            sf::Vertex v(pos + sf::Vector2f(x, y), color);
            strip.append(v);
        }
    }

    target.draw(strip, sf::BlendAdd);
}

void drawShipShape(sf::RenderTarget& target, sf::Vector2f pos, float s, float angle, sf::Color color) {
    sf::ConvexShape body;
    body.setPointCount(5);
    body.setPoint(0, sf::Vector2f(34, 0));
    body.setPoint(1, sf::Vector2f(-18, -13));
    body.setPoint(2, sf::Vector2f(-38, -6));
    body.setPoint(3, sf::Vector2f(-38, 6));
    body.setPoint(4, sf::Vector2f(-18, 13));
    body.setOrigin(0, 0);
    body.setScale(s, s);
    body.setRotation(angle);
    body.setPosition(pos);
    body.setFillColor(color);
    target.draw(body);

    sf::RectangleShape mast(sf::Vector2f(46 * s, 2 * s));
    mast.setOrigin(23 * s, 1 * s);
    mast.setPosition(pos);
    mast.setRotation(angle + 90);
    mast.setFillColor(sf::Color(color.r, color.g, color.b, 90));
    target.draw(mast);
}

void drawPhenomenon(sf::RenderTarget& target, const Phenomenon& p, float time, float alpha) {
    std::mt19937 rng(p.seed);
    sf::Color a = p.primary;
    sf::Color b = p.secondary;
    a.a = static_cast<sf::Uint8>(a.a * alpha);
    b.a = static_cast<sf::Uint8>(b.a * alpha);

    sf::Vector2f pos = p.pos;
    pos.x += std::sin(time * 0.018f + p.phase) * 18.f;
    pos.y += std::cos(time * 0.013f + p.phase) * 10.f;

    float pulse = 0.5f + 0.5f * std::sin(time * 0.7f + p.phase);

    switch (p.type) {
        case Nebula: {
            for (int i = 0; i < 26; ++i) {
                sf::Vector2f q = pos + sf::Vector2f(randf(rng, -180, 180), randf(rng, -95, 95)) * p.scale;
                float r = randf(rng, 50, 170) * p.scale;
                sf::Color c = mixColor(a, b, randf(rng, 0.f, 1.f));
                c.a = static_cast<sf::Uint8>(c.a * 0.34f);
                drawSoftCircle(target, q, r, c, 12);
            }
            break;
        }

        case StarCluster: {
            drawSoftCircle(target, pos, 150 * p.scale, sf::Color(a.r, a.g, a.b, 55), 12);
            for (int i = 0; i < 140; ++i) {
                float ang = randf(rng, 0, PI * 2);
                float rad = std::pow(randf(rng, 0, 1), 2.2f) * 180 * p.scale;
                sf::CircleShape s(randf(rng, 0.8f, 2.8f));
                s.setPosition(pos + sf::Vector2f(std::cos(ang) * rad, std::sin(ang) * rad));
                s.setFillColor(mixColor(a, sf::Color::White, randf(rng, 0, 1)));
                target.draw(s, sf::BlendAdd);
            }
            break;
        }

        case BinaryStars: {
            float d = 62 * p.scale;
            sf::Vector2f p1 = pos + sf::Vector2f(std::cos(time * 0.08f) * d, std::sin(time * 0.08f) * d * 0.5f);
            sf::Vector2f p2 = pos - sf::Vector2f(std::cos(time * 0.08f) * d, std::sin(time * 0.08f) * d * 0.5f);
            drawSoftCircle(target, p1, 86 * p.scale, a, 18);
            drawSoftCircle(target, p2, 70 * p.scale, b, 18);
            sf::CircleShape c1(13 * p.scale);
            c1.setOrigin(13 * p.scale, 13 * p.scale);
            c1.setPosition(p1);
            c1.setFillColor(sf::Color(255, 245, 220, static_cast<sf::Uint8>(220 * alpha)));
            target.draw(c1, sf::BlendAdd);
            sf::CircleShape c2(10 * p.scale);
            c2.setOrigin(10 * p.scale, 10 * p.scale);
            c2.setPosition(p2);
            c2.setFillColor(sf::Color(190, 220, 255, static_cast<sf::Uint8>(210 * alpha)));
            target.draw(c2, sf::BlendAdd);
            break;
        }

        case Pulsar: {
            drawSoftCircle(target, pos, 90 * p.scale, a, 14);
            float ang = time * 0.65f + p.phase;
            sf::RectangleShape beam(sf::Vector2f(520 * p.scale, 5 * p.scale));
            beam.setOrigin(260 * p.scale, 2.5f * p.scale);
            beam.setPosition(pos);
            beam.setRotation(ang * 180 / PI);
            beam.setFillColor(sf::Color(a.r, a.g, a.b, static_cast<sf::Uint8>((70 + pulse * 120) * alpha)));
            target.draw(beam, sf::BlendAdd);

            sf::CircleShape core(11 * p.scale);
            core.setOrigin(11 * p.scale, 11 * p.scale);
            core.setPosition(pos);
            core.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(230 * alpha)));
            target.draw(core, sf::BlendAdd);
            break;
        }

        case RingedPlanet:
        case GasGiant:
        case IceWorld: {
            float r = (p.type == GasGiant ? 88 : 64) * p.scale;
            sf::Color planet = p.type == IceWorld ? sf::Color(170, 230, 255, static_cast<sf::Uint8>(220 * alpha)) : a;
            drawSoftCircle(target, pos, r * 1.5f, sf::Color(planet.r, planet.g, planet.b, 60), 10);

            if (p.type == RingedPlanet || p.type == GasGiant)
                drawRing(target, pos, r * 1.75f, r * 0.46f, -0.25f, sf::Color(220, 210, 190, static_cast<sf::Uint8>(105 * alpha)), 5.f * p.scale);

            sf::CircleShape body(r);
            body.setOrigin(r, r);
            body.setPosition(pos);
            body.setFillColor(planet);
            target.draw(body);

            for (int i = 0; i < 7; ++i) {
                sf::RectangleShape band(sf::Vector2f(r * 1.7f, 5 * p.scale));
                band.setOrigin(r * 0.85f, 2.5f * p.scale);
                band.setPosition(pos.x, pos.y - r * 0.45f + i * r * 0.15f);
                band.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>((18 + i * 5) * alpha)));
                target.draw(band, sf::BlendAdd);
            }
            break;
        }

        case AsteroidField: {
            for (int i = 0; i < 90; ++i) {
                float ang = randf(rng, 0, PI * 2);
                float rad = randf(rng, 30, 240) * p.scale;
                sf::CircleShape rock(randf(rng, 2, 8) * p.scale, randi(rng, 5, 9));
                rock.setOrigin(rock.getRadius(), rock.getRadius());
                rock.setPosition(pos + sf::Vector2f(std::cos(ang) * rad, std::sin(ang) * rad * 0.38f));
                rock.setRotation(randf(rng, 0, 360) + time * randf(rng, -3, 3));
                rock.setFillColor(sf::Color(100, 100, 105, static_cast<sf::Uint8>(110 * alpha)));
                target.draw(rock);
            }
            break;
        }

        case Comet: {
            sf::Vector2f head = pos + sf::Vector2f(std::sin(time * 0.04f) * 80, std::cos(time * 0.03f) * 35);
            for (int i = 0; i < 16; ++i) {
                sf::CircleShape tail(12 * p.scale + i * 5 * p.scale);
                tail.setOrigin(tail.getRadius(), tail.getRadius());
                tail.setPosition(head - sf::Vector2f(i * 18 * p.scale, i * 6 * p.scale));
                tail.setFillColor(sf::Color(a.r, a.g, a.b, static_cast<sf::Uint8>((70 - i * 3) * alpha)));
                target.draw(tail, sf::BlendAdd);
            }
            drawSoftCircle(target, head, 42 * p.scale, sf::Color(200, 245, 255, static_cast<sf::Uint8>(160 * alpha)), 12);
            break;
        }

        case BlackHole:
        case LensingEvent: {
            drawRing(target, pos, 135 * p.scale, 55 * p.scale, time * 0.02f, sf::Color(255, 210, 130, static_cast<sf::Uint8>(150 * alpha)), 8.f * p.scale);
            drawRing(target, pos, 180 * p.scale, 78 * p.scale, -time * 0.013f, sf::Color(120, 180, 255, static_cast<sf::Uint8>(80 * alpha)), 3.f * p.scale);
            drawSoftCircle(target, pos, 105 * p.scale, sf::Color(90, 120, 255, static_cast<sf::Uint8>(75 * alpha)), 10);
            sf::CircleShape hole(52 * p.scale);
            hole.setOrigin(52 * p.scale, 52 * p.scale);
            hole.setPosition(pos);
            hole.setFillColor(sf::Color(0, 0, 2, static_cast<sf::Uint8>(245 * alpha)));
            target.draw(hole);
            break;
        }

        case Quasar: {
            drawSoftCircle(target, pos, 90 * p.scale, a, 12);
            sf::RectangleShape jet(sf::Vector2f(760 * p.scale, 7 * p.scale));
            jet.setOrigin(380 * p.scale, 3.5f * p.scale);
            jet.setPosition(pos);
            jet.setRotation(-20 + std::sin(time * 0.02f) * 4);
            jet.setFillColor(sf::Color(170, 220, 255, static_cast<sf::Uint8>(135 * alpha)));
            target.draw(jet, sf::BlendAdd);
            break;
        }

        case DysonSphere: {
            float r = 88 * p.scale;
            drawSoftCircle(target, pos, 120 * p.scale, sf::Color(255, 210, 130, static_cast<sf::Uint8>(120 * alpha)), 12);
            for (int i = 0; i < 28; ++i) {
                float ang = i / 28.f * PI * 2 + time * 0.015f;
                sf::RectangleShape panel(sf::Vector2f(11 * p.scale, 22 * p.scale));
                panel.setOrigin(5.5f * p.scale, 11 * p.scale);
                panel.setPosition(pos + sf::Vector2f(std::cos(ang) * r, std::sin(ang) * r));
                panel.setRotation(ang * 180 / PI + 90);
                panel.setFillColor(sf::Color(170, 185, 190, static_cast<sf::Uint8>(155 * alpha)));
                target.draw(panel);
            }
            break;
        }

        case OneillCylinder: {
            sf::RectangleShape cyl(sf::Vector2f(190 * p.scale, 42 * p.scale));
            cyl.setOrigin(95 * p.scale, 21 * p.scale);
            cyl.setPosition(pos);
            cyl.setRotation(18 + std::sin(time * 0.02f) * 3);
            cyl.setFillColor(sf::Color(150, 165, 170, static_cast<sf::Uint8>(120 * alpha)));
            target.draw(cyl);
            for (int i = -3; i <= 3; ++i) {
                sf::RectangleShape stripe(sf::Vector2f(2 * p.scale, 42 * p.scale));
                stripe.setOrigin(1 * p.scale, 21 * p.scale);
                stripe.setPosition(pos);
                stripe.move(i * 25 * p.scale, 0);
                stripe.setRotation(cyl.getRotation());
                stripe.setFillColor(sf::Color(230, 245, 255, static_cast<sf::Uint8>(70 * alpha)));
                target.draw(stripe, sf::BlendAdd);
            }
            break;
        }

        case DerelictShip: {
            drawShipShape(target, pos, p.scale, -12 + std::sin(time * 0.03f) * 5, sf::Color(120, 130, 135, static_cast<sf::Uint8>(130 * alpha)));
            drawSoftCircle(target, pos + sf::Vector2f(-34 * p.scale, 4), 20 * p.scale, sf::Color(255, 120, 80, static_cast<sf::Uint8>(50 * alpha)), 6);
            break;
        }

        case AlienStructure: {
            for (int i = 0; i < 9; ++i) {
                float ang = i / 9.f * PI * 2 + time * 0.006f;
                sf::RectangleShape arm(sf::Vector2f(120 * p.scale, 4 * p.scale));
                arm.setOrigin(0, 2 * p.scale);
                arm.setPosition(pos);
                arm.setRotation(ang * 180 / PI);
                arm.setFillColor(sf::Color(a.r, a.g, a.b, static_cast<sf::Uint8>(90 * alpha)));
                target.draw(arm, sf::BlendAdd);
            }
            drawSoftCircle(target, pos, 42 * p.scale, sf::Color(a.r, a.g, a.b, static_cast<sf::Uint8>(85 * alpha)), 10);
            break;
        }

        case CosmicStorm: {
            for (int i = 0; i < 36; ++i) {
                float ang = randf(rng, 0, PI * 2);
                float rad = randf(rng, 40, 210) * p.scale;
                sf::RectangleShape arc(sf::Vector2f(randf(rng, 50, 120) * p.scale, 2 * p.scale));
                arc.setOrigin(0, 1 * p.scale);
                arc.setPosition(pos + sf::Vector2f(std::cos(ang + time * 0.02f) * rad, std::sin(ang) * rad * 0.55f));
                arc.setRotation(ang * 180 / PI + randf(rng, -25, 25));
                arc.setFillColor(sf::Color(a.r, a.g, a.b, static_cast<sf::Uint8>(65 * alpha)));
                target.draw(arc, sf::BlendAdd);
            }
            break;
        }
    }
}

void drawUniverse(sf::RenderTarget& target, const WorldLayer& layer, float time, float alpha, float drift) {
    sf::Color top = layer.bgA;
    sf::Color bottom = layer.bgB;
    top.a = static_cast<sf::Uint8>(255 * alpha);
    bottom.a = static_cast<sf::Uint8>(255 * alpha);

    drawGradient(target, top, bottom);

    for (const auto& s : layer.stars) {
        float parallax = s.z;
        sf::Vector2f p = s.base;
        p.x += std::fmod(drift * (0.025f + parallax * 0.09f), W + 160.f) - 80.f;
        p.y += std::sin(time * 0.015f + s.twinkle) * parallax * 1.5f;

        if (p.x > W + 80) p.x -= W + 160;
        if (p.x < -80) p.x += W + 160;

        float tw = 0.65f + 0.35f * std::sin(time * (0.35f + s.z * 0.8f) + s.twinkle);
        sf::CircleShape c(s.size);
        c.setOrigin(s.size, s.size);
        c.setPosition(p);
        sf::Color col = s.color;
        col.a = static_cast<sf::Uint8>(col.a * alpha * tw);
        c.setFillColor(col);
        target.draw(c, sf::BlendAdd);
    }

    for (const auto& p : layer.phenomena) {
        drawPhenomenon(target, p, time, alpha);
    }
}

void drawFloatingObject(sf::RenderTarget& target, const FloatObject& o, float globalTime) {
    float a = smoothstep(o.life / 3.f) * smoothstep((o.maxLife - o.life) / 3.f);
    sf::Transform t;
    t.translate(o.pos);
    t.rotate(o.angle + std::sin(globalTime * 0.6f) * 2.f);
    t.scale(o.scale, o.scale);

    sf::Color paper(220, 218, 200, static_cast<sf::Uint8>(120 * a));
    sf::Color ink(80, 90, 105, static_cast<sf::Uint8>(90 * a));

    if (o.type == 0) {
        sf::RectangleShape notebook(sf::Vector2f(66, 42));
        notebook.setOrigin(33, 21);
        notebook.setFillColor(paper);
        notebook.setOutlineColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(30 * a)));
        notebook.setOutlineThickness(1);
        target.draw(notebook, t);

        for (int i = 0; i < 4; ++i) {
            sf::RectangleShape line(sf::Vector2f(42, 1));
            line.setOrigin(21, 0.5f);
            line.setPosition(2, -12 + i * 8);
            line.setFillColor(ink);
            target.draw(line, t);
        }
    } else if (o.type == 1) {
        sf::RectangleShape pen(sf::Vector2f(74, 5));
        pen.setOrigin(37, 2.5f);
        pen.setFillColor(sf::Color(160, 190, 220, static_cast<sf::Uint8>(150 * a)));
        target.draw(pen, t);

        sf::CircleShape cap(4);
        cap.setOrigin(4, 4);
        cap.setPosition(39, 0);
        cap.setFillColor(sf::Color(230, 240, 255, static_cast<sf::Uint8>(130 * a)));
        target.draw(cap, t);
    } else if (o.type == 2) {
        sf::RectangleShape pouch(sf::Vector2f(48, 34));
        pouch.setOrigin(24, 17);
        pouch.setFillColor(sf::Color(150, 95, 70, static_cast<sf::Uint8>(130 * a)));
        target.draw(pouch, t);

        sf::CircleShape valve(5);
        valve.setOrigin(5, 5);
        valve.setPosition(12, -6);
        valve.setFillColor(sf::Color(230, 220, 190, static_cast<sf::Uint8>(140 * a)));
        target.draw(valve, t);
    } else {
        sf::RectangleShape photo(sf::Vector2f(58, 38));
        photo.setOrigin(29, 19);
        photo.setFillColor(sf::Color(235, 235, 220, static_cast<sf::Uint8>(135 * a)));
        target.draw(photo, t);

        sf::RectangleShape image(sf::Vector2f(46, 26));
        image.setOrigin(23, 13);
        image.setFillColor(sf::Color(60, 95, 130, static_cast<sf::Uint8>(120 * a)));
        target.draw(image, t);
    }
}

void drawInterior(sf::RenderTarget& target, float time, const std::vector<FloatObject>& floating) {
    float vibX = std::sin(time * 17.f) * 0.45f + std::sin(time * 43.f) * 0.12f;
    float vibY = std::cos(time * 19.f) * 0.35f;

    sf::Transform ship;
    ship.translate(vibX, vibY);

    sf::RectangleShape top(sf::Vector2f(W, 105));
    top.setFillColor(sf::Color(9, 11, 14, 245));
    target.draw(top, ship);

    sf::RectangleShape bottom(sf::Vector2f(W, 175));
    bottom.setPosition(0, H - 175);
    bottom.setFillColor(sf::Color(7, 8, 10, 250));
    target.draw(bottom, ship);

    sf::RectangleShape left(sf::Vector2f(185, H));
    left.setFillColor(sf::Color(8, 10, 13, 248));
    target.draw(left, ship);

    sf::RectangleShape right(sf::Vector2f(185, H));
    right.setPosition(W - 185, 0);
    right.setFillColor(sf::Color(8, 10, 13, 248));
    target.draw(right, ship);

    sf::ConvexShape frame;
    frame.setPointCount(8);
    frame.setPoint(0, sf::Vector2f(120, 80));
    frame.setPoint(1, sf::Vector2f(W - 120, 80));
    frame.setPoint(2, sf::Vector2f(W - 170, 135));
    frame.setPoint(3, sf::Vector2f(W - 185, H - 190));
    frame.setPoint(4, sf::Vector2f(W - 245, H - 130));
    frame.setPoint(5, sf::Vector2f(245, H - 130));
    frame.setPoint(6, sf::Vector2f(185, H - 190));
    frame.setPoint(7, sf::Vector2f(170, 135));
    frame.setFillColor(sf::Color(19, 22, 27, 235));
    frame.setOutlineThickness(4);
    frame.setOutlineColor(sf::Color(55, 61, 68, 210));
    target.draw(frame, ship);

    sf::ConvexShape glass;
    glass.setPointCount(4);
    glass.setPoint(0, sf::Vector2f(215, 128));
    glass.setPoint(1, sf::Vector2f(W - 215, 128));
    glass.setPoint(2, sf::Vector2f(W - 250, H - 178));
    glass.setPoint(3, sf::Vector2f(250, H - 178));
    glass.setFillColor(sf::Color(170, 210, 255, 15));
    glass.setOutlineThickness(1);
    glass.setOutlineColor(sf::Color(180, 220, 255, 45));
    target.draw(glass, ship);

    for (int i = 0; i < 5; ++i) {
        float x = 285.f + i * 185.f;
        sf::RectangleShape brace(sf::Vector2f(7, H - 270));
        brace.setOrigin(3.5f, 0);
        brace.setPosition(x, 125);
        brace.setRotation(i == 0 ? -4 : i == 4 ? 4 : 0);
        brace.setFillColor(sf::Color(30, 34, 39, 185));
        target.draw(brace, ship);
    }

    sf::RectangleShape panel(sf::Vector2f(720, 88));
    panel.setOrigin(360, 44);
    panel.setPosition(W / 2.f, H - 75);
    panel.setFillColor(sf::Color(16, 18, 22, 240));
    panel.setOutlineColor(sf::Color(60, 64, 72, 160));
    panel.setOutlineThickness(1);
    target.draw(panel, ship);

    for (int i = 0; i < 34; ++i) {
        float x = W / 2.f - 330.f + i * 20.f;
        float y = H - 90.f + std::sin(i * 1.7f) * 8.f;
        float blink = 0.35f + 0.65f * smoothstep(std::sin(time * (0.2f + i * 0.013f) + i) * 0.5f + 0.5f);
        sf::CircleShape light(2.5f + (i % 5 == 0 ? 1.5f : 0.f));
        light.setOrigin(light.getRadius(), light.getRadius());
        light.setPosition(x, y);
        sf::Color col = i % 7 == 0 ? sf::Color(120, 240, 190) : sf::Color(90, 170, 255);
        col.a = static_cast<sf::Uint8>(45 + blink * 115);
        light.setFillColor(col);
        target.draw(light, ship);
    }

    if (std::fmod(time, 19.f) < 2.7f) {
        sf::RectangleShape signal(sf::Vector2f(60, 3));
        signal.setOrigin(30, 1.5f);
        signal.setPosition(W / 2.f + 295, H - 57);
        signal.setFillColor(sf::Color(120, 255, 210, 95));
        target.draw(signal, ship);

        for (int i = 0; i < 3; ++i) {
            sf::CircleShape dot(3);
            dot.setOrigin(3, 3);
            dot.setPosition(W / 2.f + 267 + i * 22, H - 66);
            dot.setFillColor(sf::Color(120, 255, 210, static_cast<sf::Uint8>(50 + 45 * std::sin(time * 5 + i))));
            target.draw(dot, ship);
        }
    }

    for (const auto& o : floating) {
        drawFloatingObject(target, o, time);
    }

    for (int i = 0; i < 7; ++i) {
        sf::CircleShape reflection(75 + i * 16);
        reflection.setOrigin(reflection.getRadius(), reflection.getRadius());
        reflection.setPosition(340 + std::sin(time * 0.05f + i) * 35, 170 + i * 22);
        reflection.setFillColor(sf::Color(180, 220, 255, static_cast<sf::Uint8>(8 - i)));
        target.draw(reflection, ship);
    }

    sf::RectangleShape vignetteTop(sf::Vector2f(W, 90));
    vignetteTop.setFillColor(sf::Color(0, 0, 0, 90));
    target.draw(vignetteTop);

    sf::RectangleShape vignetteBottom(sf::Vector2f(W, 120));
    vignetteBottom.setPosition(0, H - 120);
    vignetteBottom.setFillColor(sf::Color(0, 0, 0, 120));
    target.draw(vignetteBottom);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(W, H), "Cupola: Infinite Procedural Space", sf::Style::Titlebar | sf::Style::Close);
    window.setVerticalSyncEnabled(true);

    std::random_device rd;
    unsigned baseSeed = rd() ^ static_cast<unsigned>(std::time(nullptr));

    WorldLayer current = generateLayer(baseSeed);
    WorldLayer next = generateLayer(baseSeed + 1009);

    sf::Clock clock;
    float globalTime = 0.f;
    float transitionClock = 0.f;
    float segmentLength = 82.f;
    int segment = 0;

    std::mt19937 rng(baseSeed ^ 0xBADC0DEu);

    std::vector<Particle> particles;
    for (int i = 0; i < 90; ++i) {
        Particle p;
        p.pos = sf::Vector2f(randf(rng, 190, W - 190), randf(rng, 110, H - 170));
        p.vel = sf::Vector2f(randf(rng, -3, 3), randf(rng, -2, 2));
        p.size = randf(rng, 0.7f, 2.2f);
        p.phase = randf(rng, 0, 100);
        particles.push_back(p);
    }

    std::vector<FloatObject> floating;
    float spawnTimer = 4.f;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        dt = clampf(dt, 0.f, 0.05f);
        globalTime += dt;
        transitionClock += dt;

        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed)
                window.close();

            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape)
                window.close();
        }

        if (transitionClock > segmentLength) {
            current = next;
            segment++;
            next = generateLayer(baseSeed + 1009u * static_cast<unsigned>(segment + 2) + rd());
            transitionClock = 0.f;
            segmentLength = randf(rng, 70.f, 120.f);
        }

        spawnTimer -= dt;
        if (spawnTimer <= 0.f && floating.size() < 5) {
            FloatObject o;
            o.type = randi(rng, 0, 3);
            o.pos = sf::Vector2f(randf(rng, 220, W - 220), randf(rng, H - 140, H - 70));
            o.vel = sf::Vector2f(randf(rng, -8, 8), randf(rng, -18, -5));
            o.angle = randf(rng, -25, 25);
            o.spin = randf(rng, -8, 8);
            o.scale = randf(rng, 0.55f, 1.0f);
            o.life = 0.f;
            o.maxLife = randf(rng, 38.f, 78.f);
            floating.push_back(o);
            spawnTimer = randf(rng, 18.f, 42.f);
        }

        for (auto& o : floating) {
            o.life += dt;
            o.pos += o.vel * dt;
            o.pos.x += std::sin(globalTime * 0.4f + o.life) * dt * 4.f;
            o.angle += o.spin * dt;
        }

        floating.erase(
            std::remove_if(floating.begin(), floating.end(),
                [](const FloatObject& o) { return o.life > o.maxLife; }),
            floating.end()
        );

        for (auto& p : particles) {
            p.pos += p.vel * dt;
            p.pos.x += std::sin(globalTime * 0.2f + p.phase) * dt * 3.f;
            p.pos.y += std::cos(globalTime * 0.17f + p.phase) * dt * 2.f;

            if (p.pos.x < 185) p.pos.x = W - 185;
            if (p.pos.x > W - 185) p.pos.x = 185;
            if (p.pos.y < 105) p.pos.y = H - 175;
            if (p.pos.y > H - 175) p.pos.y = 105;
        }

        float t = smoothstep(transitionClock / segmentLength);
        float fadeNext = smoothstep((transitionClock - segmentLength * 0.62f) / (segmentLength * 0.38f));
        float fadeCurrent = 1.f - fadeNext;
        float drift = globalTime * 16.f;

        window.clear(sf::Color::Black);

        drawUniverse(window, current, globalTime, fadeCurrent, drift);
        if (fadeNext > 0.001f)
            drawUniverse(window, next, globalTime, fadeNext, drift * 0.7f + 200.f);

        for (const auto& p : particles) {
            float a = 20 + 35 * (0.5f + 0.5f * std::sin(globalTime * 0.7f + p.phase));
            sf::CircleShape dust(p.size);
            dust.setOrigin(p.size, p.size);
            dust.setPosition(p.pos);
            dust.setFillColor(sf::Color(190, 220, 255, static_cast<sf::Uint8>(a)));
            window.draw(dust, sf::BlendAdd);
        }

        drawInterior(window, globalTime, floating);

        window.display();
    }

    return 0;
}

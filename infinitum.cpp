
#include <SFML/Graphics.hpp>

#include <complex>
#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>

using Complex = std::complex<double>;

constexpr unsigned WIDTH = 1200;
constexpr unsigned HEIGHT = 800;

class Mandelbrot
{
private:
    double centerX = -0.5;
    double centerY = 0.0;

    double zoom = 1.0;

    int maxIterations = 150;

    double colorTime = 0.0;

    sf::Image image;
    sf::Texture texture;
    sf::Sprite sprite;

    bool dragging = false;
    sf::Vector2i lastMouse;

public:

    Mandelbrot()
        : image({WIDTH, HEIGHT}, sf::Color::Black),
          texture({WIDTH, HEIGHT}),
          sprite(texture)
    {
        texture.setSmooth(false);
    }

    // Convert screen coordinates into complex plane coordinates
    Complex screenToComplex(double x, double y) const
    {
        double aspect = static_cast<double>(WIDTH) / HEIGHT;

        double scale = 3.5 / zoom;

        double real = centerX +
            (x / WIDTH - 0.5) * scale * aspect;

        double imag = centerY +
            (y / HEIGHT - 0.5) * scale;

        return Complex(real, imag);
    }

    // Calculate Mandelbrot escape iterations
    int calculate(Complex c) const
    {
        Complex z(0.0, 0.0);

        for (int i = 0; i < maxIterations; ++i)
        {
            z = z * z + c;

            if (std::norm(z) > 256.0)
                return i;
        }

        return maxIterations;
    }

    // Smooth colour palette
    sf::Color getColor(int iterations, Complex c) const
    {
        if (iterations == maxIterations)
            return sf::Color(5, 5, 15);

        double magnitude = std::abs(c);

        double value = iterations + 1.0 -
            std::log2(std::max(1.0, std::log2(magnitude)));

        double t = value * 0.045 + colorTime;

        int r = static_cast<int>(
            127 + 127 * std::sin(t)
        );

        int g = static_cast<int>(
            127 + 127 * std::sin(t + 2.094)
        );

        int b = static_cast<int>(
            127 + 127 * std::sin(t + 4.188)
        );

        return sf::Color(
            std::clamp(r, 0, 255),
            std::clamp(g, 0, 255),
            std::clamp(b, 0, 255)
        );
    }

    // Render the entire fractal
    void render()
    {
        for (unsigned y = 0; y < HEIGHT; ++y)
        {
            for (unsigned x = 0; x < WIDTH; ++x)
            {
                Complex c = screenToComplex(x, y);

                int iterations = calculate(c);

                image.setPixel(
                    {x, y},
                    getColor(iterations, c)
                );
            }
        }

        texture.update(image);
    }

    // Zoom towards the mouse position
    void zoomAt(sf::Vector2i mouse, double factor)
    {
        Complex before = screenToComplex(
            mouse.x, mouse.y
        );

        zoom *= factor;

        zoom = std::clamp(zoom, 0.1, 1e12);

        Complex after = screenToComplex(
            mouse.x, mouse.y
        );

        centerX += before.real() - after.real();
        centerY += before.imag() - after.imag();
    }

    // Pan the camera
    void pan(int dx, int dy)
    {
        double scale = 3.5 / zoom;

        centerX -=
            dx * scale / WIDTH *
            (static_cast<double>(WIDTH) / HEIGHT);

        centerY -= dy * scale / HEIGHT;
    }

    void updateColor(double delta)
    {
        colorTime += delta;
    }

    void handleEvent(const sf::Event& event)
    {
        if (const auto* mouse =
            event.getIf<sf::Event::MouseButtonPressed>())
        {
            if (mouse->button == sf::Mouse::Button::Left)
            {
                dragging = true;
                lastMouse = mouse->position;
            }
        }

        if (const auto* mouse =
            event.getIf<sf::Event::MouseButtonReleased>())
        {
            if (mouse->button == sf::Mouse::Button::Left)
                dragging = false;
        }

        if (const auto* mouse =
            event.getIf<sf::Event::MouseMoved>())
        {
            if (dragging)
            {
                int dx = mouse->position.x - lastMouse.x;
                int dy = mouse->position.y - lastMouse.y;

                pan(dx, dy);

                lastMouse = mouse->position;
                render();
            }
        }

        if (const auto* wheel =
            event.getIf<sf::Event::MouseWheelScrolled>())
        {
            sf::Vector2i position(
                static_cast<int>(wheel->position.x),
                static_cast<int>(wheel->position.y)
            );

            if (wheel->delta > 0)
                zoomAt(position, 1.25);
            else
                zoomAt(position, 0.8);

            render();
        }

        if (const auto* key =
            event.getIf<sf::Event::KeyPressed>())
        {
            if (key->code == sf::Keyboard::Key::Up)
                maxIterations += 50;

            if (key->code == sf::Keyboard::Key::Down)
                maxIterations = std::max(50, maxIterations - 50);

            if (key->code == sf::Keyboard::Key::R)
            {
                centerX = -0.5;
                centerY = 0.0;
                zoom = 1.0;
                maxIterations = 150;
            }

            if (key->code == sf::Keyboard::Key::Escape)
                std::cout << "INFINITUM running...\n";

            render();
        }
    }

    void draw(sf::RenderWindow& window)
    {
        window.draw(sprite);
    }
};


int main()
{
    sf::RenderWindow window(
        sf::VideoMode({WIDTH, HEIGHT}),
        "INFINITUM | Fractal Explorer"
    );

    window.setFramerateLimit(60);

    Mandelbrot fractal;

    sf::Clock clock;

    std::cout << "\n";
    std::cout << "=============================\n";
    std::cout << "       INFINITUM v1.0\n";
    std::cout << "=============================\n";
    std::cout << "Mouse wheel : Zoom\n";
    std::cout << "Left drag   : Pan\n";
    std::cout << "UP          : More iterations\n";
    std::cout << "DOWN        : Fewer iterations\n";
    std::cout << "R           : Reset\n";
    std::cout << "=============================\n";

    fractal.render();

    while (window.isOpen())
    {
        while (const std::optional event =
            window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            fractal.handleEvent(*event);
        }

        double delta = clock.restart().asSeconds();

        fractal.updateColor(delta * 0.5);

        // Refresh colour animation
        // (full recalculation for this initial version)
        fractal.render();

        window.clear(sf::Color::Black);

        fractal.draw(window);

        window.display();
    }

    return 0;
}

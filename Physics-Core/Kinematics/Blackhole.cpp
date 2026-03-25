#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>

using namespace std;

struct Star {
    sf::Vector2f pos;
    float depth;
    float phase;
};

int main() {
    // SFML 3: VideoMode requires explicit construction
    sf::RenderWindow window(sf::VideoMode({1000, 1000}), "Kerr Singularity: Cinematic Final");
    window.setFramerateLimit(60);

    // 🌌 PARALLAX STARS & TWINKLE
    vector<Star> starData;
    for(int i = 0; i < 4000; i++) {
        starData.push_back({
            {(float)(rand() % 1000), (float)(rand() % 1000)}, 
            0.4f + (rand() % 100) / 40.0f, 
            (float)(rand() % 100)          
        });
    }

    sf::RenderTexture mainPass;
    if (!mainPass.resize({1000, 1000})) return -1;

    sf::Shader bhShader;
    // FIXED SHADER: Added #version and fixed array access
    if (!bhShader.loadFromMemory(R"(
        #version 120
        uniform sampler2D texture;
        uniform vec2 center;
        uniform float time;
        uniform float wave;

        float noise(vec2 p) {
            return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
        }

        vec3 filmic(vec3 x) {
            float a = 2.51;
            float b = 0.03;
            float c = 2.43;
            float d = 0.59;
            float e = 0.14;
            return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
        }

        void main() {
            // FIX: Access gl_TexCoord as an array element [0]
            vec2 uv = gl_TexCoord[0].xy;
            vec2 dir = uv - center;
            float dist = length(dir);
            float angle = atan(dir.y, dir.x);

            // 1. KERR FRAME DRAGGING & WAVE
            float ripple = sin(dist * 30.0 - time * 8.0) * wave * 0.015;
            float twist = 0.15 / (dist + 0.02);
            float twistedAngle = angle + twist;
            vec2 twistedDir = vec2(cos(twistedAngle), sin(twistedAngle)) * (dist + ripple);

            // 2. CHROMATIC ABERRATION
            float rs = 0.058;
            float lensR = rs / (dist + 0.005);
            float lensB = rs / (dist + 0.008);
            vec2 uvR = center + twistedDir * (1.0 + lensR * 2.2);
            vec2 uvB = center + twistedDir * (1.0 + lensB * 2.2);
            
            vec3 space = vec3(texture2D(texture, uvR).r, texture2D(texture, uv).g, texture2D(texture, uvB).b);
            space += vec3(0.02, 0.01, 0.05) * noise(uv * 2.0) * (1.0 - dist); // Nebula

            // 3. CINEMATIC DISK (Thickness + Redshift)
            float inner = 0.075, outer = 0.45;
            vec3 diskColor = vec3(0.0);
            if (dist > inner && dist < outer) {
                float v = 1.5 / sqrt(dist);
                float beaming = pow(1.0 + (dir.x / dist) * 0.9, 4.0);
                float redshift = smoothstep(inner, inner + 0.3, dist);
                vec3 col = mix(vec3(1.0, 0.05, 0.0), vec3(1.0, 0.7, 0.3), redshift);
                
                float pattern = sin(30.0 * log(dist) - (twistedAngle + time * v));
                float thick = pow(max(0.0, 1.0 - abs(dir.y) / (dist + 0.1)), 3.0);
                diskColor = col * beaming * thick * (0.4 + 0.6 * noise(vec2(dist, twistedAngle)) * pattern);
                diskColor *= smoothstep(inner, inner + 0.01, dist);
            }

            // 4. JETS & RING
            float ring = smoothstep(0.006, 0.0, abs(dist - (inner * 1.15))) * 3.0;
            float jet = pow(max(0.0, 1.0 - abs(dir.x)/(dist + 0.01)), 90.0);
            vec3 highEnergy = vec3(1.0, 0.9, 0.7) * ring + vec3(0.3, 0.5, 1.0) * jet * (0.7 + 0.3 * sin(time * 10.0));

            if (dist < rs - 0.004) {
                gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
            } else {
                gl_FragColor = vec4(filmic(space + diskColor + highEnergy), 1.0);
            }
        }
    )", sf::Shader::Type::Fragment)) {
        return -1;
    }

    sf::Clock clock;
    float waveAmp = 0.0f;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            if (event->is<sf::Event::MouseButtonPressed>()) waveAmp = 1.2f;
        }

        float t = clock.getElapsedTime().asSeconds();
        waveAmp *= 0.94f;

        mainPass.clear(sf::Color::Black);
        vector<sf::Vertex> starVertices;
        for (auto& s : starData) {
            float b = 110.f + 145.f * sin(t * 1.3f + s.phase);
            float x = fmod(s.pos.x + (s.depth * 0.15f), 1000.f);
            starVertices.push_back({{x, s.pos.y}, {(uint8_t)b, (uint8_t)b, 255}, {0,0}});
        }
        mainPass.draw(starVertices.data(), starVertices.size(), sf::PrimitiveType::Points);
        mainPass.display();

        bhShader.setUniform("center", sf::Glsl::Vec2{0.5f, 0.5f});
        bhShader.setUniform("time", t);
        bhShader.setUniform("wave", waveAmp);
        bhShader.setUniform("texture", sf::Shader::CurrentTexture);

        window.clear();
        sf::Sprite scene(mainPass.getTexture());
        
        // Final Bloom Rendering
        window.draw(scene, &bhShader);
        scene.setColor(sf::Color(255, 255, 255, 80)); 
        window.draw(scene, sf::BlendAdd); // Additive Glow
        
        window.display();
    }
    return 0;
}

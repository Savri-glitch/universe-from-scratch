#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <algorithm>

struct Particle
{
    sf::Vector2f pos;
    sf::Vector2f vel;
};

struct Fish
{
    sf::Vector2f pos;
    sf::Vector2f vel;
    sf::Color color;
    float dir;
};

float length(sf::Vector2f v)
{
    return std::sqrt(v.x*v.x+v.y*v.y);
}

float randf(float a,float b)
{
    return a + static_cast<float>(rand())/RAND_MAX*(b-a);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode({900,700}),"Water Simulator");
    window.setFramerateLimit(60);

    const float gravity=900.f;
    const float interactionRadius=20.f;
    const float stiffness=0.45f;
    const float damping=0.995f;

    std::vector<Particle> particles;
    std::vector<Fish> fishes;

    float left=100.f;
    float right=800.f;
    float top=100.f;
    float bottom=650.f;

    float startSurface=420.f;

    for(int i=0;i<600;i++)
    {
        Particle p;
        p.pos={randf(left+50,right-50),randf(startSurface,bottom-50)};
        p.vel={0.f,0.f};
        particles.push_back(p);
    }

    for(int i=0;i<10;i++)
    {
        Fish f;
        f.pos={randf(left+100,right-100),randf(startSurface+40,bottom-40)};
        f.vel={randf(-30,30),randf(-10,10)};
        f.color=sf::Color(rand()%255,rand()%255,rand()%255);
        f.dir=0;
        fishes.push_back(f);
    }

    sf::RectangleShape glass({700.f,550.f});
    glass.setPosition({100.f,100.f});
    glass.setFillColor(sf::Color(255,255,255,20));
    glass.setOutlineThickness(3.f);
    glass.setOutlineColor(sf::Color(180,220,255,200));

    sf::CircleShape particleShape(3.f);

    sf::Clock clock;

    sf::RectangleShape boat({90.f,20.f});
    boat.setFillColor(sf::Color(120,70,20));
    boat.setOrigin({45.f,10.f});

    float boatX=450.f;
    float boatVel=0.f;

    while(window.isOpen())
    {
        float dt=clock.restart().asSeconds();

        while(auto event=window.pollEvent())
        {
            if(event->is<sf::Event::Closed>())
                window.close();
        }

        // slower boat movement
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            boatVel-=120.f*dt;

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            boatVel+=120.f*dt;

        boatVel*=0.90f;
        boatX+=boatVel;

        boatX=std::clamp(boatX,left+40.f,right-40.f);

        sf::Vector2f mouse=(sf::Vector2f)sf::Mouse::getPosition(window);

        for(auto &p:particles)
            p.vel.y+=gravity*dt;

        for(size_t i=0;i<particles.size();i++)
        {
            for(size_t j=i+1;j<particles.size();j++)
            {
                sf::Vector2f diff=particles[j].pos-particles[i].pos;
                float dist=length(diff);

                if(dist<interactionRadius && dist>0)
                {
                    sf::Vector2f dir=diff/dist;
                    float force=(interactionRadius-dist)*stiffness;

                    particles[i].vel-=dir*force;
                    particles[j].vel+=dir*force;
                }
            }
        }

        // mouse interaction
        for(auto &p:particles)
        {
            sf::Vector2f diff=p.pos-mouse;
            float dist=length(diff);

            if(dist<120 && dist>0)
            {
                diff/=dist;
                p.vel+=diff*200.f*dt;
            }
        }

        for(auto &p:particles)
        {
            p.vel*=damping;
            p.pos+=p.vel*dt;

            if(p.pos.x<left){p.pos.x=left;p.vel.x*=-0.6f;}
            if(p.pos.x>right){p.pos.x=right;p.vel.x*=-0.6f;}
            if(p.pos.y<top){p.pos.y=top;p.vel.y*=-0.6f;}
            if(p.pos.y>bottom){p.pos.y=bottom;p.vel.y*=-0.6f;}
        }

        float surfaceY=0.f;
        for(auto &p:particles)
            surfaceY+=p.pos.y;

        surfaceY/=particles.size();
        surfaceY-=110.f;

        float boatY=surfaceY-12.f;

        boat.setPosition({boatX,boatY});
        boat.setRotation(sf::degrees(std::sin(clock.getElapsedTime().asSeconds()*2)*4));

        // create waves from boat movement
        for(auto &p:particles)
        {
            float dx=p.pos.x-boatX;
            float dy=p.pos.y-boatY;
            float d=sqrt(dx*dx+dy*dy);

            if(d<90)
                p.vel.x+=boatVel*0.05f;
        }

        // fish movement
        for(auto &f:fishes)
        {
            sf::Vector2f diff=f.pos-boat.getPosition();

            if(length(diff)<120)
                f.vel+=diff*0.01f;

            f.pos+=f.vel*dt;

            f.vel*=0.99f;

            f.dir=atan2(f.vel.y,f.vel.x)*180/3.14159;

            if(f.pos.x<left+20||f.pos.x>right-20)
                f.vel.x*=-1;

            if(f.pos.y<surfaceY+25)
                f.pos.y=surfaceY+25;

            if(f.pos.y>bottom-20)
                f.vel.y*=-1;
        }

        window.clear(sf::Color(15,15,30));

        // water glow
        sf::CircleShape glow(24);
        glow.setOrigin({24,24});

        for(auto &p:particles)
        {
            glow.setPosition(p.pos);
            glow.setFillColor(sf::Color(60,120,255,40));
            window.draw(glow,sf::BlendAdd);
        }

        for(auto &p:particles)
        {
            particleShape.setPosition(p.pos);
            particleShape.setFillColor(sf::Color(80,140,255));
            window.draw(particleShape);
        }

        // smoother water surface
        sf::VertexArray surface(sf::PrimitiveType::TriangleStrip,100);

        for(int i=0;i<50;i++)
        {
            float x=left+i*(right-left)/49.f;

            float wave=
            sin(clock.getElapsedTime().asSeconds()*1.8+i*0.35f)*6 +
            sin(clock.getElapsedTime().asSeconds()*0.9+i*0.6f)*4;

            surface[i*2].position={x,surfaceY+wave};
            surface[i*2].color=sf::Color(90,160,255);

            surface[i*2+1].position={x,bottom};
            surface[i*2+1].color=sf::Color(30,70,180);
        }

        window.draw(surface);

        // better fish drawing
        for(auto &f:fishes)
        {
            sf::ConvexShape fish;
            fish.setPointCount(3);
            fish.setPoint(0,{0,0});
            fish.setPoint(1,{14,5});
            fish.setPoint(2,{0,10});

            fish.setFillColor(f.color);
            fish.setPosition(f.pos);
            fish.setRotation(sf::degrees(f.dir));

            window.draw(fish);
        }

        window.draw(boat);
        window.draw(glass);

        window.display();
    }

    return 0;
}

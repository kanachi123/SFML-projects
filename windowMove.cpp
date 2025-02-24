#include <SFML/Graphics.hpp>
#include <random>
#include <iostream>

using namespace sf;

std::random_device dev;
std::mt19937 gen(dev());

std::gamma_distribution<double> p(0.1, 100.0);
std::cauchy_distribution<double> p2(0.1,100.0);

int main() {

    RenderWindow win(VideoMode(800, 800), "Gamma Distribution");
    RenderWindow win2(VideoMode(800, 800), "Cauchy Distribution");

    win.setPosition(Vector2i(0.0f, 0.f));
    win2.setPosition(Vector2i(win.getSize().x, 0));

    std::vector<Vertex> points;

    std::vector<Vertex> points2;
    int count = 0;
    for (int i = 0; i < 1000; ++i) {
        double value = 0.8*p(gen);
        double value2 = p2(gen);
       if (value < 800) 
       {
            points.push_back(Vertex(Vector2f(i, value), Color::Color(value,-5*value,value,-5*value)));

       }
       if (value2 < 800)
       {
           points2.push_back(Vertex(Vector2f(i, value), Color::Color(value,  value, value,  value)));

       }

       if (i == 999) 
       {
           count++;
           i = 0;
           if (count == 1000) {
               break;
           }
       }
    }
    int i = 0;
    while (win.isOpen() || win2.isOpen()) {
        Event event1,event2;

        while (win.pollEvent(event1)) {
            if (event1.type == Event::Closed) {
                win.close();
            }
        }
        while (win2.pollEvent(event2)) 
        {
            if (event2.type == Event::Closed) 
            {
                win2.close();
            }
        }

        win.draw(&points[0], points.size(), Points);
        win2.draw(&points2[0], points2.size(), Points);
        
        win.setPosition(Vector2i(win.getPosition().x + sin(i*6.24/180), win.getPosition().y + cos(i * 6.24 / 180)));

        win2.setPosition(Vector2i(win2.getPosition().x - sin(i * 6.24 / 180), win2.getPosition().y - cos(i * 6.24 / 180)));
        i += 0.2;
        
        if (win.getPosition().y >= 1080 || win.getPosition().y <= 0) {
            win.close();
        }      
        
        Vector2i win2Pos = win2.getPosition();
        Vector2u win2Size = win2.getSize();
        if (win2Pos.y + win2Size.y <= 0 || win2Pos.y >= 1080) {
            win2.close();
        }

        win.display();
        win2.display();
        win.clear();
        win2.clear();
    }

    return 0;
}

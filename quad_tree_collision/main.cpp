#include "crpo.h"
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

using namespace sf;
using namespace std;

const int N = 1000;
const int WINDOWSIZE = 800;

struct particle {
  circle back;
  CircleShape front;
  double mass = 0.001;
  float vx = 0, vy = 0;
};

struct Quadtree {

    float x, y, width, height;
    int capacity;
    vector<particle*> particles;
    bool divided = false;
    Quadtree *nw = nullptr, *ne = nullptr, *sw = nullptr, *se = nullptr;

    Quadtree(float x_, float y_, float w_, float h_, int cap = 4)
        : x(x_), y(y_), width(w_), height(h_), capacity(cap) {}

    ~Quadtree() {
        delete nw; delete ne; delete sw; delete se;
    }

    bool contains(const particle& p) const {
        float px = p.back.getPoint().getX();
        float py = p.back.getPoint().getY();
        return (px >= x && px < x + width && py >= y && py < y + height);
    }

    void subdivide() {
        float hw = width / 2, hh = height / 2;
        nw = new Quadtree(x, y, hw, hh, capacity);
        ne = new Quadtree(x + hw, y, hw, hh, capacity);
        sw = new Quadtree(x, y + hh, hw, hh, capacity);
        se = new Quadtree(x + hw, y + hh, hw, hh, capacity);
        divided = true;
    }

    void insert(particle* p) {
        if (!contains(*p)) return;
        if (particles.size() < capacity) {
            particles.push_back(p);
        } else {
            if (!divided) subdivide();
            nw->insert(p); ne->insert(p); sw->insert(p); se->insert(p);
        }
    }

    void query(float qx, float qy, float qr, vector<particle*>& found) const {
        
        if (qx + qr < x || qx - qr > x + width || qy + qr < y || qy - qr > y + height)
            return;
        for (auto* p : particles) {
            float px = p->back.getPoint().getX();
            float py = p->back.getPoint().getY();
            float dx = px - qx, dy = py - qy;
            if (dx * dx + dy * dy <= qr * qr)
                found.push_back(p);
        }
        if (divided) {
            nw->query(qx, qy, qr, found);
            ne->query(qx, qy, qr, found);
            sw->query(qx, qy, qr, found);
            se->query(qx, qy, qr, found);
        }
    }
};

void updatePosition(vector<particle> &shape, Quadtree& qtree) {
    
    for (int i = 0; i < N; i++) {
        setNewX(shape[i].back, static_cast<double>(shape[i].vx));
        setNewY(shape[i].back, static_cast<double>(shape[i].vy));
    }

    for (int i = 0; i < N; i++) {
        vector<particle*> neighbors;
        float radius = static_cast<float>(shape[i].back.getRadius()) * 2.0f;
        qtree.query(
            shape[i].back.getPoint().getX(),
            shape[i].back.getPoint().getY(),
            radius,
            neighbors
        );
        for (auto* other : neighbors) {
            if (other == &shape[i]) continue;
            if (other < &shape[i]) continue;                       
            if (getDistance(shape[i].back, other->back) < shape[i].back.getRadius() + other->back.getRadius()) {
                
                shape[i].vx *= -1.f;
                shape[i].vy *= -1.f;
                other->vx *= -1.f;
                other->vy *= -1.f;
                
            }
        }
    }
    
    for (int i = 0; i < N; i++) {
        float x = shape[i].back.getPoint().getX();
        float y = shape[i].back.getPoint().getY();
        
        float r = static_cast<float>(shape[i].back.getRadius());
        if (x - r < 0 || x + r > WINDOWSIZE) shape[i].vx *= -1.f;
        if (y - r < 0 || y + r > WINDOWSIZE) shape[i].vy *= -1.f;
    }

    for (int i = 0; i < N; i++) {
        shape[i].front.setPosition(
            static_cast<float>(shape[i].back.getPoint().getX()),
            static_cast<float>(shape[i].back.getPoint().getY()));
    }
}

void initParticle(vector<particle> &shape) {
  srand(static_cast<unsigned int>(time(nullptr)));

  for (int i = 0; i < N; i++) {
    CircleShape tmp;
    circle c(2);
    particle p;
    c.setPoint(Point(rand() % 800, rand() % 800));
    Vector2f position(static_cast<float>(c.getPos().getX()),
                      static_cast<float>(c.getPos().getY()));

    tmp.setFillColor(Color(rand() % 255, rand() % 255, rand() % 255));
    tmp.setOutlineColor(Color(rand() % 255, rand() % 255, rand() % 255));
    tmp.setRadius(static_cast<float>(c.getRadius()));
    tmp.setPosition(position);

    p.back = c;
    p.front = tmp;
    p.mass = static_cast<double>(rand() % 200+1);
    p.vx = static_cast<float>(rand()%50);
    p.vy = static_cast<float>( rand()%50);

    shape.push_back(p);
  }
}

int main() {
    RenderWindow window(VideoMode(WINDOWSIZE, WINDOWSIZE), "project 4");
    vector<particle> shape;

    initParticle(shape);

    while (window.isOpen()) {
        Quadtree qtree(0, 0, WINDOWSIZE, WINDOWSIZE);

        for (int i = 0; i < N; i++) {
            qtree.insert(&shape[i]);
        }

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
        }
        window.clear(Color::White);
        for (int i = 0; i < N; i++) {
            window.draw(shape[i].front);
        }
        updatePosition(shape, qtree);
        window.display();
    }
}
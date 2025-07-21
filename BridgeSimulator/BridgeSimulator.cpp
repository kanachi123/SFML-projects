#include "SFML/Graphics.hpp"
#include <vector>
#include <iostream>
#include <cmath>

/*
press R for fixed points
press N for non fixed points
press O for add point
press P for add point and connect to other points
press right mouse button for play simulation or end simluation
press C for clear simulation
*/

double R = 10;

class vertex
{

public:
    double x = 0, y = 0;
    bool isFixed = false;
    sf::CircleShape shape;

    vertex(double _x = 0, double _y = 0, bool fixed = false) : x(_x), y(_y), isFixed(fixed) 
    {
        shape.setRadius(R);
        shape.setPosition(x, y);
        shape.setFillColor(isFixed ? sf::Color::Red : sf::Color::Green);
    };

    void move(double dx, double dy) 
    {
        if (!isFixed) {
            x += dx;
            y += dy;
            shape.setPosition(x, y);
        }
    }

    inline void mergeWith(const vertex& other)
    {
        this->x = (this->x + other.x) / 2;
        this->y = (this->y + other.y) / 2;
        shape.setPosition(this->x, this->y);
    }

    inline sf::Vector2f getPos() 
    {
        sf::Vector2f coord(x, y);
        return coord;
    }
};

class vertexSystem 
{

public:
    std::vector<vertex> vertexArray;
    std::vector<std::pair<size_t, size_t>> edges;

    double springConstant = 0.2f;
    double damping = 0.2f;
    double gravity = 0.7f;
    double restLength = 100.0f;

    bool dragging = false; 
    size_t draggedVertexIndex = -1;

    void addVertex(bool fixed, const sf::RenderWindow& window) 
    {

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        if (vertexArray.empty() || !isCloseToExistingVertex(mousePos))
        {
            vertexArray.push_back(vertex(mousePos.x, mousePos.y, fixed));
        }

        else
        {
            size_t lastIndex = vertexArray.size() - 1;
            size_t closestIndex = findClosestVertex(mousePos);
            edges.push_back({ lastIndex, closestIndex });
            vertex tmp = vertexArray[closestIndex];
            vertexArray.erase(vertexArray.begin() + closestIndex);
            vertexArray.push_back(tmp);
        }

    }

    bool isCloseToExistingVertex(const sf::Vector2i& mousePos) 
    {
        for (const auto& v : vertexArray) {
            double dx = mousePos.x - v.x;
            double dy = mousePos.y - v.y;
            double distance = std::sqrt(dx * dx + dy * dy);

            if (distance < 2 * R) {
                return true;
            }
        }
        return false;
    }

    size_t findClosestVertex(const sf::Vector2i& mousePos) 
    {
        size_t closestIndex = 0;
        double minDistance = std::numeric_limits<double>::max();

        for (size_t i = 0; i < vertexArray.size(); ++i) 
        {
            double dx = mousePos.x - vertexArray[i].x;
            double dy = mousePos.y - vertexArray[i].y;
            double distance = std::sqrt(dx * dx + dy * dy);

            if (distance < minDistance)
            {
                minDistance = distance;
                closestIndex = i;
            }
        }

        return closestIndex;
    }

    void addEdge(size_t vertex1, size_t vertex2) 
    {
        if (vertex1 < vertexArray.size() && vertex2 < vertexArray.size()) 
        {
            edges.push_back({ vertex1, vertex2 });
        }
    }

    void addEdgesForNewVertex() 
    {
        size_t n = vertexArray.size();
        for (size_t i = 0; i < n - 1; ++i)
        {
            addEdge(i, n - 1);
        }
    }

    void clear() {
        vertexArray.clear();
        edges.clear();
    }

    void applyForces() 
    {
        for (auto& v : vertexArray) 
        {
            if (!v.isFixed) 
            {
                v.move(0, gravity);
            }
        }

        for (const auto& edge : edges) 
        {
            size_t v1 = edge.first;
            size_t v2 = edge.second;

            double dx = vertexArray[v2].x - vertexArray[v1].x;
            double dy = vertexArray[v2].y - vertexArray[v1].y;
            double distance = std::sqrt(dx * dx + dy * dy);

            double force = springConstant * (distance - restLength);

            double fx = force * dx / distance;
            double fy = force * dy / distance;

            if (!vertexArray[v1].isFixed) 
            {
                vertexArray[v1].move(fx * damping, fy * damping);
            }
            if (!vertexArray[v2].isFixed) 
            {
                vertexArray[v2].move(-fx * damping, -fy * damping);
            }
            
        }
    }

    void render(sf::RenderWindow& window) {
        for (const auto& edge : edges) {
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(vertexArray[edge.first].getPos().x + R,vertexArray[edge.first].getPos().y + R), sf::Color::White),
                sf::Vertex(sf::Vector2f(vertexArray[edge.second].getPos().x + R,vertexArray[edge.second].getPos().y + R), sf::Color::White)
            };
            window.draw(line, 2, sf::Lines);
        }
        for (auto& vertex : vertexArray)
        {
            window.draw(vertex.shape);
        }
    }

    void startDragging(const sf::Vector2i& mousePos) {
        size_t closestIndex = findClosestVertex(mousePos);
        double dx = mousePos.x - vertexArray[closestIndex].x;
        double dy = mousePos.y - vertexArray[closestIndex].y;
        double distance = std::sqrt(dx * dx + dy * dy);

        if (distance < 2 * R) {
            dragging = true;
            draggedVertexIndex = closestIndex;
        }
    }

    void stopDragging() {
        dragging = false;
        draggedVertexIndex = -1;
    }

    void drag(const sf::Vector2i& mousePos) {
        if (dragging && draggedVertexIndex != -1) {
            vertex& v = vertexArray[draggedVertexIndex];
            v.move(mousePos.x - v.x, mousePos.y - v.y);
        }
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 800), "Bridge Simulation");

    vertexSystem bridge;

    bool isFixedMode = false;
    bool start = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::R) {
                    isFixedMode = true;
                }
                else if (event.key.code == sf::Keyboard::N) {
                    isFixedMode = false;
                }
                else if (event.key.code == sf::Keyboard::C) {
                    bridge.clear();
                }

                if (event.key.code == sf::Keyboard::P) {
                    bridge.addVertex(isFixedMode, window);
                    bridge.addEdgesForNewVertex();
                }


                if (event.key.code == sf::Keyboard::O) {
                    bridge.addVertex(isFixedMode, window);

                    if (bridge.vertexArray.size() > 1) {
                        bridge.addEdge(bridge.vertexArray.size() - 2, bridge.vertexArray.size() - 1);
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left && bridge.vertexArray.empty() == 0) 
            {
                bridge.startDragging(sf::Mouse::getPosition(window));
            }

            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) 
            {
                bridge.stopDragging();
            }

            if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && bridge.vertexArray.empty() == 0)
            {
                bridge.drag(sf::Mouse::getPosition(window));
            }

            if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
            {
                start = !start;
            }
        }

        if (start) 
        {
            bridge.applyForces();
        }

        window.clear();
        bridge.render(window);
        window.display();
    }

    return 0;
}

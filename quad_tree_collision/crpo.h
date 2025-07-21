#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

const double PI = 3.14;

std::ostringstream out;

class Point {
    double _x = 0, _y = 0;
public:
    Point(double x = 0, double y = 0) : _x(x), _y(y) {}

    Point& setX(double x) { _x = x; return *this; }
    double getX() const { return _x; }
    Point& setY(double y) { _y = y; return *this; }
    double getY() const { return _y; }

    Point& operator++() { ++_x; ++_y; return *this; }
    Point operator++(int) { Point temp = *this; _x++; _y++; return temp; }
    Point& operator--() { --_x; --_y; return *this; }
    Point operator--(int) { Point temp = *this; _x--; _y--; return temp; }

    Point operator+(const Point& other) const { return Point(_x + other._x, _y + other._y); }
    Point operator-(const Point& other) const { return Point(_x - other._x, _y - other._y); }
    Point& operator+=(const Point& other) { _x += other._x; _y += other._y; return *this; }
    Point& operator-=(const Point& other) { _x -= other._x; _y -= other._y; return *this; }
    bool operator==(const Point& other) const { return _x == other._x && _y == other._y; }
    bool operator!=(const Point& other) const { return !(*this == other); }
    bool operator>(const Point& other) const { return std::hypot(_x, _y) > std::hypot(other._x, other._y); }
    bool operator<(const Point& other) const { return std::hypot(_x, _y) < std::hypot(other._x, other._y); }
    bool operator>=(const Point& other) const { return *this == other || *this > other; }
    bool operator<=(const Point& other) const { return *this == other || *this < other; }

    friend std::istream& operator>>(std::istream& in, Point& point);
    friend std::ostream& operator<<(std::ostream& os, const Point& point);
};

std::istream& operator>>(std::istream& in, Point& point) {
    in >> point._x >> point._y;
    return in;
}

std::ostream& operator<<(std::ostream& os, const Point& point) {
    os << "(" << point._x << " , " << point._y << ")" << std::endl;
    return os;
}

class circle {
public:
    circle() : pos(new Point()), radius(1) { _count++; }
    circle(double radius) : circle() { std::cout << "circle count updated\ncount = " << get_count() << std::endl << std::endl; this->radius = (radius > 0) ? radius : 1; }
    circle(double x, double y, double radius) : circle(radius) { pos->setX(x).setY(y); }
    circle(const circle& other) : pos(new Point(*(other.pos))), radius(other.radius) { _count++; }
    ~circle() { delete pos; _count--; std::cout << "circle has been deleted" << std::endl; }

    circle& setPos(const Point&);
    Point getPos() const;
    circle& setRadius(double);
    double getRadius() const;
    static int get_count() { return _count; };
    std::string info()const;
    circle& setPoint(const Point& p) { pos->setX(p.getX()).setY(p.getY()); return *this; };
    Point getPoint()const { return *pos; };
    circle& operator =(const circle& other) { if (this != &other) { *pos = *(other.pos); radius = other.radius; } return *this; }
    friend double getArea(const circle& c);
    friend std::istream& operator>>(std::istream& in, circle& c);
    friend std::ostream& operator<<(std::ostream& os, const circle& c);
private:
    Point* pos;
    double radius = 2;
    int static _count;
};

std::istream& operator>>(std::istream& in, circle& c) {
    double x, y;
    in >> x >> y;
    c.setPos(Point(x, y));
    return in;
}
std::ostream& operator<<(std::ostream& os, const circle& c) {
    os << "(" << c.getPos().getX() << " , " << c.getPos().getY() << ")" << std::endl;
    return os;
}

int circle::_count = 0;

circle& circle::setPos(const Point& p) {
    delete pos;
    pos = new Point(p.getX(), p.getY());
    return *this;
}
Point circle::getPos()const { return *pos; }
double circle::getRadius()const { return radius; }
std::string circle::info() const {
    out << "circle info\n\nradius = " << radius << "\nx = " << pos->getX() << "\ny = " << pos->getY() << std::endl << std::endl;
    std::string result = out.str();
    return result;
}
circle& circle::setRadius(double radius) { this->radius = radius; return *this; }
bool isPoint(const Point& point, const circle& pos, double radius) {
    double dx = point.getX() - pos.getPos().getX();
    double dy = point.getY() - pos.getPos().getY();
    return (dx * dx + dy * dy == radius * radius);
}
double getDistance(const circle& c1, const circle& c2) {
    double dx = c2.getPos().getX() - c1.getPos().getX();
    double dy = c2.getPos().getY() - c1.getPos().getY();
    return std::sqrt(dx * dx + dy * dy);
}
int number_of_intersections(const circle& c1, const circle& c2) {
    double d = getDistance(c1, c2);
    double r1 = c1.getRadius();
    double r2 = c2.getRadius();
    if (d > r1 + r2 || d < std::abs(r1 - r2))
        return 0;
    else if (std::abs(d - (r1 + r2)) < 1e-6 || std::abs(d - std::abs(r1 - r2)) < 1e-6)
        return 1;
    else
        return 2;
}
double getLength(const circle& c) { return 2 * PI * c.getRadius(); }
double getArea(const circle& c) { return PI * std::pow(c.radius, 2); }
void setNewX(circle& c, double len) {
    Point p = c.getPos();
    p.setX(p.getX() + len);
    c.setPos(p);
}
void setNewY(circle& c, double len) {
    Point p = c.getPos();
    p.setY(p.getY() + len);
    c.setPos(p);
}
void setNewSize(circle& c, double k) { c.setRadius(k); }

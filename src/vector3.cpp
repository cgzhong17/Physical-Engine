#include "vector3.hpp"
#include <cmath>
#include <stdexcept>
using namespace std;

Vector3::Vector3() : x{ 0.0 }, y{ 0.0 }, z{ 0.0 } {}

Vector3::Vector3(double x, double y, double z) : x{ x }, y{ y }, z{ z } {}

double Vector3::getX() const {
    return x;
}

double Vector3::getY() const {
    return y;
}

double Vector3::getZ() const {
    return z;
}

Vector3 Vector3 :: operator-(const Vector3& other) const {
    return Vector3(x - other.x, y - other.y, z - other.z);
}

Vector3 Vector3 :: operator+(const Vector3& other) const {
    return Vector3(x + other.x, y + other.y, z + other.z);
}

Vector3 Vector3 :: operator*(double scaler) const {
    return Vector3(x * scaler, y * scaler, z * scaler);
}

Vector3 Vector3 :: operator/(double scaler) const {
    if (scaler == 0.0)
        throw domain_error("Cannot divide Vector3 by zero");
    return Vector3(x / scaler, y / scaler, z / scaler);
}

double Vector3::dot(const Vector3& other) const {
    return x * other.x + y * other.y + z * other.z;
}

double Vector3::length() const {
    return sqrt(x * x + y * y + z * z);
}

Vector3 Vector3::normalized() const {
    const double magnitude = length();
    return magnitude == 0.0 ? Vector3() : *this / magnitude;
}

PhysicalObjects::PhysicalObjects(Vector3 position, shared_ptr<Color> color_ptr, double mass)
    : position{ position }, color{ color_ptr }, mass{ mass }, velocity{}, accumulatedForce{} {
    if (!isfinite(mass) || mass <= 0.0)
        throw invalid_argument("Mass must be finite and positive");
}

Vector3 PhysicalObjects::getPosition() const {
    return position;
}

void PhysicalObjects::setPosition(const Vector3& position) {
    this->position = position;
}

shared_ptr<Color> PhysicalObjects::getColor_ptr() const {
    return color;
}

Color::Color() : r{ 0.0 }, g{ 0.0 }, b{ 0.0 } {}

Color::Color(double r, double g, double b) : r{ r }, g{ g }, b{ b } {
    if (!isfinite(r) || !isfinite(g) || !isfinite(b) ||
        r < 0.0 || r > 255.0 || g < 0.0 || g > 255.0 || b < 0.0 || b > 255.0)
        throw out_of_range("Color channel must be in [0, 255]");
}

double Color::GetR() const {
    return r;
}

double Color::GetG() const {
    return g;
}

double Color::GetB() const {
    return b;
}

Sphere::Sphere(Vector3 position, std::shared_ptr<Color> color, double radius, double mass)
    : PhysicalObjects{ position, color, mass }, radius{ radius } {
    if (radius <= 0.0)
        throw invalid_argument("Sphere radius must be positive");
}

double Sphere::volumn() const {
    return (4.0 / 3.0) * PI * pow(radius, 3);
}

double Sphere::getRadius() const {
    return radius;
}

Vector3 Sphere::getNormal(const Vector3& point) const {
    return (point - getPosition()).normalized();
}

Cube::Cube(Vector3 position, std::shared_ptr<Color> color, double length, double width, double height,
           double mass)
    : PhysicalObjects{ position, color, mass }, length{ length }, width{ width }, height{ height } {}

double Cube::volumn() const {
    return length * width * height;
}

double Cube::getLength() const {
    return length;
}

double Cube::getWidth() const {
    return width;
}

double Cube::getHeight() const {
    return height;
}

Vector3 Cube::getNormal(const Vector3& point) const {
    (void)point;
    return Vector3(0.0, 0.0, 0.0);
}

void PhysicalObjects::addForce(const Vector3& force) {
    accumulatedForce = accumulatedForce + force;
}

void PhysicalObjects::addGravity() {
    addForce(Vector3(0.0, -GRAVITY, 0.0) * mass);
}

void PhysicalObjects::updateForce(double deltaTime) {
    if (!isfinite(deltaTime) || deltaTime < 0.0)
        throw invalid_argument("Time step must be finite and nonnegative");
    //if (deltaTime == 0.0)
    //   return;

    const Vector3 acceleration = accumulatedForce / mass;
    velocity = velocity + acceleration * deltaTime;
    position = position + velocity * deltaTime;
    accumulatedForce = Vector3();
}

double PhysicalObjects::getMass() const {
    return mass;
}

Vector3 PhysicalObjects::getVelocity() const {
    return velocity;
}

void PhysicalObjects::setVelocity(const Vector3& velocity) {
    this->velocity = velocity;
}



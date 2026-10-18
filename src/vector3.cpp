#include "vector3.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
using namespace std;

Vector3::Vector3() : x{ 0.0 }, y{ 0.0 }, z{ 0.0 } {}

Vector3::Vector3(double x, double y, double z) : x{ x }, y{ y }, z{ z } {}

Vector3::Vector3(const QVector3D& vector)
    : Vector3(vector.x(), vector.y(), vector.z()) {}

QVector3D Vector3::toQVector3D() const {
    return QVector3D(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
}

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

PhysicalObjects::PhysicalObjects(Vector3 position, shared_ptr<Color> color_ptr) : position{ position }, color{ color_ptr } {}

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

Color::Color(const QColor& color) : Color() {
    if (!color.isValid())
        throw invalid_argument("Cannot convert an invalid QColor");
    const QColor rgb = color.toRgb();
    r = clamp(static_cast<double>(rgb.redF()), 0.0, 1.0) * 255.0;
    g = clamp(static_cast<double>(rgb.greenF()), 0.0, 1.0) * 255.0;
    b = clamp(static_cast<double>(rgb.blueF()), 0.0, 1.0) * 255.0;
}

QColor Color::toQColor() const {
    return QColor::fromRgbF(static_cast<float>(r / 255.0),
                           static_cast<float>(g / 255.0),
                           static_cast<float>(b / 255.0));
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

Sphere::Sphere(Vector3 position, std::shared_ptr<Color> color, double radius) : PhysicalObjects{ position, color }, radius{ radius } {
    if (radius <= 0.0)
        throw invalid_argument("Sphere radius must be positive");
}

double Sphere::shape() const {
    return (4.0 / 3.0) * PI * pow(radius, 3);
}

double Sphere::getRadius() const {
    return radius;
}

Vector3 Sphere::getNormal(const Vector3& point) const {
    return (point - getPosition()).normalized();
}

Cube::Cube(Vector3 position, std::shared_ptr<Color> color, double length, double width, double height) : PhysicalObjects{ position, color }, length{ length }, width{ width }, height{ height } {}

double Cube::shape() const {
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

void PhysicalObjects::addForce(const Vector3& force, double mass, double timeStep) {
	double x = this->getPosition().getX() + (force.getX() / mass) * timeStep;
	double y = this->getPosition().getY() + (force.getY() / mass) * timeStep;
	double z = this->getPosition().getZ() + (force.getZ() / mass) * timeStep;
	this->setPosition(Vector3(x, y, z));
}

void PhysicalObjects::addGravity(double mass, double timeStep) {

}

void registerPhysicsMetaTypes() {
    qRegisterMetaType<Vector3>("Vector3");
    qRegisterMetaType<Color>("Color");
}

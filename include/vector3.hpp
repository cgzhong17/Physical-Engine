#pragma once
#include <memory>

constexpr double PI = 3.14159265358979;
constexpr double GRAVITY = 9.81;
class Vector3 {
public:
    Vector3();
    Vector3(double x, double y, double z);
    double getX() const;
    double getY() const;
    double getZ() const;
    Vector3 operator-(const Vector3& other) const;
    Vector3 operator+(const Vector3& other) const;
    Vector3 operator*(double scaler) const;
    Vector3 operator/(double scaler) const;
    double dot(const Vector3& other) const;
    double length() const;
    Vector3 normalized() const;

private:
    double x;
    double y;
    double z;
};

class Color {
public:
    Color();
    Color(double r, double g, double b);
    double GetR() const;
    double GetG() const;
    double GetB() const;
private:
    double r;
    double g;
    double b;
};

class PhysicalObjects {
public:
    PhysicalObjects(Vector3 position, std::shared_ptr<Color> color, double mass = 1.0);
    virtual ~PhysicalObjects() = default;
    virtual double volumn() const = 0;
    Vector3 getPosition() const;
    void setPosition(const Vector3& position);
    std::shared_ptr<Color> getColor_ptr() const;
    virtual Vector3 getNormal(const Vector3& point) const = 0;
    void addForce(const Vector3& force);
    void addGravity();
    void updateForce(double deltaTime);
    double getMass() const;
    Vector3 getVelocity() const;
    void setVelocity(const Vector3& velocity);
private:
    Vector3 position;
    std::shared_ptr<Color> color;
    double mass;
    Vector3 velocity;
    Vector3 accumulatedForce;
};


class Sphere : public PhysicalObjects {
public:
    Sphere(Vector3 position, std::shared_ptr<Color> color, double radius, double mass = 1.0);
    double getRadius() const;
    double volumn() const override;
    Vector3 getNormal(const Vector3& point) const override;

private:
    double radius;
};

class Cube : public PhysicalObjects {
public:
    Cube(Vector3 position, std::shared_ptr<Color> color, double length, double width, double height,
         double mass = 1.0);
    double getLength() const;
    double getWidth() const;
    double getHeight() const;
    double volumn() const override;
    Vector3 getNormal(const Vector3& point) const override;

private:
    double length;
    double width;
    double height;
};

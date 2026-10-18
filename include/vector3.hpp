#pragma once
#include <memory>
#include <QColor>
#include <QMetaType>
#include <QVector3D>

constexpr double PI = 3.14159265358979;
constexpr double GRAVITY = 9.81;
class Vector3 {
public:
    Vector3();
    Vector3(double x, double y, double z);
    explicit Vector3(const QVector3D& vector);
    // Qt uses float coordinates; calculations in this class retain double precision.
    QVector3D toQVector3D() const;
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
    // RGB channels remain in [0, 255]. QColor alpha is not stored.
    explicit Color(const QColor& color);
    QColor toQColor() const;
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
    PhysicalObjects(Vector3 position, std::shared_ptr<Color> color);
    virtual ~PhysicalObjects() = default;
    virtual double shape() const = 0;
    Vector3 getPosition() const;
    void setPosition(const Vector3& position);
    std::shared_ptr<Color> getColor_ptr() const;
    virtual Vector3 getNormal(const Vector3& point) const = 0;
	void addForce(const Vector3& force, double mass, double timeStep);
	void addGravity(double mass, double timeStep);
private:
    Vector3 position;
    std::shared_ptr<Color> color;
};


class Sphere : public PhysicalObjects {
public:
    Sphere(Vector3 position, std::shared_ptr<Color> color, double radius);
    double getRadius() const;
    double shape() const override;
    Vector3 getNormal(const Vector3& point) const override;

private:
    double radius;
};

class Cube : public PhysicalObjects {
public:
    Cube(Vector3 position, std::shared_ptr<Color> color, double length, double width, double height);
    double getLength() const;
    double getWidth() const;
    double getHeight() const;
    double shape() const override;
    Vector3 getNormal(const Vector3& point) const override;

private:
    double length;
    double width;
    double height;
};

Q_DECLARE_METATYPE(Vector3)
Q_DECLARE_METATYPE(Color)

// Call once before connecting queued signals/slots that carry these value types.
void registerPhysicsMetaTypes();

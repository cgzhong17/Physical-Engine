#include "vector3.hpp"

#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

namespace {
void require(bool condition, const char* message) {
    if (!condition)
        throw std::runtime_error(message);
}

template <typename Exception, typename Function>
void requireThrows(Function function, const char* message) {
    try {
        function();
    } catch (const Exception&) {
        return;
    }
    throw std::runtime_error(message);
}

void requireVectorNear(const Vector3& actual, const Vector3& expected, const char* message) {
    require((actual - expected).length() < 1e-10, message);
}

void testForceIntegration(const std::shared_ptr<Color>& material) {
    Sphere body(Vector3(), material, 1.0, 2.0);
    require(body.getMass() == 2.0, "Mass was not initialized");
    requireVectorNear(body.getVelocity(), Vector3(), "Initial velocity must be zero");

    // Equal and opposite forces cancel, and adding forces does not advance time.
    body.addForce(Vector3(3, 4, 5));
    body.addForce(Vector3(-3, -4, -5));
    requireVectorNear(body.getPosition(), Vector3(), "Adding force moved the object");
    requireVectorNear(body.getVelocity(), Vector3(), "Adding force changed velocity");
    body.updateForce(0.5);
    requireVectorNear(body.getPosition(), Vector3(), "Opposite forces did not cancel");

    // Multiple forces combine with gravity before a single integration step.
    body.addForce(Vector3(2, 0, 0));
    body.addForce(Vector3(6, 0, 0));
    body.addGravity();
    requireVectorNear(body.getPosition(), Vector3(), "Adding gravity moved the object");
    body.updateForce(0.5);
    requireVectorNear(body.getVelocity(), Vector3(2, -GRAVITY * 0.5, 0),
                      "Net force did not update velocity");
    requireVectorNear(body.getPosition(), Vector3(1, -GRAVITY * 0.25, 0),
                      "Position must use the updated velocity");
    const Vector3 velocity = body.getVelocity();
    body.updateForce(0.5);
    requireVectorNear(body.getVelocity(), velocity, "Force was not cleared after the step");
    requireVectorNear(body.getPosition(), Vector3(2, -GRAVITY * 0.5, 0),
                      "Object did not continue moving by inertia");

    // Gravity has the same acceleration for different masses, over 60 steps.
    Sphere light(Vector3(), material, 1.0);
    Cube heavy(Vector3(), material, 1, 1, 1, 10.0);
    light.setVelocity(Vector3(3, 0, 0));
    heavy.setVelocity(Vector3(3, 0, 0));
    const double dt = 1.0 / 60.0;
    for (int step = 0; step < 60; ++step) {
        light.addGravity();
        heavy.addGravity();
        light.updateForce(dt);
        heavy.updateForce(dt);
    }
    require(light.getMass() == 1.0, "Default mass must be one");
    requireVectorNear(light.getVelocity(), Vector3(3, -GRAVITY, 0),
                      "One second of gravity produced the wrong velocity");
    // Semi-implicit Euler sums g * dt^2 * (1 + ... + 60).
    requireVectorNear(light.getPosition(), Vector3(3, -GRAVITY * dt * dt * 60 * 61 / 2, 0),
                      "Fixed-step gravity produced the wrong position");
    requireVectorNear(heavy.getVelocity(), light.getVelocity(), "Gravity depends on mass");
    requireVectorNear(heavy.getPosition(), light.getPosition(), "Fall distance depends on mass");

    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double infinity = std::numeric_limits<double>::infinity();
    for (double invalidMass : {0.0, -1.0, nan, infinity}) {
        requireThrows<std::invalid_argument>([&] {
            (void)Sphere(Vector3(), material, 1.0, invalidMass);
        }, "Invalid mass must fail");
    }
    Sphere paused(Vector3(), material, 1.0);
    paused.addForce(Vector3(1, 0, 0));
    paused.updateForce(0.0);
    requireVectorNear(paused.getPosition(), Vector3(), "Zero time step moved the object");
    for (double invalidTime : {-1.0, nan, infinity}) {
        requireThrows<std::invalid_argument>([&] { paused.updateForce(invalidTime); },
                                             "Invalid time step must fail");
    }
    paused.updateForce(1.0);
    requireVectorNear(paused.getVelocity(), Vector3(1, 0, 0),
                      "Zero or invalid time step consumed the pending force");
}
}

int main() {
    try {
        const Vector3 a(1, 2, 3), b(4, 5, 6);
        const Vector3 sum = a + b, difference = b - a;
        require(sum.getX() == 5 && sum.getY() == 7 && sum.getZ() == 9,
                "Vector addition failed");
        require(difference.getX() == 3 && difference.getY() == 3 && difference.getZ() == 3,
                "Vector subtraction failed");
        require((a * 2).getZ() == 6 && (b / 2).getY() == 2.5 && a.dot(b) == 32,
                "Vector scaling or dot product failed");
        require(std::abs(Vector3(3, 4, 0).normalized().length() - 1.0) < 1e-12,
                "Normalization failed");
        require(Vector3().normalized().length() == 0, "Zero normalization failed");
        require(Vector3(1.0000000001, 0, 0).getX() == 1.0000000001,
                "Physics lost double precision");
        requireThrows<std::domain_error>([&] { (void)(a / 0); }, "Division by zero must fail");

        const auto material = std::make_shared<Color>(12.5, 100.25, 254.5);
        require(material->GetR() == 12.5 && material->GetG() == 100.25 &&
                material->GetB() == 254.5, "Color channels lost precision");
        requireThrows<std::out_of_range>([] { (void)Color(256, 0, 0); },
                                         "Out-of-range color must fail");
        requireThrows<std::out_of_range>([] {
            (void)Color(std::numeric_limits<double>::quiet_NaN(), 0, 0);
        }, "NaN color must fail");

        Sphere sphere(Vector3(1, 2, 3), material, 2);
        require(std::abs(sphere.volumn() - 32.0 * PI / 3.0) < 1e-12,
                "Sphere volume failed");
        const Vector3 normal = sphere.getNormal(Vector3(1, 2, 5));
        require(normal.getX() == 0 && normal.getY() == 0 && normal.getZ() == 1,
                "Sphere normal failed");
        requireThrows<std::invalid_argument>([&] { (void)Sphere(a, material, 0); },
                                             "Zero radius must fail");
        Cube cube(a, material, 2, 3, 4);
        cube.setPosition(b);
        require(cube.volumn() == 24 && cube.getPosition().getZ() == 6 &&
                cube.getColor_ptr() == material, "Geometry access failed");
        testForceIntegration(material);
        std::cout << "Backend vector, color, geometry and force integration checks passed\n";
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}

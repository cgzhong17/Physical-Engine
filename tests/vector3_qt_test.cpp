#include "qt_adapter.hpp"
#include "qt_test_receiver.hpp"

#include <QCoreApplication>
#include <QMetaObject>
#include <QVariant>
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
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    try {
        registerPhysicsMetaTypes();
        const QVector3D qtVector(1.25f, -2.5f, 3.75f);
        const Vector3 vector = PhysicsQt::fromQVector3D(qtVector);
        require(vector.getX() == 1.25 && vector.getY() == -2.5 && vector.getZ() == 3.75,
                "QVector3D import failed");
        require(PhysicsQt::toQVector3D(vector) == qtVector, "QVector3D round trip failed");
        const Vector3 precise(1.0000000001, 0.0, 0.0);
        require(precise.getX() == 1.0000000001, "Physics lost double precision");
        require(Vector3().normalized().length() == 0.0, "Zero normalization failed");
        requireThrows<std::domain_error>([&] { (void)(vector / 0.0); },
                                         "Division by zero must fail");

        const QColor qtColor(25, 128, 250);
        const Color color = PhysicsQt::fromQColor(qtColor);
        require(PhysicsQt::toQColor(color).rgb() == qtColor.rgb(), "QColor round trip failed");
        require(std::abs(PhysicsQt::toQColor(Color(12.5, 100.25, 254.5)).redF() * 255.0 - 12.5) < 0.01,
                "Fractional RGB channel was truncated");
        require(PhysicsQt::toQColor(PhysicsQt::fromQColor(QColor(1, 2, 3, 4))).alpha() == 255,
                "RGB-only alpha policy failed");
        requireThrows<std::invalid_argument>([] { (void)PhysicsQt::fromQColor(QColor()); },
                                             "Invalid QColor must fail");
        requireThrows<std::out_of_range>([] {
            (void)Color(std::numeric_limits<double>::quiet_NaN(), 0.0, 0.0);
        }, "NaN color must fail");
        requireThrows<std::out_of_range>([] { (void)Color(256.0, 0.0, 0.0); },
                                         "Out-of-range color must fail");

        require(QVariant::fromValue(vector).value<Vector3>().getZ() == vector.getZ(),
                "Vector3 QVariant storage failed");
        require(PhysicsQt::toQColor(QVariant::fromValue(color).value<Color>()) == PhysicsQt::toQColor(color),
                "Color QVariant storage failed");
        Receiver receiver;
        require(QMetaObject::invokeMethod(&receiver, "receiveVector", Qt::QueuedConnection,
                                         Q_ARG(Vector3, vector)), "Vector queue failed");
        require(QMetaObject::invokeMethod(&receiver, "receiveColor", Qt::QueuedConnection,
                                         Q_ARG(Color, color)), "Color queue failed");
        require(!receiver.vectorReceived && !receiver.colorReceived, "Call was not queued");
        QCoreApplication::processEvents();
        require(receiver.vectorReceived && receiver.vector.getX() == vector.getX(),
                "Queued Vector3 delivery failed");
        require(receiver.colorReceived && PhysicsQt::toQColor(receiver.color) == PhysicsQt::toQColor(color),
                "Queued Color delivery failed");

        auto material = std::make_shared<Color>(255.0, 100.0, 0.0);
        Sphere sphere(Vector3(), material, 1.0);
        Cube cube(Vector3(1, 2, 3), material, 2.0, 3.0, 4.0);
        cube.setPosition(PhysicsQt::fromQVector3D(QVector3D(4, 5, 6)));
        require(cube.getPosition().getX() == 4 && cube.getLength() == 2.0 &&
                cube.getWidth() == 3.0 && cube.getHeight() == 4.0 && sphere.getRadius() == 1.0,
                "Renderer geometry access failed");
        std::cout << "Qt conversions, queued delivery and geometry checks passed\n";
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
    return 0;
}


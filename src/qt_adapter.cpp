#include "qt_adapter.hpp"

#include <algorithm>
#include <stdexcept>

namespace PhysicsQt {
Vector3 fromQVector3D(const QVector3D& vector) {
    return Vector3(vector.x(), vector.y(), vector.z());
}

QVector3D toQVector3D(const Vector3& vector) {
    return QVector3D(static_cast<float>(vector.getX()), static_cast<float>(vector.getY()),
                     static_cast<float>(vector.getZ()));
}

Color fromQColor(const QColor& color) {
    if (!color.isValid())
        throw std::invalid_argument("Cannot convert an invalid QColor");
    const QColor rgb = color.toRgb();
    return Color(std::clamp(static_cast<double>(rgb.redF()), 0.0, 1.0) * 255.0,
                 std::clamp(static_cast<double>(rgb.greenF()), 0.0, 1.0) * 255.0,
                 std::clamp(static_cast<double>(rgb.blueF()), 0.0, 1.0) * 255.0);
}

QColor toQColor(const Color& color) {
    return QColor::fromRgbF(static_cast<float>(color.GetR() / 255.0),
                            static_cast<float>(color.GetG() / 255.0),
                            static_cast<float>(color.GetB() / 255.0));
}
}

void registerPhysicsMetaTypes() {
    qRegisterMetaType<Vector3>("Vector3");
    qRegisterMetaType<Color>("Color");
}

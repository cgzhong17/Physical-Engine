#pragma once

#include "vector3.hpp"
#include <QColor>
#include <QMatrix4x4>
#include <QMetaType>
#include <QPointF>
#include <QVector3D>

Q_DECLARE_METATYPE(Vector3)
Q_DECLARE_METATYPE(Color)

// Qt conversions belong to the optional PhysicalEngineQt target.
namespace PhysicsQt {
Vector3 fromQVector3D(const QVector3D& vector);
QVector3D toQVector3D(const Vector3& vector);
// RGB channels remain in [0, 255]. QColor alpha is not stored.
Color fromQColor(const QColor& color);
QColor toQColor(const Color& color);
}

// Call once before connecting queued signals/slots carrying physics value types.
void registerPhysicsMetaTypes();

Vector3 generateVectorFromMouse(const QPointF& mousePos, const QPointF& origin,
                                const QMatrix4x4& camera, double scale);

#include "vector3.hpp"

#include <QMatrix4x4>
#include <QMouseEvent>
#include <QPointF>
#include <QVector3D>

Vector3 generateVectorFromMouse(const QPointF& mousePos, const QPointF& origin, const QMatrix4x4& camera, double scale) {
	// Convert mouse position to view coordinates
	float viewX = static_cast<float>((mousePos.x() - origin.x()) / scale);
	float viewY = static_cast<float>((origin.y() - mousePos.y()) / scale); // Invert Y axis
	// Create a QVector3D in view space
	QVector3D viewpoint(viewX, viewY, 0.0f);
	// Transform the view point back to world coordinates using the inverse of the camera matrix
	QMatrix4x4 inverseCamera = camera.inverted();
	QVector3D worldPoint = inverseCamera.map(viewpoint);
	return Vector3(worldPoint.x(), worldPoint.y(), worldPoint.z());
}


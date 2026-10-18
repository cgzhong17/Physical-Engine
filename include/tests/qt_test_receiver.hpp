#pragma once

#include "vector3.hpp"
#include <QObject>

// AUTOMOC generates this header's meta-object code as a separate source file.
class Receiver : public QObject {
    Q_OBJECT
public:
    Vector3 vector;
    Color color;
    bool vectorReceived = false;
    bool colorReceived = false;

public slots:
    void receiveVector(Vector3 value) { vector = value; vectorReceived = true; }
    void receiveColor(Color value) { color = value; colorReceived = true; }
};

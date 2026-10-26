#include "qt_adapter.hpp"

#include <QApplication>
#include <QFontDatabase>
#include <QImage>
#include <QMatrix4x4>
#include <QPainter>
#include <QPolygonF>
#include <QRadialGradient>
#include <QTimer>
#include <QWidget>
#include <algorithm>
#include <array>

// The physics objects own world coordinates and dimensions. This widget only
// projects and paints them; future simulation steps should call update().
class SceneWidget : public QWidget {
public:
    SceneWidget()
        : sphere(Vector3(-1.8, 0.0, 0.0), std::make_shared<Color>(72, 161, 255), 0.9),
          cube(Vector3(1.5, 0.0, 0.0), std::make_shared<Color>(255, 167, 73), 1.6, 1.6, 1.6) {
        setWindowTitle("Physical Engine - Qt Preview");
        resize(900, 600);
        camera.rotate(22.0f, 1.0f, 0.0f, 0.0f);
        camera.rotate(-30.0f, 0.0f, 1.0f, 0.0f);
    }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.fillRect(rect(), QColor(20, 27, 40));
        painter.setPen(QColor(230, 235, 245));
        painter.drawText(30, 38, "C++ physics objects / Qt rendering");
        painter.setPen(QColor(150, 165, 188));
        painter.drawText(30, 64, "Static scene preview - world coordinates use double precision");

        scale = std::min(width() / 9.0, height() / 6.0);
        painter.save();
        painter.translate(width() * 0.5, height() * 0.55);
        painter.setPen(QPen(QColor(46, 59, 78), 1.0));
        for (int line = -4; line <= 4; ++line) {
            painter.drawLine(project(QVector3D(-4, -1, line)), project(QVector3D(4, -1, line)));
            painter.drawLine(project(QVector3D(line, -1, -4)), project(QVector3D(line, -1, 4)));
        }
        // The two separated objects do not overlap in this fixed camera view.
        drawCube(painter);
        drawSphere(painter);
        painter.restore();
        painter.setPen(QColor(150, 165, 188));
        painter.drawText(30, height() - 25, "Blue: Sphere    Orange: Cube    View: orthographic");
    }

private:
    QPointF project(const QVector3D& point) const {
        const QVector3D view = camera.map(point);
        return QPointF(view.x() * scale, -view.y() * scale);
    }

    void drawSphere(QPainter& painter) const {
        const QPointF center = project(PhysicsQt::toQVector3D(sphere.getPosition()));
        const double radius = sphere.getRadius() * scale;
        const QColor color = PhysicsQt::toQColor(*sphere.getColor_ptr());
        QRadialGradient gradient(center - QPointF(radius * 0.3, radius * 0.3), radius * 1.3);
        gradient.setColorAt(0.0, color.lighter(150));
        gradient.setColorAt(0.6, color);
        gradient.setColorAt(1.0, color.darker(200));
        painter.setPen(QPen(color.lighter(120), 1.0));
        painter.setBrush(gradient);
        painter.drawEllipse(center, radius, radius);
    }

    void drawCube(QPainter& painter) const {
        const float halfX = static_cast<float>(cube.getLength() / 2.0);
        const float halfY = static_cast<float>(cube.getHeight() / 2.0);
        const float halfZ = static_cast<float>(cube.getWidth() / 2.0);
        const QVector3D center = PhysicsQt::toQVector3D(cube.getPosition());
        const std::array<QVector3D, 8> vertices = {{
            center + QVector3D(-halfX, -halfY, -halfZ),
            center + QVector3D( halfX, -halfY, -halfZ),
            center + QVector3D( halfX,  halfY, -halfZ),
            center + QVector3D(-halfX,  halfY, -halfZ),
            center + QVector3D(-halfX, -halfY,  halfZ),
            center + QVector3D( halfX, -halfY,  halfZ),
            center + QVector3D( halfX,  halfY,  halfZ),
            center + QVector3D(-halfX,  halfY,  halfZ)
        }};
        struct Face { std::array<int, 4> indices; int brightness; double depth = 0.0; };
        std::array<Face, 6> faces = {{
            {{0, 1, 2, 3}, 75}, {{4, 5, 6, 7}, 100},
            {{0, 4, 7, 3}, 85}, {{1, 5, 6, 2}, 115},
            {{0, 1, 5, 4}, 65}, {{3, 2, 6, 7}, 140}
        }};
        for (auto& face : faces)
            for (int index : face.indices)
                face.depth += camera.map(vertices[index]).z();
        std::sort(faces.begin(), faces.end(), [](const Face& a, const Face& b) {
            return a.depth < b.depth;
        });
        const QColor color = PhysicsQt::toQColor(*cube.getColor_ptr());
        for (const auto& face : faces) {
            QPolygonF polygon;
            for (int index : face.indices)
                polygon << project(vertices[index]);
            painter.setPen(QPen(color.darker(180), 1.0));
            painter.setBrush(color.lighter(face.brightness));
            painter.drawPolygon(polygon);
        }
    }

    Sphere sphere;
    Cube cube;
    QMatrix4x4 camera;
    double scale = 1.0;
};

int main(int argc, char** argv) {
    QApplication app(argc, argv);
#ifdef Q_OS_WIN
    // The offscreen platform used by the smoke test may not discover system fonts.
    const int fontId = QFontDatabase::addApplicationFont(
        qEnvironmentVariable("WINDIR") + "/Fonts/segoeui.ttf");
    if (fontId >= 0) {
        const QStringList families = QFontDatabase::applicationFontFamilies(fontId);
        if (!families.isEmpty())
            app.setFont(QFont(families.first(), 10));
    }
#endif
    registerPhysicsMetaTypes();
    SceneWidget viewer;
    viewer.show();
    if (app.arguments().contains("--smoke-test")) {
        QTimer::singleShot(0, &app, [&] {
            QImage frame(viewer.size(), QImage::Format_ARGB32_Premultiplied);
            frame.fill(Qt::transparent);
            viewer.render(&frame);
            const int saveIndex = app.arguments().indexOf("--save-preview");
            const bool saved = saveIndex < 0 || (saveIndex + 1 < app.arguments().size() &&
                frame.save(app.arguments().at(saveIndex + 1)));
            app.exit(saved && !frame.isNull() ? 0 : 1);
        });
    }
    return app.exec();
}

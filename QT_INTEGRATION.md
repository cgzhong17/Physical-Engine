# C++ 物理层与 Qt 渲染层

当前采用 **Qt Widgets + QPainter**。`include/vector3.hpp` 与 `src/vector3.cpp` 保存物体数据并执行计算；
`src/examples/qt_viewer.cpp` 读取这些数据，把三维坐标投影到窗口中。
示例使用固定正交相机，展示静态球体和立方体，没有增加运动或碰撞模拟。

## 目录结构

```text
include/
  vector3.hpp
  tests/qt_test_receiver.hpp
src/
  vector3.cpp
  generatevector.cpp
  examples/qt_viewer.cpp
  tests/vector3_qt_test.cpp
build/
  <configuration>/bin/  可执行文件
  <configuration>/lib/  编译生成的库
  legacy-out/           原 out 目录的历史构建文件
```

所有 `.cpp` 放在 `src/` 下，所有 `.hpp` 放在 `include/` 下。
CMake 按目标显式列出源文件；新增源码时需将它加入对应目标。
旧构建目录中的缓存保留在 `build/legacy-out/`，新配置使用新的构建目录。

## 在 Qt Creator 中运行

1. 打开根目录的 `CMakeLists.txt`。
2. 选择本机 **Qt 6.12.0 MinGW 64-bit** 套件。此套件不能与 Visual Studio 的 MSVC 编译器混用。
3. 将构建目录设为项目下的 `build/<configuration>`，例如 `build/qt-mingw`，构建并运行 `PhysicalEngineQtDemo`。

本机 Visual Studio 的 `CMakeSettings.json` 也使用 `build/<configuration>`。
该文件包含本机 Qt 路径，已作为本地配置加入 `.gitignore`。

也可以在项目根目录用 PowerShell 构建，以下路径对应当前机器的 Qt 安装：

```powershell
$env:PATH = 'C:\Qt\Tools\mingw1310_64\bin;C:\Qt\6.12.0\mingw_64\bin;' + $env:PATH
& 'C:\Qt\Tools\CMake_64\bin\cmake.exe' -S . -B build/qt-mingw -G Ninja '-DCMAKE_MAKE_PROGRAM=C:/Qt/Tools/Ninja/ninja.exe' '-DCMAKE_CXX_COMPILER=C:/Qt/Tools/mingw1310_64/bin/g++.exe' '-DCMAKE_PREFIX_PATH=C:/Qt/6.12.0/mingw_64' -DCMAKE_BUILD_TYPE=Debug
& 'C:\Qt\Tools\CMake_64\bin\cmake.exe' --build build/qt-mingw
& 'C:\Qt\Tools\CMake_64\bin\ctest.exe' --test-dir build/qt-mingw --output-on-failure
& '.\build\qt-mingw\bin\PhysicalEngineQtDemo.exe'
```

## 在自己的窗口中读取物理对象

```cpp
#include "vector3.hpp"

Sphere ball(Vector3(0.0, 1.0, 0.0),
            std::make_shared<Color>(72.0, 161.0, 255.0), 0.9);

// 物理计算继续使用 Vector3 和 double。
ball.setPosition(ball.getPosition() + Vector3(0.1, 0.0, 0.0));

// 绘制时才转换成 Qt 类型。
QVector3D renderPosition = ball.getPosition().toQVector3D();
QColor renderColor = ball.getColor_ptr()->toQColor();
double radius = ball.getRadius();

// Qt 数据也可显式转回物理层类型。
Vector3 worldPosition(QVector3D(1.0f, 2.0f, 3.0f));
Color material(QColor(255, 128, 64));
```

立方体提供 `getLength()`、`getWidth()`、`getHeight()`。
预览约定 x 为长、y 为高、z 为宽，物体位置为中心点。
自己的物理计算更新位置后，在 GUI 线程调用窗口的 `update()` 请求重绘。
`QVector3D` 使用 float，转换只用于绘制，物理坐标仍保留 double 精度。
`Color` 保留原有 0–255 RGB 通道，不保存透明度；无效 QColor 会抛出异常。

## Qt 信号和槽

`Vector3`、`Color` 可存入 `QVariant`。若需要队列连接，在建立连接前调用一次：

```cpp
registerPhysicsMetaTypes();
```

使用自己的 `QObject` 控制器发送数据；数值类继续保持可复制。
跨线程时应传递数据副本，并让窗口在 GUI 线程更新，避免同时读写同一个物体。

## 构建选项与当前范围

- `PHYSICAL_ENGINE_BUILD_QT_DEMO=OFF` 可关闭示例窗口，只构建物理库及其测试。
- `BUILD_TESTING=OFF` 可关闭测试。库公开链接 Qt Core 和 Gui，示例另需 Widgets。
- `Cube::getNormal()` 仍是原有占位实现，不可用于立方体碰撞法线计算。
- 示例使用 QPainter 绘制投影图形，不是通用三维深度渲染器。

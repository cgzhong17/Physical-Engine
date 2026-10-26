# 后端计算与可选 Qt 渲染

`PhysicalEngine` 是纯标准 C++20 静态库，不依赖 Qt。Mac 可以只开发、编译和测试后端；
Windows 构建同一套后端源码，并按需启用 `PhysicalEngineQt` 适配层和 Qt Widgets 示例。
Mac 生成的静态库不能直接供 Windows 链接，Windows 需要使用自己的工具链重新编译。

测试源码和测试专用头文件统一位于项目根目录的 `tests/`，生产代码位于 `src/` 和 `include/`。

## Mac / 无 Qt 的后端开发

在项目根目录运行：

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DPHYSICAL_ENGINE_WITH_QT=OFF -DPHYSICAL_ENGINE_BUILD_QT_DEMO=OFF
cmake --build build
ctest --test-dir build --output-on-failure
```

默认关闭 Qt。后端只需包含 `vector3.hpp`，链接 `PhysicalEngine`。
构建产物位于 `build/lib/` 和 `build/bin/`。

VS Code 安装 Microsoft C/C++ 和 CMake Tools，以 CMake Tools 为 configuration provider，
执行 `CMake: Configure` 后获取 include 路径。Makefiles / Ninja 构建会导出
`build/compile_commands.json`。无 Qt 环境下，Qt 适配层、Qt 示例和 Qt 测试文件仍需要
Qt 开发库才能单独解析；它们不参与后端构建。

## Windows / Qt 构建

使用与 Qt 安装匹配的编译器：MinGW Qt 对应 MinGW，MSVC Qt 对应兼容的 MSVC 工具链。
在 Qt Creator 中打开 `CMakeLists.txt`，选择 Qt 套件，并在 CMake 配置中设置
`PHYSICAL_ENGINE_WITH_QT=ON`、`PHYSICAL_ENGINE_BUILD_QT_DEMO=ON`。

也可以在已配置好工具链的终端运行，替换成自己的 Qt 安装前缀：

```powershell
cmake -S . -B build/qt -DPHYSICAL_ENGINE_WITH_QT=ON -DPHYSICAL_ENGINE_BUILD_QT_DEMO=ON -DCMAKE_PREFIX_PATH=C:/Qt/<version>/<kit>
cmake --build build/qt --config Debug
ctest --test-dir build/qt -C Debug --output-on-failure
```

当前 Mac 的 `.vscode/settings.json` 显式关闭 Qt；Windows 使用 VS Code 时需将其中的
这两个选项改为 `true`，并配置当地 Qt 路径。

Qt 适配层链接 Qt Core、Gui，示例额外链接 Widgets。自己的 Qt 程序链接：

```cmake
target_link_libraries(MyQtApp PRIVATE PhysicalEngineQt Qt${QT_VERSION_MAJOR}::Widgets)
```

## Qt 类型转换

原先放在后端类中的 Qt 构造函数、`toQVector3D()`、`toQColor()` 已移入
`qt_adapter.hpp`，使用 `PhysicsQt` 的独立函数。Qt 调用端需同步调整：

```cpp
#include "qt_adapter.hpp"

Sphere ball(Vector3(0.0, 1.0, 0.0),
            std::make_shared<Color>(72.0, 161.0, 255.0), 0.9);

QVector3D renderPosition = PhysicsQt::toQVector3D(ball.getPosition());
QColor renderColor = PhysicsQt::toQColor(*ball.getColor_ptr());
Vector3 worldPosition = PhysicsQt::fromQVector3D(QVector3D(1.0f, 2.0f, 3.0f));
Color material = PhysicsQt::fromQColor(QColor(255, 128, 64));
```

后端坐标保留 double 精度；Qt 的 QVector3D 转换使用 float。
Color 保存 0–255 的 RGB 通道，不保存 alpha；无效 QColor 转换抛出异常。
鼠标到世界坐标的 `generateVectorFromMouse()` 也声明在 `qt_adapter.hpp`，只在 Qt 层编译。

## Qt 信号与槽

Qt 调用端包含 `qt_adapter.hpp` 后，Vector3、Color 可存入 QVariant。
使用队列连接之前调用一次：

```cpp
registerPhysicsMetaTypes();
```

通过自己的 QObject 控制器传递数据；数值类继续保持可复制。跨线程传递副本，
在 GUI 线程更新窗口，避免并发读写同一个物体。

## 力、重力和时间步

物体质量由构造函数最后一个参数指定，单位 kg，默认 1，必须为有限正数。
初始速度默认为零，可通过 `setVelocity()` 设置，并用 `getVelocity()` 读取。
坐标以 m 为单位，+Y 向上；力以 N 为单位，速度以 m/s 为单位。

`addForce(force)` 累计外力，`addGravity()` 累计向下的重力 `mass * GRAVITY`，
这两个函数不接收时间步，也不立即改变位置或速度。
每一步添加完所有力后，调用一次 `updateForce(dt)`，dt 以秒为单位：

```cpp
Sphere ball(Vector3(0, 10, 0), std::make_shared<Color>(72, 161, 255), 0.9, 2.0);
const double dt = 1.0 / 60.0;
for (int step = 0; step < 60; ++step) {
    ball.addForce(Vector3(1, 0, 0));
    ball.addGravity();
    ball.updateForce(dt);
}
```

上面的循环模拟一秒，但不会等待现实中的一秒。
`updateForce()` 使用半隐式欧拉法，先更新速度，再更新位置，最后清空本步合力。
速度会保留到下一步；持续力（包括重力）必须每一步重新添加。
负数或非有限 dt 会抛出异常，dt 为零时保持状态和待处理的力。

## 当前运算范围

- `vector3_test` 不依赖 Qt，检查向量、颜色、几何、合力、惯性和固定步长重力积分。
- `vector3_qt_test` 和 `qt_viewer_smoke` 仅在对应 Qt 选项启用时构建。
- `BUILD_TESTING=OFF` 可关闭所有测试。
- `Cube::getNormal()` 仍返回零向量，不可用于立方体碰撞法线计算。
- Qt 示例只绘制静态场景，尚未加入运动或碰撞模拟。


cmake_minimum_required(VERSION 3.20)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Use an out-of-source build: cmake -S . -B build/<configuration>")
endif()

project(PhysicalEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Keep generated binaries and libraries inside the selected build directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui)

# List each target's sources explicitly to keep application and test code separate.
add_library(PhysicalEngine STATIC
    src/vector3.cpp
    src/generatevector.cpp
    include/vector3.hpp
)

target_include_directories(PhysicalEngine PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(PhysicalEngine PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
)

option(PHYSICAL_ENGINE_BUILD_QT_DEMO "Build the Qt Widgets scene preview" ON)
if(PHYSICAL_ENGINE_BUILD_QT_DEMO)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
    add_executable(PhysicalEngineQtDemo src/examples/qt_viewer.cpp)
    target_link_libraries(PhysicalEngineQtDemo PRIVATE
        PhysicalEngine Qt${QT_VERSION_MAJOR}::Widgets
    )
endif()

include(CTest)
if(BUILD_TESTING)
    add_executable(vector3_qt_test
        src/tests/vector3_qt_test.cpp
        include/tests/qt_test_receiver.hpp
    )
    set_target_properties(vector3_qt_test PROPERTIES AUTOMOC ON)
    target_link_libraries(vector3_qt_test PRIVATE PhysicalEngine)
    add_test(NAME vector3_qt_test COMMAND vector3_qt_test)
    if(PHYSICAL_ENGINE_BUILD_QT_DEMO)
        add_test(NAME qt_viewer_smoke COMMAND PhysicalEngineQtDemo --smoke-test)
        set_tests_properties(qt_viewer_smoke PROPERTIES
            ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
        )
    endif()
endif()

# 编译器警告
if(MSVC)
    target_compile_options(PhysicalEngine PRIVATE /W4)
else()
    target_compile_options(PhysicalEngine PRIVATE
        -Wall -Wextra -Wpedantic
    )
endif()

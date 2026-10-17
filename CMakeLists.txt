
cmake_minimum_required(VERSION 3.20)

project(PhysicalEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 自动查找根目录中的 C++ 源文件
file(GLOB SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# 自动查找头文件
file(GLOB HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

# 创建静态库（不要求 main 函数）
add_library(PhysicalEngine STATIC
    ${SOURCES}
    ${HEADERS}
)

# 让编译器找到当前目录的头文件
target_include_directories(PhysicalEngine PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# 编译器警告
if(MSVC)
    target_compile_options(PhysicalEngine PRIVATE /W4)
else()
    target_compile_options(PhysicalEngine PRIVATE
        -Wall -Wextra -Wpedantic
    )
endif()

# filepath: /home/shadyskies/Desktop/dockstar/dockstar/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

project(Dockstar)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(dockstar src/main.cpp src/dock/dock.cpp src/dock/dock.h)

target_link_libraries(dockstar Qt5::Widgets)
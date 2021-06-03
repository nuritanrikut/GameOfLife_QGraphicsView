cmake_minimum_required(VERSION 3.5)

project(game_of_life_qgraphicsview LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        MainWindow.cpp
        MainWindow.hpp
        Cell.cpp
        Cell.hpp
)

qt_add_executable(game_of_life_qgraphicsview
    ${PROJECT_SOURCES}
)

target_link_libraries(game_of_life_qgraphicsview PRIVATE Qt6::Widgets)

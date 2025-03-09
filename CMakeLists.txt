cmake_minimum_required(VERSION 3.5)

project(CollegeManager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

add_executable(${PROJECT_NAME}
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    DatabaseManager.cpp
    DatabaseManager.h
    TripPlanner.h
    TripPlanner.cpp
)

target_link_libraries(${PROJECT_NAME} Qt6::Widgets Qt6::Sql)
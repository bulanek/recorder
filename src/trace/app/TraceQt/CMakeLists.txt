cmake_minimum_required(VERSION 3.5)

project(TraceQt LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(TraceQt
  main.cpp
  mainwindow.cpp
  mainwindow.h
  mainwindow.ui
)

target_link_libraries(TraceQt PRIVATE Qt5::Widgets)

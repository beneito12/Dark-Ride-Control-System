cmake_minimum_required(VERSION 3.16)

project(VirtualAttractionControl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CSS_STANDARD_REQUIRED ON)

add_executable(attraction 
main.cpp
src/Block.cpp
src/VehicleController.cpp
src/RideController.cpp
)

target_include_directories(attraction PRIVATE include)
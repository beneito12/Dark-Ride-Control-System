cmake_minimum_required(VERSION 3.16)

project(VirtualAttractionControl)

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.8.1
)

FetchContent_MakeAvailable(Catch2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(attraction 
main.cpp
src/Block.cpp
src/VehicleController.cpp
src/RideController.cpp
src/StationController.cpp
src/CanBus.cpp
src/ShowController.cpp
)

target_include_directories(attraction PRIVATE include)

add_executable(tests
    tests/test_station.cpp
    tests/test_ride.cpp
    tests/test_can.cpp
    tests/test_show.cpp
    src/Block.cpp
    src/VehicleController.cpp
    src/StationController.cpp
    src/RideController.cpp
    src/CanBus.cpp
    src/ShowController.cpp
)

target_include_directories(tests PRIVATE include)

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
cmake_minimum_required(VERSION 3.20)
project(
    LiveStats
    VERSION 0.0.1
    DESCRIPTION "C++ library to compute real-time statistics"
    LANGUAGES CXX
)

find_package(Boost 1.82 REQUIRED)

enable_testing()

add_subdirectory(lib)
add_subdirectory(test)

cmake_minimum_required(VERSION 3.23)
project(Solomon)

set(CMAKE_CXX_STANDARD 14)

add_executable(Solomon main.cpp customer.h customer.cpp solomon.h solomon.cpp SimulatedAnealing.cpp SimulatedAnealing.h Shaw_Removal.cpp Shaw_Removal.h Data.cpp Data.h)

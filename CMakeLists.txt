cmake_minimum_required(VERSION 3.23)
project(Solomon)

set(CMAKE_CXX_STANDARD 14)

add_executable(Solomon LNS.cpp Customer.h Customer.cpp solomon.h solomon.cpp SimulatedAnnealing.cpp SimulatedAnnealing.h ShawRemoval.cpp ShawRemoval.h test.cpp test.h Vehicle.cpp Vehicle.h CustomersTimeWindow.cpp CustomersTimeWindow.h)

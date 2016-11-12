cmake_minimum_required (VERSION 2.6)
project (Simulator)
add_definitions(-std=c++11)
add_executable (Simulator Task.h Event.h Device.h Readyset.h EventQueue.h Main.cpp)


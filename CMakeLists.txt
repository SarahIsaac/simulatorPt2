cmake_minimum_required (VERSION 2.6)
project (Simulator)
add_executable (Simulator Task.h Event.h Device.h Readyset.h EventQueue.h Main.cpp)
target_link_libraries (Simulator ${CMAKE_THREAD_LIBS_INIT})

cmake_minimum_required(VERSION 3.28) # CMake 3.12 or higher is required for CXX_STANDARD 20

project(prime-finder)


set(CMAKE_CXX_STANDARD 17) # Set C++ standard to C++20


set(CMAKE_COLOR_MAKEFILE ON)


file(GLOB SOURCES "*.cpp")


add_executable(${PROJECT_NAME} ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)


add_subdirectory(tests)


cmake_minimum_required(VERSION 3.28) # CMake 3.12 or higher is required for CXX_STANDARD 20

project(prime-finder)


set(CMAKE_CXX_STANDARD 20) # Set C++ standard to C++20


set(CMAKE_COLOR_MAKEFILE ON)


file(GLOB SOURCES "*.cpp")


find_package(Poco REQUIRED COMPONENTS Foundation Util Net)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} Poco::Foundation Poco::Util Poco::Net)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# only if testing options is enabled

option(BUILD_TESTS "Build the tests" OFF)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
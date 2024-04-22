cmake_minimum_required(VERSION 3.28) # CMake 3.12 or higher is required for CXX_STANDARD 20

project(prime-finder)

set(CMAKE_CXX_STANDARD 17) # Set C++ standard to C++20

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_COLOR_MAKEFILE ON)

add_executable(prime-finder)

target_sources(prime-finder
    PUBLIC
    "main.cpp"
    "peer.cpp"
    "listener.cpp"
    "connector.cpp"
    "connection.cpp"
    "message.cpp"
    "receiver.cpp"
    "sender.cpp"
)
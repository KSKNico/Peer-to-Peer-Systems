cmake_minimum_required(VERSION 3.28) # CMake 3.12 or higher is required for CXX_STANDARD 20

project(prime-finder)

set(CMAKE_CXX_STANDARD 17) # Set C++ standard to C++20

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_COLOR_MAKEFILE ON)

add_executable(prime-finder)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/f8d7d77c06936315286eb55f8de22cd23c188571.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

target_sources(prime-finder
    PUBLIC
    "main.cpp"
    "peer.cpp"
    "listener.cpp"
    "connector.cpp"
    "connection.cpp"
    "message.cpp"
)

enable_testing()

add_executable(
  send_receive
  tests/send_receive.cpp
)

target_link_libraries(
  send_receive
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(send_receive)

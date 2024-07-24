cmake_minimum_required(VERSION 3.12)

# Name of the project
project(MyProjectTests)

# Add GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/master.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
)
FetchContent_MakeAvailable(googletest)

# Enable testing functionality
# enable_testing()

# Include the source directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

# Gather all test files
file(GLOB TEST_SOURCES "*.cpp")

file(GLOB_RECURSE PROJECT_SOURCES "../src/*.cpp")

# remove main file
list(REMOVE_ITEM PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../src/main.cpp)

# Create the test executable
add_executable(runTests ${TEST_SOURCES} ${PROJECT_SOURCES})

find_package(Poco REQUIRED COMPONENTS Foundation Util Net Crypto)

# Link GoogleTest and GoogleMock to the test executable
target_link_libraries(runTests gtest gtest_main)

# Link Poco libraries to the test executable
target_link_libraries(runTests Poco::Foundation Poco::Util Poco::Net Poco::Crypto)

# Add the tests to CTest
include(GoogleTest)
gtest_discover_tests(runTests)
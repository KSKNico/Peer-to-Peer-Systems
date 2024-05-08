cmake_minimum_required(VERSION 3.28)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/f8d7d77c06936315286eb55f8de22cd23c188571.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(SOURCES ../peer.cpp ../message.cpp ../connection.cpp ../peerConfig.cpp
../listener.cpp
../connector.cpp)

set(TEST_SOURCES send_receive.cpp)

add_executable(
peerTest
${SOURCES}
${TEST_SOURCES}
)

target_link_libraries(
peerTest
gtest_main
)

include(GoogleTest)
gtest_discover_tests(peerTest)
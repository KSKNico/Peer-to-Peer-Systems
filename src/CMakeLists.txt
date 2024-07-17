cmake_minimum_required(VERSION 3.30) # CMake 3.12 or higher is required for CXX_STANDARD 20

project(prime-finder)


file(GLOB_RECURSE SOURCES "*.cpp")





# target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(Poco REQUIRED COMPONENTS Foundation Util Net)

target_link_libraries(${PROJECT_NAME} Poco::Foundation Poco::Util Poco::Net ${LIBRARY_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# only if testing options is enabled
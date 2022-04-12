# cmake
cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(PROJECT_SOURCE_DIR "src")
project(vision)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++17")
include_directories(${PROJECT_SOURCE_DIR})
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/*.cpp)
add_executable(${PROJECT_NAME} main.cpp ${SRC_FILES})
#find_package(OpenSSL REQUIRED)
target_link_libraries(${PROJECT_NAME} rdkafka++)
install (TARGETS ${PROJECT_NAME} RUNTIME DESTINATION /usr/local/bin)

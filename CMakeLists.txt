cmake_minimum_required(VERSION 3.10)
project(vision)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")
set(PROJECT_SOURCE_DIR "src")
#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-L/usr/local/lib")

include_directories(${PROJECT_SOURCE_DIR})
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/*.c)
add_executable(${PROJECT_NAME} main.c ${SRC_FILES})

find_package(OpenSSL REQUIRED)

target_link_libraries(${PROJECT_NAME} zmq tarantool)
install (TARGETS ${PROJECT_NAME} RUNTIME DESTINATION /usr/local/bin)

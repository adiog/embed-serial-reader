CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(serial-reader CXX)

ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/deps/cxx-serialport)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/deps/cxx-serialport/include)

FILE(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
FILE(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

add_executable(${PROJECT_NAME} ${sources} ${headers})
target_link_libraries(${PROJECT_NAME} serial)


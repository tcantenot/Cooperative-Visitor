cmake_minimum_required(VERSION 2.6)

project(CooperativeVisitor)

# Executable path
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})

# Headers
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/code/include/*)

# Sources
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/code/src/*)

# C++11 flag
list(APPEND CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")


include_directories(${CMAKE_SOURCE_DIR}/code/include)

# Executable
add_executable(

    CooperativeVisitor

    ${HEADERS}

    ${SOURCES}

)

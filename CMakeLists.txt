cmake_minimum_required(VERSION 3.10)

# Project name and version
project(http_server VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Set compiler flags
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

# Add source files
set(SOURCES
    http_server.cpp
    thread_pools.cpp
)

# Add the executable
add_executable(http_server ${SOURCES})

# Link against necessary libraries
target_link_libraries(http_server
    -lpthread
)

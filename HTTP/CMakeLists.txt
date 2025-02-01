cmake_minimum_required(VERSION 3.10)

# Project name and version
project(http_server VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set compiler flags
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

# Add include directories
include_directories(/opt/homebrew/include)


# Add source files
# Source files
set(SOURCES
    http_server.cpp
    ../common/thread_pools.cpp
    ../common/parsing.cpp
    ../common/handler_post.cpp
    ../common/handler_get.cpp
)
# Add the executable
add_executable(https_server_main ${SOURCES})

# Link against necessary libraries
target_link_libraries(
    http_server
    -lpthread
)

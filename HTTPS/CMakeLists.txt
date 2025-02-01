cmake_minimum_required(VERSION 3.10)

# Project name and version
project(https_server VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set compiler flags
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

# Add include directories
include_directories(/opt/homebrew/include)

# Find OpenSSL
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

# Add source files
# Source files
set(SOURCES
    https_server_main.cpp
    ../common/thread_pools.cpp
    ../common/parsing.cpp
    ../common/handler.cpp
    ../common/handler_post.cpp
    ../common/handler_get.cpp
    https_server.cpp
)
# Add the executable
add_executable(https_server_main ${SOURCES})

# Link against necessary libraries
target_link_libraries(https_server_main
    ${OPENSSL_LIBRARIES}
    -lpthread
)

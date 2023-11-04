cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# Create project
project(lan_eye)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON" FORCE)

# Add executable
add_executable(${PROJECT_NAME}
        # Main
        src/main.cpp
        # Common utils
        inc/common.hpp inc/ratio_utils.hpp
        # Sources
        inc/server.hpp inc/logger.hpp inc/scanner.hpp
        src/server.cpp src/logger.cpp src/scanner.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC inc)

# Add CPM
file(
        DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.38.6/CPM.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
        EXPECTED_HASH SHA256=11c3fa5f1ba14f15d31c2fb63dbc8628ee133d81c8d764caad9a8db9e0bacb07
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

# Add dependencies (via CPM)

# Boost
CPMAddPackage(
        NAME Boost
        VERSION 1.83.0
        GIT_TAG boost-1.83.0
        GITHUB_REPOSITORY boostorg/boost
)

# Drogon webserver
CPMAddPackage(
        NAME drogon
        VERSION 1.8.7
        GIT_TAG v1.8.7
        GITHUB_REPOSITORY drogonframework/drogon
        EXCLUDE_FROM_ALL yes
)

# StaticStringMap
CPMAddPackage(
        NAME static_map
        GIT_TAG v2.0.2
        GITHUB_REPOSITORY BrainStone/StaticMap
        EXCLUDE_FROM_ALL yes
)

target_link_libraries(${PROJECT_NAME} PRIVATE drogon static_map Boost::asio Boost::process)

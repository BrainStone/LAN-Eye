cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

# Create project
project(lan_eye)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON" FORCE)

# Add executable
add_executable(${PROJECT_NAME} src/main.cpp)

# Add CPM
file(
        DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.38.6/CPM.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
        EXPECTED_HASH SHA256=11c3fa5f1ba14f15d31c2fb63dbc8628ee133d81c8d764caad9a8db9e0bacb07
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

# Add dependencies (via CPM)

# Drogon webserver
CPMAddPackage(
        NAME drogon
        VERSION 1.8.7
        GIT_TAG v1.8.7
        GITHUB_REPOSITORY drogonframework/drogon
)

target_link_libraries(${PROJECT_NAME} PRIVATE drogon)

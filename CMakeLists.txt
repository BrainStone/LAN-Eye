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
        inc/server.hpp inc/logger.hpp inc/scanner.hpp inc/capability.hpp
        src/server.cpp src/logger.cpp src/scanner.cpp src/capability.cpp
        src/host_information.cpp
        inc/host_information.hpp
)
target_include_directories(${PROJECT_NAME} PUBLIC inc)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD ${CMAKE_CXX_STANDARD})

# Add CPM
file(
        DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.38.6/CPM.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
        EXPECTED_HASH SHA256=11c3fa5f1ba14f15d31c2fb63dbc8628ee133d81c8d764caad9a8db9e0bacb07
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

# (System) library dependencies

## libcap
find_library(LIBCAP_LIBRARY cap)

# Add dependencies (via CPM)

## Boost
CPMAddPackage(
        NAME Boost
        VERSION 1.83.0
        GIT_TAG boost-1.83.0
        GITHUB_REPOSITORY boostorg/boost
        EXCLUDE_FROM_ALL yes
        GIT_SHALLOW ON
)

## Drogon webserver
CPMAddPackage(
        NAME drogon
        VERSION 1.8.7
        GIT_TAG v1.8.7
        GITHUB_REPOSITORY drogonframework/drogon
        EXCLUDE_FROM_ALL yes
)

## StaticStringMap
CPMAddPackage(
        NAME static_map
        GIT_TAG v2.0.2
        GITHUB_REPOSITORY BrainStone/StaticMap
        EXCLUDE_FROM_ALL yes
)

## Pugixml
CPMAddPackage(
        NAME pugixml
        GIT_TAG v1.14
        GITHUB_REPOSITORY zeux/pugixml
        EXCLUDE_FROM_ALL yes
)

# Put everything together
target_link_libraries(${PROJECT_NAME} PRIVATE
        # STL stuff
        stdc++fs
        # (System) libraries
        ${LIBCAP_LIBRARY}
        # CPM dependencies
        ## Boost
        Boost::asio Boost::process
        ## Normal CPM deps
        drogon static_map pugixml
)

cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# Create project
project(lan_eye)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON" FORCE)

# Add library
add_library(${PROJECT_NAME}_lib STATIC
        # Common utils
        inc/common.hpp inc/ratio_utils.hpp
        # Sources
        inc/server.hpp inc/logger.hpp inc/scanner.hpp inc/capability.hpp inc/host_information.hpp
        src/server.cpp src/logger.cpp src/scanner.cpp src/capability.cpp src/host_information.cpp
)
target_include_directories(${PROJECT_NAME}_lib PUBLIC inc)
set_property(TARGET ${PROJECT_NAME}_lib PROPERTY CXX_STANDARD ${CMAKE_CXX_STANDARD})

# Add executable
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_lib)

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
set(CPM_USE_LOCAL_PACKAGES On)

## Boost
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
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
set(BUILD_STATIC_MAP_TESTS OFF)
CPMAddPackage(
        NAME static_map
        GIT_TAG v2.0.3
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
target_link_libraries(${PROJECT_NAME}_lib PUBLIC
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

###############################################################################
## testing ####################################################################
###############################################################################

enable_testing()
include(GoogleTest)
add_subdirectory(test)

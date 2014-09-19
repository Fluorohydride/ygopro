project (ygopro)

cmake_minimum_required (VERSION 2.8) 
cmake_policy(SET CMP0005 OLD)

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/macros")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(CheckCXXSourceRuns)
include(CheckIncludeFiles)

include (AutoFiles)
include (CreateLaunchers)

include (platform/settings)

if (NOT CMAKE_PREFIX_PATH)
    set (CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/dependencies" CACHE PATH "Path to dependencies")
    set (CMAKE_FIND_FRAMEWORK LAST)
endif()

if (MSVC)

else ()
    find_package(LibEvent REQUIRED)
    find_package(Lua52 REQUIRED)
    find_package(sqlite REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(glew REQUIRED)
    find_package(FREETYPE REQUIRED)
    find_package(GLFW3 REQUIRED)
    find_package(glm REQUIRED)
    find_package(zlib REQUIRED)
endif ()

add_subdirectory (netc)
add_subdirectory (buildin)
add_subdirectory (ocgcore)
add_subdirectory (nets)

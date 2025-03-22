project (ygo)

cmake_minimum_required (VERSION 3.8)

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

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include (AutoFiles)

include (platform/settings)

if (MSVC)
    add_subdirectory (event)
    add_subdirectory (freetype)
    add_subdirectory (irrlicht)
    add_subdirectory (sqlite3)
    add_subdirectory (lua)
else ()
    find_package(LibEvent REQUIRED)
    find_package(Freetype REQUIRED)
    find_package(Irrlicht REQUIRED)
    find_package(Sqlite REQUIRED)
    find_package(Lua REQUIRED)
    find_package(OpenGL REQUIRED)
endif ()

option(USE_IRRKLANG "Use irrKlang sound library" OFF)
if (USE_IRRKLANG)
    set(IRRKLANG_DIR ${CMAKE_SOURCE_DIR}/irrKlang)
endif ()

add_subdirectory (ocgcore)
add_subdirectory (gframe)

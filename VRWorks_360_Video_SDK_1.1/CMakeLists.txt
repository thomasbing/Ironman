cmake_minimum_required(VERSION 3.0.0)

# Set path where samples will be installed
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR} CACHE PATH "Path to where the samples will be installed")
option(INSTALL_SDK "Install binaries into the samples folder" OFF)
option(INSTALL_FLAT "Install binaries into the root folder" OFF)
if (INSTALL_SDK AND INSTALL_FLAT)
    message(FATAL_ERROR "INSTALL_SDK and INSTALL_FLAT are mutually exclusive!")
endif()

project(vrworks360 CXX)

set(CMAKE_CONFIGURATION_TYPES "Release")

# Require C++11 and disable non-standard extensions
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_definitions(-DNOMINMAX -DWIN32_LEAN_AND_MEAN)

# Set common build path for all targets
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(ENABLE_SAMPLES TRUE)
add_subdirectory(external)
add_subdirectory(utilities)
add_subdirectory(nvstitch)
add_subdirectory(nvcalib)
add_subdirectory(samples)

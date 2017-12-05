cmake_minimum_required (VERSION 2.6)
project (vimal)

set(VIMAL_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src/${PROJECT_NAME})

# version
set(VSCPP_VERSION_MAJOR 0u)
set(VSCPP_VERSION_MINOR 1u)
configure_file (
    "${PROJECT_SOURCE_DIR}/version.h.in"
    "${VIMAL_SOURCE_DIR}/version.h"
    )

# configuratin
set(CMAKE_CXX_FLAGS "-std=c++14")
include_directories(
    "${VIMAL_SOURCE_DIR}"
    )

# source files
set(VSCPP_SOURCE
    ${VIMAL_SOURCE_DIR}/error.cpp
    ${VIMAL_SOURCE_DIR}/log.cpp
    src/main.cpp
    )

# executable target
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
add_executable(${PROJECT_NAME} ${VSCPP_SOURCE})
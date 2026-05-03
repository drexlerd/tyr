cmake_minimum_required(VERSION 3.21)
project(InstallNanobind)

include(ExternalProject)

set(TYR_NANOBIND_GIT_TAG v2.12.0 CACHE STRING "Git tag used to build nanobind.")

message(STATUS "Dependency \"nanobind\"...")

# Attempt to find the installed library
find_package(Python 3.8
  REQUIRED COMPONENTS Interpreter Development.Module
  OPTIONAL_COMPONENTS Development.SABIModule)

find_package(nanobind QUIET PATHS ${CMAKE_INSTALL_PREFIX} NO_DEFAULT_PATH)

if (NOT nanobind_FOUND)
    list(APPEND CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            -DNB_TEST=OFF
    )

    message(STATUS "Preparing external project \"nanobind\" with args:")
    foreach(CMAKE_ARG ${CMAKE_ARGS})
        message(STATUS "-- ${CMAKE_ARG}")
    endforeach()


    # If the library is not found, use ExternalProject_Add to download and build it
    ExternalProject_Add(
            nanobind
            GIT_REPOSITORY https://github.com/wjakob/nanobind.git
            GIT_TAG ${TYR_NANOBIND_GIT_TAG}
            PREFIX ${CMAKE_BINARY_DIR}/nanobind
            STAMP_DIR ${CMAKE_BINARY_DIR}/nanobind/stamp/${TYR_NANOBIND_GIT_TAG}
            UPDATE_COMMAND ""
            CMAKE_ARGS ${CMAKE_ARGS}
    )
else()
    message(STATUS "nanobind is already installed.")
endif()


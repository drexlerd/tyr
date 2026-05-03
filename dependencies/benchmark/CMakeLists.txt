cmake_minimum_required(VERSION 3.21)
project(InstallBenchmark)

include(ExternalProject)

set(TYR_BENCHMARK_GIT_TAG v1.8.3 CACHE STRING "Git tag used to build benchmark.")

message(STATUS "Dependency \"benchmark\"...")

find_package(benchmark QUIET PATHS ${CMAKE_INSTALL_PREFIX} NO_DEFAULT_PATH)

if (NOT benchmark_FOUND)

    list(APPEND CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_INSTALL_LIBDIR=${CMAKE_INSTALL_LIBDIR}
            -DCMAKE_BUILD_TYPE=Release
            -DBUILD_SHARED_LIBS:BOOL=${TYR_BUILD_SHARED_DEPENDENCIES}
            -DCMAKE_INSTALL_RPATH=${TYR_DEPENDENCY_INSTALL_RPATH}
            -DBENCHMARK_ENABLE_GTEST_TESTS=OFF
    )

    message(STATUS "Preparing external project \"benchmark\" with args:")
    foreach(CMAKE_ARG ${CMAKE_ARGS})
        message(STATUS "-- ${CMAKE_ARG}")
    endforeach()

    ExternalProject_Add(
            benchmark
            GIT_REPOSITORY https://github.com/google/benchmark.git
            GIT_TAG ${TYR_BENCHMARK_GIT_TAG}
            PREFIX ${CMAKE_BINARY_DIR}/benchmark
            STAMP_DIR ${CMAKE_BINARY_DIR}/benchmark/stamp/${TYR_BENCHMARK_GIT_TAG}
            UPDATE_COMMAND ""
            CMAKE_ARGS ${CMAKE_ARGS}
    )


else()
    message(STATUS "benchmark is already installed.")
endif()

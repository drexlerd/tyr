cmake_minimum_required(VERSION 3.21)
project(InstallGoogletest)

include(ExternalProject)

set(TYR_GOOGLETEST_GIT_TAG release-1.12.1 CACHE STRING "Git tag used to build googletest.")

message(STATUS "Dependency \"GTest\"...")

# Attempt to find the installed googletest library
find_package(GTest QUIET PATHS ${CMAKE_INSTALL_PREFIX} NO_DEFAULT_PATH)

if (NOT GTest_FOUND)

    list(APPEND CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_INSTALL_LIBDIR=${CMAKE_INSTALL_LIBDIR}
            -DBUILD_SHARED_LIBS:BOOL=${TYR_BUILD_SHARED_DEPENDENCIES}
            -DCMAKE_INSTALL_RPATH=${TYR_DEPENDENCY_INSTALL_RPATH}
    )

    message(STATUS "Preparing external project \"googletest\" with args:")
    foreach (CMAKE_ARG ${CMAKE_ARGS})
        message(STATUS "-- ${CMAKE_ARG}")
    endforeach ()

    ExternalProject_Add(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG ${TYR_GOOGLETEST_GIT_TAG}
            PREFIX ${CMAKE_BINARY_DIR}/googletest
            STAMP_DIR ${CMAKE_BINARY_DIR}/googletest/stamp/${TYR_GOOGLETEST_GIT_TAG}
            UPDATE_COMMAND ""
            CMAKE_ARGS ${CMAKE_ARGS}
    )
else ()
    message(STATUS "googletest is already installed.")
endif ()

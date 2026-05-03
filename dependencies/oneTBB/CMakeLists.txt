cmake_minimum_required(VERSION 3.21)
project(InstallOneTBB)

include(ExternalProject)

set(TYR_ONETBB_GIT_TAG v2022.2.0 CACHE STRING "Git tag used to build oneTBB.")

message(STATUS "Dependency \"oneTBB\"...")

# Attempt to find the installed oneTBB library
find_package(oneTBB QUIET PATHS ${CMAKE_INSTALL_PREFIX} NO_DEFAULT_PATH)

if (NOT oneTBB_FOUND)
    list(APPEND CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_INSTALL_LIBDIR=${CMAKE_INSTALL_LIBDIR}
            -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON
            -DBUILD_SHARED_LIBS=${TYR_BUILD_SHARED_DEPENDENCIES}
            -DCMAKE_INSTALL_RPATH=${TYR_DEPENDENCY_INSTALL_RPATH}
            -DTBB_STRICT=OFF
            -DTBB_TEST=OFF
    )

    message(STATUS "Preparing external project \"oneTBB\" with args:")
    foreach(CMAKE_ARG ${CMAKE_ARGS})
        message(STATUS "-- ${CMAKE_ARG}")
    endforeach()
    ExternalProject_Add(
        oneTBB
        GIT_REPOSITORY https://github.com/uxlfoundation/oneTBB.git
        GIT_TAG ${TYR_ONETBB_GIT_TAG}
        PREFIX ${CMAKE_BINARY_DIR}/oneTBB
        STAMP_DIR ${CMAKE_BINARY_DIR}/oneTBB/stamp/${TYR_ONETBB_GIT_TAG}
        UPDATE_COMMAND ""
        CMAKE_ARGS ${CMAKE_ARGS}
    )
else()
    message(STATUS "oneTBB is already installed.")
endif()

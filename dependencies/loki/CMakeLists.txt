cmake_minimum_required(VERSION 3.21)
project(InstallLoki)

include(ExternalProject)

message(STATUS "Dependency \"loki\"...")

set(LOKI_GIT_REPOSITORY "https://github.com/drexlerd/Loki.git" CACHE STRING "Loki Git repository used by the dependency build.")
set(LOKI_GIT_TAG "dev" CACHE STRING "Loki Git tag, branch, or commit used by the dependency build.")

# Attempt to find the installed loki library
find_package(loki QUIET PATHS ${CMAKE_INSTALL_PREFIX} NO_DEFAULT_PATH)

if (NOT loki_FOUND)


    list(APPEND CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON
            -DBUILD_SHARED_LIBS=ON
    )

    message(STATUS "Preparing external project \"loki\" with args:")
    foreach(CMAKE_ARG ${CMAKE_ARGS})
        message(STATUS "-- ${CMAKE_ARG}")
    endforeach()
    ExternalProject_Add(
            loki
            GIT_REPOSITORY ${LOKI_GIT_REPOSITORY}
            GIT_TAG ${LOKI_GIT_TAG}
            PREFIX ${CMAKE_BINARY_DIR}/loki
            DEPENDS boost gtl fmt argparse
            CMAKE_ARGS ${CMAKE_ARGS}
    )
else()
    message(STATUS "loki is already installed.")
endif()

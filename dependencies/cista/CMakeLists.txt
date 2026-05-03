cmake_minimum_required(VERSION 3.21)
project(InstallCista)

include(ExternalProject)

set(TYR_CISTA_GIT_TAG 0065e285d25797c52206a570d06ab19aa1cb1887 CACHE STRING "Git tag used to build cista.")

message(STATUS "Dependency \"cista\"...")

# Attempt to find the installed loki library
find_package(cista QUIET PATHS ${CMAKE_INSTALL_PREFIX} NO_DEFAULT_PATH)

if (NOT cista_FOUND)


    list(APPEND CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_INSTALL_LIBDIR=${CMAKE_INSTALL_LIBDIR}
            -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON
            -DBUILD_SHARED_LIBS=${TYR_BUILD_SHARED_DEPENDENCIES}
            -DCMAKE_INSTALL_RPATH=${TYR_DEPENDENCY_INSTALL_RPATH}
            -DCISTA_INSTALL=ON
    )

    message(STATUS "Preparing external project \"cista\" with args:")
    foreach(CMAKE_ARG ${CMAKE_ARGS})
        message(STATUS "-- ${CMAKE_ARG}")
    endforeach()
    ExternalProject_Add(
        cista
        GIT_REPOSITORY https://github.com/drexlerd/cista.git
        GIT_TAG ${TYR_CISTA_GIT_TAG}
        PREFIX ${CMAKE_BINARY_DIR}/cista
        STAMP_DIR ${CMAKE_BINARY_DIR}/cista/stamp/${TYR_CISTA_GIT_TAG}
        UPDATE_COMMAND ""
        CMAKE_ARGS ${CMAKE_ARGS}
    )
else()
    message(STATUS "cista is already installed.")
endif()

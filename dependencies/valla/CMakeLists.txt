cmake_minimum_required(VERSION 3.21)
project(InstallValla)

include(ExternalProject)

set(TYR_VALLA_GIT_TAG 6a5ae8e2ce59edf9ce1fe015099e3cb24f249bdd CACHE STRING "Git tag used to build valla.")

message(STATUS "Dependency \"valla\"...")

# Attempt to find the installed valla library
find_package(valla QUIET PATHS ${CMAKE_INSTALL_PREFIX} NO_DEFAULT_PATH)

if (NOT valla_FOUND)
    list(APPEND CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_INSTALL_LIBDIR=${CMAKE_INSTALL_LIBDIR}
            -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON
            -DBUILD_SHARED_LIBS=${TYR_BUILD_SHARED_DEPENDENCIES}
            -DCMAKE_INSTALL_RPATH=${TYR_DEPENDENCY_INSTALL_RPATH}
            -DTBB_TEST=OFF
    )

    message(STATUS "Preparing external project \"valla\" with args:")
    foreach(CMAKE_ARG ${CMAKE_ARGS})
        message(STATUS "-- ${CMAKE_ARG}")
    endforeach()
    ExternalProject_Add(
        valla
        GIT_REPOSITORY https://github.com/drexlerd/tree-compression.git
        GIT_TAG ${TYR_VALLA_GIT_TAG}
        PREFIX ${CMAKE_BINARY_DIR}/valla
        STAMP_DIR ${CMAKE_BINARY_DIR}/valla/stamp/${TYR_VALLA_GIT_TAG}
        UPDATE_COMMAND ""
        DEPENDS loki_dependencies
        CMAKE_ARGS ${CMAKE_ARGS}
    )
else()
    message(STATUS "valla is already installed.")
endif()

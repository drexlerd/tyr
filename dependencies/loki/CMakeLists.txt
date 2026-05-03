cmake_minimum_required(VERSION 3.21)
project(InstallLoki)

include(ExternalProject)

message(STATUS "Dependency \"loki\"...")

set(LOKI_GIT_REPOSITORY "https://github.com/drexlerd/Loki.git" CACHE STRING "Loki Git repository used by the dependency build.")
set(LOKI_GIT_TAG "886498f38addef50c8f692bfa4847686d67093a7" CACHE STRING "Loki Git tag, branch, or commit used by the dependency build.")

# Attempt to find the installed loki library
find_package(loki QUIET PATHS ${CMAKE_INSTALL_PREFIX} NO_DEFAULT_PATH)

if (NOT loki_FOUND)

    if(TYR_BUILD_SHARED_DEPENDENCIES)
        set(LOKI_LINK_STATIC_DEPENDENCIES OFF)
    else()
        set(LOKI_LINK_STATIC_DEPENDENCIES ON)
    endif()

    list(APPEND LOKI_DEPENDENCIES_CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_INSTALL_LIBDIR=${CMAKE_INSTALL_LIBDIR}
            -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
            -DLOKI_BUILD_SHARED_DEPENDENCIES=${TYR_BUILD_SHARED_DEPENDENCIES}
    )

    message(STATUS "Preparing external project \"loki_dependencies\" with args:")
    foreach(CMAKE_ARG ${LOKI_DEPENDENCIES_CMAKE_ARGS})
        message(STATUS "-- ${CMAKE_ARG}")
    endforeach()
    ExternalProject_Add(
            loki_dependencies
            GIT_REPOSITORY ${LOKI_GIT_REPOSITORY}
            GIT_TAG ${LOKI_GIT_TAG}
            SOURCE_SUBDIR dependencies
            PREFIX ${CMAKE_BINARY_DIR}/loki_dependencies
            STAMP_DIR ${CMAKE_BINARY_DIR}/loki_dependencies/stamp/${LOKI_GIT_TAG}
            UPDATE_COMMAND ""
            INSTALL_COMMAND ""
            CMAKE_ARGS ${LOKI_DEPENDENCIES_CMAKE_ARGS}
    )

    list(APPEND CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_INSTALL_LIBDIR=${CMAKE_INSTALL_LIBDIR}
            -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON
            -DBUILD_SHARED_LIBS=${TYR_BUILD_SHARED_DEPENDENCIES}
            -DLOKI_LINK_STATIC_DEPENDENCIES=${LOKI_LINK_STATIC_DEPENDENCIES}
            -DCMAKE_INSTALL_RPATH=${TYR_DEPENDENCY_INSTALL_RPATH}
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
            STAMP_DIR ${CMAKE_BINARY_DIR}/loki/stamp/${LOKI_GIT_TAG}
            UPDATE_COMMAND ""
            DEPENDS loki_dependencies
            CMAKE_ARGS ${CMAKE_ARGS}
    )
else()
    add_custom_target(loki_dependencies)
    message(STATUS "loki is already installed.")
endif()

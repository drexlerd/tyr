find_package(Python 3.8 REQUIRED
  COMPONENTS Interpreter Development.Module
  OPTIONAL_COMPONENTS Development.SABIModule)

include(CMakePackageConfigHelpers)

find_package(nanobind CONFIG REQUIRED PATHS ${CMAKE_PREFIX_PATH})
message(STATUS "Found nanobind: ${nanobind_DIR} (found version ${nanobind_VERSION})")

set(PYTYR_NATIVE_DIR "yggdrasil")
set(PYTYR_NATIVE_INCLUDEDIR "${PYTYR_NATIVE_DIR}/${CMAKE_INSTALL_INCLUDEDIR}")
set(PYTYR_NATIVE_LIBDIR "${PYTYR_NATIVE_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(PYTYR_NATIVE_BINDIR "${PYTYR_NATIVE_DIR}/${CMAKE_INSTALL_BINDIR}")
set(PYTYR_NATIVE_CMAKEDIR "${PYTYR_NATIVE_LIBDIR}/cmake")

nanobind_add_module(pytyr
    pytyr.cpp
    pytyr/common.cpp
    pytyr/formalism/formalism.cpp
    pytyr/formalism/planning.cpp
    pytyr/formalism/planning/datas.cpp
    pytyr/formalism/planning/domains.cpp
    pytyr/formalism/planning/indices.cpp
    pytyr/formalism/planning/invariants.cpp
    pytyr/formalism/planning/mutable.cpp
    pytyr/formalism/planning/views.cpp
    pytyr/planning/planning.cpp
)

target_link_libraries(pytyr PRIVATE tyr::core)

if(TYR_BUILD_SHARED)
    if(APPLE)
        set(PYTYR_LIBRARY_ORIGIN "@loader_path/../${PYTYR_NATIVE_DIR}/${CMAKE_INSTALL_LIBDIR}")
    elseif(UNIX)
        set(PYTYR_LIBRARY_ORIGIN "$ORIGIN/../${PYTYR_NATIVE_DIR}/${CMAKE_INSTALL_LIBDIR}")
    endif()

    if(PYTYR_LIBRARY_ORIGIN)
        set_target_properties(pytyr PROPERTIES
            BUILD_RPATH "${PYTYR_LIBRARY_ORIGIN}"
            INSTALL_RPATH "${PYTYR_LIBRARY_ORIGIN}")
    endif()
endif()

install(TARGETS pytyr DESTINATION "pytyr" COMPONENT pytyr)
if(TYR_BUILD_SHARED)
    set_property(TARGET core PROPERTY INTERFACE_INCLUDE_DIRECTORIES
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>")
    install(TARGETS core
            EXPORT pytyrTyrcoreTargets
            LIBRARY DESTINATION "${PYTYR_NATIVE_LIBDIR}" COMPONENT pytyr
            ARCHIVE DESTINATION "${PYTYR_NATIVE_LIBDIR}" COMPONENT pytyr
            RUNTIME DESTINATION "${PYTYR_NATIVE_BINDIR}" COMPONENT pytyr
            INCLUDES DESTINATION "${PYTYR_NATIVE_INCLUDEDIR}")
endif()
install(DIRECTORY "${CMAKE_SOURCE_DIR}/include/tyr"
        DESTINATION "${PYTYR_NATIVE_INCLUDEDIR}"
        COMPONENT pytyr)
configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/tyrConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/tyrConfig.cmake"
        INSTALL_DESTINATION "${PYTYR_NATIVE_CMAKEDIR}/tyr"
        NO_CHECK_REQUIRED_COMPONENTS_MACRO)
install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/tyrConfig.cmake"
            "${CMAKE_BINARY_DIR}/tyrConfigVersion.cmake"
        DESTINATION "${PYTYR_NATIVE_CMAKEDIR}/tyr"
        COMPONENT pytyr)
if(TYR_BUILD_SHARED)
    install(EXPORT pytyrTyrcoreTargets
            NAMESPACE tyr::
            FILE "tyrcoreTargets.cmake"
            DESTINATION "${PYTYR_NATIVE_CMAKEDIR}/tyr"
            COMPONENT pytyr)
endif()
install(DIRECTORY "${CMAKE_SOURCE_DIR}/cmake/"
        DESTINATION "${PYTYR_NATIVE_CMAKEDIR}/tyr/cmake"
        COMPONENT pytyr)

set(PYTYR_DEPENDENCY_HEADER_DIRS
    boost
    cista
    fmt
    gtl
    loki
    oneapi
    tbb
)

set(PYTYR_DEPENDENCY_CMAKE_PACKAGES
    Boost-*
    boost_headers-*
    fmt
    gtl
    loki
)

foreach(prefix IN LISTS CMAKE_PREFIX_PATH)
    if(EXISTS "${prefix}/include")
        foreach(header_dir IN LISTS PYTYR_DEPENDENCY_HEADER_DIRS)
            if(EXISTS "${prefix}/include/${header_dir}")
                install(DIRECTORY "${prefix}/include/${header_dir}"
                        DESTINATION "${PYTYR_NATIVE_INCLUDEDIR}"
                        COMPONENT pytyr)
            endif()
        endforeach()
    endif()
    foreach(dependency_libdir IN ITEMS "${prefix}/${CMAKE_INSTALL_LIBDIR}" "${prefix}/lib" "${prefix}/lib64")
        if(TYR_BUILD_SHARED AND EXISTS "${dependency_libdir}")
            file(GLOB PYTYR_DEPENDENCY_RUNTIME_LIBRARIES
                "${dependency_libdir}/libfmt${CMAKE_SHARED_LIBRARY_SUFFIX}*"
                "${dependency_libdir}/libfmt.*${CMAKE_SHARED_LIBRARY_SUFFIX}*"
                "${dependency_libdir}/libloki_parsers${CMAKE_SHARED_LIBRARY_SUFFIX}*"
                "${dependency_libdir}/libloki_parsers.*${CMAKE_SHARED_LIBRARY_SUFFIX}*"
                "${dependency_libdir}/libtbb${CMAKE_SHARED_LIBRARY_SUFFIX}*"
                "${dependency_libdir}/libtbb.*${CMAKE_SHARED_LIBRARY_SUFFIX}*"
            )
            if(PYTYR_DEPENDENCY_RUNTIME_LIBRARIES)
                install(FILES ${PYTYR_DEPENDENCY_RUNTIME_LIBRARIES}
                        DESTINATION "${PYTYR_NATIVE_LIBDIR}"
                        COMPONENT pytyr)
            endif()
        endif()
        if(EXISTS "${dependency_libdir}/cmake")
            foreach(cmake_package IN LISTS PYTYR_DEPENDENCY_CMAKE_PACKAGES)
                file(GLOB PYTYR_DEPENDENCY_CMAKE_DIRS LIST_DIRECTORIES true "${dependency_libdir}/cmake/${cmake_package}")
                foreach(cmake_dir IN LISTS PYTYR_DEPENDENCY_CMAKE_DIRS)
                    if(IS_DIRECTORY "${cmake_dir}")
                        install(DIRECTORY "${cmake_dir}"
                                DESTINATION "${PYTYR_NATIVE_CMAKEDIR}"
                                COMPONENT pytyr)
                    endif()
                endforeach()
            endforeach()
        endif()
    endforeach()
    if(EXISTS "${prefix}/nanobind")
        install(DIRECTORY "${prefix}/nanobind"
                DESTINATION "${PYTYR_NATIVE_DIR}"
                COMPONENT pytyr)
    endif()
endforeach()

set(PYTYR_PYTHON_FILES
    pytyr/__init__.py
    pytyr/common.py
    pytyr/formalism/__init__.py
    pytyr/formalism/planning.py
    pytyr/planning/__init__.py
    pytyr/planning/ground/__init__.py
    pytyr/planning/ground/astar_eager.py
    pytyr/planning/ground/gbfs_lazy.py
    pytyr/planning/lifted/__init__.py
    pytyr/planning/lifted/astar_eager.py
    pytyr/planning/lifted/gbfs_lazy.py
)

foreach(python_file IN LISTS PYTYR_PYTHON_FILES)
    get_filename_component(python_file_directory "${python_file}" DIRECTORY)
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${python_file}"
            DESTINATION "${python_file_directory}"
            COMPONENT pytyr)
endforeach()

nanobind_add_stub(
    pytyr_stubs
    MODULE pytyr
    RECURSIVE
    INSTALL_TIME
    OUTPUT_PATH "."
    PYTHON_PATH "."
    COMPONENT pytyr
)

add_custom_target(pytyr_wheel DEPENDS pytyr)

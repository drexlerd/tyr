find_package(Python 3.8 REQUIRED
  COMPONENTS Interpreter Development.Module
  OPTIONAL_COMPONENTS Development.SABIModule)

include(CMakePackageConfigHelpers)

find_package(nanobind CONFIG REQUIRED PATHS ${CMAKE_PREFIX_PATH})
message(STATUS "Found nanobind: ${nanobind_DIR} (found version ${nanobind_VERSION})")

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
        set(PYTYR_LIBRARY_ORIGIN "@loader_path/../lib")
    elseif(UNIX)
        set(PYTYR_LIBRARY_ORIGIN "$ORIGIN/../lib")
    endif()

    if(PYTYR_LIBRARY_ORIGIN)
        set_target_properties(pytyr PROPERTIES
            BUILD_RPATH "${PYTYR_LIBRARY_ORIGIN}"
            INSTALL_RPATH "${PYTYR_LIBRARY_ORIGIN}")
    endif()
endif()

install(TARGETS pytyr DESTINATION "pytyr" COMPONENT pytyr)
if(TYR_BUILD_SHARED)
    install(TARGETS core
            EXPORT pytyrTyrcoreTargets
            LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT pytyr
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT pytyr
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT pytyr
            INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
endif()
install(DIRECTORY "${CMAKE_SOURCE_DIR}/include/tyr"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        COMPONENT pytyr)
configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/tyrConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/tyrConfig.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/tyr"
        NO_CHECK_REQUIRED_COMPONENTS_MACRO)
install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/tyrConfig.cmake"
            "${CMAKE_BINARY_DIR}/tyrConfigVersion.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/tyr"
        COMPONENT pytyr)
if(TYR_BUILD_SHARED)
    install(EXPORT pytyrTyrcoreTargets
            NAMESPACE tyr::
            FILE "tyrcoreTargets.cmake"
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/tyr"
            COMPONENT pytyr)
endif()
install(DIRECTORY "${CMAKE_SOURCE_DIR}/cmake/"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/tyr/cmake"
        COMPONENT pytyr)
foreach(prefix IN LISTS CMAKE_PREFIX_PATH)
    if(EXISTS "${prefix}/include")
        install(DIRECTORY "${prefix}/include/"
                DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
                COMPONENT pytyr)
    endif()
    if(TYR_BUILD_SHARED AND EXISTS "${prefix}/${CMAKE_INSTALL_LIBDIR}")
        install(DIRECTORY "${prefix}/${CMAKE_INSTALL_LIBDIR}/"
                DESTINATION "${CMAKE_INSTALL_LIBDIR}"
                COMPONENT pytyr
                FILES_MATCHING
                    PATTERN "*.so"
                    PATTERN "*.so.*"
                    PATTERN "*.dylib"
                    PATTERN "*.dylib.*"
                    PATTERN "*.dll"
                    PATTERN "*.a"
                    PATTERN "*.lib")
    endif()
    if(EXISTS "${prefix}/${CMAKE_INSTALL_LIBDIR}/cmake")
        install(DIRECTORY "${prefix}/${CMAKE_INSTALL_LIBDIR}/cmake/"
                DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake"
                COMPONENT pytyr)
    endif()
    if(EXISTS "${prefix}/nanobind")
        install(DIRECTORY "${prefix}/nanobind"
                DESTINATION "."
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

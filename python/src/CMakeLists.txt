find_package(Python 3.8 REQUIRED
  COMPONENTS Interpreter Development.Module
  OPTIONAL_COMPONENTS Development.SABIModule)

include(CMakePackageConfigHelpers)

find_package(nanobind CONFIG REQUIRED PATHS ${CMAKE_PREFIX_PATH})
message(STATUS "Found nanobind: ${nanobind_DIR} (found version ${nanobind_VERSION})")

set(PYTYR_NATIVE_DIR "pytyr/native")
set(PYTYR_NATIVE_INCLUDEDIR "${PYTYR_NATIVE_DIR}/${CMAKE_INSTALL_INCLUDEDIR}")
set(PYTYR_NATIVE_LIBDIR "${PYTYR_NATIVE_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(PYTYR_NATIVE_BINDIR "${PYTYR_NATIVE_DIR}/${CMAKE_INSTALL_BINDIR}")
set(PYTYR_NATIVE_CMAKEDIR "${PYTYR_NATIVE_LIBDIR}/cmake")

nanobind_add_module(_pytyr
    module.cpp
    pytyr/module.cpp
    pytyr/common/module.cpp
    pytyr/formalism/module.cpp
    pytyr/formalism/planning.cpp
    pytyr/formalism/planning/datas.cpp
    pytyr/formalism/planning/domains.cpp
    pytyr/formalism/planning/indices.cpp
    pytyr/formalism/planning/invariants.cpp
    pytyr/formalism/planning/mutable.cpp
    pytyr/formalism/planning/views.cpp
    pytyr/planning/module.cpp
    pytyr/planning/ground/module.cpp
    pytyr/planning/lifted/module.cpp
    pytyr/planning/ground/astar_eager/module.cpp
    pytyr/planning/ground/brfs/module.cpp
    pytyr/planning/ground/gbfs_lazy/module.cpp
    pytyr/planning/ground/iw/module.cpp
    pytyr/planning/ground/siw/module.cpp
    pytyr/planning/lifted/astar_eager/module.cpp
    pytyr/planning/lifted/brfs/module.cpp
    pytyr/planning/lifted/gbfs_lazy/module.cpp
    pytyr/planning/lifted/iw/module.cpp
    pytyr/planning/lifted/siw/module.cpp
)

target_link_libraries(_pytyr PRIVATE tyr::core TBB::tbb)

if(TYR_BUILD_SHARED)
    if(APPLE)
        set(PYTYR_MODULE_RPATH
            "@loader_path/native/${CMAKE_INSTALL_LIBDIR}"
            "@loader_path/../pypddl/native/${CMAKE_INSTALL_LIBDIR}"
            "@loader_path/../pyyggdrasil/${CMAKE_INSTALL_LIBDIR}")
        set(PYTYR_CORE_RPATH
            "@loader_path"
            "@loader_path/../../../pypddl/native/${CMAKE_INSTALL_LIBDIR}"
            "@loader_path/../../../pyyggdrasil/${CMAKE_INSTALL_LIBDIR}")
    elseif(UNIX)
        set(PYTYR_MODULE_RPATH
            "$ORIGIN/native/${CMAKE_INSTALL_LIBDIR}"
            "$ORIGIN/../pypddl/native/${CMAKE_INSTALL_LIBDIR}"
            "$ORIGIN/../pyyggdrasil/${CMAKE_INSTALL_LIBDIR}")
        set(PYTYR_CORE_RPATH
            "$ORIGIN"
            "$ORIGIN/../../../pypddl/native/${CMAKE_INSTALL_LIBDIR}"
            "$ORIGIN/../../../pyyggdrasil/${CMAKE_INSTALL_LIBDIR}")
    endif()

    if(PYTYR_MODULE_RPATH)
        set_target_properties(_pytyr PROPERTIES
            BUILD_RPATH "${PYTYR_MODULE_RPATH}"
            INSTALL_RPATH "${PYTYR_MODULE_RPATH}")
    endif()
endif()

install(TARGETS _pytyr DESTINATION "pytyr" COMPONENT pytyr)
if(TYR_BUILD_SHARED)
    set_property(TARGET core PROPERTY INTERFACE_INCLUDE_DIRECTORIES
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>")
    set_property(TARGET core PROPERTY INSTALL_RPATH "${PYTYR_CORE_RPATH}")
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
configure_package_config_file("${CMAKE_SOURCE_DIR}/Config.cmake.in"
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

if(APPLE)
    install(SCRIPT "${CMAKE_SOURCE_DIR}/cmake/install/fix_macos_runtime_paths.cmake"
            COMPONENT pytyr)
elseif(UNIX)
    install(SCRIPT "${CMAKE_SOURCE_DIR}/cmake/install/fix_unix_runtime_paths.cmake"
            COMPONENT pytyr)
endif()

set(PYTYR_PYTHON_FILES
    pytyr/__init__.py
    pytyr/__init__.pyi
    pytyr/common/__init__.py
    pytyr/formalism/__init__.py
    pytyr/formalism/planning/__init__.py
    pytyr/planning/__init__.py
    pytyr/planning/ground/__init__.py
    pytyr/planning/ground/astar_eager/__init__.py
    pytyr/planning/ground/brfs/__init__.py
    pytyr/planning/ground/gbfs_lazy/__init__.py
    pytyr/planning/ground/iw/__init__.py
    pytyr/planning/ground/siw/__init__.py
    pytyr/planning/lifted/__init__.py
    pytyr/planning/lifted/astar_eager/__init__.py
    pytyr/planning/lifted/brfs/__init__.py
    pytyr/planning/lifted/gbfs_lazy/__init__.py
    pytyr/planning/lifted/iw/__init__.py
    pytyr/planning/lifted/siw/__init__.py
)

foreach(python_file IN LISTS PYTYR_PYTHON_FILES)
    get_filename_component(python_file_directory "${python_file}" DIRECTORY)
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${python_file}"
            DESTINATION "${python_file_directory}"
            COMPONENT pytyr)
endforeach()

set(PYTYR_INSTALL_LIBRARY_PATHS)
foreach(native_prefix IN ITEMS "${PYPDDL_NATIVE_PREFIX}" "${YGGDRASIL_NATIVE_PREFIX}")
    if(native_prefix)
        file(GLOB native_lib_dirs LIST_DIRECTORIES true "${native_prefix}/lib*")
        foreach(native_lib_dir IN LISTS native_lib_dirs)
            if(IS_DIRECTORY "${native_lib_dir}")
                list(APPEND PYTYR_INSTALL_LIBRARY_PATHS "${native_lib_dir}")
            endif()
        endforeach()
    endif()
endforeach()
list(JOIN PYTYR_INSTALL_LIBRARY_PATHS ":" PYTYR_INSTALL_LIBRARY_PATH)

if(PYTYR_INSTALL_LIBRARY_PATH)
    install(CODE
        "set(ENV{LD_LIBRARY_PATH} \"${PYTYR_INSTALL_LIBRARY_PATH}:$ENV{LD_LIBRARY_PATH}\")
         set(ENV{DYLD_LIBRARY_PATH} \"${PYTYR_INSTALL_LIBRARY_PATH}:$ENV{DYLD_LIBRARY_PATH}\")"
        COMPONENT pytyr)
endif()

nanobind_add_stub(
    pytyr_stubs
    MODULE pytyr._pytyr
    RECURSIVE
    INSTALL_TIME
    OUTPUT_PATH "pytyr"
    PYTHON_PATH "."
    COMPONENT pytyr
)

install(SCRIPT "${CMAKE_SOURCE_DIR}/cmake/patch_python_stubs.cmake" COMPONENT pytyr)

add_custom_target(pytyr_wheel DEPENDS _pytyr)

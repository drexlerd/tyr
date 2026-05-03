@PACKAGE_INIT@

include(CMakeFindDependencyMacro)

find_dependency(loki COMPONENTS parsers REQUIRED PATHS ${CMAKE_PREFIX_PATH} NO_DEFAULT_PATH)

set(_tyr_supported_components core)

foreach(_comp ${tyr_FIND_COMPONENTS})
    if(NOT _comp IN_LIST _tyr_supported_components)
        set(tyr_FOUND FALSE)
        set(tyr_NOT_FOUND_MESSAGE "Unsupported component: ${_comp}")
        return()
    endif()
endforeach()

include("${CMAKE_CURRENT_LIST_DIR}/tyrcoreTargets.cmake")

get_filename_component(tyr_CONFIG_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

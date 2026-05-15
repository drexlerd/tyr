set(native_lib_dir "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/pytyr/native/lib")
if(NOT EXISTS "${native_lib_dir}")
    return()
endif()

find_program(INSTALL_NAME_TOOL_EXECUTABLE install_name_tool)
if(NOT INSTALL_NAME_TOOL_EXECUTABLE)
    message(WARNING "install_name_tool not found; pytyr native libraries keep their original runtime paths")
    return()
endif()
if(NOT DEFINED TYR_PYTHON_NATIVE_LIBRARY_RPATHS)
    set(TYR_PYTHON_NATIVE_LIBRARY_RPATHS "@loader_path")
endif()

file(GLOB_RECURSE native_libraries LIST_DIRECTORIES false
    "${native_lib_dir}/*.dylib"
    "${native_lib_dir}/*.dylib.*")

foreach(native_library IN LISTS native_libraries)
    get_filename_component(native_library_name "${native_library}" NAME)

    execute_process(
        COMMAND "${INSTALL_NAME_TOOL_EXECUTABLE}" -id "@rpath/${native_library_name}" "${native_library}"
        RESULT_VARIABLE install_name_result
        ERROR_VARIABLE install_name_error)
    if(NOT install_name_result EQUAL 0)
        message(WARNING "Could not update install name of ${native_library}: ${install_name_error}")
    endif()

    foreach(dependency_rpath IN LISTS TYR_PYTHON_NATIVE_LIBRARY_RPATHS)
        execute_process(
            COMMAND "${INSTALL_NAME_TOOL_EXECUTABLE}" -delete_rpath "${dependency_rpath}" "${native_library}"
            OUTPUT_QUIET
            ERROR_QUIET)
        execute_process(
            COMMAND "${INSTALL_NAME_TOOL_EXECUTABLE}" -add_rpath "${dependency_rpath}" "${native_library}"
            RESULT_VARIABLE dependency_rpath_result
            ERROR_VARIABLE dependency_rpath_error)
        if(NOT dependency_rpath_result EQUAL 0)
            message(WARNING "Could not add ${dependency_rpath} rpath to ${native_library}: ${dependency_rpath_error}")
        endif()
    endforeach()
endforeach()

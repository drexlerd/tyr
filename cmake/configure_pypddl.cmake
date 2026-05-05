function(configure_pypddl)
  if(TYR_DISABLE_PYPDDL_DISCOVERY)
    return()
  endif()

  find_package(Python3 QUIET COMPONENTS Interpreter)
  if(NOT Python3_Interpreter_FOUND)
    return()
  endif()

  execute_process(
    COMMAND "${Python3_EXECUTABLE}" -c "import pypddl; print(pypddl.native_prefix())"
    RESULT_VARIABLE pypddl_result
    OUTPUT_VARIABLE pypddl_prefix
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  if(pypddl_result EQUAL 0 AND EXISTS "${pypddl_prefix}")
    list(PREPEND CMAKE_PREFIX_PATH "${pypddl_prefix}")
    set(PYPDDL_NATIVE_PREFIX "${pypddl_prefix}" PARENT_SCOPE)
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" PARENT_SCOPE)
    message(STATUS "Found pypddl native prefix: ${pypddl_prefix}")
  endif()
endfunction()

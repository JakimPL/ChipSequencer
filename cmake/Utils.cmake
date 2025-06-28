function(add_chipsequencer_library target_name)
    cmake_parse_arguments(LIB "" "" "SOURCES;INCLUDE_DIRS;LINK_LIBS" ${ARGN})

    add_library(${target_name} STATIC ${LIB_SOURCES})

    if(LIB_INCLUDE_DIRS)
        target_include_directories(${target_name} PUBLIC ${LIB_INCLUDE_DIRS})
    endif()

    if(LIB_LINK_LIBS)
        target_link_libraries(${target_name} PUBLIC ${LIB_LINK_LIBS})
    endif()

    target_compile_options(${target_name} PRIVATE
        $<$<COMPILE_LANGUAGE:C>:-Werror=switch-enum>
        $<$<COMPILE_LANGUAGE:CXX>:-Werror=switch-enum>
    )
endfunction()

function(find_dependency dep_name)
    cmake_parse_arguments(DEP "REQUIRED" "ERROR_MESSAGE" "NAMES;PATHS" ${ARGN})

    if(DEP_REQUIRED)
        find_package(${dep_name} REQUIRED)
        if(NOT ${dep_name}_FOUND)
            if(DEP_ERROR_MESSAGE)
                message(FATAL_ERROR ${DEP_ERROR_MESSAGE})
            else()
                message(FATAL_ERROR "${dep_name} not found")
            endif()
        endif()
    else()
        find_package(${dep_name})
    endif()
endfunction()

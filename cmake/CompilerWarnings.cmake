==============================================================================
Compiler warning configuration
==============================================================================

function(enable_project_warnings target)

if(NOT ENABLE_WARNINGS)
    return()
endif()

if(MSVC)

    target_compile_options(${target}
        PRIVATE
            /W4
            /permissive-
            /w14242
            /w14254
            /w14263
            /w14265
            /w14287
            /we4289
            /w14296
            /w14311
            /w14545
            /w14546
            /w14547
            /w14549
            /w14555
            /w14619
            /w14640
            /w14826
            /w14905
            /w14906
            /w14928
    )

elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

    target_compile_options(${target}
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -Wsign-conversion
            -Wshadow
            -Wformat=2
            -Wnull-dereference
            -Wdouble-promotion
            -Wimplicit-fallthrough
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(${target}
            PRIVATE
                -Wthread-safety
        )
    endif()

endif()


endfunction()
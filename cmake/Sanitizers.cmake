==============================================================================
Sanitizer configuration
==============================================================================

function(enable_project_sanitizers target)

if(NOT ENABLE_SANITIZERS)
    return()
endif()

if(MSVC)

    message(WARNING
        "Sanitizers are not configured for MSVC in this project."
    )

    return()

endif()

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

    message(WARNING
        "Sanitizers are supported only for GCC/Clang."
    )

    return()

endif()

target_compile_options(${target}
    PRIVATE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
)

target_link_options(${target}
    PRIVATE
        -fsanitize=address,undefined
)


endfunction()
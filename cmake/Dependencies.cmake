==============================================================================
C++ HTTPS Server - Dependencies
==============================================================================

include_guard(GLOBAL)

------------------------------------------------------------------------------
Threads
------------------------------------------------------------------------------

find_package(Threads REQUIRED)

------------------------------------------------------------------------------
OpenSSL
------------------------------------------------------------------------------

find_package(OpenSSL 3.0 REQUIRED COMPONENTS SSL Crypto)

if(NOT OpenSSL_FOUND)
message(FATAL_ERROR
"OpenSSL was not found. "
"Install OpenSSL development packages and try again."
)
endif()

message(STATUS "OpenSSL version: ${OPENSSL_VERSION}")

------------------------------------------------------------------------------
Helper function
------------------------------------------------------------------------------

function(link_project_dependencies target)

target_link_libraries(${target}
    PRIVATE
        Threads::Threads
        OpenSSL::SSL
        OpenSSL::Crypto
)


endfunction()
cmake_minimum_required(VERSION 3.20)

==============================================================================
C++ HTTPS Server - Test Suite
==============================================================================

enable_testing()

------------------------------------------------------------------------------
Test directories
------------------------------------------------------------------------------

add_subdirectory(http)
add_subdirectory(server)
add_subdirectory(tls)

------------------------------------------------------------------------------
Optional test output configuration
------------------------------------------------------------------------------

set_property(
DIRECTORY
PROPERTY CMAKE_CTEST_ARGUMENTS "--output-on-failure"
)
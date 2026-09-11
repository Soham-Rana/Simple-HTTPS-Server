cmake_minimum_required(VERSION 3.20)

project(
cpp_https_server
VERSION 1.0.0
DESCRIPTION "A modern HTTPS server implemented in C++"
LANGUAGES CXX
)

------------------------------------------------------------------------------
Project configuration
------------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

Generate compile_commands.json for IDEs and static analysis tools.

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

------------------------------------------------------------------------------
Build options
------------------------------------------------------------------------------

option(BUILD_TESTS "Build unit and integration tests" ON)
option(BUILD_BENCHMARKS "Build performance benchmarks" OFF)
option(ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
option(ENABLE_WARNINGS "Enable strict compiler warnings" ON)

------------------------------------------------------------------------------
Dependencies
------------------------------------------------------------------------------

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

------------------------------------------------------------------------------
Source files
------------------------------------------------------------------------------

set(SERVER_SOURCES
src/main.cpp

src/server/server.cpp
src/server/server_config.cpp
src/server/connection.cpp

src/net/socket.cpp
src/net/tcp_listener.cpp
src/net/address.cpp

src/tls/tls_context.cpp
src/tls/tls_connection.cpp

src/http/parser.cpp
src/http/request.cpp
src/http/response.cpp
src/http/router.cpp

src/thread/thread_pool.cpp

src/utils/logger.cpp
src/utils/config.cpp


)

------------------------------------------------------------------------------
Main executable
------------------------------------------------------------------------------

add_executable(${PROJECT_NAME}
${SERVER_SOURCES}
)

target_include_directories(${PROJECT_NAME}
PRIVATE
${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
PRIVATE
OpenSSL::SSL
OpenSSL::Crypto
Threads::Threads
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
endif()

------------------------------------------------------------------------------
Compiler warnings
------------------------------------------------------------------------------

if(ENABLE_WARNINGS)
if(MSVC)
target_compile_options(${PROJECT_NAME} PRIVATE
/W4
/permissive-
)
else()
target_compile_options(${PROJECT_NAME} PRIVATE
-Wall
-Wextra
-Wpedantic
-Wconversion
-Wsign-conversion
-Wshadow
)
endif()
endif()

------------------------------------------------------------------------------
Sanitizers
------------------------------------------------------------------------------

if(ENABLE_SANITIZERS AND NOT MSVC)
target_compile_options(${PROJECT_NAME} PRIVATE
-fsanitize=address,undefined
-fno-omit-frame-pointer
)

target_link_options(${PROJECT_NAME} PRIVATE
    -fsanitize=address,undefined
)


endif()

------------------------------------------------------------------------------
Tests
------------------------------------------------------------------------------

if(BUILD_TESTS)
enable_testing()
add_subdirectory(tests)
endif()

------------------------------------------------------------------------------
Benchmarks
------------------------------------------------------------------------------

if(BUILD_BENCHMARKS)
add_subdirectory(benchmarks)
endif()

------------------------------------------------------------------------------
Installation
------------------------------------------------------------------------------

include(GNUInstallDirs)

install(
TARGETS ${PROJECT_NAME}
RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
DIRECTORY include/
DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
DIRECTORY config/
DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME}
)

------------------------------------------------------------------------------
Build information
------------------------------------------------------------------------------

message(STATUS "")
message(STATUS "========== ${PROJECT_NAME} ==========")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "Build Tests: ${BUILD_TESTS}")
message(STATUS "Build Benchmarks: ${BUILD_BENCHMARKS}")
message(STATUS "Sanitizers: ${ENABLE_SANITIZERS}")
message(STATUS "======================================")
message(STATUS "")
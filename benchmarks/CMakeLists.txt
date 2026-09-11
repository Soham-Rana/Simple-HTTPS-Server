cmake_minimum_required(VERSION 3.20)

------------------------------------------------------------------------------
HTTP benchmark executable
------------------------------------------------------------------------------

add_executable(http_benchmark
http_benchmark.cpp
)

target_include_directories(http_benchmark
PRIVATE
${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(http_benchmark
PRIVATE
OpenSSL::SSL
OpenSSL::Crypto
Threads::Threads
)

target_compile_features(http_benchmark
PRIVATE
cxx_std_20
)

if(ENABLE_WARNINGS)
if(MSVC)
target_compile_options(http_benchmark PRIVATE
/W4
/permissive-
)
else()
target_compile_options(http_benchmark PRIVATE
-Wall
-Wextra
-Wpedantic
-Wconversion
-Wsign-conversion
-Wshadow
)
endif()
endif()

if(ENABLE_SANITIZERS AND NOT MSVC)
target_compile_options(http_benchmark PRIVATE
-fsanitize=address,undefined
-fno-omit-frame-pointer
)

target_link_options(http_benchmark PRIVATE
    -fsanitize=address,undefined
)


endif()
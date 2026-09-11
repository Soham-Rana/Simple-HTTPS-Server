add_executable(tls_tests
tls_context_tests.cpp
tls_connection_tests.cpp
)

target_include_directories(tls_tests
PRIVATE
${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(tls_tests
PRIVATE
tls
net
OpenSSL::SSL
OpenSSL::Crypto
)

target_compile_features(tls_tests
PRIVATE
cxx_std_20
)

if(ENABLE_WARNINGS)
if(MSVC)
target_compile_options(tls_tests PRIVATE
/W4
/permissive-
)
else()
target_compile_options(tls_tests PRIVATE
-Wall
-Wextra
-Wpedantic
-Wconversion
-Wsign-conversion
-Wshadow
)
endif()
endif()

add_test(
NAME tls_tests
COMMAND tls_tests
)
add_executable(server_tests
server_config_tests.cpp
server_tests.cpp
)

target_include_directories(server_tests
PRIVATE
${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(server_tests
PRIVATE
server
http
net
tls
thread
utils
)

target_compile_features(server_tests
PRIVATE
cxx_std_20
)

if(ENABLE_WARNINGS)
if(MSVC)
target_compile_options(server_tests PRIVATE
/W4
/permissive-
)
else()
target_compile_options(server_tests PRIVATE
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
NAME server_tests
COMMAND server_tests
)
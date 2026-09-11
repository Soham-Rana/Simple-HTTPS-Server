add_executable(http_tests
parser_tests.cpp
request_tests.cpp
response_tests.cpp
router_tests.cpp
)

target_include_directories(http_tests
PRIVATE
${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(http_tests
PRIVATE
http
)

target_compile_features(http_tests
PRIVATE
cxx_std_20
)

if(ENABLE_WARNINGS)
if(MSVC)
target_compile_options(http_tests PRIVATE
/W4
/permissive-
)
else()
target_compile_options(http_tests PRIVATE
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
NAME http_tests
COMMAND http_tests
)
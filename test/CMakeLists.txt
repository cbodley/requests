add_subdirectory(dependency)

add_library(sanitizers INTERFACE)
target_compile_options(sanitizers INTERFACE "-fsanitize=address")
target_link_libraries(sanitizers INTERFACE "-fsanitize=address")

add_executable(test_uri_view test_uri_view.cc)
target_link_libraries(test_uri_view requests gtest_main)
add_test(test_uri_view test_uri_view)

add_executable(test_uri_string test_uri_string.cc)
target_link_libraries(test_uri_string sanitizers requests gtest_main)
add_test(test_uri_string test_uri_string)

add_executable(test_request test_request.cc)
target_link_libraries(test_request sanitizers requests gtest_main)
add_test(test_request test_request)

add_executable(test_http_error_std test_http_error_std.cc)
target_link_libraries(test_http_error_std sanitizers requests gtest_main)
add_test(test_http_error_std test_http_error_std)

add_executable(test_http_error_boost test_http_error_boost.cc)
target_link_libraries(test_http_error_boost sanitizers requests gtest_main)
add_test(test_http_error_boost test_http_error_boost)

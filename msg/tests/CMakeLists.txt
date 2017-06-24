add_executable( packages_test packages_gtest.cxx )
target_link_libraries( packages_test ${GTEST_BOTH_LIBRARIES} msg)
add_test("packages" packages_test)
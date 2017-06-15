add_executable( MsgPack_test MsgPack_gtest.cxx )
target_link_libraries( MsgPack_test ${GTEST_BOTH_LIBRARIES} MsgPack )
add_test("MsgPack" MsgPack_test)
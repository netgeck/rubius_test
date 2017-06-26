add_executable( packages_test packages_gtest.cxx )

target_link_libraries( packages_test 
	${GTEST_BOTH_LIBRARIES} 
	msg 
	pthread # костыль для gtest'а собранного без тредов
)

add_test("packages" packages_test)

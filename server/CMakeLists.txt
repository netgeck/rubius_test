set(target_name server)

add_subdirectory( tcpConnect )

add_executable( ${target_name}
	main.cxx
)

target_link_libraries( ${target_name} 
PRIVATE
	boost_system
	pthread
PUBLIC 
	tcpConnect
)

install(TARGETS ${target_name} 
	DESTINATION ${INSTALL_DIR}
)

target_include_directories( ${target_name} 
PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR}/include 
	tcpConnect
)
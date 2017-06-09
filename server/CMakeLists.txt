set(target_name server)

add_executable( ${target_name}
	main.cxx
)

target_link_libraries( ${target_name}
	boost_system
	pthread
)

install(TARGETS ${target_name} 
	DESTINATION ${INSTALL_DIR}
)
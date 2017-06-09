set(target_name client)

add_executable( ${target_name}
	main.cxx
)

install(TARGETS ${target_name} 
	DESTINATION ${INSTALL_DIR}
)
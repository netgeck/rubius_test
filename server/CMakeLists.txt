set(target_name server)

add_executable( ${target_name}
	main.cxx
)

target_link_libraries( ${target_name} 
	boost_system
	packages
)

target_compile_definitions( ${target_name} PRIVATE PROGNAME="${target_name}" )

install(TARGETS ${target_name} DESTINATION ${INSTALL_DIR})

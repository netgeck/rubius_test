set(target_name server)

include(${QT_USE_FILE})

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable( ${target_name}
	main.cxx
	tcpServer.cxx
	clientSession.cxx
)

target_link_libraries( ${target_name} 
	${QT_LIBRARIES}
	msg
)

target_compile_definitions( ${target_name} PRIVATE PROGNAME="${target_name}" )

install(TARGETS ${target_name} DESTINATION ${INSTALL_BINARY_SUBDIR} )

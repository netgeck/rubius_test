set(target_name client)

project(${target_name})

include(${QT_USE_FILE})

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt4_wrap_ui(target_ui
	MainWindow.ui
)

add_executable(${target_name} 
	main.cxx 
	MainWindow.cxx
	${target_ui}
)

target_link_libraries(${target_name} 
	${QT_LIBRARIES} 
)

target_include_directories(${target_name} 
PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}
)

install(TARGETS ${target_name} 
	DESTINATION ${INSTALL_BINARY_SUBDIR}
)

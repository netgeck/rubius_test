set(target_name mainWindow)

add_library( ${target_name} 
	mainwindow.cxx
)

target_link_libraries( ${target_name} pthread boost_system)

target_include_directories( ${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )
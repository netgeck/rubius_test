set(target_name client)

project(${target_name})

find_package(Qt4 REQUIRED QtCore QtGui QtNetwork)
include(${QT_USE_FILE})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(${target_name} main.cxx MainWindow.cxx)

target_link_libraries(${target_name} ${QT_LIBRARIES} MessagePack)

target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS ${target_name} 
	DESTINATION ${INSTALL_DIR}
)

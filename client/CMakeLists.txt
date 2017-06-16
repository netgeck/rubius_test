set(target_name client)

cmake_minimum_required(VERSION  3.1.0)
project(${target_name})

find_package(Qt4 REQUIRED QtCore QtGui QtNetwork)
include(${QT_USE_FILE})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory( mainWindow )

add_executable( ${target_name} main.cxx )

target_link_libraries(${target_name} ${QT_LIBRARIES} mainWindow)

install(TARGETS ${target_name} 
	DESTINATION ${INSTALL_DIR}
)

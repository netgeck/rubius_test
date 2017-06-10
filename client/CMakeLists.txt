set(target_name client)

cmake_minimum_required(VERSION  3.1.0)
project(${target_name})

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

set(${target_name}_SOURCES main.cxx mainwindow.cxx)
set(${target_name}_FORMS mainwindow.ui)
set(${target_name}_HEADERS mainwindow.h)

QT4_WRAP_CPP(${target_name}_HEADERS_MOC ${${target_name}_HEADERS})
QT4_WRAP_UI(${target_name}_FORMS_HEADERS ${${target_name}_FORMS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
add_executable( ${target_name}
	${${target_name}_SOURCES}
	${${target_name}_HEADERS_MOC}
	${${target_name}_FORMS_HEADERS}
)
target_link_libraries(${target_name} ${QT_LIBRARIES})

install(TARGETS ${target_name} 
	DESTINATION ${INSTALL_DIR}
)

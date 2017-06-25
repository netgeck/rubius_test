cmake_minimum_required(VERSION  3.1.0)
project( rubius_test )

set (CMAKE_CXX_STANDARD 14)

# Qt
find_package(Qt4 REQUIRED QtCore QtGui QtNetwork)

# Unit-тесты
enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
set( GTEST_RESULT_ROOT ${CMAKE_CURRENT_BINARY_DIR}/Testing )

set (INSTALL_DIR ${CMAKE_BINARY_DIR}/install)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )

add_subdirectory( client )
add_subdirectory( server )
add_subdirectory( msg )

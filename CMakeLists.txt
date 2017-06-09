cmake_minimum_required( VERSION 3.2.2 )
project( rubius_test )

set (CMAKE_CXX_STANDARD 14)

set (INSTALL_DIR ${CMAKE_BINARY_DIR}/install)

add_subdirectory( client )
add_subdirectory( server )


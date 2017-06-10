
set(target_name tcpConnect )

add_library( ${target_name} tcpConnect.cxx )

target_include_directories( ${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )
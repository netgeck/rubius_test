
set(target_name "msg")

add_library( ${target_name} INTERFACE )

target_include_directories( ${target_name}
INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}
)

# unit-тесты
if( NOT TEST_DISABLE )
add_subdirectory( tests )
endif()

set(target_name "packages")

add_library( ${target_name}
	streamPkg.cxx
)

target_include_directories( ${target_name}
PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)

# unit-тесты
add_subdirectory( tests )
cmake_minimum_required(VERSION 2.8)
project(MessagePack)

add_definitions( -Wall -Wextra -Weffc++ )

include_directories( include )
include_directories( MsgPack )

add_subdirectory( MsgPack )

# unit-тесты
add_subdirectory( tests )


add_library( MessagePack )


# Задаём список include-директорий для сборки цели "MessagePack".
# Директории с областью видимости PUBLIC будут автоматически цепляться при сборке зависимых целей.
target_include_directories( MessagePack PUBLIC
	include
	MsgPack
)

target_link_libraries( MessagePack 
	MsgPack
)


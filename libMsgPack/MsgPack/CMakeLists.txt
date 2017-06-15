
add_subdirectory( MsgPack_core )


add_library( MsgPack 
	common.cxx
	MsgPack_pack.cxx 
	MsgPack_unpack.cxx 
	internal_common.cxx
)

target_include_directories( MsgPack
	PRIVATE
	MsgPack_internal
	MsgPack_core
)

target_link_libraries( MsgPack 
	PRIVATE MsgPack_core
)


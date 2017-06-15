/* 
 * File:   MsgPack.cxx
 * Author: kostya
 * 
 * Created on 9 декабря 2015 г., 10:34
 */
#include <boost/assign/list_of.hpp>

#include "MsgPack.h"


using namespace MsgPack;

using namespace internal;


const internal::MessagePack::bm_type internal::MessagePack::m_bimap = 
	boost::assign::list_of< internal::MessagePack::bm_type::relation >
	(internal::type::nil,		header(0xc0)) //11000000
	// never used				      //11000001 
	(internal::type::false_,	header(0xc2)) //11000010
	(internal::type::true_,		header(0xc3)) //11000011
	(internal::type::bin_8,		header(0xc4)) //11000100
	(internal::type::bin_16,	header(0xc5)) //11000101
	(internal::type::bin_32,	header(0xc6)) //11000110
	(internal::type::ext_8,		header(0xc7)) //11000111
	(internal::type::ext_16,	header(0xc8)) //11001000
	(internal::type::ext_32,	header(0xc9)) //11001001
	(internal::type::float_32,	header(0xca)) //11001010
	(internal::type::float_64,	header(0xcb)) //11001011
	(internal::type::uint_8,	header(0xcc)) //11001100
	(internal::type::uint_16,	header(0xcd)) //11001101
	(internal::type::uint_32,	header(0xce)) //11001110
	(internal::type::uint_64,	header(0xcf)) //11001111
	(internal::type::int_8,		header(0xd0)) //11010000
	(internal::type::int_16,	header(0xd1)) //11010001
	(internal::type::int_32,	header(0xd2)) //11010010
	(internal::type::int_64,	header(0xd3)) //11010011
	(internal::type::fixext_1,	header(0xd4)) //11010100
	(internal::type::fixext_2,	header(0xd5)) //11010101
	(internal::type::fixext_4,	header(0xd6)) //11010110
	(internal::type::fixext_8,	header(0xd7)) //11010111
	(internal::type::fixext_16,	header(0xd8)) //11011000
	(internal::type::str_8,		header(0xd9)) //11011001
	(internal::type::str_16,	header(0xda)) //11011010
	(internal::type::str_32,	header(0xdb)) //11011011
	(internal::type::array_16,	header(0xdc)) //11011100
	(internal::type::array_32,	header(0xdd)) //11011101
	(internal::type::map_16,	header(0xde)) //11011110
	(internal::type::map_32,	header(0xdf)) //11011111
	;


int internal::MessagePack::getPkgType(const header& head){
	if (head >= internal::mask::negative_fixint) {
		return static_cast<int>(internal::type::negative_fixint);
	} else if (head >= internal::mask::others) {
		auto it = internal::MessagePack::m_bimap.right.find(head);
		if ( it == internal::MessagePack::m_bimap.right.end()) { return -1; }
		return static_cast<int>(it->second);
	} else if (head >= internal::mask::fixstr) {
		return static_cast<int>(internal::type::fixstr);
	} else if (head >= internal::mask::fixarray) {
		return static_cast<int>(internal::type::fixarray);
	} else if (head >= internal::mask::fixmap) {
		return static_cast<int>(internal::type::fixmap);
	} else { // if head >= internal::mask::positive_fixint
		return static_cast<int>(internal::type::positive_fixint);
	}
}


package_data MessagePack::getHead(internal::type type) {
	return m_bimap.left.at(type);
}


MsgPack::type MsgPack::innerToInterfaceType(MsgPack::internal::type type){
	switch(type){
// типы с прямым соответствием:
	case internal::type::nil:	return MsgPack::type::nil;
	case internal::type::float_32:	return MsgPack::type::float_32;
	case internal::type::float_64:	return MsgPack::type::float_64;
	case internal::type::uint_16:	return MsgPack::type::uint_16;
	case internal::type::uint_32:	return MsgPack::type::uint_32;
	case internal::type::uint_64:	return MsgPack::type::uint_64;
	case internal::type::int_16:	return MsgPack::type::int_16;	
	case internal::type::int_32:	return MsgPack::type::int_32;	
	case internal::type::int_64:	return MsgPack::type::int_64;	
// семейства типов:
	case internal::type::false_:
	case internal::type::true_:
		return MsgPack::type::boolean;
	case internal::type::bin_8:
	case internal::type::bin_16:
	case internal::type::bin_32:
		return MsgPack::type::bin;
	case internal::type::positive_fixint:
	case internal::type::uint_8:		
		return MsgPack::type::uint_8;
	case internal::type::negative_fixint:
	case internal::type::int_8:		
		return MsgPack::type::int_8;
	case internal::type::fixext_1:
	case internal::type::fixext_2:
	case internal::type::fixext_4:
	case internal::type::fixext_8:
	case internal::type::fixext_16:
	case internal::type::ext_8:
	case internal::type::ext_16:
	case internal::type::ext_32:
		return MsgPack::type::ext;
	case internal::type::fixstr:
	case internal::type::str_8:
	case internal::type::str_16:
	case internal::type::str_32:
		return MsgPack::type::str;
	case internal::type::fixarray:
	case internal::type::array_16:
	case internal::type::array_32:		
		return MsgPack::type::array;
	case internal::type::fixmap:
	case internal::type::map_16:
	case internal::type::map_32:
		return MsgPack::type::map;
	}
	
	throw std::runtime_error("Неизвестный тип пакета");
}


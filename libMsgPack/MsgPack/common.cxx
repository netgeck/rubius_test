/* 
 * File:   MsgPack.cxx
 * Author: kostya
 * 
 * Created on 9 декабря 2015 г., 10:34
 */

#include "common.h"

#include <iostream>
#include <stdexcept>

#include "MsgPack.h"


using namespace MsgPack;


bool MsgPack::isPgkCorrect(const package& pkg) noexcept {
	return internal::isPkgCorrect(pkg.begin(), pkg.end());
}


type MsgPack::packageType(const package& pkg){
	internal::type type = internal::MessagePack::packageType(pkg.front());
	
	return MsgPack::innerToInterfaceType(type);
}


std::string MsgPack::type_name(MsgPack::type tp){
	switch (tp){
	case type::nil:			return "nil";
	//case type:://(never used)	11000001	0xc1
	case type::boolean:		return "boolean";
	case type::bin:			return "bin";
	case type::float_32:		return "float_32";
	case type::float_64:		return "float_64";
	case type::uint_8:		return "uint_8";
	case type::uint_16:		return "uint_16";
	case type::uint_32:		return "uint_32";
	case type::uint_64:		return "uint_64";
	case type::int_8:		return "int_8";
	case type::int_16:		return "int_16";
	case type::int_32:		return "int_32";
	case type::int_64:		return "int_64";
	case type::ext:			return "ext";
	case type::str:			return "str";
	case type::array:		return "array";
	case type::map:			return "map";
	}
	
	throw std::runtime_error("[" + std::string(__func__) + "] "
		"неизвестное имя типа");
}


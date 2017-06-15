/* 
 * File:   internal_common.cxx
 * Author: kostya
 * 
 * Created on 11 февраля 2016 г., 11:48
 */

#include <stdexcept>

#include "MsgPack.h"
#include "types_handle.h"
#include "common.h"

#include "internal_common.h"


using namespace MsgPack;


std::string internal::type_name(internal::type tp){
	switch (tp){
	case type::positive_fixint:	return "positive_fixint";
	case type::fixmap:		return "fixmap";
	case type::fixarray:		return "fixarray";
	case type::fixstr:		return "fixstr";
	case type::nil:			return "nil";
	case type::false_:		return "false_";
	case type::true_:		return "true_";
	case type::bin_8:		return "bin_8";
	case type::bin_16:		return "bin_16";
	case type::bin_32:		return "bin_32";
	case type::ext_8:		return "ext_8";
	case type::ext_16:		return "ext_16";
	case type::ext_32:		return "ext_32";
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
	case type::fixext_1:		return "fixext_1";
	case type::fixext_2:		return "fixext_2";
	case type::fixext_4:		return "fixext_4";
	case type::fixext_8:		return "fixext_8";
	case type::fixext_16:		return "fixext_16";
	case type::str_8:		return "str_8";
	case type::str_16:		return "str_16";
	case type::str_32:		return "str_32";
	case type::array_16:		return "array_16";
	case type::array_32:		return "array_32";
	case type::map_16:		return "map_16";
	case type::map_32:		return "map_32";
	case type::negative_fixint:	return "negative_fixint";
	}
	
	throw std::runtime_error("Неизвестный внутрениий тип MsgPack");
}


bool isHeaderCorrect(MsgPack::header head){
	if (internal::MessagePack::getPkgType(head) >= 0) {
		return true;
	}
	return false;
}


bool internal::isPkgCorrect(const package::const_iterator begin, 
				const package::const_iterator end) {
	try {
		package::const_iterator::difference_type size = end - begin;
		
		if ((size) == 0) { // если пакет пустой, он точно не корректный
			return false;
		}
	
		// Проверяем корректность заголовка
		if (!isHeaderCorrect(*begin)) {
			return false;
		}
	
		// Проверяем соответствует ли размер пакета размеру указанному в самом пакете
		// в случае сложных типов пакета (array, map), будет анализироваться всё содержимое пакета
		if ((1 + packageBodySize(begin, end)) != 
			static_cast<std::make_unsigned<decltype(size)>::type>(size)) {
			return false;
		}
	} catch (std::exception& e){
		return false;
	}
	
	return true;
}


template <MsgPack::type INTERFACE_TYPE>
inline size_t elementSize(const package::const_iterator begin, const package::const_iterator end);

template <>	// метод подсчета длины для map
inline size_t elementSize<MsgPack::type::map>(const package::const_iterator begin, 
						const package::const_iterator end){
	size_t firstSize = 1 + internal::packageBodySize(begin, end);
	return firstSize + (1 + internal::packageBodySize(begin+firstSize, end));
}

template <>	// метод подсчета длины для array
inline size_t elementSize<MsgPack::type::array>(const package::const_iterator begin, 
						const package::const_iterator end){
	return 1 + internal::packageBodySize(begin, end);
}


template <MsgPack::type INTERFACE_TYPE>
inline size_t sumElementSize(uint32_t elmNum, 
		const package::const_iterator begin, 
		const package::const_iterator end) {
	size_t sum(0);
	for (uint32_t i=0; i < elmNum; ++i){
		if ((begin + sum) >= end){
			throw std::runtime_error("Ошибка подсчета объема элементов контейнера:"
				"элементов меньше чем указано в заголовке");
		}
		sum += elementSize<INTERFACE_TYPE>(begin+sum, end);
	}
	return sum;
}


size_t complexPkgBodySize(internal::type type,
				const package::const_iterator begin, 
				const package::const_iterator end) {
	uint32_t elmNum;
	size_t pkgBodySize(0);
	
	switch(type) {
	case internal::type::fixmap:	
		elmNum = internal::mask::fixmap ^ (*begin);
		break;
	case internal::type::fixarray:	
		elmNum = internal::mask::fixarray ^ (*begin);
		break;
	case internal::type::map_16:
	case internal::type::array_16:
		elmNum = internal::unpackInt_betoh_safe<uint16_t>(begin+1, end);
		pkgBodySize += sizeof(uint16_t);
		break;
	case internal::type::map_32:
	case internal::type::array_32:
		elmNum = internal::unpackInt_betoh_safe<uint32_t>(begin+1, end);
		pkgBodySize += sizeof(uint32_t);
		break;
	default:
		throw std::runtime_error("не верный тип пакета: " + internal::type_name(type));
	}
	
	switch(innerToInterfaceType(type)){
	case MsgPack::type::array:	return pkgBodySize + 
			sumElementSize<MsgPack::type::array>(elmNum, begin + 1 + pkgBodySize, end);
	case MsgPack::type::map:	return pkgBodySize + 
			sumElementSize<MsgPack::type::map>(elmNum, begin + 1 + pkgBodySize, end);
	default:	
		throw std::runtime_error("не верный тип пакета: " +
			MsgPack::type_name(innerToInterfaceType(type)));
	}
}


size_t internal::packageBodySize(const package::const_iterator begin, 
				const package::const_iterator end) {
	if (begin == end) {
		throw std::runtime_error("Ошибка определения размера пакета: пакет нулевой длины");
	}
	internal::type type = internal::MessagePack::packageType(*begin);
	
	switch(type) {
// Типы пакетов размер которых закодирован(!) в заголовке:
	case internal::type::fixstr:		return internal::mask::fixstr ^ (*begin);
// Типы размер которых заранее известен
	case internal::type::uint_8:
	case internal::type::int_8:		return 1;
	case internal::type::uint_16:
	case internal::type::int_16:		return 2;
	case internal::type::uint_32:
	case internal::type::int_32:		
	case internal::type::float_32:		return 4;
	case internal::type::uint_64:	
	case internal::type::int_64:		
	case internal::type::float_64:		return 8;
	case internal::type::fixext_1:		return 2;
	case internal::type::fixext_2:		return 3;
	case internal::type::fixext_4:		return 5;
	case internal::type::fixext_8:		return 9;
	case internal::type::fixext_16:		return 17;
// Типы состоящие из одного заголовка:
	case internal::type::positive_fixint:
	case internal::type::negative_fixint:
	case internal::type::nil:
	case internal::type::false_:
	case internal::type::true_:		return 0;
// Типы размер которых занимает 1 байт после заголовка:
	case internal::type::bin_8:
	case internal::type::str_8:		return sizeof(uint8_t) + internal::unpackInt_betoh_safe<uint8_t>(begin+1, end);
	case internal::type::ext_8:		return 1 + sizeof(uint8_t) + internal::unpackInt_betoh_safe<uint8_t>(begin+1, end);
// Типы размер которых занимает 2 байта после заголовка:
	case internal::type::bin_16:
	case internal::type::str_16:		return sizeof(uint16_t) + internal::unpackInt_betoh_safe<uint16_t>(begin+1, end);
	case internal::type::ext_16:		return 1 + sizeof(uint16_t) + internal::unpackInt_betoh_safe<uint16_t>(begin+1, end);
// Типы размер которых занимает 4 байта после заголовка:
	case internal::type::bin_32:
	case internal::type::str_32:		return sizeof(uint32_t) + internal::unpackInt_betoh_safe<uint32_t>(begin+1, end);
	case internal::type::ext_32:		return 1 + sizeof(uint32_t) + internal::unpackInt_betoh_safe<uint32_t>(begin+1, end);
// Типы размер которых можно узнать только проанализировав весь пакет
	case internal::type::fixarray:
	case internal::type::array_16:
	case internal::type::array_32:
	case internal::type::fixmap:
	case internal::type::map_16:
	case internal::type::map_32:		return complexPkgBodySize(type, begin, end);
	}
	
	throw std::runtime_error("Невозможно определить размер пакета: "
				"неизвестный тип пакета");
}


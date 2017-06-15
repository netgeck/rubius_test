/* 
 * File:   Msg_Pack_unpack.cxx
 * Author: kostya
 * 
 * Created on 10 декабря 2015 г., 10:40
 */

#include "types_handle.h"
#include "MsgPack.h"
#include "MsgPack_unpack.h"


using namespace MsgPack;


//######################    common     ######################

/**
 * @brief Распаковка двоичных данных
 * Возвращаемый тип задается параметром TYPE
 * @param pkg	Пакет в формате MessagePack
 * @return объект заданного формата заполненный двоичными данными из MsgPack-пакета.
 */
template <typename TYPE, typename INT_T>
TYPE unpack_data(const package& pkg){
	constexpr uint8_t servSize = 1 + sizeof(INT_T); // размер служебной части пакета
	
	// Используется range constructor.
	return TYPE(	pkg.data() + servSize,		// указатель на начало данных
			pkg.data() + servSize +		// указатель на конец данных
				internal::unpackInt_betoh_unsafe<INT_T>(pkg.begin()+1) );
}


//#######################    I N T    #######################

/**
 * @brief Перегруженный вариант функции для распаковки uint8_t
 * @param pkg	пакет формата MessagePack
 * @return возвращает переменную uint8_t
 */
template<typename T>
inline T unpackInt(const package&);

template<>
inline uint8_t unpackInt<uint8_t>(const package& pkg) {
	switch (internal::MessagePack::packageType(pkg.front())){
	case internal::type::positive_fixint:	return pkg[0];
	case internal::type::uint_8:		return pkg[1];
	default:
		throw std::runtime_error("Неверный тип пакета");
	}
}

template<>
inline uint16_t unpackInt<uint16_t>(const package& pkg) {
	if (internal::MessagePack::packageType(pkg.front()) != internal::type::uint_16){
		throw std::runtime_error("Неверный тип пакета");
	}
		
	return internal::unpackInt_betoh_unsafe<uint16_t>(pkg.begin()+1);
}

template<>
inline uint32_t unpackInt<uint32_t>(const package& pkg) {
	if (internal::MessagePack::packageType(pkg.front()) != internal::type::uint_32){
		throw std::runtime_error("Неверный тип пакета");
	}
	
	return internal::unpackInt_betoh_unsafe<uint32_t>(pkg.begin()+1);
}

template<>
inline uint64_t unpackInt<uint64_t>(const package& pkg) {
	if (internal::MessagePack::packageType(pkg.front()) != internal::type::uint_64){
		throw std::runtime_error("Неверный тип пакета");
	}
	
	return internal::unpackInt_betoh_unsafe<uint64_t>(pkg.begin()+1);
}

template<>
inline int8_t unpackInt<int8_t>(const package& pkg) {
	switch (internal::MessagePack::packageType(pkg.front())){
	case internal::type::negative_fixint:	return pkg[0];
	case internal::type::int_8:		return pkg[1];
	default:
		throw std::runtime_error("Неверный тип пакета");
	}
}

template<>
inline int16_t unpackInt<int16_t>(const package& pkg) {
	if (internal::MessagePack::packageType(pkg.front()) != internal::type::int_16){
		throw std::runtime_error("Неверный тип пакета");
	}
	
	return internal::unpackInt_betoh_unsafe<int16_t>(pkg.begin()+1);
}

template<>
inline int32_t unpackInt<int32_t>(const package& pkg) {
	if (internal::MessagePack::packageType(pkg.front()) != internal::type::int_32){
		throw std::runtime_error("Неверный тип пакета");
	}
	
	return internal::unpackInt_betoh_unsafe<int32_t>(pkg.begin()+1);
}

template<>
inline int64_t unpackInt<int64_t>(const package& pkg) {
	if (internal::MessagePack::packageType(pkg.front()) != internal::type::int_64){
		throw std::runtime_error("Неверный тип пакета");
	}
	
	return internal::unpackInt_betoh_unsafe<int64_t>(pkg.begin()+1);
}


template<typename INT_T>
INT_T unpack::integer(const package& pkg) {
	return unpackInt<INT_T>(pkg);
}

// Инстанциации метода упаковки целочисленного беззнакового значения:
template uint8_t unpack::integer<uint8_t>(const package&);
template uint16_t unpack::integer<uint16_t>(const package&);
template uint32_t unpack::integer<uint32_t>(const package&);
template uint64_t unpack::integer<uint64_t>(const package&);
// Инстанциации метода упаковки целочисленного значения:
template int8_t unpack::integer<int8_t> (const package&);
template int16_t unpack::integer<int16_t> (const package&);
template int32_t unpack::integer<int32_t> (const package&);
template int64_t unpack::integer<int64_t> (const package&);

//######################    N I L L    ######################

std::nullptr_t unpack::nill(const package& pkg) {
	if (internal::MessagePack::packageType(pkg.front()) != internal::type::nil){
		throw std::runtime_error("Неверный тип пакета");
	}
	return NULL;
}
	
	
bool unpack::boolean(const package& pkg) {
	switch (internal::MessagePack::packageType(pkg.front())){
	case internal::type::false_:	return false;
	case internal::type::true_:	return true;
	default:
		throw std::runtime_error("Неверный тип пакета");
	}
}


//######################    F L O A T    ######################

template<typename FP_T>
FP_T unpack::float_(const package& pkg) {
	if (std::is_same<FP_T, float>::value){
		if (internal::MessagePack::packageType(pkg.front()) != internal::type::float_32){
			throw std::runtime_error("Неверный тип пакета");
		}
		
		// Создаём вспомогательное целочисленное представление дробной переменной
		int32_t intTmp = internal::unpackInt_betoh_unsafe<int32_t>(pkg.begin()+1);
		
		// Реинтерпретируем дробную переменную из вспомогательной целочисленной
		return internal::reinterpret_as<float>(&intTmp);
	}
	
	if (std::is_same<FP_T, double>::value){
		if (internal::MessagePack::packageType(pkg.front()) != internal::type::float_64){
			throw std::runtime_error("Неверный тип пакета");
		}
		
		// Создаём вспомогательное целочисленное представление дробной переменной
		int64_t intTmp = internal::unpackInt_betoh_unsafe<int64_t>(pkg.begin()+1);
		
		// Реинтерпретируем дробную переменную из вспомогательной целочисленной
		return internal::reinterpret_as<double>(&intTmp);
	}
}

// Инстанциации метода распаковки дробного значения:
template float unpack::float_<float>(const package&);
template double unpack::float_<double>(const package&);

//#####################    S T R I N G    #####################

std::string unpack::str(const package& pkg) {
	switch (internal::MessagePack::packageType(pkg.front())){
	case internal::type::fixstr:
		return std::string(++pkg.begin(), pkg.end());
	case internal::type::str_8:
		return unpack_data<std::string, uint8_t>(pkg);
	case internal::type::str_16:
		return unpack_data<std::string, uint16_t>(pkg);
	case internal::type::str_32:
		return unpack_data<std::string, uint32_t>(pkg);
	default:
		throw std::runtime_error("Неверный тип пакета");
	}
}


//#######################    B I N    #######################

rawData unpack::bin(const package& pkg){
	auto pkg_type = internal::MessagePack::packageType(pkg.front());
	switch (pkg_type){
	case internal::type::bin_8:
		return unpack_data<rawData, uint8_t>(pkg);
	case internal::type::bin_16:
		return unpack_data<rawData, uint16_t>(pkg);
	case internal::type::bin_32:
		return unpack_data<rawData, uint32_t>(pkg);
	default:
		throw std::runtime_error("Неверный тип пакета");
	}
}


//#####################    A R R A Y    #####################

array_description extract_array(const package::const_iterator begin,
					const package::const_iterator end, 
					uint32_t size){
	array_description array_descript;
	
//	printf("extract_map pkg: %ld\n", end - begin);
	
	package::const_iterator it = begin;
	
	for (uint32_t i = 0; i < size; i++) {
		// Определяем позицию следующего элемента
		package::const_iterator it_nextPos = it +	// к начальной позиции элемента
			1 + internal::packageBodySize(it, end);	// прибавляем размер элемента
		
		array_descript.emplace_back(
				package(it, it_nextPos)
			);
		
		it = it_nextPos;
	}
	
	return array_descript;
}

array_description unpack::array(const package& pkg){
	switch (internal::MessagePack::packageType(pkg.front())){
	case internal::type::fixarray:
		return extract_array(pkg.begin()+1, pkg.end(), 
			internal::mask::fixarray ^ pkg.front());
	case internal::type::array_16:
		return extract_array(pkg.begin()+1+sizeof(uint16_t), pkg.end(), 
			internal::unpackInt_betoh_unsafe<uint16_t>(pkg.begin()+1));
	case internal::type::array_32:
		return extract_array(pkg.begin()+1+sizeof(uint32_t), pkg.end(), 
			internal::unpackInt_betoh_unsafe<uint32_t>(pkg.begin()+1));
	default:
		throw std::runtime_error("Неверный тип пакета");
	}
}


//#######################    M A P    #######################

map_description extract_map(const package::const_iterator begin,
					const package::const_iterator end, 
					uint32_t size){
	map_description map_descript;
	
	package::const_iterator it(begin), it_second, it_nextPair;
	
	for (uint32_t i = 0; i < size; i++) {
		// Определяем позицию следующего элемента в паре
		// к начальной позиции элемента прибавляем размер элемента
		it_second = it + (1 + internal::packageBodySize(it, end));
		
		// Определяем позицию следующей пары
		// к позиции второго элемента прибавляем размер элемента
		it_nextPair = it_second + (1 + internal::packageBodySize(it_second, end));
				
		map_descript.insert(
			std::make_pair(
				package(it,		it_second), 
				package(it_second,	it_nextPair)
			));
		
		it = it_nextPair;
	}
	
	return map_descript;
}


map_description unpack::map(const package& pkg) {
	switch (internal::MessagePack::packageType(pkg.front())){
	case internal::type::fixmap:
		return extract_map(pkg.begin()+1, pkg.end(),
			internal::mask::fixmap ^ pkg.front());
	case internal::type::map_16:
		return extract_map(pkg.begin()+1+sizeof(uint16_t), pkg.end(), 
			internal::unpackInt_betoh_unsafe<uint16_t>(pkg.begin()+1));
	case internal::type::map_32:
		return extract_map(pkg.begin()+1+sizeof(uint32_t), pkg.end(), 
			internal::unpackInt_betoh_unsafe<uint32_t>(pkg.begin()+1));
	default:
		throw std::runtime_error("Неверный тип пакета");
	}
}


//#######################    E X T    #######################

ext_description unpack::ext(const package& pkg) {
	internal::type pkg_type = internal::MessagePack::packageType(pkg.front());
	ext_description ext;
	uint32_t size(0);
	
	switch (pkg_type) {
	case internal::type::fixext_1:		size = 1;	break;
	case internal::type::fixext_2:		size = 2;	break;
	case internal::type::fixext_4:		size = 4;	break;
	case internal::type::fixext_8:		size = 8;	break;
	case internal::type::fixext_16:		size = 16;	break;
		default: break;
	}
	
	if (size != 0){
		ext.first = *(pkg.begin()+1);
		ext.second.reserve(size);
		ext.second.assign(pkg.begin()+2, pkg.begin()+2+size);
		return ext;
	}
	
	package::const_iterator data_begin;
	switch (pkg_type) {
	case internal::type::ext_8:
		size = internal::unpackInt_betoh_unsafe<uint8_t>(pkg.begin()+1);
		data_begin = pkg.begin()+1+sizeof(uint8_t);
		break;
	case internal::type::ext_16:
		size = internal::unpackInt_betoh_unsafe<uint16_t>(pkg.begin()+1);
		data_begin = pkg.begin()+1+sizeof(uint16_t);
		break;
	case internal::type::ext_32:
		size = internal::unpackInt_betoh_unsafe<uint32_t>(pkg.begin()+1);
		data_begin = pkg.begin()+1+sizeof(uint32_t);
		break;
	default:
		throw std::runtime_error("Неверный тип пакета");
	}
	
	ext.first = *data_begin;
	ext.second.assign(data_begin+1, data_begin + 1 + size);
	return ext;
}


template<typename T>
typename std::enable_if< std::is_floating_point<T>::value, T >::type
unpack_arithmetic_val(const package::const_iterator begin) {
	if (std::is_same<T, float>::value) {
		uint32_t tmpVar = internal::unpackInt_betoh_unsafe<uint32_t>(begin);
		return internal::reinterpret_as<float>(&tmpVar);
	} else if (std::is_same<T, double>::value) {
		uint64_t tmpVar = internal::unpackInt_betoh_unsafe<uint64_t>(begin);
		return internal::reinterpret_as<double>(&tmpVar);
	}
	
	throw std::runtime_error("неподдерживаемый тип");
}

template<typename T>
typename std::enable_if< std::is_integral<T>::value, T >::type
unpack_arithmetic_val(const package::const_iterator begin) {
	return internal::unpackInt_betoh_unsafe<T>(begin);
}


template <typename T>
T MsgPack::unpack_arithmetic(const package::const_iterator begin) {
	return unpack_arithmetic_val<T>(begin);
}

template uint8_t  MsgPack::unpack_arithmetic<uint8_t> ( const package::const_iterator begin );
template uint16_t MsgPack::unpack_arithmetic<uint16_t>( const package::const_iterator begin );
template uint32_t MsgPack::unpack_arithmetic<uint32_t>( const package::const_iterator begin );
template uint64_t MsgPack::unpack_arithmetic<uint64_t>( const package::const_iterator begin );
template int8_t   MsgPack::unpack_arithmetic<int8_t>  ( const package::const_iterator begin );
template int16_t  MsgPack::unpack_arithmetic<int16_t> ( const package::const_iterator begin );
template int32_t  MsgPack::unpack_arithmetic<int32_t> ( const package::const_iterator begin );
template int64_t  MsgPack::unpack_arithmetic<int64_t> ( const package::const_iterator begin );
template float    MsgPack::unpack_arithmetic<float>   ( const package::const_iterator begin );
template double   MsgPack::unpack_arithmetic<double>  ( const package::const_iterator begin );


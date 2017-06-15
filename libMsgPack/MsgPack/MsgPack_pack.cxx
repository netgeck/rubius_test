/* 
 * File:   MsgPack_pack.cxx
 * Author: kostya
 * 
 * Created on 10 декабря 2015 г., 10:39
 */

#include "types_handle.h"
#include "MsgPack.h"
#include "MsgPack_pack.h"


using namespace MsgPack;


//######################    common     ######################

/**
 * @brief Упаковка двоичных данных
 * @param pkg_head	Заголовок пакета
 * @param ptr		Указатель на данные
 * @param size		Длина данных
 * @return Возвращает сформированный пакет формата MessagePack
 */
template <typename UINT_T>
package pack_data(package_data pkg_head, const void* ptr, size_t size){
	constexpr uint8_t servSize = 1 + sizeof(UINT_T); // размер служебной части пакета
	package pkg(size + servSize);
	pkg[0] = pkg_head; // добавляем заголовок пакета
	
	internal::packInt_htobe_unsafe<UINT_T>(pkg.begin()+1, size);
	
	memcpy(pkg.data() + servSize, ptr, size);
	
	return pkg;
}


//######################    N I L L    ######################

package pack::nill() {
	return package({ internal::MessagePack::getHead(internal::type::nil) });
}


//######################    B O O L    ######################

package pack::boolean(bool val) {
	if (val){
		return package({ internal::MessagePack::getHead(internal::type::true_) });
	}
	
	return package({ internal::MessagePack::getHead(internal::type::false_) });
}


//#######################    I N T    #######################

/**
 * @brief Упаковка целочисленной переменной.
 * Функция не работает с однобайтовым целым! ((u)int8_t)
 * @param pkg_head	Заголовок пакета
 * @param var		Сериализуемая переменная
 * @return 
 */
template <typename INT_T>
inline package pack_int(package_data pkg_head, INT_T var){
	package pkg(sizeof(INT_T)+1);
	pkg[0] = pkg_head; // Пишем заголовок в первый байт массива

	// В оставшуюся память массива пишем переменную приведенную к big-endian
	internal::packInt_htobe_unsafe<INT_T>(pkg.begin()+1, var);
	
	return pkg;
}


template <typename T>
inline package packInt(T);

template<>
inline package packInt<uint8_t>(uint8_t var) {
	if (var <= 127){
		return package({static_cast<package_data>(var)});
	}
	return package({ internal::MessagePack::getHead(internal::type::uint_8), 
			static_cast<package_data>(var)});
}

template<>
inline package packInt<uint16_t>(uint16_t var) {
	return pack_int<uint16_t>( internal::MessagePack::getHead(internal::type::uint_16), var);
}

template<>
inline package packInt<uint32_t>(uint32_t var) {
	return pack_int<uint32_t>( internal::MessagePack::getHead(internal::type::uint_32), var);
}

template<>
inline package packInt<uint64_t>(uint64_t var) {
	return pack_int<uint64_t>( internal::MessagePack::getHead(internal::type::uint_64), var);
}

template<>
inline package packInt<int8_t>(int8_t var) {
	if (var <= -1 && var >= -32){ // знаковое целое в диапазоне -32..-1
		return package({static_cast<package_data>(var)});
	}
	
	return package({ internal::MessagePack::getHead(internal::type::int_8),
			static_cast<package_data>(var) });
}

template<>
inline package packInt<int16_t>(int16_t var) {
	return pack_int( internal::MessagePack::getHead(internal::type::int_16), var);
}

template<>
inline package packInt<int32_t>(int32_t var) {
	return pack_int( internal::MessagePack::getHead(internal::type::int_32), var);
}

template<>
inline package packInt<int64_t>(int64_t var) {
	return pack_int( internal::MessagePack::getHead(internal::type::int_64), var);
}


template<typename INT_T>
package pack::integer(INT_T var) {
	return packInt<INT_T>(var);
}
// Инстанциации метода упаковки целочисленного беззнакового значения:
template package pack::integer<uint8_t>(uint8_t var);
template package pack::integer<uint16_t>(uint16_t var);
template package pack::integer<uint32_t>(uint32_t var);
template package pack::integer<uint64_t>(uint64_t var);
// Инстанциации метода упаковки целочисленного значения:
template package pack::integer<int8_t> (int8_t var);
template package pack::integer<int16_t> (int16_t var);
template package pack::integer<int32_t> (int32_t var);
template package pack::integer<int64_t> (int64_t var);


//#######################    F L O A T    ######################

template<typename FP_T>
package pack::float_(FP_T var) {
	if (std::is_same<FP_T, float>::value){
		// Упаковка дробной переменной функцией упаковки целочисленных.
		// Для этого интерпретируем дробную как целочисленную
		return pack_int<uint32_t>( internal::MessagePack::getHead(internal::type::float_32),
					internal::reinterpret_as<uint32_t>(&var) );
	} else if (std::is_same<FP_T, double>::value){
		// Упаковка дробной переменной функцией упаковки целочисленных.
		// Для этого интерпретируем дробную как целочисленную
		return pack_int<uint64_t>( internal::MessagePack::getHead(internal::type::float_64),
					internal::reinterpret_as<uint64_t>(&var) );
	}
}

// Инстанциации метода упаковки дробного значения:
template package pack::float_<float>(float var);
template package pack::float_<double>(double var);


//######################    S T R I N G    #####################

inline package fixstr(const std::string& str) {
	package pkg; 
	pkg.reserve(str.size()+1); // пакет размером = длина строки + заголовок
	
	pkg.emplace_back(internal::mask::fixstr ^ str.size());
	pkg.insert(pkg.end(), str.begin(), str.end());
	
	return pkg;
}


package pack::str(const std::string& str) {
	if (str.size() <= 31){
		return fixstr(str);
	} else if (str.size() <= UINT8_MAX){
		return pack_data<uint8_t>( internal::MessagePack::getHead(internal::type::str_8), str.data(), str.size());
	} else if (str.size() <= UINT16_MAX){
		return pack_data<uint16_t>( internal::MessagePack::getHead(internal::type::str_16), str.data(), str.size());
	} else if (str.size() <= UINT32_MAX){
		return pack_data<uint32_t>( internal::MessagePack::getHead(internal::type::str_32), str.data(), str.size());
	} else {
		throw std::runtime_error("Строка слишком большой длины");
	}
}


//#######################    B I N    #######################

inline package pack_bin(const void* ptr, size_t size) {
	if (size <= UINT8_MAX) {
		return pack_data<uint8_t>( internal::MessagePack::getHead(internal::type::bin_8), ptr, size);
	} else if (size <= UINT16_MAX) {
		return pack_data<uint16_t>( internal::MessagePack::getHead(internal::type::bin_16), ptr, size);
	} else if (size <= UINT32_MAX) {
		return pack_data<uint32_t>( internal::MessagePack::getHead(internal::type::bin_32), ptr, size);
	} else {
		throw std::runtime_error("Пакет слишком большой длины");
	}
}

package pack::bin(const rawData& bin) {
	return pack_bin(bin.data(), bin.size());
}

package pack::bin(const void* ptr, size_t size) {
	return pack_bin(ptr, size);
}

//#####################    A R R A Y    #####################

inline uint64_t array_description_capacity(const array_description& descript) {
	if (descript.size() > UINT32_MAX){
		throw std::runtime_error("array слишком большой длины");
	}
	
	uint64_t sumSize(0);
	
	for (auto descripElm : descript) {
		sumSize += descripElm.size();
	}
	
	return sumSize;
}


package pack::array(const array_description& descript) {
	package pkg;
	if (descript.size() <= 15){
		pkg.emplace_back(internal::mask::fixarray ^ descript.size());
	} else if (descript.size() <= UINT16_MAX){
		pkg.push_back(internal::MessagePack::getHead(internal::type::array_16));
		pkg.resize(pkg.size() + sizeof(uint16_t));
		internal::packInt_htobe_unsafe<uint16_t>(pkg.begin()+1, descript.size());
	} else if (descript.size() <= UINT32_MAX){
		pkg.push_back(internal::MessagePack::getHead(internal::type::array_32));
		pkg.resize(pkg.size() + sizeof(uint32_t));
		internal::packInt_htobe_unsafe<uint32_t>(pkg.begin()+1, descript.size());
	} else {
		throw std::runtime_error("map слишком большой длины");
	}
	
	pkg.reserve(pkg.size() + array_description_capacity(descript));
	
	for (const auto& map_elm : descript){
		pkg.insert(pkg.end(), map_elm.begin(), map_elm.end());
	}
	
	return pkg;
}


//#######################    M A P    #######################

uint64_t map_description_capacity(const map_description& descript) {
	if (descript.size() > UINT32_MAX){
		throw std::runtime_error("map слишком большой длины");
	}
	
	uint64_t sumSize(0);
	
	for (auto descripElm : descript) {
		sumSize += descripElm.first.size() + descripElm.second.size();
	}
	
	return sumSize;
}


package pack::map(const map_description& descript) {
	package pkg;
	if (descript.size() <= 15){
		pkg.emplace_back(internal::mask::fixmap ^ descript.size());
	} else if (descript.size() <= UINT16_MAX){
		pkg.push_back(internal::MessagePack::getHead(internal::type::map_16));
		pkg.resize(pkg.size() + sizeof(uint16_t));
		internal::packInt_htobe_unsafe<uint16_t>(pkg.begin()+1, descript.size());
	} else if (descript.size() <= UINT32_MAX){
		pkg.push_back(internal::MessagePack::getHead(internal::type::map_32));
		pkg.resize(pkg.size() + sizeof(uint32_t));
		internal::packInt_htobe_unsafe<uint32_t>(pkg.begin()+1, descript.size());
	} else {
		throw std::runtime_error("map слишком большой длины");
	}
	
	pkg.reserve(pkg.size() + map_description_capacity(descript));
	
	for (const auto& map_elm : descript){
		pkg.insert(pkg.end(), map_elm.first.begin(), map_elm.first.end());
		pkg.insert(pkg.end(), map_elm.second.begin(), map_elm.second.end());
	}
	
	return pkg;
}


//#######################    E X T    #######################

package pack::ext(const ext_description& descript) {
	package pkg;
	if (descript.second.size() <= 16) { // до 16 байт - Возможно(!) фиксированный ext
		switch(descript.second.size()){
		case 1:
			pkg.reserve(1+1+1);
			pkg.push_back(internal::MessagePack::getHead(internal::type::fixext_1));
			break;
		case 2:
			pkg.reserve(1+1+2);
			pkg.push_back(internal::MessagePack::getHead(internal::type::fixext_2));
			break;
		case 4:
			pkg.reserve(1+1+4);
			pkg.push_back(internal::MessagePack::getHead(internal::type::fixext_4));
			break;
		case 8:
			pkg.reserve(1+1+8);
			pkg.push_back(internal::MessagePack::getHead(internal::type::fixext_8));
			break;
		case 16:
			pkg.reserve(1+1+16);
			pkg.push_back(internal::MessagePack::getHead(internal::type::fixext_16));
			break;
		}
		if (pkg.size() > 0){
			pkg.push_back(descript.first);
			pkg.insert(pkg.end(), descript.second.begin(), descript.second.end());
			return pkg;
		}
	}
	
	// Не фиксированный Ext
	if (descript.second.size() <= UINT8_MAX) {
		pkg.push_back(internal::MessagePack::getHead(internal::type::ext_8));
		pkg.resize(pkg.size() + sizeof(uint8_t));
		internal::packInt_htobe_unsafe<uint8_t>(pkg.begin()+1, descript.second.size());
	} else if (descript.second.size() <= UINT16_MAX) {
		pkg.push_back(internal::MessagePack::getHead(internal::type::ext_16));
		pkg.resize(pkg.size() + sizeof(uint16_t));
		internal::packInt_htobe_unsafe<uint16_t>(pkg.begin()+1, descript.second.size());
	} else if (descript.second.size() <= UINT32_MAX) {
		pkg.push_back(internal::MessagePack::getHead(internal::type::ext_32));
		pkg.resize(pkg.size() + sizeof(uint32_t));
		internal::packInt_htobe_unsafe<uint32_t>(pkg.begin()+1, descript.second.size());
	}
	
	pkg.push_back(descript.first);
	
	pkg.reserve(pkg.size() + descript.second.size());
	pkg.insert(pkg.end(), descript.second.begin(), descript.second.end());
	
	return pkg;
}


template<typename T>
typename std::enable_if< std::is_floating_point<T>::value >::type
pack_arithmetic_val(const package::iterator begin, T var) {
	if (std::is_same<T, float>::value) {
		internal::packInt_htobe_unsafe<uint32_t>(begin,
			internal::reinterpret_as<uint32_t>((void*)&(var)));
	} else if (std::is_same<T, double>::value) {
		internal::packInt_htobe_unsafe<uint64_t>(begin,
			internal::reinterpret_as<uint64_t>((void*)&(var)));
	}
}

template<typename T>
typename std::enable_if< std::is_integral<T>::value >::type
pack_arithmetic_val(const package::iterator begin, T var) {
	internal::packInt_htobe_unsafe<T>(begin, var);
}

template <typename T>
void MsgPack::pack_arithmetic(const package::iterator begin, T var) {
	pack_arithmetic_val(begin, var);
}

template void MsgPack::pack_arithmetic<uint8_t> ( const package::iterator begin, uint8_t  var );
template void MsgPack::pack_arithmetic<uint16_t>( const package::iterator begin, uint16_t var );
template void MsgPack::pack_arithmetic<uint32_t>( const package::iterator begin, uint32_t var );
template void MsgPack::pack_arithmetic<uint64_t>( const package::iterator begin, uint64_t var );
template void MsgPack::pack_arithmetic<int8_t>  ( const package::iterator begin, int8_t   var );
template void MsgPack::pack_arithmetic<int16_t> ( const package::iterator begin, int16_t  var );
template void MsgPack::pack_arithmetic<int32_t> ( const package::iterator begin, int32_t  var );
template void MsgPack::pack_arithmetic<int64_t> ( const package::iterator begin, int64_t  var );
template void MsgPack::pack_arithmetic<float>   ( const package::iterator begin, float    var );
template void MsgPack::pack_arithmetic<double>  ( const package::iterator begin, double   var );


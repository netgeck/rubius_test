#ifndef __MSGPACK_TYPES_H__
#define __MSGPACK_TYPES_H__


#include <cstdint>
#include <vector>
#include <map>


namespace MsgPack {


/**
 * @brief Перечисление типов пакетов MessagePack
 * Перечислены пакеты известные пользователю
 */
enum class type {
	nil,		//!< Nil
	boolean,	//!< Boolean
	bin,		//!< Binary
	float_32, float_64, //!< Float
	uint_8,	uint_16, uint_32, uint_64, int_8, int_16, int_32, int_64, //!< Integer
	ext,		//!< Extension
	str,		//!< String
	array,		//!< Array
	map		//!< Map
};


typedef uint8_t				header;	//!< Тип заголовка пакета
typedef uint8_t				package_data; //!< Тип данных в пакете
typedef std::vector<package_data>	package; //!< Сформированный пакет в формате MessagePack
typedef std::vector<char>		rawData; //!< Массив бинарных данных

/// Описатель пакета типа array
typedef std::vector<package>		array_description;
/// Описатель типа map
typedef std::map<package, package>	map_description;
/// Описатель типа ext
typedef std::pair<header, package>	ext_description;


} // namespace MessagePack


#endif // __MSGPACK_TYPES_H__

/* 
 * File:   Msg_Pack_unpack.h
 * Author: kostya
 *
 * Created on 10 декабря 2015 г., 10:40
 */

#ifndef MSG_PACK_UNPACK_H
#define MSG_PACK_UNPACK_H


#include "MsgPack_types.h"


namespace MsgPack {


/**
 * @brief Класс десериализатор
 * Содержит набор статических методов для десериализации данных из формата MessagePack
 */
namespace unpack {
	/**
	 * @brief Фунция распаковки пакета типа Nil
	 * @param pkg	пакет формата MessagePack
	 * @return возвращает нуль
	 */
	std::nullptr_t nill(const package& pkg);
	
	/**
	 * @brief Функция распаковки пакета типа Boolean
	 * @param pkg	пакет формата MessagePack
	 * @return вовзращает переменную типа bool
	 */
	bool boolean(const package& pkg);

	/**
	 * @brief Функция распаковки пакета типа Integer
	 * @param pkg	пакет формата MessagePack
	 * @return возвращает целочисленную переменную
	 */
	template <typename INT_T>
	INT_T integer(const package& pkg);
	
	/**
	 * @brief Функция распаковки пакета типа Float
	 * @param pkg	пакет формата MessagePack
	 * @return возвращает вещественную переменную
	 */
	template <typename FP_T>
	FP_T float_(const package& pkg);

	/**
	 * @brief Функция распаковки пакета типа String
	 * @param pkg	пакет формата MessagePack
	 * @return возвращает строку
	 */
	std::string str(const package& pkg);
	
	/**
	 * @brief Функция распаковки пакета типа Binary
	 * @param pkg	пакет формата MessagePack
	 * @return вовращает пакет бинарных данных
	 */
	rawData bin(const package& pkg);
	
	/**
	 * @brief Функция распаковки пакета типа Array
	 * @param pkg	пакет формата MessagePack
	 * @return вовзращает описатель массива разнородных значений
	 */
	array_description array(const package& pkg);
	
	/**
	 * @brief Функция распаковки пакета типа Map
	 * @param pkg	пакет формата MessagePack
	 * @return возвращает описатель ассоциативного контейнера разнородных пар
	 */
	map_description map(const package& pkg);
	
	/**
	 * @brief Функция распаковки пакета типа Extension
	 * Данные распаковываются как есть, без преобразования к порядку байт хоста.
	 * Читатель описателя пакета отвественнен 
	 * за преобразование арифметических типов к порядку байтов хоста!!!
	 * 
	 * @param pkg	пакет формата MessagePack
	 * @return возвращает описатель пользовательского пакета 
	 */
	ext_description ext(const package& pkg);
}


// Предоставляемые инстанциации метода распаковки целочисленного беззнакового значения:
extern template uint8_t unpack::integer<uint8_t>(const package&);
extern template uint16_t unpack::integer<uint16_t>(const package&);
extern template uint32_t unpack::integer<uint32_t>(const package&);
extern template uint64_t unpack::integer<uint64_t>(const package&);
// Предоставляемые инстанциации метода распаковки целочисленного значения:
extern template int8_t unpack::integer<int8_t> (const package&);
extern template int16_t unpack::integer<int16_t> (const package&);
extern template int32_t unpack::integer<int32_t> (const package&);
extern template int64_t unpack::integer<int64_t> (const package&);

// Предоставляемые инстанциации метода распаковки дробного значения:
extern template float unpack::float_<float>(const package&);
extern template double unpack::float_<double>(const package&);


/**
 * @brief Функция распаковки арифметического значения. 
 * Позиция для размещения значения в пакете задаётся итератором.
 * Функция не проверяет может ли пакет содержать переменную с размером заданного типа!!!
 * @param begin	итератор указывающий место значения в пакете
 * @return Возвращает распакованное и привидённое к порядку байт хоста значение
 */
template <typename T>
T unpack_arithmetic(const package::const_iterator begin);

extern template uint8_t  unpack_arithmetic<uint8_t> ( const package::const_iterator begin );
extern template uint16_t unpack_arithmetic<uint16_t>( const package::const_iterator begin );
extern template uint32_t unpack_arithmetic<uint32_t>( const package::const_iterator begin );
extern template uint64_t unpack_arithmetic<uint64_t>( const package::const_iterator begin );
extern template int8_t   unpack_arithmetic<int8_t>  ( const package::const_iterator begin );
extern template int16_t  unpack_arithmetic<int16_t> ( const package::const_iterator begin );
extern template int32_t  unpack_arithmetic<int32_t> ( const package::const_iterator begin );
extern template int64_t  unpack_arithmetic<int64_t> ( const package::const_iterator begin );
extern template float    unpack_arithmetic<float>   ( const package::const_iterator begin );
extern template double   unpack_arithmetic<double>  ( const package::const_iterator begin );


} // namespace MessagePack


#endif /* MSG_PACK_UNPACK_H */


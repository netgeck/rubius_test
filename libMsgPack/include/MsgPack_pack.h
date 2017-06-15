/* 
 * File:   MsgPack_pack.h
 * Author: kostya
 *
 * Created on 10 декабря 2015 г., 10:39
 */

#ifndef MSGPACK_PACK_H
#define MSGPACK_PACK_H


#include "MsgPack_types.h"



namespace MsgPack {
	

/**
 * @brief Класс сериализатор.
 * Содержит набор статических методов для сериализации данных в формат MessagePack
 */
namespace pack {
	/**
	 * @brief Функция создания пакет содержащего нуль
	 * @return возвращает пакет в формате MessagePack
	 */
	package nill();

	/**
	 * @brief Функция упаковки булевого значения
	 * @param val	переменная типа bool
	 * @return возвращает пакет в формате MessagePack
	 */
	package boolean(bool val);

	/**
	 * @brief Функция упаковки целочисленного значения
	 * @param var	целочисленная переменная
	 * @return возвращает пакет в формате MessagePack
	 */
	template <typename INT_T>
	package integer(INT_T var);
	
	/**
	 * @brief Функция упаковки вещественных чисел
	 * @param var	переменная с плавающей запятой
	 * @return возвращает пакет в формате MessagePack
	 */
	template <typename FP_T>
	package float_(FP_T var);
	
	/**
	 * @brief Функция упаковки строк
	 * @param str	строка
	 * @return возвращает пакет в формате MessagePack
	 */
	package str(const std::string& str);
	
	/**
	 * @brief Функция упаковки бинарных данных
	 * @param bin	массив двоичных данных
	 * @return возвращает пакет в формате MessagePack
	 */
	package bin(const MsgPack::rawData& bin);

	/**
	 * @brief Функция упаковки бинарных данных
	 * @param ptr	указатель на данные
	 * @param size	длина данных в байтах
	 * @return возвращает пакет в формате MessagePack
	 */
	package bin(const void* ptr, size_t size);
	
	/**
	 * @brief Функция упаковки массива.
	 * В MessagePack'е массив может состоять из элементов разных(!) типов
	 * @param descript	описатель массива разнородных элементов
	 * @return возвращает пакет в формате MessagePack
	 */
	package array(const MsgPack::array_description& descript);

	/**
	 * @brief Функция упаковки ассоциативного массива map
	 * В MessagePack'е массив может состоять из элементов разных(!) типов
	 * @param descript	описатель контейнера map (может состоять из разнородных элементов)
	 * @return возвращает пакет в формате MessagePack
	 */
	package map(const MsgPack::map_description& descript);
	
	/**
	 * @brief Функция упаковки пользовательских расширений формата MessagePack.
	 * Данные упаковываются как есть, без преобразования к bit-endian.
	 * Формирователь описателя пакета отвественнен 
	 * за преобразование арифметических типов к big-endian!!!
	 * 
	 * @param descript	описатель расширенного типа
	 * @return возвращает пакет в формате MessagePack
	 */
	package ext(const MsgPack::ext_description& descript);
}


// Предоставляемые инстанциации метода упаковки целочисленного беззнакового значения:
extern template package pack::integer<uint8_t>(uint8_t var);
extern template package pack::integer<uint16_t>(uint16_t var);
extern template package pack::integer<uint32_t>(uint32_t var);
extern template package pack::integer<uint64_t>(uint64_t var);
// Предоставляемые инстанциации метода упаковки целочисленного значения:
extern template package pack::integer<int8_t> (int8_t var);
extern template package pack::integer<int16_t> (int16_t var);
extern template package pack::integer<int32_t> (int32_t var);
extern template package pack::integer<int64_t> (int64_t var);

// Предоставляемые инстанциации метода упаковки дробного значения:
extern template package pack::float_<float>(float var);
extern template package pack::float_<double>(double var);


/**
 * @brief Функция упаковки арифметического значения. 
 * Позиция для размещения значения в пакете задаётся итератором.
 * Функция не проверяет достаточно ли в пакете места для размещения переменной заданного типа!!!
 * @param begin	итератор указывающий место для записи в пакете
 * @param var	значение арифметического типа
 */
template <typename T>
void pack_arithmetic(const package::iterator begin, T var);

// Предоставляемые инстанциации функции упаковки арфиметических значений
extern template void pack_arithmetic<uint8_t> ( const package::iterator begin, uint8_t  var );
extern template void pack_arithmetic<uint16_t>( const package::iterator begin, uint16_t var );
extern template void pack_arithmetic<uint32_t>( const package::iterator begin, uint32_t var );
extern template void pack_arithmetic<uint64_t>( const package::iterator begin, uint64_t var );
extern template void pack_arithmetic<int8_t>  ( const package::iterator begin, int8_t   var );
extern template void pack_arithmetic<int16_t> ( const package::iterator begin, int16_t  var );
extern template void pack_arithmetic<int32_t> ( const package::iterator begin, int32_t  var );
extern template void pack_arithmetic<int64_t> ( const package::iterator begin, int64_t  var );
extern template void pack_arithmetic<float>   ( const package::iterator begin, float    var );
extern template void pack_arithmetic<double>  ( const package::iterator begin, double   var );


} // namespace MsgPack


#endif /* MSGPACK_PACK_H */


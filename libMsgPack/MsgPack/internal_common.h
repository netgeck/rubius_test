/* 
 * File:   internal_common.h
 * Author: kostya
 *
 * Created on 11 февраля 2016 г., 11:48
 */

#ifndef __INTERNAL_COMMON_H__
#define __INTERNAL_COMMON_H__


#include "MsgPack_types.h"


namespace MsgPack {

	
namespace internal {

	
/**
 * @brief Типы пакетов MessagePack
 * Перечисление внутренних типов пакетов
 */
enum class type {
	positive_fixint,//0xxxxxxx	0x00 - 0x7f
	fixmap,		//1000xxxx	0x80 - 0x8f
	fixarray,	//1001xxxx	0x90 - 0x9f
	fixstr,		//101xxxxx	0xa0 - 0xbf
	nil,		//11000000	0xc0
	//(never used)	11000001	0xc1
	false_,		//11000010	0xc2
	true_,		//11000011	0xc3
	bin_8,		//11000100	0xc4
	bin_16,		//11000101	0xc5
	bin_32,		//11000110	0xc6
	ext_8,		//11000111	0xc7
	ext_16,		//11001000	0xc8
	ext_32,		//11001001	0xc9
	float_32,	//11001010	0xca
	float_64,	//11001011	0xcb
	uint_8,		//11001100	0xcc
	uint_16,	//11001101	0xcd
	uint_32,	//11001110	0xce
	uint_64,	//11001111	0xcf
	int_8,		//11010000	0xd0
	int_16,		//11010001	0xd1
	int_32,		//11010010	0xd2
	int_64,		//11010011	0xd3
	fixext_1,	//11010100	0xd4
	fixext_2,	//11010101	0xd5
	fixext_4,	//11010110	0xd6
	fixext_8,	//11010111	0xd7
	fixext_16,	//11011000	0xd8
	str_8,		//11011001	0xd9
	str_16,		//11011010	0xda
	str_32,		//11011011	0xdb
	array_16,	//11011100	0xdc
	array_32,	//11011101	0xdd
	map_16,		//11011110	0xde
	map_32,		//11011111	0xdf
	negative_fixint,//111xxxxx	0xe0 - 0xff
};


/**
 * @brief Имя внутреннего типа пакета MessagePack
 * @param tp	тип пакета MessagePack
 * @return возвращает имя типа
 */
std::string type_name(type tp);


/**
 * @brief Определить является ли пакет корректным пакетом формата MessagePack
 * Функция принимает итераторы чтобы исключить копирование.
 * Итераторы begin и end должны четко обозначать границы одного пакета.
 * @param begin	начало пакета
 * @param end	конец пакета (или конец области памяти содержавшей пакет)
 * @return
 */
bool isPkgCorrect(const package::const_iterator begin, const package::const_iterator end);


/**
 * @brief Определить размер тела пакета.
 * Полный размер пакета всегда равен: размер_тела + 1;
 * Функция принимает итераторы чтобы исключить копирование
 * @param begin	начало пакета
 * @param end	конец пакета (или конец области памяти содержавшей пакет)
 * @return возвращает размер тела пакета
 */
size_t packageBodySize(const package::const_iterator begin, const package::const_iterator end);


} //namespace internal


} // namespace MessagePack


#endif /* __INTERNAL_COMMON_H__ */


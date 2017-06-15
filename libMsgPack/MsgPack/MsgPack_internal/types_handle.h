/* 
 * File:   types.h
 * Author: kostya
 *
 * Created on 11 февраля 2016 г., 16:03
 */

#ifndef __TYPES_HANDLE_H__
#define __TYPES_HANDLE_H__


#include <endian.h>
#include <boost/lexical_cast.hpp>

#include "MsgPack_types.h"


namespace MsgPack {

	
namespace internal {
	

/**
 * @brief Реинтерпретировать указатель на переменную как переменную типа T
 * @param pVar	указатель на переменную
 * @return возвращается переменная типа T
 */	
template <typename T>
inline T reinterpret_as(void* pVar){
	return *(reinterpret_cast<T*>(pVar));
}


/**
 * Перегруженные обёртки для конвертации big-endian переменной к порядку байт хоста.
 * Обёртки понадобились для единообразного вызова разных функций
 */
inline uint8_t betoh (uint8_t var){
	return var;
}
inline uint16_t betoh (uint16_t var){
	return be16toh(var);
}
inline uint32_t betoh (uint32_t var){
	return be32toh(var);
}
inline uint64_t betoh (uint64_t var){
	return be64toh(var);
}
inline int8_t betoh (int8_t var){
	return var;
}
inline int16_t betoh (int16_t var){
	return be16toh(var);
}
inline int32_t betoh (int32_t var){
	return be32toh(var);
}
inline int64_t betoh (int64_t var){
	return be64toh(var);
}


/**
 * Перегруженные обёртки для конвертации переменной от порядка байт хоста к big-endian.
 * Обёртки понадобились для единообразного вызова разных функций
 */
inline uint8_t htobe (uint8_t var){
	return var;
}
inline uint16_t htobe (uint16_t var){
	return htobe16(var);
}
inline uint32_t htobe (uint32_t var){
	return htobe32(var);
}
inline uint64_t htobe (uint64_t var){
	return htobe64(var);
}
inline int8_t htobe (int8_t var){
	return var;
}
inline int16_t htobe (int16_t var){
	return htobe16(var);
}
inline int32_t htobe (int32_t var){
	return htobe32(var);
}
inline int64_t htobe (int64_t var){
	return htobe64(var);
}


/**
 * @brief Распаковать целочисленную переменную заданного размера из участка пакета.
 * Производит конвертацию переменной от Big-endian к порядку байт хоста.
 * Не производит проверок размер области памяти перед распаковкой.
 * @param begin	Начало участка где распаложена переменная
 * @return Целочисленная переменная заданого типа
 */
template <typename UINT_T>
inline UINT_T unpackInt_betoh_unsafe(const package::const_iterator begin){
	// Получаем из итератора указатель и реинтерпретируем его как указатель на переменную 
	// требуемого типа. Через union было бы наглядней, но дольше.
	UINT_T var;
	memcpy(&var, &(*begin), sizeof(UINT_T));
	return betoh(var);
}


/**
 * @brief Распаковать целочисленную переменную заданного размера из участка пакета.
 * Производит конвертацию переменной от Big-endian к порядку байт хоста.
 * Проверяет размер области памяти перед распаковкой оттуда переменной.
 * @param begin	Начало участка где распаложена переменная
 * @param end	Конец участка содержащего переменную (либо конец пакета)
 * @return Целочисленная переменная заданого типа
 */
template <typename UINT_T>
UINT_T unpackInt_betoh_safe(const package::const_iterator begin, 
			const package::const_iterator end){
	if (end - begin < 0 || 
		static_cast<size_t>(end - begin) < sizeof(UINT_T)){
		throw std::runtime_error("Заданный отрезок пакета не может "
			"вмещать переменную запрашиваемого размера"
			" (" + boost::lexical_cast<std::string>(end-begin) // размер области памяти
			+ " < " + boost::lexical_cast<std::string>(sizeof(UINT_T)) //размер перемен.
			+ ")" );
	}
	
	return unpackInt_betoh_unsafe<UINT_T>(begin);
}


/**
 * @brief Упаковка целочисленной переменной в заданный участок пакета.
 * Производит конвертацию переменной от порядка байт хоста к Big-endian.
 * Не производит проверок размер области памяти перед упаковкой.
 * @param begin		Начало области доступной для упаковки переменной
 * @param var		Сериализуемая переменная
 */
template <typename INT_T>
inline void packInt_htobe_unsafe(const package::iterator begin, INT_T var){
	// Приводим переменную к big-endian и записываем в память по заданной позиции
	INT_T beVar = htobe(var);
	memcpy(&(*begin), &beVar, sizeof(INT_T));
}


} //namespace internal


} // namespace MessagePack


#endif /* __TYPES_HANDLE_H__ */


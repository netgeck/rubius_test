/* 
 * File:   MsgPack.h
 * Author: kostya
 *
 * Created on 9 декабря 2015 г., 10:34
 */

#ifndef MSGPACK_H
#define MSGPACK_H


#include <boost/bimap.hpp>

#include "MsgPack_types.h"
#include "internal_common.h"


namespace MsgPack {
	
	
/**
 * @brief Конвертация внутреннего представления типа пакета к более общему внешнему
 * @param type	внутренний тип пакета
 * @return возвращает внешний (интерфейсный) тип пакета
 */
MsgPack::type innerToInterfaceType(MsgPack::internal::type type);

	
namespace internal {
	

/**
 * @brief Битовые маски
 */
enum mask {
	positive_fixint = 0x00,	//0xxxxxxx	0x00 - 0x7f
	fixmap          = 0x80,	//1000xxxx	0x80 - 0x8f
	fixarray        = 0x90,	//1001xxxx	0x90 - 0x9f
	fixstr          = 0xa0,	//101xxxxx	0xa0 - 0xbf
	others		= 0xc0,	//110XXXXX	0xc0
	negative_fixint = 0xe0	//111xxxxx	0xe0 - 0xff
};

	
/**
 * @brief Предок для классов упаковщика/распаковщика.
 * Содержит доступные только наследникам данные
 */
namespace MessagePack {
	/// Двунаправленный map для хранения типов пакета в связке с заголовками
	typedef boost::bimap< 
		boost::bimaps::set_of< internal::type >,
		boost::bimaps::set_of< header >
	> bm_type;
	
	/**
	 * @brief Определение типа пакета.
	 * @param head	Заголовок пакета
	 * @return Возвращает тип пакета в int'овом представлении либо -1 в случае ошибки
	 */
	int getPkgType(const header& head);
	
	/**
	 * @brief Определение типа пакета
	 * Если не возможно определить тип, выбрасывает исключение
	 * @param head	Заголовок пакета
	 * @return Возвращает тип
	 */
	inline type packageType(const header& head) {
		int type = getPkgType(head);
		if (type == -1) { throw std::runtime_error("Неизвестный тип пакета"); }
	
		return static_cast<internal::type>(type);
	}
	
	package_data getHead(internal::type type);
	
	/// Двунаправленный map, связывающий тип пакета с заголовком
	extern const bm_type	m_bimap; 
}


} //namespace internal


} // namespace MessagePack


#endif /* MSGPACK_H */


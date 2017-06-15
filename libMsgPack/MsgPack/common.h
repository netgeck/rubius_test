/* 
 * File:   common.h
 * Author: kostya
 *
 * Created on 7 апреля 2016 г., 15:06
 */

#ifndef __MSGPACK_COMMON_H__
#define __MSGPACK_COMMON_H__


#include <string>

#include "MsgPack_types.h"


namespace MsgPack {


/**
 * @brief Определить является ли пакет корректным пакетом формата MessagePack
 * Функция не выбрасывает исключений
 * @param pkg	пакет проверяемый на соответствие формату MessagePack
 * @return
 */
bool isPgkCorrect(const package& pkg) noexcept;

/**
 * @brief Определение типа пакета
 * @param pkg	пакет MessagePack
 * @return возвращает тип
 */
type packageType(const package& pkg);

/**
 * @brief Имя типа пакета
 * @param tp	тип пакета
 * @return возвращает имя в виде строки
 */
std::string type_name(type tp);


} // namespace MessagePack


#endif /* __MSGPACK_COMMON_H__ */


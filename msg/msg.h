/* 
 * File:   msg.h
 * Author: kostya
 *
 * Created on 24 июня 2017 г., 19:03
 */

#ifndef __STREAM_PKG__H__
#define __STREAM_PKG__H__

#include <stdint.h>
#include <vector>
#include <stddef.h> // size_t
#include <string.h> // memcpy
#include <stdexcept>
#include <gtest/gtest_prod.h>

#include "gtest_prod_util.h"


#define ERROR_PKG_IS_NOT_FULL	"пакет не заполнен"
#define ERROR_OUT_OF_RANGE	"Данных больше чем указано в заголовке пакета"


FORWARD_DECLARE_TEST(package, request_pack_unpack);
FORWARD_DECLARE_TEST(package, answer_pack_unpack);


/**
 * Классы и функции для работы с сообщением.
 * Содержимое пакета зависит от используемой функции заполненеия. 
 * Работа с сообщением только на устройствах с одним порядком байт.
 */
namespace msg {

/**
 * @brief Класс - универсальный пакет.
 * Пакет содержит свой полный размер в заголовке.
 * Остальное содержимое не специфицировано и зависит от используемых методов.
 */
class package {
public:
	typedef uint32_t header; //!< тип заголовка пакета
	
	/**
	 * @brief Добавить данные в конец пакета
	 * @param chunk	кусок данных
	 */
	void pushBack(std::vector<char> chunk) {
		m_buffer.insert(m_buffer.end(), chunk.begin(), chunk.end());
		
		if(m_buffer.size() > sizeof(header) && 
			m_buffer.size() > *reinterpret_cast<const header*>(&m_buffer[0])) {
			throw std::out_of_range(ERROR_OUT_OF_RANGE);
		}
	};
	
	/**
	 * @brief Добавить данные в конец пакета
	 * @param src	указатель на данные
	 * @param size	длина данных
	 */
	void pushBack(const char* src, size_t size) {
		m_buffer.insert(m_buffer.end(), src, src + size);
		
		if(m_buffer.size() > sizeof(header) 
			&& m_buffer.size() > *reinterpret_cast<const header*>(&m_buffer[0])) {
			throw std::out_of_range(ERROR_OUT_OF_RANGE);
		}
	}
	
	/**
	 * @brief Добавить данные в конец пакета
	 * @param src_begin	указатель на начало данных
	 * @param src_end	указатель на конец данных
	 */
	void pushBack(const char* src_begin, const char* src_end) {
		m_buffer.insert(m_buffer.end(), src_begin, src_end);
		
		if(m_buffer.size() > sizeof(header) && 
			m_buffer.size() > *reinterpret_cast<const header*>(&m_buffer[0])) {
			throw std::out_of_range(ERROR_OUT_OF_RANGE);
		}
	}
	
	/**
	 * @brief заполненность пакета
	 * Пакет заполнен если размер записанный в заголовок соответствует реальному размеру.
	 * @return true - если пакет заполнен; false - нет
	 */
	bool isFull() const {
		if (m_buffer.empty() || m_buffer.size() < sizeof(header)) {
			return false;
		}
		
		if(m_buffer.size() == *reinterpret_cast<const header*>(&m_buffer[0])) {
			return true;
		}
	};
	
	/**
	 * @brief очистить содержимое пакета
	 */
	void clear() {
		m_buffer.clear();
	}
	
	/// итератор на первый элемент пакета
	std::vector<char>::const_iterator begin() const { return m_buffer.begin(); };
	/// итератор на конец пакета (элемент за последним)
	std::vector<char>::const_iterator end() const { return m_buffer.end(); };
	size_t size() const { return m_buffer.size(); };
private:
	// дружественные тесты:
	FRIEND_TEST(::package, request_pack_unpack);
	FRIEND_TEST(::package, answer_pack_unpack);
	
	std::vector<char> m_buffer; //!< данные пакета
};

/// работа с пакетом сообщения как с запросом
namespace request {
	
typedef uint8_t wordSize; //!< тип под-заголовка(хранящего размер слова) в запросном пакете 

/**
 * @brief Заполнить универсальный пакет данными для запроса
 * @param pkg	ссылка на пакет
 * @param word_begin	начало слова
 * @param word_end	конец слова
 * @param file_begin	начало файла
 * @param file_end	конец файла
 */
static void packageFill(package& pkg,
	std::string::const_iterator word_begin, std::string::const_iterator word_end,
	std::vector<char>::const_iterator file_begin, std::vector<char>::const_iterator file_end) {
	
	const request::wordSize wordSize = word_end - word_begin;
	const auto fileSize = file_end - file_begin;
	
	const package::header dataSize = 
		sizeof(dataSize) + sizeof(wordSize) +	// заголовки
		wordSize + fileSize;			// + данные
	
	pkg.pushBack(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize)); // размер пакета (заголовок)
	pkg.pushBack(reinterpret_cast<const char*>(&wordSize), sizeof(wordSize)); // размер слова
	pkg.pushBack(&(*word_begin), wordSize);
	pkg.pushBack(&(*file_begin), fileSize);
}

/**
 * @brief Указатель на начало слова в пакет
 * @param pkg	ссылка на пакет
 * @return 
 */
static const char* word_begin(const package& pkg) {
	if (!pkg.isFull()) {
		throw std::runtime_error(ERROR_PKG_IS_NOT_FULL);
	}
	
	return &(*(pkg.begin() + sizeof(package::header) + sizeof(request::wordSize)));
}

/**
 * @brief указатель на конец слова в пакете
 * @param pkg	ссылка на пакет
 * @return 
 */
static const char* word_end(const package& pkg) {
	if (!pkg.isFull()) {
		throw std::runtime_error(ERROR_PKG_IS_NOT_FULL);
	}
	
	const request::wordSize wordSize = *(pkg.begin() + sizeof(package::header));
	
	return word_begin(pkg) + wordSize;
}

/**
 * @brief Указатель на начало файла в пакете
 * @param pkg	ссылка на пакет
 * @return 
 */
static const char* file_begin(const package& pkg) {
	if (!pkg.isFull()) {
		throw std::runtime_error(ERROR_PKG_IS_NOT_FULL);
	}
	
	return word_end(pkg);
}

/**
 * @brief указатель на конец файла в пакете
 * @param pkg	ссылка на пакет
 * @return 
 */
static const char* file_end(const package& pkg) {
	if (!pkg.isFull()) {
		throw std::runtime_error(ERROR_PKG_IS_NOT_FULL);
	}
	
	const package::header fileSize =		// общая длина пакета без заголовков
		pkg.size() - sizeof(package::header) - sizeof(request::wordSize) 
		- (word_end(pkg) - word_begin(pkg));	// и без слова
	
	return file_begin(pkg) + fileSize;
}

} // namespace request


/// работа с пакетом сообщения как с ответом
namespace answer {

typedef int32_t value;	//!< тип значения в ответном пакете.

static const value errCode = -1; //!< Код ошибки

/**
 * @brief Заполнить универсальный пакет значением ответа
 * @param pkg	ссылка на пакет
 * @param num	значение ответа
 */
static void packageFill(package& pkg, value num) {
	const package::header dataSize = sizeof(dataSize) + sizeof(num);
	pkg.pushBack(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));
	
	pkg.pushBack(reinterpret_cast<char*>(&num), sizeof(num));
}

/**
 * @brief Получить значение из ответного пакета
 * @param pkg	ссылка на пакет
 * @return 
 */
static value getNum(const package& pkg) {
	if (!pkg.isFull()) {
		throw std::runtime_error(ERROR_PKG_IS_NOT_FULL);
	}
	
	return *reinterpret_cast<const value*>(&(*(pkg.begin() + sizeof(package::header))));
}

} // namespace answer

} // namespace package

#endif /* __STREAM_PKG__H__ */


/* 
 * File:   streamPkg.h
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


FORWARD_DECLARE_TEST(package, request_pack_unpack);
FORWARD_DECLARE_TEST(package, answer_pack_unpack);


namespace msg {
	
class package {
public:
	typedef uint32_t header;
	
	void pushBack(std::vector<char> chunk) {
		m_buffer.insert(m_buffer.end(), chunk.begin(), chunk.end());
	};
	
	void pushBack(const char* src, size_t size) {
		m_buffer.insert(m_buffer.end(), src, src + size);
	}
	
	void pushBack(const char* src_begin, const char* src_end) {
		m_buffer.insert(m_buffer.end(), src_begin, src_end);
	}
	
	bool isFull() const {
		if (m_buffer.empty() || m_buffer.size() < sizeof(header)) {
			return false;
		}
		
		header fullSize;
		memcpy(&fullSize, m_buffer.data(), sizeof(header));
		
		if(m_buffer.size() == fullSize) {
			return true;
		}
	};
	
	void clear() {
		m_buffer.clear();
	}
	
	std::vector<char>::const_iterator begin() const { return m_buffer.begin(); };
	std::vector<char>::const_iterator end() const { return m_buffer.end(); };
	size_t size() const { return m_buffer.size(); };
private:
	FRIEND_TEST(::package, request_pack_unpack);
	FRIEND_TEST(::package, answer_pack_unpack);
	
	std::vector<char> m_buffer;
};

namespace request {
	
typedef uint8_t wordSize;

static void packageFill(package& pkg,
	std::string::const_iterator word_begin, std::string::const_iterator word_end,
	std::vector<char>::const_iterator file_begin, std::vector<char>::const_iterator file_end) {
	
	const request::wordSize wordSize = word_end - word_begin;
	const auto fileSize = file_end - file_begin;
	
	const package::header dataSize = 
		sizeof(dataSize) + sizeof(wordSize) +	// заголовки
		wordSize + fileSize;			// + данные
	
	pkg.pushBack(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));
	pkg.pushBack(reinterpret_cast<const char*>(&wordSize), sizeof(wordSize));
	pkg.pushBack(&(*word_begin), wordSize);
	pkg.pushBack(&(*file_begin), fileSize);
}

static const char* word_begin(const package& pkg) {
	if (!pkg.isFull()) {
		throw std::runtime_error("пакет не заполнен");
	}
	
	return &(*(pkg.begin() + sizeof(package::header) + sizeof(request::wordSize)));
}

static const char* word_end(const package& pkg) {
	if (!pkg.isFull()) {
		throw std::runtime_error("пакет не заполнен");
	}
	
	const request::wordSize wordSize = *(pkg.begin() + sizeof(package::header));
	
	return word_begin(pkg) + wordSize;
}

static const char* file_begin(const package& pkg) {
	if (!pkg.isFull()) {
		throw std::runtime_error("пакет не заполнен");
	}
	
	return word_end(pkg);
}

static const char* file_end(const package& pkg) {
	if (!pkg.isFull()) {
		throw std::runtime_error("пакет не заполнен");
	}
	
	const package::header fileSize =		// общая длина пакета без заголовков
		pkg.size() - sizeof(package::header) - sizeof(request::wordSize) 
		- (word_end(pkg) - word_begin(pkg));	// и без слова
	
	return file_begin(pkg) + fileSize;
}

} // namespace request


/// работа с пакетом в ответном режиме
namespace answer {
	
typedef uint32_t value;
	
static void packageFill(package& pkg, value num) {
	const package::header dataSize = sizeof(dataSize) + sizeof(num);
	pkg.pushBack(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));
	
	pkg.pushBack(reinterpret_cast<char*>(&num), sizeof(num));
}

static value getNum(const package& pkg) {
	if (!pkg.isFull()) {
		throw std::runtime_error("пакет не заполнен");
	}
	
	return *reinterpret_cast<const value*>(&(*(pkg.begin() + sizeof(package::header))));
}

} // namespace answer

} // namespace package

#endif /* __STREAM_PKG__H__ */


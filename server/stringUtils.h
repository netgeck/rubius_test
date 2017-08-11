/* 
 * File:   stringUtils.h
 * Author: kostya
 *
 * Created on 25 июня 2017 г., 18:53
 */

#ifndef __STRING_CUTTING__H__
#define __STRING_CUTTING__H__

#include <string>
#include <algorithm>
#include <map>
#include <cwctype>
#include <locale>
#include <codecvt>
#ifndef NDEBUG
	#include <iostream>
#endif

typedef std::map<std::string, size_t> word_count;

/**
 * @brief Конвертировать UTF-8 строку в wstring
 * @param first указатель на первый символ строки UTF-8
 * @param last указатель на последний символ строки UTF-8
 * @return wstring
 */
inline std::wstring utf8_to_wstring(const char* first, const char* last) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.from_bytes(first, last);
}

/**
 * @brief Конвертировать wstring в UTF-8 строку
 * @param first указатель на первый символ строки wstring
 * @param last указатель на последний символ строки wstring
 * @return строка UTF-8
 */
inline std::string wstring_to_utf8(const wchar_t* first, const wchar_t* last) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.to_bytes(first, last);
}

/**
 * @brief Проверка символа на допустимость использования в слове
 * @param c	"широкий" символ
 * @return true - корректный символ; false - нет.
 */
static bool isWordChar(wchar_t c) {
	std::locale::global(std::locale("ru_RU.UTF-8"));
	return std::iswalpha(c) || std::iswdigit(c);
}

/**
 * @brief Функция нарезки строки на отдельные слова.
 * Словом считается последовательность букв и/или цифр.
 * Результат записывается в ассоциативный контейнер <слово, частота>.
 * @param words	ссылка на контейнер слов для записи результата
 * @param begin итератор начала строки
 * @param end итератор конца строки
 */
static void cutter(word_count& words, std::wstring::iterator begin, std::wstring::iterator end) {
	auto it_correct = std::find_if(begin, end, isWordChar);
	if (it_correct == end) {
		return;
	}
	
	auto it_incorrect = std::find_if_not(it_correct, end, isWordChar);
	// добавляем слово и увеличиваем счётчик
	words[wstring_to_utf8(&(*it_correct), &(*it_incorrect))]++;
#ifndef NDEBUG
	auto it_last = words.find(wstring_to_utf8(&(*it_correct), &(*it_incorrect)));
	if (it_last != words.end()) {
		std::cout << "добавлено \"" << it_last->first << "\": " << it_last->second << std::endl;
	}
#endif
	
	if (it_incorrect == end) {
		return;
	}
	
	cutter(words, it_incorrect, end);
}


#endif /* __STRING_CUTTING__H__ */


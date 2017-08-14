/* 
 * File:   stringUtils.h
 * Author: kostya
 *
 * Created on 25 июня 2017 г., 18:53
 */

#ifndef __STRING_CUTTING__H__
#define __STRING_CUTTING__H__

#include <QChar>

#include <algorithm>
#include <map>


typedef std::map<QString, size_t> word_count;


/**
 * @brief Проверка символа на допустимость использования в слове
 * @param c	- символ
 * @return true - корректный символ; false - нет.
 */
static bool isWordChar(QChar c) {
	return c.isLetter() || c.isDigit();
}

/**
 * @brief Функция нарезки строки на отдельные слова.
 * Словом считается последовательность букв и/или цифр.
 * Результат записывается в ассоциативный контейнер <слово, частота>.
 * @param words	ссылка на контейнер слов для записи результата
 * @param begin итератор начала строки
 * @param end итератор конца строки
 */
static void cutter(word_count& words, QString::iterator begin, QString::iterator end) {
	auto it_correct = std::find_if(begin, end, isWordChar);
	if (it_correct == end) {
		return;
	}
	
	auto it_incorrect = std::find_if_not(it_correct, end, isWordChar);
	// добавляем слово и увеличиваем счётчик
	words[QString(it_correct, it_incorrect - it_correct)]++;
#ifndef NDEBUG
	auto it_last = words.find(QString(it_correct, it_incorrect - it_correct));
	if (it_last != words.end()) {
		qDebug() << "добавлено \"" << it_last->first << "\": " << it_last->second;
	}
#endif
	
	if (it_incorrect == end) {
		return;
	}
	
	cutter(words, it_incorrect, end);
}


#endif /* __STRING_CUTTING__H__ */


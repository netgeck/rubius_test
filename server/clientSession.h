/* 
 * File:   clientSession.h
 * Author: kostya
 *
 * Created on 25 июня 2017 г., 22:44
 */

#ifndef __CLIENT_SESSION__H__
#define __CLIENT_SESSION__H__

#include <QObject>
#include <QTcpSocket>
#include <syslog.h>

#include <msg.h>

#include "stringUtils.h"


#define READBUFFER_SIZE	512


/**
 * @brief Класс сессия.
 * Слушает входящие пакеты, обрабатывает их и отправляет результат клиенту.
 */
class clientSession : public QObject {
	Q_OBJECT
public:
	clientSession(QTcpSocket *socket, QObject *parent) : QObject(parent), 
	m_pSock(socket), 
	m_recvPkg() {
		connect(m_pSock, SIGNAL(readyRead()), this, SLOT(readPkg()));
	};
	
public slots:
	void readPkg() {
		std::vector<char> buffer(READBUFFER_SIZE);
	
		size_t avail = m_pSock->bytesAvailable();
		int byteRead = m_pSock->read(buffer.data(), std::min(buffer.size(), avail));
	
		m_recvPkg.pushBack(&(*buffer.begin()), byteRead);
		
		if (m_recvPkg.isFull()) {
			handlePkg();
			m_recvPkg.clear();
		} else {
#ifndef NDEBUG
			std::cout << "Принятый пакет не корректен. Размер пакета: " 
				<< m_recvPkg.size() << "байт. Ждём продолжения" << std::endl;
#endif
			readPkg();
		}
	}
private:
	QTcpSocket* m_pSock;
	msg::package m_recvPkg;
	
	void handlePkg() {
		uint32_t res(0);
		std::string word(msg::request::word_begin(m_recvPkg), msg::request::word_end(m_recvPkg));

		std::vector<char> mappedFile(msg::request::file_begin(m_recvPkg), msg::request::file_end(m_recvPkg));
		
		// Конвертируем полученный файл в wstring
//		std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> ucs2conv;
		std::wstring file = utf8_to_wstring(&mappedFile.front(), &mappedFile.back());
		
		// Нарезаем отдельные слова
		word_count words;
		cutter(words, file.begin(), file.end());

		res = words.count(word) ? words[word] : 0;
		answer(res);
	}
	
	void answer(uint32_t result) {
		msg::package resPkg;
		msg::answer::packageFill(resPkg, result);
		
		m_pSock->write(&(*resPkg.begin()), resPkg.size());
		syslog(LOG_INFO, "Передан ответ");
	}
};



#endif /* __CLIENT_SESSION__H__ */


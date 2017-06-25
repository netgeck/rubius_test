/* 
 * File:   clientSession.cxx
 * Author: kostya
 * 
 * Created on 25 июня 2017 г., 22:44
 */

#include <syslog.h>

#include "stringUtils.h"

#include "clientSession.h"


clientSession::clientSession(QTcpSocket *socket, QObject *parent) : QObject(parent), 
m_pSock(socket), 
m_recvPkg() {
	connect(m_pSock, SIGNAL(readyRead()), this, SLOT(readPkg()));
}
	
void clientSession::readPkg() {
	std::vector<char> buffer(READBUFFER_SIZE);

	size_t avail = m_pSock->bytesAvailable();
	int byteRead = m_pSock->read(buffer.data(), std::min(buffer.size(), avail));

	try {
		m_recvPkg.pushBack(&(*buffer.begin()), byteRead);
	} catch(std::exception& e) {
		syslog(LOG_ERR, "Ошибка получения пакета: %s", e.what());
		answer(msg::answer::errCode); // возвращаем ошибку клиенту
		m_recvPkg.clear();
		return;
	}

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
	
void clientSession::handlePkg() {
	msg::answer::value res(0);
	std::string word(msg::request::word_begin(m_recvPkg), msg::request::word_end(m_recvPkg));

	std::vector<char> mappedFile(msg::request::file_begin(m_recvPkg), msg::request::file_end(m_recvPkg));

	// Конвертируем полученный файл в wstring
	std::wstring file = utf8_to_wstring(&mappedFile.front(), &mappedFile.back());

	// Нарезаем отдельные слова
	word_count words;
	cutter(words, file.begin(), file.end());

	res = words.count(word) ? words[word] : 0;
	answer(res);
}
	
void clientSession::answer(msg::answer::value result) {
	msg::package resPkg;
	msg::answer::packageFill(resPkg, result);

	m_pSock->write(&(*resPkg.begin()), resPkg.size());
	syslog(LOG_INFO, "Передан ответ");
}


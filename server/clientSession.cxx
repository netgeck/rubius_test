/* 
 * File:   clientSession.cxx
 * Author: kostya
 * 
 * Created on 25 июня 2017 г., 22:44
 */

#include <QtDebug>

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
	if (!avail) {
		return;
	}
	
	int byteRead = m_pSock->read(buffer.data(), std::min(buffer.size(), avail));
	
	m_recvPkg.append(buffer.data(), byteRead);
	
	static qint32 pkgSize(0);
	if (pkgSize == 0 && m_recvPkg.size() >= sizeof(qint32)) {
		QDataStream in(m_recvPkg);
		in >> pkgSize;
	}
	
	if (m_recvPkg.size() == pkgSize) {
		handlePkg();
		m_recvPkg.clear();
		pkgSize = 0;
	} else {
#ifndef NDEBUG
		std::cout << "Принятый пакет не корректен. Размер пакета: " 
			<< m_recvPkg.size() << "байт. Ждём продолжения" << std::endl;
#endif
		readPkg();
	}
}

void clientSession::handlePkg() {
	QDataStream in(m_recvPkg);
	QString qsWord, qsFile;
	in.skipRawData(sizeof(qint32));
	in >> qsFile >> qsWord;
	
	qint32 res(0);
	std::string word = qsWord.toStdString();

	std::string mappedFile = qsFile.toStdString();

	// Конвертируем полученный файл в wstring
	std::wstring file = utf8_to_wstring(&mappedFile.front(), &mappedFile.back());

	// Нарезаем отдельные слова
	word_count words;
	cutter(words, file.begin(), file.end());

	res = words.count(word) ? words[word] : 0;
	answer(res);
}
	
void clientSession::answer(qint32 result) {
	QDataStream out(m_pSock);
	out << result;
	qDebug() << "Передан ответ";
}


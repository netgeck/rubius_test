/* 
 * File:   clientSession.cxx
 * Author: kostya
 * 
 * Created on 25 июня 2017 г., 22:44
 */

#include <QtDebug>

#include <common.h>

#include "stringUtils.h"

#include "clientSession.h"


clientSession::clientSession(QTcpSocket *socket, QObject *parent) : QObject(parent), 
m_pSock(socket), 
m_recvPkg() {
	connect(m_pSock, SIGNAL(readyRead()), this, SLOT(readPkg()));
	connect(m_pSock, SIGNAL(error(QAbstractSocket::SocketError)), this, 
		SLOT(socketError(QAbstractSocket::SocketError)));
}

void clientSession::readPkg() {
	m_recvPkg.append(m_pSock->readAll());
	
	static qint32 pkgSize(0);
	if (pkgSize == 0 && m_recvPkg.size() >= sizeof(qint32)) {
		QDataStream in(m_recvPkg);
		in >> pkgSize;
		qDebug() << "Приём пакета размером " <<  pkgSize << "...";
	}
	
	if (m_recvPkg.size() == pkgSize) {
		qDebug() << "Пакет принят";
		handlePkg();
		m_recvPkg.clear();
		pkgSize = 0;
	} else if (m_recvPkg.size() > pkgSize) {
		qWarning() << "Размер пакета превысил ожидания! " << m_recvPkg.size() << " из " << pkgSize;
		answer(RET_ERROR);
		m_recvPkg.clear();
	}
}

void clientSession::socketError(QAbstractSocket::SocketError /*error*/) {
	qWarning() << "Ошибка соединения: " << m_pSock->errorString();
}

void clientSession::handlePkg() {
	QDataStream in(m_recvPkg);
	QString word, file;
	in.skipRawData(sizeof(qint32)); // пропускаем заголовок
	in >> file >> word;
	
	// Нарезаем отдельные слова
	word_count words;
	cutter(words, file.begin(), file.end());

	qint32 res = words.count(word) ? words[word] : 0;
	answer(res);
}
	
void clientSession::answer(qint32 result) {
	QDataStream out(m_pSock);
	out << result;
	qDebug() << "Передан ответ";
}


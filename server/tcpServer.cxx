/* 
 * File:   tcpServer.cxx
 * Author: kostya
 * 
 * Created on 25 июня 2017 г., 17:54
 */

#include <QtDebug>

#include "clientSession.h"
#include "tcpServer.h"


tcpServer::tcpServer(uint16_t port, QObject *parent) : QObject(parent) {
	m_server = new QTcpServer(this);

	connect(m_server, SIGNAL(newConnection()), this, SLOT(accept()));

	if (!m_server->listen(QHostAddress::Any, port)) {
		qDebug() << "Не удалось запустить сервер";
	}
}

void tcpServer::accept() {
	qDebug() << "Принято соединение";
	m_sessions.emplace_back(new clientSession(m_server->nextPendingConnection(), this));
}


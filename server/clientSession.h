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

#include <msg.h>


#define READBUFFER_SIZE	512


/**
 * @brief Класс сессия.
 * Слушает входящие пакеты, обрабатывает их и отправляет результат клиенту.
 */
class clientSession : public QObject {
	Q_OBJECT
	
public:
	clientSession(QTcpSocket *socket, QObject *parent);
	
public slots:
	void readPkg();
	
private:
	void handlePkg();
	
	void answer(uint32_t result);
	
	QTcpSocket* m_pSock;
	msg::package m_recvPkg;
};


#endif /* __CLIENT_SESSION__H__ */


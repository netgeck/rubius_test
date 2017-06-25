/* 
 * File:   server.h
 * Author: kostya
 *
 * Created on 25 июня 2017 г., 17:54
 */

#ifndef __SERVER__H__
#define __SERVER__H__

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDebug>

#include <memory>
#include <vector>

class clientSession;

class tcpServer : public QObject {
	Q_OBJECT
public:
	explicit tcpServer(uint16_t port, QObject *parent = 0);

signals:

public slots:
	void accept();

private:
	QTcpServer *m_server;
	std::vector<std::shared_ptr<clientSession>> m_sessions;
};

#endif /* __SERVER__H__ */


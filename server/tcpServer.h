/* 
 * File:   server.h
 * Author: kostya
 *
 * Created on 25 июня 2017 г., 17:54
 */

#ifndef __TCP_SERVER__H__
#define __TCP_SERVER__H__

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDebug>

#include <memory>
#include <vector>

class clientSession;

/**
 * @brief Класс сервер.
 * Принимает соединения. Запускает объект-сессию для каждого нового соединения.
 */
class tcpServer : public QObject {
	Q_OBJECT
public:
	/**
	 * @brief Конструктор
	 * @param port	номер порта
	 * @param parent родительский объект
	 */
	explicit tcpServer(uint16_t port, QObject *parent = 0);

public slots:
	/// Приём соединения
	void accept();

private:
	QTcpServer *m_server; //!< tcp-сервер
	std::vector<std::shared_ptr<clientSession>> m_sessions; //!< список запущенных соединени
};

#endif /* __TCP_SERVER__H__ */


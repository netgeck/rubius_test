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


/**
 * @brief Класс сессия.
 * Слушает входящие пакеты, обрабатывает их и отправляет результат клиенту.
 */
class clientSession : public QObject {
	Q_OBJECT
	
public:
	/**
	 * @biref Конструктор
	 * @param socket	сокет
	 * @param parent	родительский объект
	 */
	clientSession(QTcpSocket *socket, QObject *parent);
	
public slots:
	/// Чтение пакета из сокета
	void readPkg();
	
	/**
	 * @brief обработка ошибок
	 * @param socketError
	 */
	void socketError(QAbstractSocket::SocketError error);
	
private:
	/// Обработка полученного пакета
	void handlePkg();
	
	/**
	 * @brief Отправка ответного пакета клиенту
	 * @param result результат
	 */
	void answer(qint32 result);
	
	QTcpSocket* m_pSock;	//!< сокет для связи с клиентом
	QByteArray m_recvPkg;	//!< принимаемый пакет
};


#endif /* __CLIENT_SESSION__H__ */


/* 
 * File:   tcpConnect.h
 * Author: kostya
 *
 * Created on 11 июня 2017 г., 2:13
 */

#ifndef __TCP_CONNECT__H__
#define __TCP_CONNECT__H__

#include <stdint.h>
#include <boost/core/noncopyable.hpp>
#include <memory>

typedef std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;

/**
 * @brief Класс для работы с tcp соединением
 * Используется синхронное api от boost::asio
 */
class tcpConnect : public boost::noncopyable{
public:
	tcpConnect(uint16_t port);
	
	void accept();
	
	socket_ptr getSock();
private:
	boost::asio::io_service m_service;
	std::unique_ptr<boost::asio::ip::tcp::acceptor> m_pAcc;
	socket_ptr m_sock;
};

#endif /* __TCP_CONNECT__H__ */


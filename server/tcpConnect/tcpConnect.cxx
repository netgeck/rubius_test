/* 
 * File:   tcpConnect.cxx
 * Author: kostya
 * 
 * Created on 11 июня 2017 г., 2:13
 */

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "tcpConnect.h"

namespace basio = boost::asio;

tcpConnect::tcpConnect(uint16_t port) :
m_service() {
	basio::ip::tcp::endpoint ep(basio::ip::tcp::v4(), port); // прослушиваемый порт
	m_pAcc.reset(new basio::ip::tcp::acceptor(m_service, ep)); // объект для приема клиентских подключений	
}

void tcpConnect::accept() {
	m_sock.reset(new basio::ip::tcp::socket(m_service));
	m_pAcc->accept(*m_sock);
}

socket_ptr tcpConnect::getSock() {
	return m_sock;
}

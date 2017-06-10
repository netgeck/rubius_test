/* 
 * File:   main.cxx
 * Author: kostya
 *
 * Created on 9 июня 2017 г., 21:19
 */

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <thread>
#include <memory>
#include <iostream>
#include <syslog.h>
#include <string>

#include <defPort.h>
#include <tcpConnect.h>

using namespace std;

namespace basio = boost::asio;

void client_session(socket_ptr sock) {
	while (true) {
		char data[512];
		size_t len = sock->read_some(basio::buffer(data));
		if (len > 0)
			write(*sock, basio::buffer("ok", 2));
	}
}

void serverLoop(uint16_t port) {
	tcpConnect tcp(port);
	
	while (true) {
		tcp.accept();
		std::thread(std::bind(client_session, tcp.getSock()));
	}
}

/*
 * @brief точка входа в программу клиента
 * Будет использоваться синхронное API, следовательно многопоточная архитектура.
 */
int main(/*int argc, char** argv*/) {
	syslog(LOG_NOTICE, "старт сервера");
	
	try {
		serverLoop(PORT_DEFAULT);
	} catch (boost::system::system_error e) {
		std::cout << e.code() << std::endl;
		syslog(LOG_ERR, "Ошибка: %s", e.code().message().c_str());
	}

	return 0;
}


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

/*
 * @brief точка входа в программу клиента
 * Будет использоваться синхронное API, следовательно многопоточная архитектура.
 */
int main(int argc, char** argv) {
	uint16_t port(0);
	if (argc < 2) {
		port = PORT_DEFAULT;
		std::cout << "Порт не задан будет использован порт по умолчанию "
			<< "(" << PORT_DEFAULT << ")" << std::endl;
	} else if (argc == 2) {
		port = std::atoi(argv[1]);
	}
	
	syslog(LOG_NOTICE, "Cтарт сервера. Port: %d", port);
	
	try {
		tcpConnect tcp(port);
		while (true) {
			tcp.accept();
			std::thread(std::bind(client_session, tcp.getSock()));
		}
	} catch (boost::system::system_error e) {
		std::cout << e.code() << std::endl;
		syslog(LOG_ERR, "Ошибка: %s", e.code().message().c_str());
	}

	return 0;
}


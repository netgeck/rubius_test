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
#include <csignal>
#include <functional>

#include <defPort.h>

using namespace std;

namespace basio = boost::asio;
using namespace boost::asio;
typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;
typedef std::shared_ptr<ip::tcp::acceptor> acceptor_ptr;
acceptor_ptr gAcc;

void signalHandler(int signum) {
	cout << "Interrupt signal (" << signum << ") received.\n";

	switch (signum) {
	case SIGINT:
	case SIGTERM:
//		gSock->cancel();
//		gSock->close();
		break;
	}

	// cleanup and close up stuff here  
	// terminate program  

	exit(signum);
}

char data[512];
void read_handler(const boost::system::error_code& error, std::size_t bytes_transferred) {
	std::string str(data, bytes_transferred);
	std::cout << "Получено: \"" << str << "\"" << std::endl;
}

void start_accept(socket_ptr sock);

void handle_accept(socket_ptr sock, const boost::system::error_code & err) {
	if (err) return;
	syslog(LOG_INFO, "Принято соединение");
	
	sock->async_read_some(basio::buffer(data), read_handler);
//	socket_ptr sock(new ip::tcp::socket(service));
}

void start_accept(socket_ptr sock) {
	using namespace std::placeholders;
	gAcc->async_accept(*sock, std::bind(handle_accept, sock, _1));
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
	} else if (argc > 2) {
		std::cout << "Использование: " << PROGNAME << " <port>" << std::endl;
		return 1;
	}
	
	syslog(LOG_NOTICE, "Cтарт сервера. Port: %d", port);
	
	// Регистрация обработчика сигналов
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	
	try {
		io_service service;
		ip::tcp::endpoint ep(ip::tcp::v4(), port); 
		gAcc.reset(new ip::tcp::acceptor(service, ep));
		socket_ptr sock(new ip::tcp::socket(service));
		start_accept(sock);
		service.run();
	} catch (boost::system::system_error e) {
		std::cerr << "Ошибка: " << e.code() << ": " << e.code().message() << std::endl;
		syslog(LOG_ERR, "Ошибка: %s", e.code().message().c_str());
	} catch (std::exception& e) {
		std::cerr << "Ошибка: " << e.what() << std::endl;
		syslog(LOG_ERR, "Ошибка: %s", e.what());
	} catch (...) {
		std::cerr << "Не обработанный тип исключения" << std::endl;
	}
		
	return 0;
}


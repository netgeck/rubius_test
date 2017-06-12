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
typedef std::shared_ptr<ip::tcp::socket> socket_ptr;
typedef std::shared_ptr<ip::tcp::acceptor> acceptor_ptr;
socket_ptr gSock;

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

void handler_write(const boost::system::error_code& error, std::size_t bytes_transferred){
	if (error) return;
	std::cout << "Передан фейковый ответ" << std::endl;
}

char data[512];
void read_handler(const boost::system::error_code& error, std::size_t bytes_transferred) {
	if (error) return;
	std::string str(data, bytes_transferred);
	std::cout << "Получено: \"" << str << "\"" << std::endl;
	
	uint32_t fakeResult = 3;
	gSock->async_write_some(buffer(&fakeResult, sizeof(fakeResult)), handler_write);
}

void handle_accept(socket_ptr sock, const boost::system::error_code & err) {
	if (err) return;
	syslog(LOG_INFO, "Принято соединение");
	
	gSock = sock;
	sock->async_read_some(basio::buffer(data), read_handler);
}

class server {
public:
	server(boost::asio::io_service& io_service,
		const ip::tcp::endpoint& endpoint)
	: m_acceptor(io_service) {
		m_acceptor.open(endpoint.protocol());
		m_acceptor.set_option(ip::tcp::acceptor::reuse_address(true));
		m_acceptor.bind(endpoint);
		m_acceptor.listen();
		
		m_pSock = std::make_shared<ip::tcp::socket>(io_service);
		
		accept();
	}

private:
	void accept() {
		m_acceptor.async_accept(*m_pSock,
			[this](boost::system::error_code err) {
				if (!err) {
					//std::make_shared<chat_session>(std::move(socket_), room_)->start();
					handle_accept(m_pSock, err);
				}

				accept();
			});
	}

	ip::tcp::acceptor m_acceptor;
	socket_ptr m_pSock;
};

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
		boost::asio::io_service io_service;
		ip::tcp::endpoint ep(ip::tcp::v4(), port); 
		server serv(io_service, ep);
		io_service.run();
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


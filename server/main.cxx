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
#include <algorithm>

#include <defPort.h>
#include <MsgPack_types.h>
#include <common.h>
#include <MsgPack_pack.h>
#include <MsgPack_unpack.h>

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
MsgPack::package pkg;

bool isValid (char c) {
	static std::locale loc;
	return std::isalpha(c, loc) || std::isdigit(c, loc);
}

void cutter(std::map<std::string, size_t> &words, std::string::iterator begin, std::string::iterator end) {
	auto it_correct = std::find_if(begin, end, isValid);
	if (it_correct == end) {
		return;
	}
	
	auto it_incorrect = std::find_if_not(it_correct, end, isValid);
	words[std::string(it_correct, it_incorrect)]++; // добавляем слово и увеличиваем счётчик
//	std::string lastAdded(it_correct, it_incorrect);
//	std::cout << "Добавлено слово \"" << lastAdded << "\"  счетчик: " << words.at(lastAdded) << std::endl;
	
	if (it_incorrect == end) {
		return;
	}
	
	cutter(words, it_incorrect, end);
}

void read_handler(const boost::system::error_code& error, std::size_t bytes_transferred) {
	if (error) return;
	
	uint32_t res(0);
	pkg.insert(pkg.end(), data, data + bytes_transferred);
	
	if (MsgPack::isPgkCorrect(pkg)) {
		std::cout << "Пакет получен" << std::endl;
		MsgPack::map_description mpd = MsgPack::unpack::map(pkg);
		auto wordPkg = mpd.at(MsgPack::pack::str("word"));
		std::string word = MsgPack::unpack::str(wordPkg);
		std::cout << "Получено слово: \"" << word << "\"" << std::endl;
		
		auto filePkg = mpd.at(MsgPack::pack::str("file"));
		std::vector<char> mappedFile = MsgPack::unpack::bin(filePkg);
		std::string file(mappedFile.begin(), mappedFile.end());
		
		std::map<std::string, size_t> words;
		
		cutter(words, file.begin(), file.end());
		
		res = words.count(word) ? words[word] : 0;
	} else {
		std::cout << "Пакет не корректен. Размер пакета: " 
			<< pkg.size() << "байт. Ждём продолжения" << std::endl;
	}
	
	
	MsgPack::package resPkg = MsgPack::pack::integer<uint32_t>(res);
	gSock->async_write_some(buffer(resPkg.data(), resPkg.size()), handler_write);
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


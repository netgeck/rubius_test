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
//typedef std::shared_ptr<ip::tcp::acceptor> acceptor_ptr;

void signalHandler(int signum) {
	cout << "Interrupt signal (" << signum << ") received.\n";

	switch (signum) {
	case SIGINT:
	case SIGTERM:
//		gSock->cancel();
//		gSock->close();
		break;
	}

	exit(signum);
}

/**
 * @brief Проверка символа на допустимость использования в слове
 * @param c	символ
 * @return true - корректный символ; false - нет.
 */
bool isWordChar (char c) {
	static std::locale loc;
	return std::isalpha(c, loc) || std::isdigit(c, loc);
}

/**
 * @brief Функция нарезки строки на отдельные слова.
 * Словом считается последовательность букв и/или цифр.
 * Результат записывается в ассоциативный контейнер <слово, частота>.
 * @param words	ссылка на контейнер слов для записи результата
 * @param begin итератор начала строки
 * @param end итератор конца строки
 */
void cutter(std::map<std::string, size_t> &words, std::string::iterator begin, std::string::iterator end) {
	auto it_correct = std::find_if(begin, end, isWordChar);
	if (it_correct == end) {
		return;
	}
	
	auto it_incorrect = std::find_if_not(it_correct, end, isWordChar);
	words[std::string(it_correct, it_incorrect)]++; // добавляем слово и увеличиваем счётчик
	
	if (it_incorrect == end) {
		return;
	}
	
	cutter(words, it_incorrect, end);
}

class clientSession {
public:
	clientSession(socket_ptr pSock) : m_pSock(pSock) {
	}
	
	void start(){
		m_pSock->async_read_some(basio::buffer(m_buffer), 
			[this](const boost::system::error_code& error, std::size_t bytes_transferred) {
				if (error) return;

				uint32_t res(0);
				m_recvPkg.insert(m_recvPkg.end(), m_buffer, m_buffer + bytes_transferred);

				if (MsgPack::isPgkCorrect(m_recvPkg)) {
					std::cout << "Пакет получен" << std::endl;
					MsgPack::map_description mpd = MsgPack::unpack::map(m_recvPkg);
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
					std::cout << "Принятый пакет не корректен. Размер пакета: " 
						<< m_recvPkg.size() << "байт. Ждём продолжения" << std::endl;
				}


				MsgPack::package resPkg = MsgPack::pack::integer<uint32_t>(res);
				m_pSock->async_write_some(buffer(resPkg.data(), resPkg.size()), 
					[this](const boost::system::error_code& error, std::size_t bytes_transferred) {
						if (error) return;
						std::cout << "Передан ответ" << std::endl;
						m_recvPkg.clear();
						start();
					});
			});
	}
private:
	socket_ptr m_pSock;
	char m_buffer[512];
	MsgPack::package m_recvPkg;
};

class server {
public:
	server(boost::asio::io_service& io_service,
		const ip::tcp::endpoint& endpoint)
	: m_acceptor(io_service),
	m_ioService(io_service) {
		m_acceptor.open(endpoint.protocol());
		m_acceptor.set_option(ip::tcp::acceptor::reuse_address(true));
		m_acceptor.bind(endpoint);
		m_acceptor.listen();
		
		accept();
	}

private:
	void accept() {
		socket_ptr m_pSock = std::make_shared<ip::tcp::socket>(m_ioService);
		m_acceptor.async_accept(*m_pSock,
			[this, m_pSock](boost::system::error_code err) {
				if (!err) {
					syslog(LOG_INFO, "Принято соединение");
					m_sessions.emplace_back(new clientSession(m_pSock));
					m_sessions.back()->start();
				}

				accept();
			});
	}

	ip::tcp::acceptor m_acceptor;
	boost::asio::io_service& m_ioService;
	std::vector<unique_ptr<clientSession>> m_sessions;
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


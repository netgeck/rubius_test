/* 
 * File:   main.cxx
 * Author: kostya
 *
 * Created on 9 июня 2017 г., 21:19
 */

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <memory>
#include <iostream>
#include <syslog.h>
#include <string>
#include <csignal>
#include <algorithm>
#include <cwctype>
#include <locale>
#include <codecvt>

#include <defPort.h>
#include <MsgPack_types.h>
#include <common.h>
#include <MsgPack_pack.h>
#include <MsgPack_unpack.h>


#define READBUFFER_SIZE	512


using namespace boost::asio;
typedef std::shared_ptr<ip::tcp::socket> socket_ptr;
typedef std::map<std::string, size_t> word_count;

/**
 * @brief Конвертировать UTF-8 строку в wstring
 * @param str строка UTF-8
 * @return wstring
 */
inline std::wstring utf8_to_wstring(const std::string& str) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.from_bytes(str);
}

/**
 * @brief Конвертировать wstring в UTF-8 строку
 * @param str wstring
 * @return строка UTF-8
 */
inline std::string wstring_to_utf8(const std::wstring& str) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.to_bytes(str);
}

/**
 * @brief Проверка символа на допустимость использования в слове
 * @param c	"широкий" символ
 * @return true - корректный символ; false - нет.
 */
bool isWordChar(wchar_t c) {
	std::locale::global(std::locale("ru_RU.UTF-8"));
	return std::iswalpha(c) || std::iswdigit(c);
}

/**
 * @brief Функция нарезки строки на отдельные слова.
 * Словом считается последовательность букв и/или цифр.
 * Результат записывается в ассоциативный контейнер <слово, частота>.
 * @param words	ссылка на контейнер слов для записи результата
 * @param begin итератор начала строки
 * @param end итератор конца строки
 */
void cutter(word_count& words, std::wstring::iterator begin, std::wstring::iterator end) {
	auto it_correct = std::find_if(begin, end, isWordChar);
	if (it_correct == end) {
		return;
	}
	
	auto it_incorrect = std::find_if_not(it_correct, end, isWordChar);
	// добавляем слово и увеличиваем счётчик
	words[wstring_to_utf8(std::wstring(it_correct, it_incorrect))]++;
	
	if (it_incorrect == end) {
		return;
	}
	
	cutter(words, it_incorrect, end);
}

/**
 * @brief Класс сессия.
 * Слушает входящие пакеты, обрабатывает их и отправляет результат клиенту.
 */
class clientSession {
public:
	clientSession(socket_ptr pSock) : m_pSock(pSock) {
	}
	
	void readPkg() {
		m_pSock->async_read_some(boost::asio::buffer(m_buffer),
			[this](const boost::system::error_code& err, std::size_t bytes_transferred) {
				if (err) {
					syslog(LOG_ERR, "start: %s", err.message().c_str());
					return;
				}
				checkPkg(bytes_transferred);
			});
	}
private:
	socket_ptr m_pSock;
	char m_buffer[READBUFFER_SIZE];
	MsgPack::package m_recvPkg;
	
	void checkPkg(std::size_t bytes_transferred) {
		m_recvPkg.insert(m_recvPkg.end(), m_buffer, m_buffer + bytes_transferred);

		if (MsgPack::isPgkCorrect(m_recvPkg)) {
			handlePkg();
		} else {
			syslog(LOG_INFO, "Принятый пакет не корректен. "
				"Размер пакета: %lu байт. Ждём продолжения", m_recvPkg.size());
			readPkg();
		}
	}
	
	void handlePkg() {
		uint32_t res(0);
		MsgPack::map_description mpd = MsgPack::unpack::map(m_recvPkg);
		auto wordPkg = mpd.at(MsgPack::pack::str("word"));
		std::string word = MsgPack::unpack::str(wordPkg);

		auto filePkg = mpd.at(MsgPack::pack::str("file"));
		std::vector<char> mappedFile = MsgPack::unpack::bin(filePkg);
		
		// Конвертируем полученный файл в wstring
		std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> ucs2conv;
		std::wstring file = utf8_to_wstring(std::string(mappedFile.begin(), mappedFile.end()));
		
		// Нарезаем отдельные слова
		word_count words;
		cutter(words, file.begin(), file.end());

		res = words.count(word) ? words[word] : 0;
		answer(res);
	}
	
	void answer(uint32_t result) {
		MsgPack::package resPkg = MsgPack::pack::integer<uint32_t>(result);
		m_pSock->async_write_some(buffer(resPkg.data(), resPkg.size()),
			[this](const boost::system::error_code& err, std::size_t bytes_transferred) {
				if (err) {
					syslog(LOG_ERR, "answer: %s", err.message().c_str());
					return;
				}
				syslog(LOG_INFO, "Передан ответ");
				m_recvPkg.clear();
				readPkg();
			});
	}
};

/**
 * @brief Класс сервер.
 * Принимает соединения. Запускает объект-сессию для каждого нового соединения.
 */
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
					m_sessions.back()->readPkg();
				} else {
					syslog(LOG_ERR, "accept: %s", err.message().c_str());
				}

				accept();
			});
	}

	ip::tcp::acceptor m_acceptor;
	boost::asio::io_service& m_ioService;
	std::vector<std::unique_ptr<clientSession>> m_sessions;
};

/*
 * @brief точка входа в программу сервер
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


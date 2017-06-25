/* 
 * File:   main.cxx
 * Author: kostya
 *
 * Created on 9 июня 2017 г., 21:19
 */

#include <iostream>
#include <syslog.h>

#include <QCoreApplication>
#include <QTextCodec>

#include <defPort.h>
#include <tcpServer.h>

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

	// настройка кодека для корректной обработки русских букв
	QTextCodec *codec1 = QTextCodec::codecForName("UTF-8");
	QTextCodec::setCodecForLocale(codec1);
	QTextCodec::setCodecForTr(codec1);
	QTextCodec::setCodecForCStrings(codec1);

	QCoreApplication app(argc, argv);
	tcpServer server(port);
	return app.exec();
}

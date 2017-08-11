/* 
 * File:   main.cxx
 * Author: kostya
 *
 * Created on 9 июня 2017 г., 21:19
 */

#include <iostream>

#include <QCoreApplication>
#include <QTextCodec>
#include <QtDebug>

#include <common.h>

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

	// настройка кодека для корректной обработки русских букв
	QTextCodec::setCodecForTr(QTextCodec::codecForName(ENCODING_UTF8));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName(ENCODING_UTF8));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName(ENCODING_UTF8));
	
	qDebug() << "Cтарт сервера. Port: " << port;
	
	QCoreApplication app(argc, argv);
	tcpServer server(port);
	return app.exec();
}

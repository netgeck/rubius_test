/* 
 * File:   main.cxx
 * Author: kostya
 *
 * Created on 9 июня 2017 г., 21:19
 */

#include <QApplication>
#include <QtCore/QTextCodec>
#include <QTextCodec>

#include "MainWindow.h"


#define  ENCODING_UTF8	"UTF-8"


/*
 * @brief точка входа в программу клиента
 */
int main(int argc, char *argv[]) {
	QTextCodec::setCodecForTr(QTextCodec::codecForName(ENCODING_UTF8));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName(ENCODING_UTF8));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName(ENCODING_UTF8));
	
	QApplication app(argc, argv);
	MainWindow win;
	win.show();
	
	return app.exec();
}

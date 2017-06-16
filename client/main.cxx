/* 
 * File:   main.cxx
 * Author: kostya
 *
 * Created on 9 июня 2017 г., 21:19
 */

#include "mainwindow.h"
#include <QApplication>
#include <QtCore/QTextCodec>
#include <QTextCodec>

/*
 * @brief точка входа в программу клиента
 */
int main(int argc, char *argv[]) {
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}

/* 
 * File:   main.cxx
 * Author: kostya
 *
 * Created on 9 июня 2017 г., 21:19
 */

#include "mainwindow.h"
#include <QApplication>
#include <QtCore/QTextCodec>
/*
 * @brief точка входа в программу клиента
 */
int main(int argc, char *argv[]) {
	QTextCodec::setCodecForTr(QTextCodec::codecForName("System"));
	
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}

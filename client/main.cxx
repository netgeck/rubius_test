/* 
 * File:   main.cxx
 * Author: kostya
 *
 * Created on 9 июня 2017 г., 21:19
 */

#include "mainwindow.h"
#include <QApplication>

/*
 * @brief точка входа в программу клиента
 */
int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}

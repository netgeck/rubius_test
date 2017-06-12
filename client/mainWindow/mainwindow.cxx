#include "mainwindow.h"
#include <ui_mainwindow.h>
#include <QObject>

#include <boost/lexical_cast.hpp>
#include <string>
#include <stdint.h>

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <thread>
#include <iostream>
#include <defPort.h>

using namespace boost::asio;
typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;

socket_ptr gSock;

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::MainWindow),
port(PORT_DEFAULT),
host("127.0.0.1") {
	ui->setupUi(this);
	QObject::connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(send()));
	QObject::connect(ui->lineEdit, SIGNAL(editingFinished()), this, SLOT(hostSet()));
	QObject::connect(ui->lineEdit_2, SIGNAL(editingFinished()), this, SLOT(portSet()));
	ui->lineEdit->setText(host.c_str());
	ui->lineEdit_2->setText(boost::lexical_cast<std::string>(port).c_str());
}

MainWindow::~MainWindow() {
	delete ui;
}

void client_session(socket_ptr sock) {
	while (true) {
		char data[512];
		size_t len = sock->read_some(buffer(data));
		if (len > 0)
			write(*sock, buffer("Test", 4));
	}
}

void write_handler(const boost::system::error_code& error, std::size_t bytes_transferred) {
	
}

void connect_handler(const boost::system::error_code & err) {
	if (err) {
		std::cout << "Соединение не установлено" << std::endl;
		return;
	}
	std::cout << "Соединение установлено" << std::endl;
	
	gSock->async_write_some(buffer("Test", 4), write_handler);
}

void MainWindow::send() {
	static uint32_t count = 0;
	
	const std::string out = "Отправлено " + boost::lexical_cast<std::string>(++count);

	io_service service;
//	ip::tcp::endpoint ep(ip::address::from_string(host), port);
	ip::tcp::resolver resolver(service);
	ip::tcp::resolver::query query(host, boost::lexical_cast<std::string>(port));
	ip::tcp::resolver::iterator iter = resolver.resolve( query);
	ip::tcp::endpoint ep = *iter;
	std::cout << ep.address().to_string() << std::endl;
	gSock.reset(new ip::tcp::socket(service));
	gSock->async_connect(ep, connect_handler);
	service.run();
	
	ui->label->setText(tr(out.c_str()));
}

void MainWindow::hostSet() {
	host = ui->lineEdit->text().toStdString();
	std::cout << "Хост задан: \"" << host << "\"" << std::endl;
}

void MainWindow::portSet() {
	port = ui->lineEdit_2->text().toUInt();
	std::cout << "порт задан: \"" << port << "\"" << std::endl;
}

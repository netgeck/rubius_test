#include "mainwindow.h"
#include <ui_mainwindow.h>
#include <QObject>
#include <QFileDialog>

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
std::function<void(uint32_t)> result;
std::function<std::string()> getWord;

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::MainWindow),
port(PORT_DEFAULT),
host("127.0.0.1") {
	using namespace std::placeholders;
	ui->setupUi(this);
	QObject::connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(send()));
	QObject::connect(ui->lineEdit, SIGNAL(editingFinished()), this, SLOT(hostSet()));
	QObject::connect(ui->lineEdit_2, SIGNAL(editingFinished()), this, SLOT(portSet()));
	QObject::connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(chooseFile()));
	QObject::connect(ui->lineEdit_3, SIGNAL(editingFinished()), this, SLOT(wordSet()));
	ui->lineEdit->setText(host.c_str());
	ui->lineEdit_2->setText(boost::lexical_cast<std::string>(port).c_str());

	//	result = [=](uint32_t result) {	this->res(result);};
	result = std::bind(&MainWindow::res, this, _1);
	getWord = [this]()->std::string {
		return word;
	};
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

char data[sizeof(uint32_t)];

void read_handler(const boost::system::error_code& error, std::size_t bytes_transferred) {
	if (error) return;
	
	uint32_t res = *data;
	std::cout << "Получен ответ: " << res << std::endl;
	result(res);
}

void write_handler(const boost::system::error_code& error, std::size_t bytes_transferred) {
	if(error) return;
	
	// Будем ждать ответ
	gSock->async_read_some(buffer(data), read_handler);
}

void connect_handler(const boost::system::error_code & err) {
	if (err) {
		std::cout << "Соединение не установлено" << std::endl;
		return;
	}
	std::cout << "Соединение установлено" << std::endl;
	
	gSock->async_write_some(buffer(getWord().data(), getWord().size()), write_handler);
}

void MainWindow::res(uint32_t res) {
	ui->label->setText(tr(boost::lexical_cast<std::string>(res).c_str()));
}

void MainWindow::send() {
	static uint32_t count = 0;

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
}

void MainWindow::chooseFile() {
//	QFileDialog fdial()
	
	auto fileName = QFileDialog::getOpenFileName(this,
		tr("Open text"), QDir::homePath(), tr("Text Files(*.txt)"));
	ui->label_4->setText(fileName);
}

void MainWindow::hostSet() {
	host = ui->lineEdit->text().toStdString();
	std::cout << "Хост задан: \"" << host << "\"" << std::endl;
}

void MainWindow::portSet() {
	port = ui->lineEdit_2->text().toUInt();
	std::cout << "порт задан: \"" << port << "\"" << std::endl;
}

void MainWindow::wordSet() {
	word = ui->lineEdit_3->text().toStdString();
}

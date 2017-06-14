#include "mainwindow.h"
#include <ui_mainwindow.h>
#include <QObject>
#include <QFileDialog>
#include <QMessageBox>

#include <boost/lexical_cast.hpp>
#include <string>
#include <stdint.h>

#include <iostream>
#include <defPort.h>

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::MainWindow),
port(PORT_DEFAULT),
host("127.0.0.1") {
	using namespace std::placeholders;
	ui->setupUi(this);
	
	tcpSocket = new QTcpSocket(this);
	
	QObject::connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(send()));
	QObject::connect(ui->lineEdit_host, SIGNAL(editingFinished()), this, SLOT(hostSet()));
	QObject::connect(ui->lineEdit_port, SIGNAL(editingFinished()), this, SLOT(portSet()));
	QObject::connect(ui->pushButton_fChoose, SIGNAL(clicked()), this, SLOT(chooseFile()));
	QObject::connect(ui->lineEdit_word, SIGNAL(editingFinished()), this, SLOT(wordSet()));
	QObject::connect(ui->pushButton_connect, SIGNAL(clicked()), this, SLOT(connection()));
	QObject::connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));
	QObject::connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readAnswer()));
	ui->lineEdit_host->setText(host.c_str());
	ui->lineEdit_port->setText(boost::lexical_cast<std::string>(port).c_str());
	ui->lineEdit_host->setInputMask("000.000.000.000;_");
	ui->lineEdit_port->setValidator( new QIntValidator( 0, UINT16_MAX ) );
	// Только буквы кириллического и латинского алфавита и цифры
	ui->lineEdit_word->setValidator(new QRegExpValidator(QRegExp(tr("^[а-яА-ЯёЁa-zA-Z0-9]+$/u")), this));
	ui->groupBox_work->setEnabled(false);
}

MainWindow::~MainWindow() {
	delete ui;
}

char data[sizeof(uint32_t)];

void MainWindow::res(uint32_t res) {
	ui->label->setText(tr(boost::lexical_cast<std::string>(res).c_str()));
}

void MainWindow::connection() {
	ui->pushButton_connect->setEnabled(false);
//	tcpSocket->connectToHost(hostLineEdit->text(),
//                             portLineEdit->text().toInt());
	tcpSocket->connectToHost(ui->lineEdit_host->text(),
                             ui->lineEdit_port->text().toUInt());
	ui->groupBox_work->setEnabled(true);
}

void MainWindow::send() {
	static uint32_t count = 0;
	
//	gSock->async_write_some(buffer(getWord().data(), getWord().size()), write_handler);
	tcpSocket->write(word.data(), word.size());
}

void MainWindow::chooseFile() {
//	QFileDialog fdial()
	
	auto fileName = QFileDialog::getOpenFileName(this,
		tr("Open text"), QDir::homePath(), tr("Text Files(*.txt)"));
	ui->label_4->setText(fileName);
}

void MainWindow::hostSet() {
	host = ui->lineEdit_host->text().toStdString();
	std::cout << "Хост задан: \"" << host << "\"" << std::endl;
}

void MainWindow::portSet() {
	port = ui->lineEdit_port->text().toUInt();
	std::cout << "порт задан: \"" << port << "\"" << std::endl;
}

void MainWindow::wordSet() {
	word = ui->lineEdit_word->text().toStdString();
}

void MainWindow::displayError(QAbstractSocket::SocketError socketError) {
	ui->groupBox_work->setEnabled(false);
	
	switch (socketError) {
		case QAbstractSocket::RemoteHostClosedError:
			break;
		case QAbstractSocket::HostNotFoundError:
			QMessageBox::information(this, tr("Клиент"),
				tr("Хост не найден. Проверьте правильно ли "
				"введено имя хоста и порт"));
			break;
		case QAbstractSocket::ConnectionRefusedError:
			QMessageBox::information(this, tr("Клиент"),
				tr("Подключение не установлено. "
				"Убедитесь что сервер запущен, "
				"и проверьте правильно ли заданы хост и порт."));
			break;
		default:
			QMessageBox::information(this, tr("Клиент"),
				tr("Получена ошибка: %1.").arg(tcpSocket->errorString()));
	}

	ui->pushButton_connect->setEnabled(true);
}

void MainWindow::readAnswer() {
	uint32_t result;
	std::vector<char> d(sizeof(result));
	tcpSocket->read(d.data(), d.size());
	result = *reinterpret_cast<uint32_t*>(d.data());
	res(result);
}

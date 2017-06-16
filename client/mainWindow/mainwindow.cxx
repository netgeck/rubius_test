#include "mainwindow.h"
#include <ui_mainwindow.h>
#include <QObject>
#include <QFileDialog>
#include <QMessageBox>

#include <boost/lexical_cast.hpp>
#include <string>
#include <stdint.h>
#include <iostream>
#include <fstream>

#include <defPort.h>
#include <MsgPack_pack.h>
#include <common.h>
#include <MsgPack_unpack.h>
#include <MsgPack_types.h>

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::MainWindow),
port(PORT_DEFAULT),
host("127.0.0.1") {
	using namespace std::placeholders;
	ui->setupUi(this);
	
	tcpSocket = new QTcpSocket(this);
	
	QObject::connect(ui->pushButton_send, SIGNAL(clicked()), this, SLOT(send()));
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
	ui->lineEdit_word->setValidator(new QRegExpValidator(QRegExp(tr("^[а-яА-ЯёЁa-zA-Z0-9]+$")), this));
	ui->groupBox_work->setEnabled(false);
	checkSendAbility();
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::res(uint32_t res) {
	ui->label_resOut->setText(tr(boost::lexical_cast<std::string>(res).c_str()));
	ui->pushButton_send->setEnabled(true);
}

void MainWindow::connection() {
	ui->groupBox_net->setEnabled(false);
//	tcpSocket->connectToHost(hostLineEdit->text(),
//                             portLineEdit->text().toInt());
	tcpSocket->connectToHost(ui->lineEdit_host->text(),
                             ui->lineEdit_port->text().toUInt());
	ui->groupBox_work->setEnabled(true);
}

void MainWindow::send() {
	ui->pushButton_send->setEnabled(false);
	ui->label_resOut->clear();
	auto pkg = MsgPack::pack::map(mpd);
	
	tcpSocket->write(reinterpret_cast<char*>(pkg.data()), pkg.size());
	std::cout << "Передан пакет " << pkg.size() << "байт" << std::endl;
}

void MainWindow::chooseFile() {
	auto fileName = QFileDialog::getOpenFileName(this,
		tr("Open text"), QDir::homePath(), tr("Text Files(*.txt)"));
	ui->label_file->setText(fileName);
	
	// TODO: маппировать файл
	std::vector<char> mappedFile;
	
	// Маппинг файла
	std::streampos size;
	std::ifstream file(fileName.toStdString(), std::ios::in | std::ios::binary | std::ios::ate);
	if (file.is_open()) {
		size = file.tellg();
		mappedFile.resize(size);
		file.seekg(0, std::ios::beg);
		file.read(mappedFile.data(), mappedFile.size());
		file.close();

		std::cout << "Файл смапирован" << std::endl;;
	} else {
		std::cout << "Не удалось открыть файл" << std::endl;
	}
	
	
	mpd[MsgPack::pack::str("file")] = MsgPack::pack::bin(mappedFile.data(), mappedFile.size());
	
	checkSendAbility();
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
	std::cout << "Задаётся слово: \"" << word << "\"" << std::endl;
	mpd[MsgPack::pack::str("word")] = MsgPack::pack::str(word);
	
	checkSendAbility();
}

void MainWindow::checkSendAbility() {
	// Должны быть заданы и слово и файл
	if (mpd.count(MsgPack::pack::str("word")) && mpd.count(MsgPack::pack::str("file"))){
		ui->pushButton_send->setEnabled(true);
	} else {
		ui->pushButton_send->setEnabled(false);
	}
}


void MainWindow::displayError(QAbstractSocket::SocketError socketError) {
	ui->groupBox_work->setEnabled(false);
	
	switch (socketError) {
		case QAbstractSocket::RemoteHostClosedError:
			QMessageBox::information(this, tr("Клиент"),
				tr("Подключение разорвано другой стороной"));
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

	ui->groupBox_net->setEnabled(true);
}

void MainWindow::readAnswer() {
	if (readBuffer.size() == 0 ) {
		readBuffer.resize(32);
	}
	
	size_t avail = tcpSocket->bytesAvailable();
	int r = tcpSocket->read(readBuffer.data(), std::min(readBuffer.size(), avail));
	
	pkgRes.insert(pkgRes.end(), readBuffer.begin(), readBuffer.begin() + r);
	
	if (MsgPack::isPgkCorrect(pkgRes)) {
		res(MsgPack::unpack::integer<uint32_t>(pkgRes));
		pkgRes.clear();
	} else {
		std::cout << "Принятый пакет не корректен. Размер пакета: " 
			<< pkgRes.size() << "байт. Ждём продолжения" << std::endl;
		readAnswer();
	}
}

#include <QObject>
#include <QFileDialog>
#include <QMessageBox>
#include <ui_MainWindow.h>

#include <boost/lexical_cast.hpp>
#include <string>
#include <stdint.h>
#include <fstream>
#ifndef NDEBUG
#include <iostream>
#endif

#include <common.h>
#include <msg.h>

#include "MainWindow.h"


#define INPUTMASK_IP	"000.000.000.000;_"
#define IP_LOCALHOST	"127.0.0.1"
#define REGEXP_RUS_ENG_NUM	"^[а-яА-ЯёЁa-zA-Z0-9]+$"
#define READBUFFER_SIZE	32


MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
m_pUI(new Ui::MainWindow),
m_port(PORT_DEFAULT),
m_host(IP_LOCALHOST) {
	using namespace std::placeholders;
	m_pUI->setupUi(this);
	
	m_pTcpSocket = new QTcpSocket(this);
	
	QObject::connect(m_pUI->pushButton_send, SIGNAL(clicked()), this, SLOT(send()));
	QObject::connect(m_pUI->lineEdit_host, SIGNAL(editingFinished()), this, SLOT(setHost()));
	QObject::connect(m_pUI->lineEdit_port, SIGNAL(editingFinished()), this, SLOT(setPort()));
	QObject::connect(m_pUI->pushButton_fChoose, SIGNAL(clicked()), this, SLOT(chooseFile()));
	QObject::connect(m_pUI->lineEdit_word, SIGNAL(textChanged(const QString &)),
		this, SLOT(wordChange(const QString &)));
	QObject::connect(m_pUI->lineEdit_word, SIGNAL(editingFinished()), this, SLOT(setWord()));
	QObject::connect(m_pUI->pushButton_connect, SIGNAL(clicked()), this, SLOT(connection()));
	QObject::connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
		this, SLOT(displaySockError(QAbstractSocket::SocketError)));
	QObject::connect(m_pTcpSocket, SIGNAL(readyRead()), this, SLOT(readAnswer()));
	m_pUI->lineEdit_host->setText(m_host.c_str());
	m_pUI->lineEdit_port->setText(boost::lexical_cast<std::string>(m_port).c_str());
	m_pUI->lineEdit_host->setInputMask(INPUTMASK_IP);
	m_pUI->lineEdit_port->setValidator(new QIntValidator(PORT_MIN, PORT_MAX));
	// Только буквы кириллического и латинского алфавита и цифры
	m_pUI->lineEdit_word->setValidator(new QRegExpValidator(QRegExp(tr(REGEXP_RUS_ENG_NUM)), this));
	m_pUI->groupBox_work->setEnabled(false);
	checkSendAbility();
}

MainWindow::~MainWindow() {
	delete m_pUI;
}

void MainWindow::result(msg::answer::value res) {
	if (res == msg::answer::errCode) {
		displayAnswerError("Сервер вернул ошибку.");
	} else {
		m_pUI->label_resOut->setText(tr(boost::lexical_cast<std::string>(res).c_str()));
	}
	m_pUI->pushButton_send->setEnabled(true);
}

void MainWindow::connection() {
	m_pUI->groupBox_net->setEnabled(false);

	m_pTcpSocket->connectToHost(m_pUI->lineEdit_host->text(),
		m_pUI->lineEdit_port->text().toUInt());
	
	m_pUI->groupBox_work->setEnabled(true);
}

void MainWindow::send() {
	m_pUI->pushButton_send->setEnabled(false);
	m_pUI->label_resOut->clear();
	msg::package pkg;
	msg::request::packageFill(pkg, 
		m_word.begin(), m_word.end(), m_mappedFile.begin(), m_mappedFile.end());
	
	m_pTcpSocket->write(&(*pkg.begin()), pkg.size());
}

void MainWindow::chooseFile() {
	auto fileName = QFileDialog::getOpenFileName(this,
		tr("Open text"), QDir::homePath(), tr("Text Files(*.txt)"));
	m_pUI->lineEdit_file->setText(fileName);
	
	// Маппинг файла
	std::streampos size;
	std::ifstream file(fileName.toStdString(), std::ios::in | std::ios::binary | std::ios::ate);
	if (file.is_open()) {
		size = file.tellg();
		m_mappedFile.resize(size);
		file.seekg(0, std::ios::beg);
		file.read(m_mappedFile.data(), m_mappedFile.size());
		file.close();
	} else {
		displayFileError(strerror(errno));
		m_mappedFile.clear(); // удаляем смапированный файл
		return;
	}
	
	checkSendAbility();
}

void MainWindow::setHost() {
	m_host = m_pUI->lineEdit_host->text().toStdString();
}

void MainWindow::setPort() {
	m_port = m_pUI->lineEdit_port->text().toUInt();
}

void MainWindow::setWord() {
	m_word = m_pUI->lineEdit_word->text().toStdString();
	
	checkSendAbility();
}

void MainWindow::checkSendAbility() {
	// Должны быть заданы и слово и файл
	if (!m_pUI->lineEdit_word->text().isEmpty() && !m_pUI->lineEdit_file->text().isEmpty()) {
		m_pUI->pushButton_send->setEnabled(true);
	} else {
		m_pUI->pushButton_send->setEnabled(false);
	}
}

void MainWindow::wordChange(const QString&) {
	MainWindow::checkSendAbility();
}

void MainWindow::displaySockError(QAbstractSocket::SocketError socketError) {
	m_pUI->groupBox_work->setEnabled(false);
	
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
				tr("Получена ошибка: %1.").arg(m_pTcpSocket->errorString()));
	}

	m_pUI->groupBox_net->setEnabled(true);
}

void MainWindow::displayFileError(const QString& err) {
	m_pUI->lineEdit_file->clear();
	checkSendAbility();
	
	QMessageBox::information(this, tr("Клиент"),
		tr("Не удалось открыть файл: %1.").arg(err));
}

void MainWindow::displayAnswerError(const QString& err) {
	QMessageBox::information(this, tr("Клиент"),
		tr("Ошибка при получении ответа: %1.").arg(err));
}

void MainWindow::readAnswer() {
	std::vector<char> buffer(READBUFFER_SIZE);
	
	size_t avail = m_pTcpSocket->bytesAvailable();
	if (!avail) {
		return;
	}
	
	int byteRead = m_pTcpSocket->read(buffer.data(), std::min(buffer.size(), avail));
	
	try {
		m_answer.pushBack(&(*buffer.begin()), byteRead);
	} catch(std::exception& e) {
		displayAnswerError("Не корректный пакет с ответом");
		m_answer.clear();
		return;
	}
	
	if (m_answer.isFull()) {
		result(msg::answer::getNum(m_answer));
		m_answer.clear();
	} else {
#ifndef NDEBUG
		std::cout << "Принятый пакет не корректен. Размер пакета: " 
			<< m_answer.size() << "байт. Ждём продолжения" << std::endl;
#endif
		readAnswer();
	}
}

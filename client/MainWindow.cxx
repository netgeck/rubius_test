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

#include <defPort.h>
#include <MsgPack_pack.h>
#include <common.h>
#include <MsgPack_unpack.h>
#include <MsgPack_types.h>

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
		this, SLOT(displayError(QAbstractSocket::SocketError)));
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

void MainWindow::result(uint32_t res) {
	m_pUI->label_resOut->setText(tr(boost::lexical_cast<std::string>(res).c_str()));
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
	auto pkg = MsgPack::pack::map(m_mpd);
	
	m_pTcpSocket->write(reinterpret_cast<char*>(pkg.data()), pkg.size());
}

void MainWindow::chooseFile() {
	auto fileName = QFileDialog::getOpenFileName(this,
		tr("Open text"), QDir::homePath(), tr("Text Files(*.txt)"));
	m_pUI->label_file->setText(fileName);
	
	// Маппинг файла
	std::vector<char> mappedFile;
	std::streampos size;
	std::ifstream file(fileName.toStdString(), std::ios::in | std::ios::binary | std::ios::ate);
	if (file.is_open()) {
		size = file.tellg();
		mappedFile.resize(size);
		file.seekg(0, std::ios::beg);
		file.read(mappedFile.data(), mappedFile.size());
		file.close();
	} else {
		displayFileError(strerror(errno));
		
		// удаляем смапированный файл из пакет (если он там есть)
		auto it = m_mpd.find(MsgPack::pack::str("file"));
		if (it != m_mpd.end())
			m_mpd.erase(it);
		return;
	}
	
	m_mpd[MsgPack::pack::str("file")] = MsgPack::pack::bin(mappedFile.data(), mappedFile.size());
	
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
	m_mpd[MsgPack::pack::str("word")] = MsgPack::pack::str(m_word);
	
	checkSendAbility();
}

void MainWindow::checkSendAbility() {
	// Должны быть заданы и слово и файл
	if (!m_pUI->lineEdit_word->text().isEmpty() && !m_pUI->label_file->text().isEmpty()) {
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
	m_pUI->label_file->clear();
	checkSendAbility();
	
	QMessageBox::information(this, tr("Клиент"),
		tr("Не удалось открыть файл: %1.").arg(err));
}

void MainWindow::readAnswer() {
	if (m_readBuffer.size() == 0) {
		m_readBuffer.resize(READBUFFER_SIZE);
	}
	
	size_t avail = m_pTcpSocket->bytesAvailable();
	int r = m_pTcpSocket->read(m_readBuffer.data(), std::min(m_readBuffer.size(), avail));
	
	m_pkgResult.insert(m_pkgResult.end(), m_readBuffer.begin(), m_readBuffer.begin() + r);
	
	if (MsgPack::isPgkCorrect(m_pkgResult)) {
		result(MsgPack::unpack::integer<uint32_t>(m_pkgResult));
		m_pkgResult.clear();
	} else {
#ifndef NDEBUG
		std::cout << "Принятый пакет не корректен. Размер пакета: " 
			<< m_pkgResult.size() << "байт. Ждём продолжения" << std::endl;
#endif
		readAnswer();
	}
}

#include <QObject>
#include <QFileDialog>
#include <QMessageBox>
#include <ui_MainWindow.h>

#include <stdint.h>
#include <fstream>
#ifndef NDEBUG
#include <iostream>
#endif

#include <common.h>

#include "MainWindow.h"


#define IP_LOCALHOST	"127.0.0.1"
#define REGEXP_RUS_ENG_NUM	"^[а-яА-ЯёЁa-zA-Z0-9]+$"
#define READBUFFER_SIZE	32


MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
m_pUI(new Ui::MainWindow),
m_port(PORT_DEFAULT) {
	m_pUI->setupUi(this);
	
	m_pTcpSocket = new QTcpSocket(this);
	
	QObject::connect(m_pUI->pushButton_send, SIGNAL(clicked()), this, SLOT(send()));
	QObject::connect(m_pUI->lineEdit_port, SIGNAL(editingFinished()), this, SLOT(setPort()));
	QObject::connect(m_pUI->pushButton_fChoose, SIGNAL(clicked()), this, SLOT(chooseFile()));
	QObject::connect(m_pUI->lineEdit_word, SIGNAL(textChanged(const QString &)),
		this, SLOT(wordChange(const QString &)));
	QObject::connect(m_pUI->lineEdit_word, SIGNAL(editingFinished()), this, SLOT(setWord()));
	QObject::connect(m_pUI->pushButton_connect, SIGNAL(clicked()), this, SLOT(connection()));
	QObject::connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
		this, SLOT(displaySockError(QAbstractSocket::SocketError)));
	QObject::connect(m_pTcpSocket, SIGNAL(readyRead()), this, SLOT(readAnswer()));
	m_pUI->lineEdit_host->setText(IP_LOCALHOST);
	m_pUI->lineEdit_port->setText(QString::number(m_port));
	m_pUI->lineEdit_port->setValidator(new QIntValidator(PORT_MIN, PORT_MAX));
	// Только буквы кириллического и латинского алфавита и цифры
	m_pUI->lineEdit_word->setValidator(new QRegExpValidator(QRegExp(REGEXP_RUS_ENG_NUM), this));
	m_pUI->groupBox_work->setEnabled(false);
	checkSendAbility();
}

MainWindow::~MainWindow() {
	delete m_pUI;
}

void MainWindow::result(qint32 res) {
	if (res == -1) {
		displayAnswerError("Сервер вернул ошибку.");
	} else {
		m_pUI->lineEdit_result->setText(QString::number(res));
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
	m_pUI->lineEdit_result->clear();

	QByteArray buffer;
	QDataStream out(&buffer, QIODevice::WriteOnly);
	qint32 size(0);
	out << size; // резервируем место под заголовок с размером всего пакета
	out << m_mappedFile;
	out << m_pUI->lineEdit_word->text();
	
	// записываем размер в заголовок пакета
	size = buffer.size();
	out.device()->seek(0);
	out << size;
	
	qint64 sended = m_pTcpSocket->write(buffer);
}

void MainWindow::chooseFile() {
	QFileDialog dialog(this, "Выбор текстового файла", QDir::homePath());
	dialog.setFileMode(QFileDialog::ExistingFile);
	QStringList filters;
	filters	<< "Текстовые файлы (*.txt)"
		<< "Любые файлы (*)";
	dialog.setNameFilters(filters);
	if (!dialog.exec()) {
		return;
	}
	
	QString fileName = dialog.selectedFiles().front();

	m_pUI->lineEdit_file->setText(fileName);
	
	// Маппинг файла
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		displayFileError(file.errorString());
		m_mappedFile.clear(); // удаляем смапированный файл
		return;
	}
	
	QTextStream in(&file);
	in.readAll().swap(m_mappedFile);
	
	checkSendAbility();
}

void MainWindow::setPort() {
	m_port = m_pUI->lineEdit_port->text().toUInt();
}

void MainWindow::setWord() {
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
			QMessageBox::information(this, "Клиент",
				"Подключение разорвано другой стороной");
			break;
		case QAbstractSocket::HostNotFoundError:
			QMessageBox::information(this, "Клиент",
				"Хост не найден. Проверьте правильно ли "
				"введено имя хоста и порт");
			break;
		case QAbstractSocket::ConnectionRefusedError:
			QMessageBox::information(this, "Клиент",
				"Подключение не установлено. "
				"Убедитесь что сервер запущен, "
				"и проверьте правильно ли заданы хост и порт.");
			break;
		default:
			QMessageBox::information(this, "Клиент",
				QString("Получена ошибка: %1.").arg(m_pTcpSocket->errorString()));
	}

	m_pUI->groupBox_net->setEnabled(true);
}

void MainWindow::displayFileError(const QString& err) {
	m_pUI->lineEdit_file->clear();
	m_pUI->lineEdit_result->clear();
	checkSendAbility();
	
	QMessageBox::information(this, "Клиент",
		QString("Не удалось открыть файл: %1.").arg(err));
}

void MainWindow::displayAnswerError(const QString& err) {
	QMessageBox::information(this, "Клиент",
		QString("Ошибка при получении ответа: %1.").arg(err));
}

void MainWindow::readAnswer() {
	std::vector<char> buffer(READBUFFER_SIZE);
	
	size_t avail = m_pTcpSocket->bytesAvailable();
	if (!avail) {
		return;
	}
	
	int byteRead = m_pTcpSocket->read(buffer.data(), std::min(buffer.size(), avail));
	
	m_answer.append(buffer.data(), byteRead);
	
	if (m_answer.size() == sizeof(qint32)) {
		QDataStream in(m_answer);
		qint32 res;
		in >> res;
		result(res);
		m_answer.clear();
	} else {
#ifndef NDEBUG
		std::cout << "Принятый пакет не корректен. Размер пакета: " 
			<< m_answer.size() << "байт. Ждём продолжения" << std::endl;
#endif
		readAnswer();
	}
}

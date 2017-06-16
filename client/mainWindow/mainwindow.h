#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <string>

#include <MsgPack_types.h>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void res(uint32_t);
private:
	Ui::MainWindow *ui;
	uint16_t port;
	std::string host;
	std::string word;
	QTcpSocket *tcpSocket;
	MsgPack::map_description mpd;
	MsgPack::package pkgRes;
	std::vector<char> readBuffer;
	
	void checkSendAbility();
	
private slots:
	void displayError(QAbstractSocket::SocketError socketError);
	void readAnswer();
	void checkEditWordForSend(const QString &);
	
public slots:
	void send();
	void chooseFile();
	void hostSet();
	void portSet();
	void wordSet();
	void connection();
};

#endif // MAINWINDOW_H

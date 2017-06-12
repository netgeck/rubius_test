#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>

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
	
	void connection();

public slots:
	void send();
	void hostSet();
	void portSet();
};

#endif // MAINWINDOW_H

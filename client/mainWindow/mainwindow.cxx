#include "mainwindow.h"
#include <ui_mainwindow.h>
#include <QObject>

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::MainWindow) {
	ui->setupUi(this);
	QObject::connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(send()));
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::send() {
	ui->label->setText(tr("Отправлено"));
}

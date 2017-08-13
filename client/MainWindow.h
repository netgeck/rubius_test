#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>

namespace Ui {
	class MainWindow;
}

/**
 * @brief Класс основного окна программы клиента.
 * Формирование и приём пакетов находится сдесь же.
 */
class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	Ui::MainWindow *m_pUI;
	uint16_t m_port;		//!< порт сервера
	QString m_mappedFile;		//!< смапированный файл
	QTcpSocket *m_pTcpSocket;	//!< Сокет для взаимодействия с сервером
	QByteArray m_answer;		//!< ответное сообщение
	
	/**
	 * @brief Вывод результата (число вхождений слова в файл)
	 * @param res	результат
	 */
	void result(qint32);

	/// Делаем кнопку отправки активной/неактивной
	void checkSendAbility();
	
	/**
	 * @brief Диалог ошибки доступа к файлу
	 * @param err	описание ошибки
	 */
	void displayFileError(const QString& err);
	
	void displayAnswerError(const QString& err);
	
private slots:
	void displaySockError(QAbstractSocket::SocketError socketError); //!< ошибки соединения
	void readAnswer(); //!< чтение ответа
	void wordChange(const QString &); //!< Реагируем на изменение слова
	void send(); //!< Отправка пакета
	void chooseFile(); //!< выбор файла
	void setPort(); //!< Задать порт
	void setWord(); //!< Задать слово
	void connection(); //!< соединение с сервером
};

#endif // MAINWINDOW_H

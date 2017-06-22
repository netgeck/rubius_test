#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <string>

#include <MsgPack_types.h>

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
	std::string m_host;		//!< адрес сервера
	std::string m_word;		//!< Слово для поиска вхождений в файле
	QTcpSocket *m_pTcpSocket;	//!< Сокет для взаимодействия с сервером
	MsgPack::map_description m_mpd;	//!< Промежуточное представление отправляемого пакета
	MsgPack::package m_pkgResult;	//!< Ответный пакет с результатом
	std::vector<char> m_readBuffer;	//!< буффер
	
	/**
	 * @brief Вывод результата (число вхождений слова в файл)
	 * @param res	результат
	 */
	void result(uint32_t);

	/// Делаем кнопку отправки активной/неактивной
	void checkSendAbility();
	
private slots:
	void displayError(QAbstractSocket::SocketError socketError); //!< ошибки соединения
	void readAnswer(); //!< чтение ответа
	void wordChange(const QString &); //!< Реагируем на изменение слова
	void send(); //!< Отправка пакета
	void chooseFile(); //!< выбор файла
	void hostSet();
	void portSet();
	void wordSet();
	void connection(); //!< соединение с сервером
};

#endif // MAINWINDOW_H

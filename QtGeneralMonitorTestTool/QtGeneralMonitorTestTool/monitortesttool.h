#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_monitortesttool.h"
#include "concurrentqueue.h"

class MonitorTestTool : public QDialog//QMainWindow
{
	Q_OBJECT

Q_SIGNALS:
	void stop();

public:
	MonitorTestTool(QWidget *parent = Q_NULLPTR);
	virtual ~MonitorTestTool();

public Q_SLOTS:
	void filterChanged(const QString &);
	void openSerialPort();
	void closeSerialPort();
	void recordTimeout();

protected:
	void readData();
	void handleReceivedData();

private:
	QGroupBox *createSettingsGroup();
	QGroupBox *createReceiveGroup();
	void initWidgetsConnections();

private:
//	Ui::MonitorTestToolClass ui;
	bool isOpened;
	QLabel *serialPortLabel;
	QComboBox *serialPortComboBox;
	QLabel *baudRateLabel;
	QComboBox *baudRateComboBox;
	QLabel *dataBitsLabel;
	QComboBox *dataBitsComboBox;
	QLabel *stopBitsLabel;
	QComboBox *stopBitsComboBox;
	QLabel *parityLabel;
	QComboBox *parityComboBox;
	QPushButton *runButton;
	QPushButton *stopButton;
	QTextEdit *receiveTextEdit;
	QStatusBar *statusBar;

	QSerialPort *serial;
	ConcurrentQueue<QString> queue;

	QString record;
	QMutex recordMutex;
	QFile rfile;

	QThread *recordThread;
	QTimer *recordTimer;
};

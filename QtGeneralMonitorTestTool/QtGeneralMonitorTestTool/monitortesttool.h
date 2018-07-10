#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_monitortesttool.h"
#include "concurrentqueue.h"

class MonitorTestTool : public QDialog//QMainWindow
{
	Q_OBJECT

Q_SIGNALS:
	void debugUpdate(qint64, const QString &);
	void sendData(const QString &);

public:
	MonitorTestTool(QWidget *parent = Q_NULLPTR);
	virtual ~MonitorTestTool();

public Q_SLOTS:
	void filterChanged(const QString &);
	void openSerialPort();
	void closeSerialPort();
	void debugTips(qint64, const QString &);
	void showData(const QString &);

protected:
	void readData();
	void recordData();
	void handleData();
	void closeEvent(QCloseEvent *);

private:
	QGroupBox *createSettingsGroup();
	QGroupBox *createReceiveGroup();
	QGroupBox *createOpeningGroup();
	void initWidgetsConnections();
	void initWidgetsStyle();
	void deleteWidgets();
	void runThread();


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
	QWaitCondition recordCondition;

	QFuture<void> retRead;
	QFuture<void> retRecord;
	QFuture<void> retHandle;
	//QThread *timerThread;
	//QTimer *recordTimer;
};

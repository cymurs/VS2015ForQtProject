#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_monitortesttool.h"
#include "concurrentqueue.h"

typedef QPair<QPointF, QString> Data;
typedef QList<Data> DataList;
typedef QList<DataList> DataTable;

QT_CHARTS_USE_NAMESPACE

class MonitorTestTool : public QDialog//QMainWindow
{
	Q_OBJECT

Q_SIGNALS:
	void debugUpdate(qint64, const QString &);
	void sendData(const QString &);

public:
	MonitorTestTool(QWidget *parent = Q_NULLPTR);
	virtual ~MonitorTestTool();

private Q_SLOTS:
	void filterChanged(const QString &);
	void openSerialPort();
	void closeSerialPort();
	void selectFile();
	void runImport();
	void cancelImport();
	void debugTips(qint64, const QString &);
	void showData(const QString &);
	void changeSeries(const QString &);
	

protected:
	void closeEvent(QCloseEvent *);
	DataTable generateRandomData(int listCount = 3, int valueMax = 20, int valueCount = 10) const;

private:
	QGroupBox *createSettingsGroup();
	QGroupBox *createReceiveGroup();
	QGroupBox *createImportGroup();
	QGroupBox *createChartGroup();
	void initWidgetsConnections();
	void initWidgetsStyle();
	void deleteWidgets();
	void readData();
	void recordData();
	void handleData();
	void runThread();
	void showMessage(const QString &);

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
	QLabel *dataFileLabel;
	QLineEdit *dataFileLineEdit;
	QPushButton *selectButton;
	QPushButton *confirmButton;
	QPushButton *cancelButton;
	QTextEdit *receiveTextEdit;
	QStatusBar *statusBar;
	QChart *splineChart;
	QSplineSeries *splineSeries;
	QScatterSeries *scatterSeries;

	QSerialPort *serial;
	ConcurrentQueue<QString> queue;

	QString record;
	QMutex recordMutex;
	QMutex writeMutex;
	QWaitCondition recordCondition;

	QString dataFile;

	quint32 maxSize;
	quint32 maxX;
	quint32 maxY;
	DataTable dataTable;	
	//QFuture<void> retRead;
	//QFuture<void> retRecord;
	//QFuture<void> retHandle;
	//QThread *timerThread;
	//QTimer *recordTimer;
};

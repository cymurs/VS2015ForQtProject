#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_monitortesttool.h"
#include "concurrentqueue.h"
#include "qdoublebufferedqueue.h"

typedef QPair<QPointF, QString> Data;
typedef QList<Data> DataList;
typedef QList<DataList> DataTable;
typedef QList<QPointF> PointList;

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
	void onShow(const QString &, bool overwrite = false);	
	void onShow(const QStringList &);
	void onSeriesChanged(const QString &);
	void onSeriesChanged(const QStringList &);
	void onDataUpdateTimer();
	void debugTips(qint64, const QString &);

protected:
	void closeEvent(QCloseEvent *);	
	DataTable generateRandomData(int listCount = 3, int valueMax = 20, int valueCount = 10) const;

private:
	QGroupBox *createSettingsGroup();
	QGroupBox *createReceiveGroup();
	QGroupBox *createImportGroup();
	QGroupBox *createChartGroup();
	QProgressDialog *createProgressDialog(qint32 min, qint32 max, const QString &text, const QString &title = tr(""));
	void setupStatusBar();
	void initWidgetsConnections();
	void initWidgetsStyle();
	void deleteWidgets();
	void readData();
	void recordData();
	void handleData();
	void runThread();
	bool parseData(const QString &data, QStringList &result);
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
	QLabel *receiveLabel;
	QLabel *threadLabel;
	QStatusBar *statusBar;
	QChart *splineChart;
	QSplineSeries *splineSeries;
	QScatterSeries *scatterSeries;
	QValueAxis *axisX;
	QValueAxis *axisY;

	QSerialPort *serial;
	QMutex serialMutex;
	QTimer windowTimer;
	ConcurrentQueue<QString> queue;
	QDoubleBufferedQueue<QString> windowQueue;
	QString frameHead;
	QString frameTail;

	QString record;
	QMutex recordMutex;
	QMutex writeMutex;
	QWaitCondition recordCondition;

	QString dataFile;

	quint32 maxSize;
	quint32 maxX;
	quint32 maxY;
	quint32 randCount;
	DataTable dataTable;	
	//QFuture<void> retRead;
	//QFuture<void> retRecord;
	//QFuture<void> retHandle;
	//QThread *timerThread;
	//QTimer *recordTimer;
};

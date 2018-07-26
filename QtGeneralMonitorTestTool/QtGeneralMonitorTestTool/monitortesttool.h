#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_monitortesttool.h"
#include "concurrentqueue.h"
#include "qdoublebufferedqueue.h"
#include "singlelogger.h"

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

private:
	enum Channel {
		CH1 = 0x01,
		CH2 = 0x02,
		CH3 = 0x04,
		CH4 = 0x08
	};
	struct SeriesPacket {
		PointList dataPoints;
		QSplineSeries *splineSeries;
		QScatterSeries *scatterSeries;
	};

public:
	MonitorTestTool(QWidget *parent = Q_NULLPTR);
	virtual ~MonitorTestTool();

private Q_SLOTS:
	void filterChanged(const QString &);
	void openSerialPort();
	void closeSerialPort();
	void selectFile();
	void runImport();
	void clearData();
	void saveSamplingSettings();
	void onShow(const QString &, bool overwrite = false);	
	void onShow(const QStringList &);
	void onSeriesChanged(const QString &);
	void onSeriesChanged(const QStringList &);
	void onDataUpdateTimer();
	void onRecordFileChangeTimer();
	void debugTips(qint64, const QString &);

protected:
	void closeEvent(QCloseEvent *);	
	DataTable generateRandomData(int listCount = 3, int valueMax = 20, int valueCount = 10) const;

private:
	QGroupBox *createSettingsGroup();
	QGroupBox *createReceiveGroup();
	QGroupBox *createImportGroup();
	QGroupBox *createChartGroup();
	QGroupBox *createSamplingGroup();
	QGroupBox *createFrameFormatGroup();
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
	bool addSeries(int x, const QString &d, Channel ch = CH1);


private:
//	Ui::MonitorTestToolClass ui;
	bool isOpened;
	bool isQuit;
	// 串口设置
	QComboBox *serialPortComboBox;
	QComboBox *baudRateComboBox;
	QComboBox *dataBitsComboBox;
	QComboBox *stopBitsComboBox;
	QComboBox *parityComboBox;
	QPushButton *runButton;
	QPushButton *stopButton;
	// 帧格式
	QLineEdit *fheadLineEdit;
	QLineEdit *ftailLineEdit;
	// 导入数据
	QLineEdit *dataFileLineEdit;
	QPushButton *selectButton;
	QPushButton *importButton;
	// 接收数据
	QPushButton *clearButton;
	QTextEdit *receiveTextEdit;
	// 实时图设置
	QLineEdit *minValueLineEdit;
	QLineEdit *maxValueLineEdit;
	QLineEdit *samplingNumLineEdit;
	QSpinBox *tickCountSpinBox;
	QPushButton *saveButton;
	// 状态栏
	QLabel *receiveLabel;
	QLabel *threadLabel;
	QStatusBar *statusBar;
	// 曲线图
	QChart *splineChart;
	SeriesPacket seriesPackets[4];
	//QSplineSeries *splineSeries;
	//QScatterSeries *scatterSeries;
	QValueAxis *axisX;
	QValueAxis *axisY;
	int channel;

	QSerialPort *serial;
	mutable QMutex serialMutex;
	QTimer windowTimer;
	ConcurrentQueue<QString> queue;
	QDoubleBufferedQueue<QString> windowQueue;
	QString frameHead;
	QString frameTail;

	QString dataFile;
	QString record;
	QFile rfile;
	QTimer recordTimer;
	mutable QMutex recordMutex;
	mutable QMutex writeMutex;
		
	int minY;
	int maxY;		
	int tickCount;
	int samplingNum;
	int randCount;
	DataTable dataTable;	
	QFuture<void> retRead;
	QFuture<void> retRecord;
	QFuture<void> retHandle;
	//QThread *timerThread;
	//QTimer *recordTimer;
};

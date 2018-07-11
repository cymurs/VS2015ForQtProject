#include "stdafx.h"
#include "monitortesttool.h"

MonitorTestTool::MonitorTestTool(QWidget *parent)
	: QDialog(parent)//QMainWindow(parent)
	, isOpened(true)
	, serialPortLabel(new QLabel(tr("端口号: ")))
	, serialPortComboBox(new QComboBox())
	, baudRateLabel(new QLabel(tr("波特率: ")))
	, baudRateComboBox(new QComboBox())
	, dataBitsLabel(new QLabel(tr("数据位: ")))
	, dataBitsComboBox(new QComboBox())
	, stopBitsLabel(new QLabel(tr("停止位: ")))
	, stopBitsComboBox(new QComboBox())
	, parityLabel(new QLabel(tr("校验位: ")))
	, parityComboBox(new QComboBox())
	, runButton(new QPushButton(tr("打开串口")))
	, stopButton(new QPushButton(tr("停止")))
	, dataFileLabel(new QLabel(tr("数据源位置:")))
	, dataFileLineEdit(new QLineEdit())
	, selectButton(new QPushButton(tr("浏览...")))
	, confirmButton(new QPushButton(tr("导入")))
	, cancelButton(new QPushButton(tr("清空")))
	, receiveTextEdit(new QTextEdit(tr("准备中......")))
	, statusBar(new QStatusBar())
	, serial(new QSerialPort(this))
	, dataFile("")
	, dataTable(generateRandomData())
{
//	ui.setupUi(this);

	
	
	
	

	auto leftLayout = new QVBoxLayout();
	leftLayout->addWidget(createSettingsGroup());
	leftLayout->addWidget(createImportGroup());
	leftLayout->addWidget(createReceiveGroup());	
	leftLayout->addWidget(statusBar);	


	auto baseLayout = new QGridLayout();
	baseLayout->addLayout(leftLayout, 0, 0);
	baseLayout->addWidget(createChartGroup(), 0, 1);
	// 设置拉升时固定比例
	baseLayout->setColumnStretch(0, 1);
	baseLayout->setColumnStretch(1, 4);
	setLayout(baseLayout);
	
	
	
	initWidgetsStyle();
	initWidgetsConnections();
	

	//emit debugUpdate(quintptr(QThread::currentThreadId()), tr("MonitorTestTool"));
	//QMessageBox::critical(this, tr("Critical Test"), tr("[%1]MonitorTestTool").arg(quintptr(QThread::currentThreadId())));
}

MonitorTestTool::~MonitorTestTool()
{
	closeSerialPort();
	delete serial;
	serial = Q_NULLPTR;
	deleteWidgets();
}

void MonitorTestTool::filterChanged(const QString &ret)
{
	QString s = QString(tr("[%1]data=%2;text=%3\n"))
		.arg(ret)
		.arg(parityComboBox->currentData().toInt())
		.arg(parityComboBox->currentText());

	receiveTextEdit->moveCursor(QTextCursor::Start);
	receiveTextEdit->insertPlainText(s);
	statusBar->showMessage(s);
}

void MonitorTestTool::openSerialPort()
{
	QString portName = serialPortComboBox->currentText();
	qint32 baudRate = baudRateComboBox->currentData().toInt();
	qint32 dataBits = dataBitsComboBox->currentText().toInt();
	qint32 parity = parityComboBox->currentData().toInt();
	qint32 stopBits = stopBitsComboBox->currentText().toInt();

	serial->setPortName(portName);
	serial->setBaudRate(baudRate);
	serial->setDataBits(QSerialPort::DataBits(dataBits));
	serial->setParity(QSerialPort::Parity(parity));
	serial->setStopBits(QSerialPort::StopBits(stopBits));
	serial->setFlowControl(QSerialPort::NoFlowControl);
	if (serial->open(QIODevice::ReadWrite)) {
		isOpened = true;
		runButton->setEnabled(false);
		confirmButton->setEnabled(false);
		cancelButton->setEnabled(false);
		stopButton->setEnabled(true);

		runThread();				
	}
	else {
		QMessageBox::critical(this, tr("Critical Error"), tr("打开端口%1失败!").arg(portName));
		statusBar->showMessage(tr("open serial port failed!\n"));
	}
}

void MonitorTestTool::closeSerialPort()
{
	if (serial && serial->isOpen()) {
		isOpened = false;
		serial->close();		
	}		
	if (runButton) {
		runButton->setEnabled(true);
		confirmButton->setEnabled(true);
		cancelButton->setEnabled(true);
		stopButton->setEnabled(false);
	}
}

void MonitorTestTool::selectFile()
{
	dataFile = QFileDialog::getOpenFileName(this, tr("数据源位置"), 
		QDir::currentPath(), tr("CSV files (*.csv);;Text files (*.txt)"));
	dataFileLineEdit->setText(dataFile);
}

void MonitorTestTool::runImport()
{
	if (dataFile.isEmpty()) return;
	if (!QFile::exists(dataFile)) {
		QMessageBox::critical(this, tr("警告"), tr("%1 不存在, 请重新选择").arg(dataFile));
		selectButton->setFocus();
		return;
	}

	QFile file(dataFile);	
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		statusBar->showMessage(tr("打开%1失败!").arg(dataFile));
		QMessageBox::critical(this, tr("警告"), tr("打开%1失败!").arg(dataFile));
		return;
	}

	//receiveTextEdit->clear();

	QTextStream in(&file);
	QString line;
	while (in.readLineInto(&line)) {
		//if (line.isEmpty()) continue;
		line.append('\n');
		showData(line);
	}

	in.flush();
	file.close();

	showMessage(tr("导入完成!"));
}

void MonitorTestTool::cancelImport()
{
	if (dataFile.isEmpty())  return;
	receiveTextEdit->clear();
	dataFileLineEdit->clear();
	dataFile.clear();
	selectButton->setFocus();

}

void MonitorTestTool::recordData()
{	
	emit debugUpdate(quintptr(QThread::currentThreadId()), tr("recordData"));
	QFile rfile(RecordFile);

	while (isOpened) { // while (isOpened || !record.isEmpty())
		try {			
			// 每隔 2s 写一次文件
			QThread::sleep(2);
			
			//QString strWriting("");
			//{
			//	QMutexLocker locker(&recordMutex);
			//	record.swap(strWriting);
			//}
			qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
			QStringList list = { "one", "two", "three" };
			qint32 cur = qrand() % 3;
			QString strWriting = QTime::currentTime().toString(tr("hh:mm:ss "));
			strWriting.push_back(list[cur]);
			strWriting.push_back(tr("\n"));

			if (!rfile.isOpen()) {
				if (!rfile.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append)) {
					QMessageBox::critical(this, tr("critical error"), tr("Open %1 failed!").arg(RecordFile));
					statusBar->showMessage(strWriting);
				}
			}
			
			
			if (rfile.isOpen()) {
				QTextStream out(&rfile);
				out << strWriting;
			}
		}
		catch (...) {
			emit debugUpdate(quintptr(QThread::currentThreadId()), tr("recordData error"));
		}

	}
	if (rfile.isOpen())
		rfile.close();
}

void MonitorTestTool::debugTips(qint64 tid, const QString &where)
{
	//QMessageBox::information(this, tr("debug tips"), tr("%1 threadId=%2").arg(where).arg(tid));
	if (statusBar) {
		statusBar->clearMessage();
		statusBar->showMessage(tr("%1 thread is %2.").arg(where).arg(tid), 4000);
	}
}

void MonitorTestTool::showData(const QString &data)
{
	receiveTextEdit->moveCursor(QTextCursor::Start);
	receiveTextEdit->insertPlainText(data);
}

void MonitorTestTool::showMessage(const QString &s)
{
	statusBar->showMessage(s);
}

void MonitorTestTool::readData()
{
	while (isOpened) {
		QByteArray data = serial->readAll();
		if (serial->bytesAvailable() > 0)
			queue.Push(data);
	}
}

void MonitorTestTool::handleData()
{	
	emit debugUpdate(quintptr(QThread::currentThreadId()), tr("handleData"));

	QString data;
	do 
	{
		while (queue.Pop(data, false)) {
			emit sendData(data);

			{
				QMutexLocker locker(&recordMutex);
				record.append(data);
			}

		}
	} while (isOpened || !queue.Empty());
	
}

void MonitorTestTool::closeEvent(QCloseEvent *e)
{
	closeSerialPort();
	delete serial;
	serial = Q_NULLPTR;
	deleteWidgets();
}

DataTable MonitorTestTool::generateRandomData(int listCount, int valueMax, int valueCount) const
{
	DataTable dataTable;

	// set seed for random stuff
	qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));

	// generate random data
	for (int i(0); i < listCount; i++) {
		DataList dataList;
		qreal yValue(0);
		for (int j(0); j < valueCount; j++) {
			yValue = yValue + (qreal)(qrand() % valueMax) / (qreal)valueCount;
			QPointF value((j + (qreal)rand() / (qreal)RAND_MAX) * ((qreal)20 / (qreal)valueCount),
				yValue);
			QString label = "Slice " + QString::number(i) + ":" + QString::number(j);
			dataList << Data(value, label);
		}
		dataTable << dataList;
	}

	return dataTable;
}

QGroupBox * MonitorTestTool::createSettingsGroup()
{
	const auto infos = QSerialPortInfo::availablePorts();
	for (const QSerialPortInfo &info : infos)
		serialPortComboBox->addItem(info.portName());

	baudRateComboBox->addItem(tr("Baud1200"), QVariant::fromValue(QSerialPort::Baud1200));
	baudRateComboBox->addItem(tr("Baud2400"), QVariant::fromValue(QSerialPort::Baud2400));
	baudRateComboBox->addItem(tr("Baud4800"), QVariant::fromValue(QSerialPort::Baud4800));
	baudRateComboBox->addItem(tr("Baud9600"), QVariant::fromValue(QSerialPort::Baud9600));
	baudRateComboBox->addItem(tr("Baud19200"), QVariant::fromValue(QSerialPort::Baud19200));
	baudRateComboBox->addItem(tr("Baud38400"), QVariant::fromValue(QSerialPort::Baud38400));
	baudRateComboBox->addItem(tr("Baud57600"), QVariant::fromValue(QSerialPort::Baud57600));
	baudRateComboBox->addItem(tr("Baud115200"), QVariant::fromValue(QSerialPort::Baud115200));

	dataBitsComboBox->addItem(tr("5"));
	dataBitsComboBox->addItem(tr("6"));
	dataBitsComboBox->addItem(tr("7"));
	dataBitsComboBox->addItem(tr("8"));

	stopBitsComboBox->addItem(tr("1"));
	stopBitsComboBox->addItem(tr("2"));
	stopBitsComboBox->addItem(tr("3"));

	parityComboBox->addItem(tr("OddParity"), QVariant::fromValue(QSerialPort::OddParity));
	parityComboBox->addItem(tr("EvenParity"), QVariant::fromValue(QSerialPort::EvenParity));
	parityComboBox->addItem(tr("NoParity"), QVariant::fromValue(QSerialPort::NoParity));

	auto settingsLayout = new QGridLayout();
	settingsLayout->addWidget(serialPortLabel, 0, 0);
	settingsLayout->addWidget(serialPortComboBox, 0, 1);
	settingsLayout->addWidget(baudRateLabel, 0, 2);
	settingsLayout->addWidget(baudRateComboBox, 0, 3);
	settingsLayout->addWidget(dataBitsLabel, 1, 0);
	settingsLayout->addWidget(dataBitsComboBox, 1, 1);
	settingsLayout->addWidget(stopBitsLabel, 1, 2);
	settingsLayout->addWidget(stopBitsComboBox, 1, 3);
	settingsLayout->addWidget(parityLabel, 2, 0);
	settingsLayout->addWidget(parityComboBox, 2, 1);
	settingsLayout->addWidget(runButton, 2, 2, 1, 1);
	settingsLayout->addWidget(stopButton, 2, 3, 1, 1);	

	auto settingsGroupBox = new QGroupBox(tr("设置串口"));
	settingsGroupBox->setLayout(settingsLayout);

	serialPortComboBox->setFocus();
	stopButton->setEnabled(false);
	return settingsGroupBox;
}

QGroupBox * MonitorTestTool::createReceiveGroup()
{	
	auto receiveLayout = new QVBoxLayout();
	receiveLayout->addWidget(receiveTextEdit);

	auto receiveGropBox = new QGroupBox(tr("接收数据"));
	receiveGropBox->setLayout(receiveLayout);

	receiveTextEdit->setReadOnly(true);
	return receiveGropBox;
}

QGroupBox * MonitorTestTool::createImportGroup()
{
	auto importLayout = new QGridLayout;
	importLayout->addWidget(dataFileLabel, 0, 0);
	importLayout->addWidget(dataFileLineEdit, 1, 0, 1, 3);
	importLayout->addWidget(selectButton, 1, 3);
	importLayout->addWidget(confirmButton, 2, 2);
	importLayout->addWidget(cancelButton, 2, 3);
	
	auto openingGroupBox = new QGroupBox(tr("导入数据"));
	openingGroupBox->setLayout(importLayout);

	return openingGroupBox;
}

QGroupBox *MonitorTestTool::createChartGroup()
{
	// spine chart
	splineChart = new QChart();
	splineChart->setTitle("serial port spline chart");
	QString name("Series ");
	int nameIndex = 0;
	foreach(DataList list, dataTable) {
		QSplineSeries *series = new QSplineSeries(splineChart);
		foreach(Data data, list)
			series->append(data.first);
		series->setName(name + QString::number(nameIndex));
		nameIndex++;
		splineChart->addSeries(series);
	}
	splineChart->createDefaultAxes();

	auto splineView = new QChartView(splineChart);
	splineView->setRenderHint(QPainter::Antialiasing); // 图像抗锯齿
	splineView->setSceneRect(0, 0, 630, 280);	 // 设置初始图表大小

	auto chartLayout = new QVBoxLayout;
	chartLayout->addWidget(splineView);

	auto chartGroupBox = new QGroupBox(tr("实时数据曲线图"));
	chartGroupBox->setLayout(chartLayout);
	chartGroupBox->setMinimumSize(400, 450); // 设置图表展示最小值
	return chartGroupBox;
}

void MonitorTestTool::initWidgetsConnections()
{
	connect(parityComboBox, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged),
		this, &MonitorTestTool::filterChanged);
	connect(runButton, &QPushButton::clicked, this, &MonitorTestTool::openSerialPort);
	connect(stopButton, &QPushButton::clicked, this, &MonitorTestTool::closeSerialPort);
	connect(selectButton, &QPushButton::clicked, this, &MonitorTestTool::selectFile);
	connect(confirmButton, &QPushButton::clicked, this, &MonitorTestTool::runImport);
	connect(cancelButton, &QPushButton::clicked, this, &MonitorTestTool::cancelImport);
	connect(this, SIGNAL(debugUpdate(qint64, QString)), this, SLOT(debugTips(qint64, QString)));	
	connect(this, SIGNAL(sendData(QString)), this, SLOT(showData(QString)));
}

void MonitorTestTool::initWidgetsStyle()
{
	statusBar->setStyleSheet("QStatusBar{ \
		border: 1px solid lightgray; \
		padding: 0px; \
		color: rgb(0, 139, 139); \
	}");
}

void MonitorTestTool::deleteWidgets()
{
	delete serialPortLabel;
	delete baudRateLabel;
	delete dataBitsLabel;
	delete stopBitsLabel;
	delete parityLabel;
	delete serialPortComboBox;
	delete baudRateComboBox;
	delete dataBitsComboBox;
	delete stopBitsComboBox;
	delete parityComboBox;
	delete runButton;
	delete stopButton;

	delete dataFileLabel;
	delete dataFileLineEdit;
	delete selectButton;
	delete confirmButton;
	delete cancelButton;

	delete receiveTextEdit;
	delete statusBar;

	serialPortLabel = Q_NULLPTR;
	baudRateLabel = Q_NULLPTR;
	dataBitsLabel = Q_NULLPTR;
	stopBitsLabel = Q_NULLPTR;
	parityLabel = Q_NULLPTR;
	serialPortComboBox = Q_NULLPTR;
	baudRateComboBox = Q_NULLPTR;
	dataBitsComboBox = Q_NULLPTR;
	stopBitsComboBox = Q_NULLPTR;
	parityComboBox = Q_NULLPTR;
	runButton = Q_NULLPTR;
	stopButton = Q_NULLPTR;

	dataFileLabel = Q_NULLPTR;
	dataFileLineEdit = Q_NULLPTR;
	selectButton = Q_NULLPTR;
	confirmButton = Q_NULLPTR;
	cancelButton = Q_NULLPTR;

	receiveTextEdit = Q_NULLPTR;
	statusBar = Q_NULLPTR;
}

void MonitorTestTool::runThread()
{
	QtConcurrent::run(this, &MonitorTestTool::readData);
	QtConcurrent::run(this, &MonitorTestTool::recordData);
	QtConcurrent::run(this, &MonitorTestTool::handleData);
}


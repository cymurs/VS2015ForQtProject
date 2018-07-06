#include "stdafx.h"
#include "monitortesttool.h"

MonitorTestTool::MonitorTestTool(QWidget *parent)
	: QDialog(parent)//QMainWindow(parent)
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
	, receiveTextEdit(new QTextEdit(tr("准备中......")))
	, statusBar(new QStatusBar())
	, serial(new QSerialPort(this))
	, rfile(RecordFile)
{
//	ui.setupUi(this);

	
	
	
	

	auto leftLayout = new QVBoxLayout();
	leftLayout->addWidget(createSettingsGroup());
	leftLayout->addWidget(createReceiveGroup());
	leftLayout->addWidget(statusBar);
	setLayout(leftLayout);

	

	//recordThread = new QThread();
	//recordTimer = new QTimer();
	//recordTimer->setInterval(1000);
	//recordTimer->start(2000);
	//recordTimer->moveToThread(recordThread);
	//connect(recordTimer, SIGNAL(timeout()), this, SLOT(recordTimeout()), Qt::DirectConnection);
	//connect(this, SIGNAL(stop()), recordTimer, SLOT(stop()));
	//recordThread->start();

	initWidgetsConnections();
}

MonitorTestTool::~MonitorTestTool()
{
	emit stop();
	//recordThread->quit();
	//recordThread->wait();
	//delete recordTimer;
	//delete recordThread;

	closeSerialPort();
	delete serial;

	if (rfile.isOpen())
		rfile.close();
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
		stopButton->setEnabled(true);

		QtConcurrent::run(this, &MonitorTestTool::readData);		
	}
	else {
		QMessageBox::critical(this, tr("Critical Error"), tr("打开端口%1失败!").arg(portName));
		
	}
}

void MonitorTestTool::closeSerialPort()
{
	if (serial->isOpen()) {
		isOpened = false;
		serial->close();		
	}		
	runButton->setEnabled(true);
	stopButton->setEnabled(false);
}

void MonitorTestTool::recordTimeout()
{
	QString strWriting("");
	{
		QMutexLocker locker(&recordMutex);
		record.swap(strWriting);
	}
	
	if (!rfile.open(QIODevice::ReadWrite | QIODevice::Text)) {
		QMessageBox::critical(this, tr("critical error"), tr("Open %1 failed!").arg(RecordFile));
		qDebug() << strWriting;
	}

	QTextStream in(&rfile);
	in << strWriting;
}

void MonitorTestTool::readData()
{
	while (isOpened) {
		QByteArray data = serial->readAll();
		if (serial->bytesAvailable() > 0)
			queue.Push(data);
	}
}

void MonitorTestTool::handleReceivedData()
{
	QString data;
	while (queue.Pop(data)) {
		receiveTextEdit->moveCursor(QTextCursor::Start);
		receiveTextEdit->insertPlainText(data);		

		{
			QMutexLocker locker(&recordMutex);
			record.append(data);
		}
		
	}
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

void MonitorTestTool::initWidgetsConnections()
{
	connect(parityComboBox, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged),
		this, &MonitorTestTool::filterChanged);
	connect(runButton, &QPushButton::clicked, this, &MonitorTestTool::openSerialPort);
	connect(stopButton, &QPushButton::clicked, this, &MonitorTestTool::closeSerialPort);
}

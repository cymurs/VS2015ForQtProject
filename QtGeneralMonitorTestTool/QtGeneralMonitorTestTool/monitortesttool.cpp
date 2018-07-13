#include "stdafx.h"
#include "monitortesttool.h"

MonitorTestTool::MonitorTestTool(QWidget *parent)
	: QDialog(parent)//QMainWindow(parent)
	, isOpened(true)
	, serialPortLabel(new QLabel(tr("�˿ں�: ")))
	, serialPortComboBox(new QComboBox())
	, baudRateLabel(new QLabel(tr("������: ")))
	, baudRateComboBox(new QComboBox())
	, dataBitsLabel(new QLabel(tr("����λ: ")))
	, dataBitsComboBox(new QComboBox())
	, stopBitsLabel(new QLabel(tr("ֹͣλ: ")))
	, stopBitsComboBox(new QComboBox())
	, parityLabel(new QLabel(tr("У��λ: ")))
	, parityComboBox(new QComboBox())
	, runButton(new QPushButton(tr("�򿪴���")))
	, stopButton(new QPushButton(tr("ֹͣ")))
	, dataFileLabel(new QLabel(tr("����Դλ��:")))
	, dataFileLineEdit(new QLineEdit())
	, selectButton(new QPushButton(tr("���...")))
	, confirmButton(new QPushButton(tr("����")))
	, cancelButton(new QPushButton(tr("���")))
	, receiveTextEdit(new QTextEdit(tr("׼����......")))
	, statusBar(new QStatusBar())
	, serial(new QSerialPort(this))
	, dataFile("")
	, maxSize(10)
	, maxX(50)
	, maxY(100)
	, randCount(1)
	, dataTable(generateRandomData(1, maxY, randCount))
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
	// ��������ʱ�̶�����
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
		QMessageBox::critical(this, tr("Critical Error"), tr("�򿪶˿�%1ʧ��!").arg(portName));
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
	dataFile = QFileDialog::getOpenFileName(this, tr("����Դλ��"), 
		QDir::currentPath(), tr("CSV files (*.csv);;Text files (*.txt)"));
	dataFileLineEdit->setText(dataFile);
}

void MonitorTestTool::runImport()
{
	if (dataFile.isEmpty()) return;
	if (!QFile::exists(dataFile)) {
		QMessageBox::critical(this, tr("����"), tr("%1 ������, ������ѡ��").arg(dataFile));
		selectButton->setFocus();
		return;
	}

	QFile file(dataFile);	
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		statusBar->showMessage(tr("��%1ʧ��!").arg(dataFile));
		QMessageBox::critical(this, tr("����"), tr("��%1ʧ��!").arg(dataFile));
		return;
	}

	//receiveTextEdit->clear();
	
	if (file.size() > 1024 * 5) {
		QTextStream in(&file);
		QString lines = in.readAll();
		qint32 max = lines.count('\n');		
		QScopedPointer<QProgressDialog> pd( createProgressDialog(0, max, tr("���ڵ���..."), tr("��������")) );		
		qint32 idx = 1;
		QStringList list = lines.split('\n');
		for (QString line : list)
		{
			//if (line.isEmpty()) continue;
			line.append('\n');
			showData(line);
			pd->setValue(idx);
			++idx;

			if (pd->wasCanceled()) {
				receiveTextEdit->clear();
				break;
			}
		}
		pd->close();
	} else {
		QTextStream in(&file);
		QString line;
		while (in.readLineInto(&line)) {
			//if (line.isEmpty()) continue;
			line.append('\n');
			showData(line);
		}	
		in.flush();
	}
	file.close();

	showMessage(tr("�������!"));
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
			// ÿ�� 2s дһ���ļ�
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
			
			
			QMutexLocker locker(&writeMutex);
			QTextStream out(&rfile);
			out << strWriting;
			
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
	static qint32 circle(0);

	if (circle > maxSize - 2) {  // ��Ϊ��һ����ʾ�����Բ���maxSize - 1֮���Ҫ��ʼɾ������֤����ΪmaxSize
		receiveTextEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);  // λ�������һ��֮��
		receiveTextEdit->moveCursor(QTextCursor::Up, QTextCursor::KeepAnchor);  // �������ϲ���ѡ�����һ��
		receiveTextEdit->textCursor().removeSelectedText();
	}
	receiveTextEdit->moveCursor(QTextCursor::Start);
	receiveTextEdit->insertPlainText(data);
	++circle;	
}

void MonitorTestTool::changeSeries(const QString &)
{
	//QThread::msleep(500);
	
	static qint32 id = randCount;
	int diff = scatterSeries->count() - maxSize;
	if (diff > 0) {
		scatterSeries->removePoints(0, diff);
		splineSeries->removePoints(0, diff);
	}

	qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
	for(DataList &dataList : dataTable) {		
		//qint32 id = dataList.back().second.trimmed().section(':', 1, 1).toInt();
		//qreal yValue = (qreal)(qrand() % maxY) / (qreal)maxSize;
		//QPointF value((id + (qreal)rand() / (qreal)RAND_MAX) * ((qreal)20 / (qreal)maxSize),
		//	yValue);
		qreal yValue = 25 + (qreal)(qrand() % (maxY / 2));
		QPointF value(id, yValue);
		QString label = "Slice 0:" + QString::number(id);
		dataList << Data(value, label);
		scatterSeries->append(value);
		splineSeries->append(value);		
		if (dataList.size() > maxSize) {
			dataList.removeFirst();
			//splineSeries->remove(0);
			//scatterSeries->remove(0);
			splineChart->axisX()->setRange(id - maxSize + 1, id);
		}
		
	}	

	//if (isVisible()) {
	//	splineSeries->clear();
	//	scatterSeries->clear();

	//	//QString name("Series ");
	//	//int nameIndex = 0;
	//	foreach(DataList list, dataTable) {
	//		foreach(Data data, list) {
	//			splineSeries->append(data.first);
	//			scatterSeries->append(data.first);
	//		}
	//		//splineSeries->setName(name + QString::number(nameIndex));
	//		//nameIndex++;
	//	}
	//	splineChart->axisX()->setRange(id - maxSize + 1, id);
	//}
	
	// ��һ��ֵ�ı��
	++id;
	if (0 == id % 3)
		QCoreApplication::processEvents();
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
		// ���Դ���
		data = std::move(QTime::currentTime().toString(tr("hh:mm:ss\n")));
		emit sendData(data);
		QThread::msleep(500);
		
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
			//QPointF value((j + (qreal)rand() / (qreal)RAND_MAX) * ((qreal)20 / (qreal)valueCount),
			//	yValue);
			QPointF value(j, yValue);
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

	baudRateComboBox->addItem(tr("1200"), QVariant::fromValue(QSerialPort::Baud1200));
	baudRateComboBox->addItem(tr("2400"), QVariant::fromValue(QSerialPort::Baud2400));
	baudRateComboBox->addItem(tr("4800"), QVariant::fromValue(QSerialPort::Baud4800));
	baudRateComboBox->addItem(tr("9600"), QVariant::fromValue(QSerialPort::Baud9600));
	baudRateComboBox->addItem(tr("19200"), QVariant::fromValue(QSerialPort::Baud19200));
	baudRateComboBox->addItem(tr("38400"), QVariant::fromValue(QSerialPort::Baud38400));
	baudRateComboBox->addItem(tr("57600"), QVariant::fromValue(QSerialPort::Baud57600));
	baudRateComboBox->addItem(tr("115200"), QVariant::fromValue(QSerialPort::Baud115200));

	dataBitsComboBox->addItem(tr("5"));
	dataBitsComboBox->addItem(tr("6"));
	dataBitsComboBox->addItem(tr("7"));
	dataBitsComboBox->addItem(tr("8"));

	stopBitsComboBox->addItem(tr("1"));
	stopBitsComboBox->addItem(tr("2"));
	stopBitsComboBox->addItem(tr("3"));

	parityComboBox->addItem(tr("��"), QVariant::fromValue(QSerialPort::OddParity));
	parityComboBox->addItem(tr("ż"), QVariant::fromValue(QSerialPort::EvenParity));
	parityComboBox->addItem(tr("��"), QVariant::fromValue(QSerialPort::NoParity));

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
	settingsLayout->addWidget(runButton, 2, 2);
	settingsLayout->addWidget(stopButton, 2, 3);	

	auto settingsGroupBox = new QGroupBox(tr("���ô���"));
	settingsGroupBox->setLayout(settingsLayout);

	serialPortComboBox->setFocus();
	stopButton->setEnabled(false);
	return settingsGroupBox;
}

QGroupBox * MonitorTestTool::createReceiveGroup()
{	
	auto receiveLayout = new QVBoxLayout();
	receiveLayout->addWidget(receiveTextEdit);

	auto receiveGropBox = new QGroupBox(tr("��������"));
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
	
	auto openingGroupBox = new QGroupBox(tr("��������"));
	openingGroupBox->setLayout(importLayout);

	return openingGroupBox;
}

QGroupBox *MonitorTestTool::createChartGroup()
{
	// spine chart
	splineChart = new QChart();
	splineChart->setTitle("serial port realtime chart");
	QString name("Series ");
	int nameIndex = 0;
	foreach(DataList list, dataTable) {
		//QSplineSeries *series = new QSplineSeries(splineChart);
		splineSeries = new QSplineSeries(splineChart);
		scatterSeries = new QScatterSeries(splineChart);
		scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
		scatterSeries->setMarkerSize(5);
		foreach(Data data, list) {
			splineSeries->append(data.first);
			scatterSeries->append(data.first);
		}
		splineSeries->setName(name + QString::number(nameIndex));
		scatterSeries->setName(name + QString::number(nameIndex));
		nameIndex++;
		splineChart->addSeries(splineSeries);
		splineChart->addSeries(scatterSeries);
	}
	splineChart->setAnimationOptions(QChart::SeriesAnimations);
	splineChart->createDefaultAxes();
	splineChart->axisY()->setRange(0, maxY);
	//splineChart->axisX()->setRange(0, maxSize);
	QValueAxis *axisX = new QValueAxis();
	axisX->setRange(0, maxSize - 1);
	axisX->setLabelFormat("%u"); //���ÿ̶ȵĸ�ʽ
	axisX->setGridLineVisible(true);
	axisX->setTickCount(maxSize);     //���ö��ٸ�
	//axisX->setMinorTickCount(3); //����ÿ��С�̶��ߵ���Ŀ	
	// ��Ҫ��x����series��
	splineChart->setAxisX(axisX, splineSeries);
	splineChart->setAxisX(axisX, scatterSeries);

	auto splineView = new QChartView(splineChart);
	splineView->setRenderHint(QPainter::Antialiasing); // ͼ�񿹾��
	splineView->setSceneRect(0, 0, 630, 280);	 // ���ó�ʼͼ���С

	auto chartLayout = new QVBoxLayout;
	chartLayout->setContentsMargins(0, 0, 0, 0);
	chartLayout->addWidget(splineView);

	auto chartGroupBox = new QGroupBox(tr("ʵʱ��������ͼ"));
	chartGroupBox->setLayout(chartLayout);
	chartGroupBox->setMinimumSize(400, 450); // ����ͼ��չʾ��Сֵ
	return chartGroupBox;
}

QProgressDialog * MonitorTestTool::createProgressDialog(qint32 min, qint32 max, const QString &text, const QString &title)
{
	QProgressDialog *progressDlg = new QProgressDialog(this); //��ʵ��һ�����Ѿ���ʼ��ʾ��������
	progressDlg->setWindowModality(Qt::WindowModal);
	progressDlg->setAttribute(Qt::WA_DeleteOnClose, true);
	progressDlg->setMinimumDuration(0);	
	progressDlg->setWindowTitle(title);
	progressDlg->setLabelText(text);
	progressDlg->setCancelButtonText(tr("ȡ��"));
	progressDlg->setRange(min, max);
	
	return progressDlg;
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
	connect(this, SIGNAL(sendData(QString)), this, SLOT(changeSeries(QString)));
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
	delete splineChart;
	delete splineSeries;
	delete scatterSeries;


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
	splineChart = Q_NULLPTR;
	splineSeries = Q_NULLPTR;
	scatterSeries = Q_NULLPTR;
}

void MonitorTestTool::runThread()
{
	QtConcurrent::run(this, &MonitorTestTool::readData);
	QtConcurrent::run(this, &MonitorTestTool::recordData);
	QtConcurrent::run(this, &MonitorTestTool::handleData);
}


#include "stdafx.h"
#include "monitortesttool.h"

MonitorTestTool::MonitorTestTool(QWidget *parent)
	: QDialog(parent)//QMainWindow(parent)
	, isOpened(true)
	, isQuit(false)
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
	, serialMutex(QMutex::Recursive)
	, dataFile("")
	, maxSize(10)
	, maxX(50)
	, maxY(100)
	, randCount(1)
	, dataTable(generateRandomData(1, maxY, randCount))
	, frameHead("A5")
	, frameTail("AA")
{
//	ui.setupUi(this);

	// 增加最大最小化按钮
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	//setupStatusBar();
	
	

	auto leftLayout = new QVBoxLayout();
	leftLayout->addWidget(createSettingsGroup());
	leftLayout->addWidget(createImportGroup());
	leftLayout->addWidget(createReceiveGroup());	
	//leftLayout->addWidget(statusBar);	


	auto baseLayout = new QGridLayout();
	baseLayout->addLayout(leftLayout, 0, 0);
	baseLayout->addWidget(createChartGroup(), 0, 1);
	baseLayout->addWidget(statusBar, 1, 0, 1, 2);
	// 设置拉升时固定比例
	baseLayout->setColumnStretch(0, 1);
	baseLayout->setColumnStretch(1, 4);
	setLayout(baseLayout);
	
	
	
	initWidgetsStyle();
	initWidgetsConnections();
	
	//windowTimer.setInterval(interval);

	//emit debugUpdate(quintptr(QThread::currentThreadId()), tr("MonitorTestTool"));
	//QMessageBox::critical(this, tr("Critical Test"), tr("[%1]MonitorTestTool").arg(quintptr(QThread::currentThreadId())));
}

MonitorTestTool::~MonitorTestTool()
{
	closeSerialPort();
	retRead.waitForFinished();
	retRecord.waitForFinished();
	retHandle.waitForFinished();
	//QMutexLocker locker(&serialMutex);
	//if (serial && serial->isOpen())
	//	serial->close();
	//delete serial;
	//serial = Q_NULLPTR;
	//deleteWidgets();
}

void MonitorTestTool::filterChanged(const QString &ret)
{
	QString s = QString(tr("[%1]data=%2;text=%3\n"))
		.arg(ret)
		.arg(parityComboBox->currentData().toInt())
		.arg(parityComboBox->currentText());

	onShow(s);
	showMessage(s);
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
	if (!serial->isOpen()) {
		if (!serial->open(QIODevice::ReadWrite)) {
			QMessageBox::critical(this, tr("Critical Error"), tr("打开端口%1失败: %2").arg(portName).arg(serial->errorString()));
			showMessage(tr("open serial port failed!\n"));
		}
	}
	if (serial->isOpen()) {
		isOpened = true;
		runButton->setEnabled(false);		
		confirmButton->setEnabled(false);
		cancelButton->setEnabled(false);
		stopButton->setEnabled(true);		
		windowTimer.start(interval);

		runThread();				
	}
}

void MonitorTestTool::closeSerialPort()
{	
	isOpened = false;	
	//if (serialMutex.tryLock(100)) {
	//	if (serial && serial->isOpen()) {			
	//		serial->clear();
	//		serial->clearError();
	//		serial->close();
	//	}
	//	serialMutex.unlock();
	//}
	if (runButton) {
		runButton->setEnabled(true);		
		confirmButton->setEnabled(true);
		cancelButton->setEnabled(true);
		stopButton->setEnabled(false);
	}
	if (windowTimer.isActive())
		windowTimer.stop();	
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
	
	//if (file.size() > 1024 * 5) {
		QTextStream in(&file);
		QString lines = in.readAll();
		onShow(lines, true);
		//qint32 max = lines.count('\n');		
		//QScopedPointer<QProgressDialog> pd(createProgressDialog(0, max, tr("正在导入..."), tr("导入数据"))); 		
		//qint32 idx = 1;
		//QStringList list = lines.split('\n');
		//for (QString line : list)
		//{
		//	//if (line.isEmpty()) continue;
		//	line.append('\n');
		//	onShow(line);
		//	pd->setValue(idx);
		//	++idx;

		//	if (pd->wasCanceled()) {
		//		receiveTextEdit->clear();
		//		break;
		//	}
		//}
		//pd->close();
	//} else {
	//	QTextStream in(&file);
	//	QString line;
	//	while (in.readLineInto(&line)) {
	//		//if (line.isEmpty()) continue;
	//		line.append('\n');
	//		onShow(line);
	//	}	
	//	in.flush();
	//}
	file.close();

	showMessage(tr("导入完成!"));
}

void MonitorTestTool::cancelImport()
{
	receiveTextEdit->clear();
	if (dataFile.isEmpty())  return;	
	dataFileLineEdit->clear();
	dataFile.clear();
	selectButton->setFocus();
}

void MonitorTestTool::recordData()
{	
	if (!isQuit)
		emit debugUpdate(quintptr(QThread::currentThreadId()), tr("recordData restart"));

	QFile rfile(RecordFile);
	while (isOpened) { // while (isOpened || !record.isEmpty())
		try {			
			// 每隔 2s 写一次文件
			QThread::sleep(2);
			
			QString strWriting("");
			{
				QMutexLocker locker(&recordMutex);
				record.swap(strWriting);
			}

			// 测试
			//qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
			//QStringList list = { "one", "two", "three" };
			//qint32 cur = qrand() % 3;
			//QString strWriting = QTime::currentTime().toString(tr("hh:mm:ss "));
			//strWriting.push_back(list[cur]);
			//strWriting.push_back(tr("\n"));

			if (!rfile.isOpen()) {
				if (!rfile.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append)) {
					//QMessageBox::critical(this, tr("critical error"), tr("Open %1 failed!").arg(RecordFile));
					//statusBar->showMessage(strWriting);
					emit sendData(tr("Open %1 failed!").arg(RecordFile));
				}
			}
			
			
			QMutexLocker locker(&writeMutex);
			QTextStream out(&rfile);
			out << strWriting;
			
		}
		catch (...) {
			emit debugUpdate(quintptr(QThread::currentThreadId()), tr("recordData error."));
		}

	}
	if (rfile.isOpen())
		rfile.close();

	//if (!isQuit)
	//	emit debugUpdate(quintptr(QThread::currentThreadId()), tr("recordData quit"));
}

// 测试
void MonitorTestTool::debugTips(qint64 tid, const QString &where)
{
	//QMessageBox::information(this, tr("debug tips"), tr("%1 threadId=%2").arg(where).arg(tid));
	if (statusBar) {		
		statusBar->showMessage(tr("[%1]%2.").arg(tid).arg(where), 4000);
		//threadLabel->setText(tr("[%1]%2.").arg(tid).arg(where));
	}
}

void MonitorTestTool::onShow(const QString &data, bool overwrite)
{
	if (overwrite) {
		receiveTextEdit->setPlainText(data);
		return;
	}

	qint32 cnt = receiveTextEdit->document()->lineCount();
	qint32 diff = cnt - maxSize + 1;

	if (diff > 0) {
		receiveTextEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);  // 位置在最后一行之后
		receiveTextEdit->moveCursor(QTextCursor::Up, QTextCursor::KeepAnchor);  // 所以向上才能选中最后一行
		--diff;
		while (diff > 0) {
			receiveTextEdit->moveCursor(QTextCursor::Up, QTextCursor::KeepAnchor);  // 所以向上才能选中最后一行
			
			--diff;
		}	
		receiveTextEdit->textCursor().removeSelectedText();
	}
	receiveTextEdit->moveCursor(QTextCursor::Start);
	receiveTextEdit->insertPlainText(data);
	//receiveTextEdit->insertHtml(data);
}

void MonitorTestTool::onShow(const QStringList &dataList)
{
	QString data = dataList.join('\n');
	onShow(data);
}

void MonitorTestTool::onSeriesChanged(const QString &data)
{	
	// 测试
	//qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
	//static qint32 id = randCount;
	//	
	//int diff = scatterSeries->count() - maxSize;
	//if (diff > 0) {
	//	scatterSeries->removePoints(0, diff);
	//	splineSeries->removePoints(0, diff);
	//}
	//
	//for(DataList &dataList : dataTable) {		
	//	//qint32 id = dataList.back().second.trimmed().section(':', 1, 1).toInt();
	//	//qreal yValue = (qreal)(qrand() % maxY) / (qreal)maxSize;
	//	//QPointF value((id + (qreal)rand() / (qreal)RAND_MAX) * ((qreal)20 / (qreal)maxSize),
	//	//	yValue);
	//	qreal yValue = 25 + (qreal)(qrand() % (maxY / 2));
	//	QPointF value(id, yValue);
	//	QString label = "Slice 0:" + QString::number(id);
	//	dataList << Data(value, label);
	//	scatterSeries->append(value);
	//	splineSeries->append(value);		
	//	if (dataList.size() > maxSize) {
	//		dataList.removeFirst();
	//		//splineSeries->remove(0);
	//		//scatterSeries->remove(0);
	//		splineChart->axisX()->setRange(id - maxSize + 1, id);
	//	}
	//	
	//}	

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
	
	static qint32 id(0);

	int diff = scatterSeries->count() - maxSize;
	if (diff > 0) {
		scatterSeries->removePoints(0, diff);
		splineSeries->removePoints(0, diff);
	}

	double y;
	if (1 == data.count('\n')) {
		y = data.toDouble();
		if (y > maxY) {
			showMessage(tr("Axis Y(=%1) bigger than maxY(=%2)! ").arg(y).arg(maxY));
			return;
		}
		QPointF value(id, y);
		
		splineSeries->append(value);
		scatterSeries->append(value);
		++id;
	} else {
		PointList points;
		QVector<QStringRef> vecData = data.splitRef('\n', QString::SkipEmptyParts);
		for(const QStringRef &d : vecData) {
			y = d.toDouble();
			if (y > maxY) {
				showMessage(tr("Axis Y(=%1) bigger than maxY(=%2)! ").arg(y).arg(maxY));
				continue;
			}
			QPointF value(id, y);
			points.append(value);
			++id;
		}
		
		splineSeries->append(points);
		scatterSeries->append(points);
	}
	if (id > maxSize) {
		splineChart->axisX()->setRange(id - maxSize, id);
		//splineChart->scroll(splineChart->plotArea().width() / axisX->tickCount(), 0);
	}
	
	// 下一次值的编号
	//++id;
	// 保证对界面其他操作的响应
	//if (0 == id % 2)
	//	QCoreApplication::processEvents();
}

void MonitorTestTool::onSeriesChanged(const QStringList &dataList)
{
	QString data = dataList.join('\n');
	onSeriesChanged(data);
}

void MonitorTestTool::onDataUpdateTimer()
{
	QString *packet;
	QString data("");
	qint32 count = windowQueue.get(&packet);	
	//QElapsedTimer timer;
	//timer.start();
	
	if (count > 0) {
		data = packet[0];
		if (count > 1) {
			for (qint32 i=1; i<count; ++i)
				data.sprintf("%s\n%s", qPrintable(data), qPrintable(packet[i]));
		}
		data.append("\n");
		showMessage(data);
		onSeriesChanged(data);
		onShow(data);
	}

	//if (timer.elapsed() > 200)
	//	QCoreApplication::processEvents();
}

void MonitorTestTool::showMessage(const QString &s)
{
	statusBar->showMessage(s);
}

void MonitorTestTool::readData()
{
	if (!isQuit)
		emit debugUpdate(quintptr(QThread::currentThreadId()), tr("readData begin"));

	while (isOpened) {
		QByteArray data;
		QMutexLocker locker(&serialMutex);
		if (!serial->isOpen()) break;
		if (serial->waitForReadyRead(100)) {
			QStringList list;
			data = std::move(serial->readAll());
			parseData(data, list);
			for (const QString d : list)
				queue.Push(d);
			//data.clear();
		}
		QString err;
		if (serial->error() == QSerialPort::ReadError) {
			err = QObject::tr("An I/O error occurred while reading the data from port %1, error: %2").arg(serial->portName()).arg(serial->errorString());			
		} else if (serial->error() == QSerialPort::TimeoutError && data.isEmpty()) {
			err = QObject::tr("No data was currently available for reading from port %1").arg(serial->portName());
		}
		if (!err.isEmpty())
			emit sendData(err);
	}

	if (!isQuit)
		emit debugUpdate(quintptr(QThread::currentThreadId()), tr("readData end"));
}

void MonitorTestTool::handleData()
{	
	if (!isQuit)
		emit debugUpdate(quintptr(QThread::currentThreadId()), tr("handleData start"));

	QString data;
	do 
	{
		// 测试代码
		//data = std::move(QTime::currentTime().toString(tr("hh:mm:ss\n")));
		////data = std::move(tr("<table border=\"1\"><tr><td>%1</td><td>num</td></tr></table>\n").arg(QTime::currentTime().toString("hh:mm:ss")) );				
		//windowQueue.put(data);
		//QThread::msleep(100 * interval);
		
		while (queue.Pop(data, false)) {			
			windowQueue.put(data);
			{
				QMutexLocker locker(&recordMutex);
				data.append("\n");
				record.append(data);
			}

		}
	} while (isOpened || (!isQuit && !queue.Empty()));
	
	//if (!isQuit)
	//	emit debugUpdate(quintptr(QThread::currentThreadId()), tr("handleData stop"));
}

void MonitorTestTool::closeEvent(QCloseEvent *e)
{
	isQuit = true;
	closeSerialPort();
	// 等待读串口线程退出
	//QMutexLocker locker(&serialMutex);
	//if (serial && serial->isOpen())
	//	serial->close();  // 在serial析构时，已经执行
	//delete serial;  // 因为构建了对象树，在父类析构时执行
	//serial = Q_NULLPTR;
	//deleteWidgets();
}

// 测试
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
	baudRateComboBox->setCurrentText(tr("115200"));

	dataBitsComboBox->addItem(tr("5"));
	dataBitsComboBox->addItem(tr("6"));
	dataBitsComboBox->addItem(tr("7"));
	dataBitsComboBox->addItem(tr("8"));
	dataBitsComboBox->setCurrentText(tr("8"));

	stopBitsComboBox->addItem(tr("1"));
	stopBitsComboBox->addItem(tr("2"));
	stopBitsComboBox->addItem(tr("3"));
	stopBitsComboBox->setCurrentText(tr("1"));

	parityComboBox->addItem(tr("奇"), QVariant::fromValue(QSerialPort::OddParity));
	parityComboBox->addItem(tr("偶"), QVariant::fromValue(QSerialPort::EvenParity));
	parityComboBox->addItem(tr("无"), QVariant::fromValue(QSerialPort::NoParity));
	parityComboBox->setCurrentText(tr("无"));

	auto settingsLayout = new QGridLayout();
	settingsLayout->addWidget(serialPortLabel, 0, 0);
	settingsLayout->addWidget(serialPortComboBox, 0, 1);	
	settingsLayout->addWidget(baudRateLabel, 0, 3);
	settingsLayout->addWidget(baudRateComboBox, 0, 4);
	settingsLayout->addWidget(dataBitsLabel, 1, 0);
	settingsLayout->addWidget(dataBitsComboBox, 1, 1);
	settingsLayout->addWidget(stopBitsLabel, 1, 3);
	settingsLayout->addWidget(stopBitsComboBox, 1, 4);
	settingsLayout->addWidget(parityLabel, 2, 0);
	settingsLayout->addWidget(parityComboBox, 2, 1);
	settingsLayout->addWidget(runButton, 2, 3);
	settingsLayout->addWidget(stopButton, 2, 4);
	settingsLayout->setColumnMinimumWidth(2, 60);	

	auto settingsGroupBox = new QGroupBox(tr("串口设置"));
	settingsGroupBox->setLayout(settingsLayout);

	serialPortComboBox->setFocus();
	stopButton->setEnabled(false);
	return settingsGroupBox;
}

QGroupBox * MonitorTestTool::createReceiveGroup()
{	
	auto receiveLayout = new QGridLayout();
	receiveLayout->addWidget(receiveTextEdit, 0, 0, 1, 5);
	receiveLayout->addWidget(cancelButton, 1, 4);

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
	importLayout->addWidget(confirmButton, 1, 4);
	//importLayout->addWidget(cancelButton, 2, 3);
	
	auto openingGroupBox = new QGroupBox(tr("导入数据"));
	openingGroupBox->setLayout(importLayout);

	return openingGroupBox;
}

QGroupBox *MonitorTestTool::createChartGroup()
{
	// spine chart
	splineChart = new QChart();
	splineChart->setTitle("Realtime Serial Data Chart");
	splineChart->setTheme(QChart::ChartThemeBlueNcs);
	//splineChart->setAnimationOptions(QChart::SeriesAnimations);
	splineChart->legend()->hide();
	QString name("Series ");
	int nameIndex = 0;
	foreach(DataList list, dataTable) {
		//QSplineSeries *series = new QSplineSeries(splineChart);
		splineSeries = new QSplineSeries(splineChart);		
		scatterSeries = new QScatterSeries(splineChart);
		scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
		scatterSeries->setMarkerSize(3);
		PointList points;
		foreach(Data data, list) {
			points.append(data.first);
		}
		splineSeries->append(points);
		scatterSeries->append(points);
		splineSeries->setName(name + QString::number(nameIndex));
		scatterSeries->setName(name + QString::number(nameIndex));		
		splineSeries->setUseOpenGL(true);
		scatterSeries->setUseOpenGL(true);
		splineChart->addSeries(splineSeries);
		splineChart->addSeries(scatterSeries);
		nameIndex++;
	}	
	splineChart->createDefaultAxes();		
	axisX = new QValueAxis();
	axisX->setRange(0, maxSize - 1);
	axisX->setLabelFormat("%u"); //设置刻度的格式
	axisX->setGridLineVisible(true);
	axisX->setTickCount(maxSize);     //设置多少格
	axisX->setTitleText(tr("Data point"));
	//axisX->setMinorTickCount(1); //设置每格小刻度线的数目	
	axisY = new QValueAxis();
	axisY->setRange(0, maxY);
	axisY->setTitleText(tr("Values"));
	// 需要将坐标轴与series绑定
	splineChart->setAxisX(axisX, splineSeries);
	splineChart->setAxisX(axisX, scatterSeries);
	splineChart->setAxisY(axisY, splineSeries);
	splineChart->setAxisY(axisY, scatterSeries);

	auto splineView = new QChartView(splineChart);
	splineView->setRenderHint(QPainter::Antialiasing); // 图像抗锯齿
	splineView->setSceneRect(0, 0, 630, 280);	 // 设置初始图表大小
	disconnect(splineSeries, SIGNAL(pointRemoved(int)), splineView, SLOT(update()));
	
	auto chartLayout = new QVBoxLayout;
	chartLayout->setContentsMargins(0, 0, 0, 0);
	chartLayout->addWidget(splineView);

	auto chartGroupBox = new QGroupBox(tr("实时数据曲线图"));
	chartGroupBox->setLayout(chartLayout);
	chartGroupBox->setMinimumSize(400, 450); // 设置图表展示最小值
	return chartGroupBox;
}

QProgressDialog * MonitorTestTool::createProgressDialog(qint32 min, qint32 max, const QString &text, const QString &title)
{
	QProgressDialog *progressDlg = new QProgressDialog(this); //其实这一步就已经开始显示进度条了
	progressDlg->setWindowModality(Qt::WindowModal);
	progressDlg->setAttribute(Qt::WA_DeleteOnClose, true);
	progressDlg->setMinimumDuration(0);	
	progressDlg->setWindowTitle(title);
	progressDlg->setLabelText(text);
	progressDlg->setCancelButtonText(tr("取消"));
	progressDlg->setRange(min, max);
	
	return progressDlg;
}

void MonitorTestTool::setupStatusBar()
{
	receiveLabel = new QLabel();
	receiveLabel->setMinimumSize(receiveLabel->sizeHint());

	threadLabel = new QLabel();
	threadLabel->setMinimumSize(threadLabel->sizeHint());

	statusBar->addWidget(receiveLabel);
	statusBar->addWidget(threadLabel);
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
	connect(&windowTimer, &QTimer::timeout, this, &MonitorTestTool::onDataUpdateTimer);

	connect(this, SIGNAL(debugUpdate(qint64, QString)), this, SLOT(debugTips(qint64, QString)), Qt::QueuedConnection);	
	connect(this, SIGNAL(sendData(QString)), statusBar, SLOT(showMessage(QString)));

	//connect(this, SIGNAL(sendData(QString)), this, SLOT(showData(QString)));
	//connect(this, SIGNAL(sendData(QString)), this, SLOT(onSeriesChanged(QString)));
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
	retRead = QtConcurrent::run(this, &MonitorTestTool::readData);
	retRecord = QtConcurrent::run(this, &MonitorTestTool::recordData);
	retHandle = QtConcurrent::run(this, &MonitorTestTool::handleData);
}

bool MonitorTestTool::parseData(const QString &data, QStringList &result)
{
	if (!data.startsWith(frameHead)) return false;

	QStringList subs = data.split(frameHead, QString::SkipEmptyParts);
	for (const QString &sub : subs) {
		int value;		
		sscanf(sub.toStdString().c_str(), "%dAA", &value);
		result.append(QString::number(value));
	}

	return true;
}


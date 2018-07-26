#include "stdafx.h"
#include "monitortesttool.h"

MonitorTestTool::MonitorTestTool(QWidget *parent)
	: QDialog(parent)//QMainWindow(parent)
	, isOpened(true)
	, isQuit(false)
	, statusBar(new QStatusBar())
	, serial(new QSerialPort(this))
	, serialMutex(QMutex::Recursive)
	, dataFile("")	
	, record("")
	, minY(0)
	, maxY(100)
	, tickCount(10)
	, samplingNum(10000)
	, randCount(1)
	, dataTable(generateRandomData(1, maxY, randCount))
	, frameHead("A5")
	, frameTail("AA")
	, channel(CH1)
{
	//ui.setupUi(this);
	// 增加最大最小化按钮
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	setWindowTitle("Real-Time Monitor Test Tool");
	//setupStatusBar();
	
	
	auto leftLayout = new QVBoxLayout();
	leftLayout->addWidget(createSettingsGroup());
	leftLayout->addWidget(createFrameFormatGroup());
	leftLayout->addWidget(createImportGroup());
	leftLayout->addWidget(createReceiveGroup());	
	//leftLayout->addWidget(statusBar);	


	auto baseLayout = new QGridLayout();
	baseLayout->addLayout(leftLayout, 0, 0, 2, 1);
	baseLayout->addWidget(createSamplingGroup(), 0, 1);
	baseLayout->addWidget(createChartGroup(), 1, 1);
	baseLayout->addWidget(statusBar, 2, 0, 1, 2);
	// 设置拉升时固定比例
	baseLayout->setColumnStretch(0, 1);
	baseLayout->setColumnStretch(1, 4);
	setLayout(baseLayout);
	
		
	initWidgetsStyle();
	initWidgetsConnections();
	

	// 初始化日志对象
	SingleLogger::instance().setLogFile("logs/record.log");
	QString recordname = QDateTime::currentDateTime().toString("yyyyMMdd_hh");
	recordname.append(".csv");
	rfile.setFileName(recordname);


	//emit debugUpdate(quintptr(QThread::currentThreadId()), tr("MonitorTestTool"));
	//QMessageBox::critical(this, tr("Critical Test"), tr("[%1]MonitorTestTool").arg(quintptr(QThread::currentThreadId())));
}

MonitorTestTool::~MonitorTestTool()
{
	closeSerialPort();
	retRead.waitForFinished();
	retRecord.waitForFinished();
	retHandle.waitForFinished();
	// 终止日志线程
	SingleLogger::instance().stop();
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
		importButton->setEnabled(false);
		clearButton->setEnabled(false);
		saveButton->setEnabled(false);
		stopButton->setEnabled(true);		
		windowTimer.start(interval);
		recordTimer.start(RecordInterval);
		frameHead = fheadLineEdit->text();
		frameTail = ftailLineEdit->text();

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
		importButton->setEnabled(true);
		clearButton->setEnabled(true);
		saveButton->setEnabled(true);
		stopButton->setEnabled(false);
	}
	if (windowTimer.isActive())
		windowTimer.stop();	
	if (recordTimer.isActive())
		recordTimer.stop();
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

	if (file.size() > 1024 * 1024 * 500) {
		QMessageBox::information(this, tr("提示"), tr("文件 %1 太大, 无法加载!").arg(dataFile));
		return;
	}

	receiveTextEdit->clear();
	//QSplineSeries *spline = seriesPackets[0].splineSeries;
	//QScatterSeries *scatter = seriesPackets[0].scatterSeries;
	//spline->clear();
	//scatter->clear();
	
	//if (file.size() > 1024 * 5) {
		QTextStream in(&file);
		QString lines = in.readAll();
		//onShow(lines, true);
		int max = lines.count('\n');		
		QScopedPointer<QProgressDialog> pd(createProgressDialog(0, max, tr("正在导入..."), tr("导入数据"))); 		
		int idprog(1), idx(1);
		QStringList list = lines.split('\n');
		for (QString line : list)
		{
			//if (line.isEmpty()) continue;
			/*if (!line.isEmpty()){
				QPointF value(idx, line.toDouble());
				scatter->append(value);
				spline->append(value);
				if (idx >=  tickCount)
					axisX->setRange(idx - tickCount + 1, idx);
			}*/					
			receiveTextEdit->append(line);
			pd->setValue(idprog++);
			if (addSeries(idx, line))
				++idx;

			if (pd->wasCanceled()) {
				receiveTextEdit->clear();
				addSeries(0, "");
				break;
			}
		}
		pd->close();
	//} else {
	//	QTextStream in(&file);
	//	QString line;
	//	//int idx = 1;
	//	while (in.readLineInto(&line)) {
	//		//if (line.isEmpty()) continue;
	//		//if (!line.isEmpty()) {
	//		//	QPointF value(idx, line.toDouble());
	//		//	scatter->append(value);
	//		//	spline->append(value);
	//		//	axisX->setRange(idx - tickCount + 1, idx);
	//		//	++idx;
	//		//}
	//		receiveTextEdit->append(line);
	//	}	
	//}
	file.close();

	showMessage(tr("导入完成!"));
}

void MonitorTestTool::clearData()
{
	receiveTextEdit->clear();
	if (dataFile.isEmpty())  return;	
	dataFileLineEdit->clear();
	dataFile.clear();
	selectButton->setFocus();
}

void MonitorTestTool::saveSamplingSettings()
{
	int curMin = minValueLineEdit->text().toInt();
	int curMax = maxValueLineEdit->text().toInt();
	int curNum = samplingNumLineEdit->text().toInt();
	int curTick = tickCountSpinBox->text().toInt();
	if (curMin == minY && curMax == maxY 
		&& curNum == samplingNum
		&& curTick == tickCount) 
		return;	
	if (curMin >= curMax) {
		QMessageBox::warning(this, tr("警告"), tr("最小采样值不能 ≥ 最大采样值!"));
		minValueLineEdit->setFocus();
		return;
	}	
	if (curNum <= 0) {
		QMessageBox::warning(this, tr("警告"), tr("采样个数必须 > 0!"));
		samplingNumLineEdit->setFocus();
		return;
	}
	
	qreal min = (curMin <= 0 ? 0 : (curMin / 5 * 5));
	qreal max = (curMax % 5 == 0) ? curMax : (curMax / 5 + 1) * 5;
	axisY->setRange(min, max);
	minY = curMin;
	maxY = curMax;
	samplingNum = curNum;
	tickCount = curTick;
	axisX->setTickCount(tickCount);	
	qreal xMax = axisX->max();
	if (xMax < tickCount) {
		axisX->setRange(0, tickCount - 1);
	} else {
		axisX->setRange(xMax - tickCount + 1, xMax);
	}
}

void MonitorTestTool::recordData()
{	
	TRACE_INFO(tr("***%1***recordData enter").arg(quintptr(QThread::currentThreadId())));
	if (!isQuit)
		emit debugUpdate(quintptr(QThread::currentThreadId()), tr("recordData restart"));

	//QFile rfile(RecordFile);
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

	TRACE_INFO(tr("***%1***recordData quit").arg(quintptr(QThread::currentThreadId())));
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
	qint32 diff = cnt - tickCount + 1;

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
	//int diff = scatterSeries->count() - tickCount;
	//if (diff > 0) {
	//	scatterSeries->removePoints(0, diff);
	//	splineSeries->removePoints(0, diff);
	//}
	//
	//for(DataList &dataList : dataTable) {		
	//	//qint32 id = dataList.back().second.trimmed().section(':', 1, 1).toInt();
	//	//qreal yValue = (qreal)(qrand() % maxY) / (qreal)tickCount;
	//	//QPointF value((id + (qreal)rand() / (qreal)RAND_MAX) * ((qreal)20 / (qreal)tickCount),
	//	//	yValue);
	//	qreal yValue = 25 + (qreal)(qrand() % (maxY / 2));
	//	QPointF value(id, yValue);
	//	QString label = "Slice 0:" + QString::number(id);
	//	dataList << Data(value, label);
	//	scatterSeries->append(value);
	//	splineSeries->append(value);		
	//	if (dataList.size() > tickCount) {
	//		dataList.removeFirst();
	//		//splineSeries->remove(0);
	//		//scatterSeries->remove(0);
	//		splineChart->axisX()->setRange(id - tickCount + 1, id);
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
	//	splineChart->axisX()->setRange(id - tickCount + 1, id);
	//}
	
	//static qint32 id(0);
	//
	//int diff = scatterSeries->count() - tickCount;
	//if (diff > 0) {
	//	scatterSeries->removePoints(0, diff);
	//	splineSeries->removePoints(0, diff);
	//}

	//double y;
	//if (1 == data.count('\n')) {
	//	y = data.toDouble();
	//	if (y > maxY) {
	//		showMessage(tr("Axis Y(=%1) bigger than maxY(=%2)! ").arg(y).arg(maxY));
	//		return;
	//	}
	//	QPointF value(id, y);
	//	
	//	splineSeries->append(value);
	//	scatterSeries->append(value);
	//	++id;
	//} else {
	//	PointList points;
	//	QVector<QStringRef> vecData = data.splitRef('\n', QString::SkipEmptyParts);
	//	for(const QStringRef &d : vecData) {
	//		y = d.toDouble();
	//		if (y > maxY) {
	//			showMessage(tr("Axis Y(=%1) bigger than maxY(=%2)! ").arg(y).arg(maxY));
	//			continue;
	//		}
	//		QPointF value(id, y);
	//		points.append(value);
	//		++id;
	//	}
	//	
	//	splineSeries->append(points);
	//	scatterSeries->append(points);
	//}
	//if (id > tickCount) {
	//	splineChart->axisX()->setRange(id - tickCount, id);
	//	//splineChart->scroll(splineChart->plotArea().width() / axisX->tickCount(), 0);
	//}
	
	// 下一次值的编号
	//++id;
	// 保证对界面其他操作的响应
	//if (0 == id % 2)
	//	QCoreApplication::processEvents();

	// 后续可将dataPoints与setRange拆分，置于不同的线程中
	Channel chans[] = {CH1, CH2, CH3, CH4};
	const int MaxTickCount = 50;
	for (int i = 0; i < 4; ++i) {
		if (!(chans[i] & channel)) continue;

		PointList &points = seriesPackets[i].dataPoints;
		QSplineSeries *spline = seriesPackets[i].splineSeries;
		QScatterSeries *scatter = seriesPackets[i].scatterSeries;
		int id = points.size();		
		++id;

		double y;
		if (1 == data.count('\n')) {
			y = data.toDouble();
			if (y > maxY || y < minY) {
				showMessage(tr("Axis Y(=%1) shouldn't bigger than maxY(=%2) \
					and shouldn't smaller than minY(=%3)! ").arg(y).arg(maxY).arg(minY));
				return;
			}
			QPointF value(id, y);
			points.append(value);
			++id;
		} else {
			QVector<QStringRef> vecData = data.splitRef('\n', QString::SkipEmptyParts);
			for (const QStringRef &d : vecData) {
				y = d.toDouble();
				if (y > maxY || y < minY) {
					showMessage(tr("Axis Y(=%1) bigger than maxY(=%2)\
						and shouldn't smaller than minY(=%3)! ").arg(y).arg(maxY).arg(minY));
					continue;
				}
				QPointF value(id, y);
				points.append(value);
				++id;
			}

		}
		spline->clear();
		scatter->clear();
		if (id <= tickCount) {
			spline->append(points);
			scatter->append(points);
			axisX->setRange(0, tickCount - 1);
		} else if (id <= MaxTickCount) {
			//PointList &pieces = points.mid(id - tickCount - 1, tickCount);
			spline->append(points);
			scatter->append(points);
			axisX->setRange(id - tickCount, id - 1);	
		} else {
			PointList &pieces = points.mid(id - MaxTickCount - 1, MaxTickCount);
			spline->append(pieces);
			scatter->append(pieces);
			axisX->setRange(id - tickCount, id - 1);
		}
	}
}

void MonitorTestTool::onSeriesChanged(const QStringList &dataList)
{
	QString data = dataList.join('\n');
	onSeriesChanged(data);
}

void MonitorTestTool::onDataUpdateTimer()
{
	QString *packet;
	QString data(""), dataShow("");
	qint32 count = windowQueue.get(&packet);	
	//QElapsedTimer timer;
	//timer.start();
	
	if (count > 0) {
		data = packet[0];
		dataShow = packet[0];
		if (count > 1) {
			for (qint32 i=1; i<count; ++i) {
				data.sprintf("%s\n%s", qPrintable(data), qPrintable(packet[i]));
				dataShow.sprintf("%s\n%s", qPrintable(packet[i]), qPrintable(data));
			}
		}
		data.append("\n");
		dataShow.append("\n");
		showMessage(data);
		onSeriesChanged(data);
		onShow(dataShow);
	}

	//if (timer.elapsed() > 200)
	//	QCoreApplication::processEvents();
}

void MonitorTestTool::onRecordFileChangeTimer()
{
	QString recordname = QDateTime::currentDateTime().toString("yyyyMMdd_hh");
	recordname.append(".csv");

	QMutexLocker locker(&writeMutex);
	if (rfile.isOpen()) rfile.close();
	rfile.setFileName(recordname);
	if (!rfile.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append)) {
		QMessageBox::critical(this, tr("critical error"), tr("Open %1 failed!").arg(recordname));
	}
}

void MonitorTestTool::showMessage(const QString &s)
{
	statusBar->showMessage(s);
}

bool MonitorTestTool::addSeries(int x, const QString &d, Channel ch)
{
	Channel chans[] = {CH1, CH2, CH3, CH4};
	for (int i = 0; i < sizeof(chans)/sizeof(Channel); ++i) {
		if (!(chans[i] & ch)) continue;

		QSplineSeries *spline = seriesPackets[i].splineSeries;
		QScatterSeries *scatter = seriesPackets[i].scatterSeries;
		if (0 == x || 1 == x) {
			spline->clear();
			scatter->clear();
			axisX->setRange(0, tickCount - 1);
		}
		
		if (d.isEmpty())  return false;

		double y = d.toDouble();
		if (y < minY || y > maxY) {
			showMessage(tr("Axis Y(=%1) shouldn't bigger than maxY(=%2) \
					and shouldn't smaller than minY(=%3)! ").arg(y).arg(maxY).arg(minY));
			return false;
		}
		QPointF value(x, y);
		scatter->append(value);
		spline->append(value);
		if (x > tickCount)
			axisX->setRange(x - tickCount + 1, x);	
		return true;
	}

	return true;
}

void MonitorTestTool::readData()
{
	TRACE_INFO(tr("***%1***readData begin").arg(quintptr(QThread::currentThreadId())));
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

	TRACE_INFO(tr("***%1***readData end").arg(quintptr(QThread::currentThreadId())));
	if (!isQuit)
		emit debugUpdate(quintptr(QThread::currentThreadId()), tr("readData end"));
}

void MonitorTestTool::handleData()
{	
	TRACE_INFO(tr("***%1***handleData start").arg(quintptr(QThread::currentThreadId())));
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
		
		if (queue.Pop(data, false)) {			
			windowQueue.put(data);
			{
				QMutexLocker locker(&recordMutex);
				data.append("\n");
				record.append(data);
			}

		}
	} while (isOpened);
	//} while (isOpened || (!isQuit && !queue.Empty()));
	
	queue.Clear();

	TRACE_INFO(tr("***%1***handleData stop").arg(quintptr(QThread::currentThreadId())));
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
	auto serialPortLabel = new QLabel(tr("端口号: "), this);
	serialPortComboBox = new QComboBox(this);
	auto baudRateLabel = new QLabel(tr("波特率: "), this);
	baudRateComboBox = new QComboBox(this);
	auto dataBitsLabel = new QLabel(tr("数据位: "), this);
	dataBitsComboBox = new QComboBox(this);
	auto stopBitsLabel = new QLabel(tr("停止位: "), this);
	stopBitsComboBox = new QComboBox(this);
	auto parityLabel = new QLabel(tr("校验位: "), this);
	parityComboBox = new QComboBox(this);
	runButton = new QPushButton(tr("打开串口"), this);
	stopButton = new QPushButton(tr("停止"), this);
	
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
	clearButton = new QPushButton(tr("清空"), this);
	receiveTextEdit = new QTextEdit(tr("准备中......"), this);
	
	auto receiveLayout = new QGridLayout();
	receiveLayout->addWidget(receiveTextEdit, 0, 0, 1, 5);
	receiveLayout->addWidget(clearButton, 1, 4);

	auto receiveGropBox = new QGroupBox(tr("接收数据"));
	receiveGropBox->setLayout(receiveLayout);

	receiveTextEdit->setReadOnly(true);
	return receiveGropBox;
}

QGroupBox * MonitorTestTool::createImportGroup()
{
	auto dataFileLabel = new QLabel(tr("数据源位置:"), this);
	dataFileLineEdit = new QLineEdit(this);
	selectButton = new QPushButton(tr("浏览..."), this);
	importButton = new QPushButton(tr("导入"), this);
	
	auto importLayout = new QGridLayout;
	importLayout->addWidget(dataFileLabel, 0, 0);
	importLayout->addWidget(dataFileLineEdit, 1, 0, 1, 3);
	importLayout->addWidget(selectButton, 1, 3);
	importLayout->addWidget(importButton, 1, 4);
	//importLayout->addWidget(clearButton, 2, 3);
	
	auto openingGroupBox = new QGroupBox(tr("导入数据"));
	openingGroupBox->setLayout(importLayout);

	return openingGroupBox;
}

QGroupBox *MonitorTestTool::createChartGroup()
{
	// spine chart
	splineChart = new QChart();
	QFont font;
	font.setPixelSize(18);
	font.setBold(true);
	splineChart->setTitleFont(font);
	//splineChart->setTitleBrush(QBrush(Qt::red));
	splineChart->setTitle("Serial Data Real-Time Chart");
	//splineChart->setTheme(QChart::ChartThemeBlueNcs); // 与样式设置冲突
	//splineChart->setAnimationOptions(QChart::SeriesAnimations);
	splineChart->legend()->hide();

	splineChart->createDefaultAxes();
	axisX = new QValueAxis();
	axisX->setRange(0, tickCount - 1);
	axisX->setLabelFormat("%u"); //设置刻度的格式
	axisX->setGridLineVisible(true);
	axisX->setTickCount(tickCount);     //设置多少格
	axisX->setTitleText(tr("Data point"));
	//axisX->setMinorTickCount(1); //设置每格小刻度线的数目	
	axisY = new QValueAxis();
	axisY->setRange(0, maxY);
	axisY->setLabelFormat("%u");
	axisY->setTitleText(tr("Values"));	

	auto splineView = new QChartView(splineChart);
	splineView->setRenderHint(QPainter::Antialiasing); // 图像抗锯齿
	splineView->setSceneRect(0, 0, 630, 280);	 // 设置初始图表大小
	
	for (auto &packet : seriesPackets) {
		packet.dataPoints.reserve(samplingNum);
		
		packet.splineSeries = new QSplineSeries(splineChart);
		packet.splineSeries->setUseOpenGL(true);
		splineChart->addSeries(packet.splineSeries);
		splineChart->setAxisX(axisX, packet.splineSeries);
		splineChart->setAxisY(axisY, packet.splineSeries);
		disconnect(packet.splineSeries, SIGNAL(pointRemoved(int)), splineView, SLOT(update()));

		packet.scatterSeries = new QScatterSeries(splineChart);
		packet.scatterSeries->setBrush(QBrush(QColor(19, 65, 166, 5)));
		packet.scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
		packet.scatterSeries->setMarkerSize(5); 
		packet.scatterSeries->setUseOpenGL(true);
		splineChart->addSeries(packet.scatterSeries);
		splineChart->setAxisX(axisX, packet.scatterSeries);
		splineChart->setAxisY(axisY, packet.scatterSeries);
		disconnect(packet.scatterSeries, SIGNAL(pointRemoved(int)), splineView, SLOT(update()));
	}

	// 需要将坐标轴与series绑定
	//splineChart->setAxisX(axisX, splineSeries);
	//splineChart->setAxisX(axisX, scatterSeries);
	//splineChart->setAxisY(axisY, splineSeries);
	//splineChart->setAxisY(axisY, scatterSeries);

	//QString name("Series ");
	//int nameIndex = 0;
	//foreach(DataList list, dataTable) {
	//	//QSplineSeries *series = new QSplineSeries(splineChart);
	//	splineSeries = new QSplineSeries(splineChart);		
	//	scatterSeries = new QScatterSeries(splineChart);
	//	scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
	//	scatterSeries->setMarkerSize(3);
	//	PointList points;
	//	foreach(Data data, list) {
	//		points.append(data.first);
	//	}
	//	splineSeries->append(points);
	//	scatterSeries->append(points);
	//	splineSeries->setName(name + QString::number(nameIndex));
	//	scatterSeries->setName(name + QString::number(nameIndex));		
	//	splineSeries->setUseOpenGL(true);
	//	scatterSeries->setUseOpenGL(true);
	//	splineChart->addSeries(splineSeries);
	//	splineChart->addSeries(scatterSeries);
	//	nameIndex++;
	//}	
	
	//auto splineView = new QChartView(splineChart);
	//splineView->setRenderHint(QPainter::Antialiasing); // 图像抗锯齿
	//splineView->setSceneRect(0, 0, 630, 280);	 // 设置初始图表大小
	//disconnect(splineSeries, SIGNAL(pointRemoved(int)), splineView, SLOT(update()));
	
	auto chartLayout = new QVBoxLayout;
	chartLayout->setContentsMargins(0, 0, 0, 0);
	chartLayout->addWidget(splineView);

	auto chartGroupBox = new QGroupBox(tr("实时数据曲线图"));
	chartGroupBox->setLayout(chartLayout);
	chartGroupBox->setMinimumSize(400, 450); // 设置图表展示最小值
	return chartGroupBox;
}

QGroupBox * MonitorTestTool::createSamplingGroup()
{
	auto minSamplingLabel = new QLabel(tr("最小采样值: "), this);
	minValueLineEdit = new QLineEdit("0", this);
	auto maxSamplingLabel = new QLabel(tr("最大采样值: "), this);
	maxValueLineEdit = new QLineEdit("100", this);
	auto tickCountLabel = new QLabel(tr("显示个数: "), this);
	tickCountSpinBox = new QSpinBox(this);
	tickCountSpinBox->setRange(10, 50);
	tickCountSpinBox->setSingleStep(10);	
	auto samplingNumLabel = new QLabel(tr("采集个数: "), this);
	samplingNumLineEdit = new QLineEdit("10000", this);
	saveButton = new QPushButton(tr("保存设置"), this);

	const int stretch = 1;
	auto samplingLayout = new QHBoxLayout;
	samplingLayout->addWidget(minSamplingLabel, 1);
	samplingLayout->addWidget(minValueLineEdit, 2);
	samplingLayout->addStretch(stretch);
	samplingLayout->addWidget(maxSamplingLabel, 1);
	samplingLayout->addWidget(maxValueLineEdit, 2);
	samplingLayout->addStretch(stretch);
	samplingLayout->addWidget(tickCountLabel, 1);
	samplingLayout->addWidget(tickCountSpinBox, 2);
	samplingLayout->addStretch(stretch);
	samplingLayout->addWidget(samplingNumLabel, 1);
	samplingLayout->addWidget(samplingNumLineEdit, 2);
	samplingLayout->addStretch(stretch);
	samplingLayout->addWidget(saveButton, 1);
	//samplingLayout->addStretch(3);

	auto samplingBox = new QGroupBox(tr("实时曲线图设置"));
	samplingBox->setLayout(samplingLayout);
	return samplingBox;
}

QGroupBox * MonitorTestTool::createFrameFormatGroup()
{
	auto fheadLabel = new QLabel(tr("帧头: "), this);
	fheadLineEdit = new QLineEdit(tr("A5"), this);
	auto ftailLabel = new QLabel(tr("帧尾: "), this);
	ftailLineEdit = new QLineEdit(tr("AA"), this);

	auto frameLayout = new QGridLayout();
	frameLayout->addWidget(fheadLabel, 0, 0);
	frameLayout->addWidget(fheadLineEdit, 0, 1);	
	frameLayout->addWidget(ftailLabel, 0, 3);
	frameLayout->addWidget(ftailLineEdit, 0, 4);
	frameLayout->setColumnMinimumWidth(2, 60);

	auto frameBox = new QGroupBox(tr("帧格式设置"));
	frameBox->setLayout(frameLayout);
	return frameBox;
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
	connect(importButton, &QPushButton::clicked, this, &MonitorTestTool::runImport);
	connect(clearButton, &QPushButton::clicked, this, &MonitorTestTool::clearData);
	connect(saveButton, &QPushButton::clicked, this, &MonitorTestTool::saveSamplingSettings);
	connect(&windowTimer, &QTimer::timeout, this, &MonitorTestTool::onDataUpdateTimer);
	connect(&recordTimer, &QTimer::timeout, this, &MonitorTestTool::onRecordFileChangeTimer);

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
	delete serialPortComboBox;
	delete baudRateComboBox;
	delete dataBitsComboBox;
	delete stopBitsComboBox;
	delete parityComboBox;
	delete runButton;
	delete stopButton;

	delete dataFileLineEdit;
	delete selectButton;
	delete importButton;
	delete clearButton;

	delete receiveTextEdit;
	delete statusBar;
	delete splineChart;
	//delete splineSeries;
	//delete scatterSeries;


	serialPortComboBox = Q_NULLPTR;
	baudRateComboBox = Q_NULLPTR;
	dataBitsComboBox = Q_NULLPTR;
	stopBitsComboBox = Q_NULLPTR;
	parityComboBox = Q_NULLPTR;
	runButton = Q_NULLPTR;
	stopButton = Q_NULLPTR;

	dataFileLineEdit = Q_NULLPTR;
	selectButton = Q_NULLPTR;
	importButton = Q_NULLPTR;
	clearButton = Q_NULLPTR;

	receiveTextEdit = Q_NULLPTR;
	statusBar = Q_NULLPTR;
	splineChart = Q_NULLPTR;
	//splineSeries = Q_NULLPTR;
	//scatterSeries = Q_NULLPTR;
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


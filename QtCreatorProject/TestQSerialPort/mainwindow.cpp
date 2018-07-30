#include "mainwindow.h"
#include "ui_mainwindow.h"

const int INTERVAL_SHOW = 10;
const int INTERVAL_SERIES = 30;
const int TICK_COUNT = 1000;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    isOpened(false),
    isGenerateRand(true)
{
    ui->setupUi(this);
    Initialize();
    connectSlots();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *)
{
    isOpened = false;
    isGenerateRand = false;
}

void MainWindow::onButtonClicked()
{
    QString curText = ui->pushButton->text();
    if (0 == curText.compare("start")) {
        ui->pushButton->setText("stop");

        QString portName = ui->comboBox->currentText();
        m_serialPort->setPortName(portName);
        m_serialPort->setBaudRate(QSerialPort::Baud115200);
        m_serialPort->setDataBits(QSerialPort::Data8);
        m_serialPort->setParity(QSerialPort::NoParity);
        m_serialPort->setStopBits(QSerialPort::OneStop);
        m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
        m_serialPort->setReadBufferSize(128*1024);

        if (!m_serialPort->isOpen()) {
            if (!m_serialPort->open(QIODevice::ReadOnly)) {
                QMessageBox::critical(this, tr("Critical Error"), tr("打开端口%1失败: %2").arg(portName).arg(m_serialPort->errorString()));
            }
        }
        if (m_serialPort->isOpen()) {
//            m_showTimer.start();

            isOpened = true;
            QtConcurrent::run(this, &MainWindow::serialPortReadThread);
        }
    } else {
        ui->pushButton->setText("start");
        isOpened = false;
//        if (m_showTimer.isActive())
//            m_showTimer.stop();
    }
}

void MainWindow::onTimerTimeout()
{
    QString *pData;
    QString allData;
    int count = m_dbQueue.get(&pData);
    if (count > 0) {
        for (int i=0; i<count; ++i) {
            QStringList list = pData[i].split("A5", QString::SkipEmptyParts);
            for (const QString &one : list) {
                int value(0);
                sscanf(one.toStdString().c_str(), "%dAA", &value);
                allData.append(QString::number(value));
                allData.append(", ");
            }
        }

        ui->statusBar->showMessage(allData);

    }

    if (allData.isEmpty())
        ui->statusBar->showMessage("No more data to show!!");
}

void MainWindow::onSeriesChanged()
{
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
    static int n = 1;
    qreal y = (qrand() % 140) + m_axisY->min();
    m_splineSeries->append(n, y);
    if (n > TICK_COUNT)
        m_axisX->setRange(n-TICK_COUNT-1, n);
    ++n;
}

void MainWindow::Initialize()
{
    m_serialPort = new QSerialPort(this);
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
        ui->comboBox->addItem(info.portName());

    QChart *splineChart = new QChart();
    m_splineSeries = new QSplineSeries(splineChart);
    splineChart->createDefaultAxes();
    m_axisX = new QValueAxis();
    m_axisY = new QValueAxis();
    m_axisX->setRange(0, TICK_COUNT);
    //m_axisX->setTickCount(TICK_COUNT + 1);
    m_axisX->setLabelFormat("%u");
    m_axisX->setTitleText("data point");
    m_axisY->setRange(10, 150);
    m_axisY->setLabelFormat("%u");
    m_axisY->setTitleText("value");
    splineChart->addSeries(m_splineSeries);
    splineChart->setAxisX(m_axisX, m_splineSeries);
    splineChart->setAxisY(m_axisY, m_splineSeries);
    QChartView *splineView = new QChartView(splineChart, this);
    splineView->setRenderHint(QPainter::Antialiasing);

    auto settingsLayout = new QHBoxLayout(this);
    settingsLayout->addWidget(ui->comboBox, 1);
    settingsLayout->addStretch(1);
    settingsLayout->addWidget(ui->pushButton, 1);

    auto settingsBox = new QGroupBox(tr("设置"), this);
    settingsBox->setLayout(settingsLayout);

    auto baseLayout = new QGridLayout(this);
    baseLayout->addWidget(settingsBox);
    baseLayout->addWidget(splineView);
    ui->centralWidget->setLayout(baseLayout);

    ui->statusBar->showMessage(tr("惊喜"), 30000);
    ui->pushButton->setText("start");
    m_showTimer.setInterval(INTERVAL_SHOW);
    m_showTimer.start();

    //QtConcurrent::run(this, &MainWindow::randomPoint4ChartThread);
    m_seriesTimer.setInterval(INTERVAL_SERIES);
    m_seriesTimer.start();
}

void MainWindow::connectSlots()
{
    connect(ui->pushButton, SIGNAL(clicked(bool)), this, SLOT(onButtonClicked()));
    connect(&m_showTimer, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));
    connect(&m_seriesTimer, SIGNAL(timeout()), this, SLOT(onSeriesChanged()));
}

void MainWindow::serialPortReadThread()
{
    while (isOpened) {
        QByteArray data;
        if (m_serialPort->waitForReadyRead(1000)) {
            data = m_serialPort->readLine(1024);
            m_dbQueue.put(data);
        }
    }
}

void MainWindow::randomPoint4ChartThread()
{
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
    int n = 1;

    while (isGenerateRand) {
        qreal y = (qrand() % 140) + m_axisY->min();
        m_splineSeries->append(n, y);
        if (n > TICK_COUNT)
            m_axisX->setRange(n-TICK_COUNT-1, n);
        ++n;

        QThread::msleep(100);
    }
}

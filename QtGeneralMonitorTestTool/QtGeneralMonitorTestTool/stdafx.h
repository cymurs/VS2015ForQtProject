#include <QtWidgets>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QtConcurrent/QtConcurrent>

#pragma execution_character_set("utf-8")  // ���������ʾ��������

extern const char *RecordFile;
extern const int RecordInterval;
extern const int interval;
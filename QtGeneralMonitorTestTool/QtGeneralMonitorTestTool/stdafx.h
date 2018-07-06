#include <QtWidgets>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QtConcurrent/QtConcurrent>

#pragma execution_character_set("utf-8")  // 解决界面显示中文乱码

extern const char *RecordFile;
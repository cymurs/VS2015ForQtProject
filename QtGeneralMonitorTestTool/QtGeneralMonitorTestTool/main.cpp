#include "stdafx.h"
#include "monitortesttool.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MonitorTestTool w;
	w.show();
	// 解决 关闭窗口进程不退出 的问题
	a.quit();
	return a.exec();
}

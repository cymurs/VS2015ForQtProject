#include "stdafx.h"
#include "mmsassist.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MMSAssist w;
	w.show();
	return a.exec();
}

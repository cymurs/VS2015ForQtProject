#include "stdafx.h"
#include "mmsassist.h"

MMSAssist::MMSAssist(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	initialize();
}

void MMSAssist::initialize()
{
	setWindowIcon(QIcon(":/MMSAssist/Resources/4.ico"));	
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint); // ���������С����ť
	
}

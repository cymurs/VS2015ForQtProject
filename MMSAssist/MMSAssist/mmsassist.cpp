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
	
	connectSlots();
}

void MMSAssist::connectSlots()
{
	connect(ui.pushButton_4, &QPushButton::clicked, this, &MMSAssist::importConfig);
	connect(ui.pushButton_5, &QPushButton::clicked, this, &MMSAssist::saveConfig);
}

void MMSAssist::saveConfig()
{
	QString m_iniFile = QFileDialog::getSaveFileName(this, tr("��������"),
		QDir::currentPath(), tr("INI files (*.ini);;"));
	
	if (!m_iniFile.isEmpty()) {
		QPalette pa;
		pa.setColor(QPalette::WindowText, Qt::red);
		ui.label_3->setPalette(pa);
		ui.label_3->setText(m_iniFile);
	}
}

void MMSAssist::importConfig()
{
	QString m_iniFile = QFileDialog::getOpenFileName(this, tr("������"),
		QDir::currentPath(), tr("INI files (*.ini)"));

	if (!m_iniFile.isEmpty()) {
		QPalette pa;
		pa.setColor(QPalette::WindowText, Qt::red);
		ui.label_3->setPalette(pa);
		ui.label_3->setText(m_iniFile);
	}
}

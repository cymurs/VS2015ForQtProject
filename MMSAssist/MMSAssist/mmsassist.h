#pragma once

#include <QtWidgets/QDialog>
#include "ui_mmsassist.h"

class MMSAssist : public QDialog
{
	Q_OBJECT

public:
	MMSAssist(QWidget *parent = Q_NULLPTR);

private:
	void initialize();
	void connectSlots();

private Q_SLOTS:
	void saveConfig();
	void importConfig();


private:
	Ui::MMSAssistClass ui;

	QString m_iniFile;
};

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

private:
	Ui::MMSAssistClass ui;
};

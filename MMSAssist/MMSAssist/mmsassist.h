#pragma once

#include <QtWidgets/QDialog>
#include "ui_mmsassist.h"

class MMSAssist : public QDialog
{
	Q_OBJECT

public:
	enum SavePolicy {
		SavePerSecond = 0x01,
		SavePerHour = 0x02,
		SavePerDay = 0x04,
		SaveUnknown = 0xff
	};

public:
	MMSAssist(QWidget *parent = Q_NULLPTR);

private:
	void initialize();
	void connectSlots();
	void loadCfg();
	void storeCfg();
	QGroupBox *createSettingsGroup();
	QGroupBox *createRWShowGroup();

private Q_SLOTS:
	void saveConfig();
	void importConfig();


private:
	Ui::MMSAssistClass ui;
	QLineEdit *m_remoteAddress;
	QLineEdit *m_remotePort;
	QCheckBox *m_savePerSecond;
	QCheckBox *m_savePerHour;
	QCheckBox *m_savePerDay;
	QLineEdit *m_fileSize;
	QComboBox *m_fileUnit;
	QLineEdit *m_storageDirectory;
	QLabel *m_hostAddress;
	QPushButton *m_connectDisconnect;

	QPlainTextEdit *m_dataShow;
	QLineEdit *m_dataSend;
	QPushButton *m_sendButton;
	QLabel *m_sendCount;
	QLabel *m_receiveCount;
	QPushButton *m_resetButton;

	QStatusBar *m_statusShow;

	QString m_iniFile;
	const QString CFG_FILE{"mmsassist.ini"};
};


#include "stdafx.h"
#include "mmsassist.h"

MMSAssist::MMSAssist(QWidget *parent)
	: QDialog(parent)
{
	//ui.setupUi(this);

	initialize();
}

void MMSAssist::initialize()
{
	setWindowIcon(QIcon(":/MMSAssist/Resources/4.ico"));	
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint); // 增加最大最小化按钮
	
	m_statusShow = new QStatusBar(this);
	auto baseLayout = new QGridLayout(this);
	baseLayout->addWidget(createSettingsGroup(), 0, 1);
	baseLayout->addWidget(createRWShowGroup(), 0, 2);
	baseLayout->addWidget(m_statusShow, 1, 0, 1, 2);
	setLayout(baseLayout);
	
	connectSlots();
}

void MMSAssist::connectSlots()
{
	connect(ui.pushButton_4, &QPushButton::clicked, this, &MMSAssist::importConfig);
	connect(ui.pushButton_5, &QPushButton::clicked, this, &MMSAssist::saveConfig);
}

void MMSAssist::loadCfg()
{
	QString cfgName(CFG_FILE);
	if (!QFile::exists(cfgName)) {
		MMS_INFO(tr("信息"), tr("%1 不存在!").arg(cfgName));
		return;
	}

	//QFile file(cfgName);
	//if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
	//	MMS_WARN(tr("警告"), tr("打开%1失败!"));
	//	return;
	//}

	QSettings file(cfgName, QSettings::IniFormat);
	QString remoteAddress = file.value("/CONFIG/RemoteAddress").toString();
	int remotePort = file.value("/CONFIG/RemotePort").toInt();
	int policy = file.value("/CONFIG/SavePolicy").toInt();
	QString fileSize = file.value("/CONFIG/FileSize").toString();
	QString directory = file.value("/CONFIG/StorageDirectory").toString();

	if (!remoteAddress.isEmpty())
		m_remoteAddress->setText(remoteAddress);
	if (0 != remotePort)
		m_remotePort->setText(QString::number(remotePort));
	if (0 != policy) {
		if (policy & MMSAssist::SavePerSecond)
			m_savePerSecond->setChecked(true);
		if (policy & MMSAssist::SavePerHour)
			m_savePerHour->setChecked(true);
		if (policy & MMSAssist::SavePerDay)
			m_savePerDay->setChecked(true);
	}
	if (!fileSize.isEmpty()) {
		QStringList list = fileSize.split(" ");
		if (2 == list.length()) {
			m_fileSize->setText(list[0]);
			m_fileUnit->setCurrentText(list[1]);
		}
	}
	if (!directory.isEmpty())
		m_storageDirectory->setText(directory);
}

void MMSAssist::storeCfg()
{
	qint64 policy(0);
	if (m_savePerSecond->isChecked()) {
		policy |= MMSAssist::SavePerSecond;
	}
	if (m_savePerHour->isChecked()) {
		policy |= MMSAssist::SavePerHour;
	}
	if (m_savePerDay->isChecked()) {
		policy |= MMSAssist::SavePerDay;
	}
	
	QString fileSize(m_fileSize->text());
	fileSize.append(" ");
	fileSize.append(m_fileUnit->currentText());

	QSettings file(CFG_FILE, QSettings::IniFormat);
	file.setValue("/CONFIG/RemoteAddress", m_remoteAddress->text());
	file.setValue("/CONFIG/RemotePort", m_remotePort->text());
	file.setValue("/CONFIG/SavePolicy", policy);
	file.setValue("/CONFIG/FileSize", fileSize);
	file.setValue("/CONFIG/StorageDirectory", m_storageDirectory->text());

}

QGroupBox * MMSAssist::createSettingsGroup()
{
	auto settingsGroup = new QGroupBox(tr("连接 && 配置"), this);
	QWidget *parent = settingsGroup;

	QLabel *remoteAddress = new QLabel(tr("服务器IP地址: "), parent);
	m_remoteAddress = new QLineEdit(tr("192.168.0.31"), parent);
	QLabel *remotePort = new QLabel(tr("服务器端口: "), parent);
	m_remotePort = new QLineEdit(tr("2018"), parent);
	m_savePerSecond = new QCheckBox(tr("按秒保存"), parent);
	m_savePerHour = new QCheckBox(tr("按小时保存"), parent);
	m_savePerDay = new QCheckBox(tr("按天保存"), parent);
	QLabel *fileSize = new QLabel(tr("文件大小: "), parent);
	m_fileSize = new QLineEdit(tr("1000"), parent);
	m_fileUnit = new QComboBox(parent);
	QLabel *storeDirectory = new QLabel(tr("文件存储目录: "), parent);
	m_storageDirectory = new QLineEdit(tr(""), parent);
	m_hostAddress = new QLabel(tr(""), parent);
	m_connectDisconnect = new QPushButton(tr("连接"), parent);

	m_remoteAddress->setInputMask("000.000.000.000;_");
	QIntValidator *intValidator = new QIntValidator(m_remotePort);
	intValidator->setRange(1024, 65535);
	m_remotePort->setValidator(intValidator);
	m_fileSize->setAlignment(Qt::AlignRight);
	QStringList items({ "KB", "MB", "GB" });
	m_fileUnit->addItems(items);
	m_fileUnit->setCurrentIndex(2);
	m_fileUnit->setSizeAdjustPolicy(QComboBox::AdjustToContents);

	loadCfg();

	auto settingsLayout = new QGridLayout(parent);
	settingsLayout->addWidget(remoteAddress, 0, 0);
	settingsLayout->addWidget(m_remoteAddress, 0, 1);
	settingsLayout->addWidget(remotePort, 1, 0);
	settingsLayout->addWidget(m_remotePort, 1, 1);
	settingsLayout->addWidget(m_savePerSecond, 2, 0);
	settingsLayout->addWidget(m_savePerHour, 3, 0);
	settingsLayout->addWidget(m_savePerDay, 4, 0);
	settingsLayout->addWidget(fileSize, 5, 0);
	settingsLayout->addWidget(m_fileSize, 5, 1);
	settingsLayout->addWidget(m_fileUnit, 5, 2);
	settingsLayout->addWidget(storeDirectory, 6, 0);
	settingsLayout->addWidget(m_storageDirectory, 7, 0, 1, 3);
	settingsLayout->addWidget(m_hostAddress, 8, 0, 1, 3);
	settingsLayout->addWidget(m_connectDisconnect, 9, 0);
	settingsGroup->setLayout(settingsLayout);

	return settingsGroup;
}

QGroupBox * MMSAssist::createRWShowGroup()
{
	auto rwShowGroup = new QGroupBox(tr("接收 && 发送"), this);
	QWidget *parent = rwShowGroup;
	
	return rwShowGroup;
}

void MMSAssist::saveConfig()
{
	QString m_iniFile = QFileDialog::getSaveFileName(this, tr("保存配置"),
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
	QString m_iniFile = QFileDialog::getOpenFileName(this, tr("打开配置"),
		QDir::currentPath(), tr("INI files (*.ini)"));

	if (!m_iniFile.isEmpty()) {
		QPalette pa;
		pa.setColor(QPalette::WindowText, Qt::red);
		ui.label_3->setPalette(pa);
		ui.label_3->setText(m_iniFile);
	}
}

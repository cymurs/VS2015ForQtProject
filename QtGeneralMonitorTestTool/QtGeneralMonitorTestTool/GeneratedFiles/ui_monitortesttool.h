/********************************************************************************
** Form generated from reading UI file 'monitortesttool.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MONITORTESTTOOL_H
#define UI_MONITORTESTTOOL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MonitorTestToolClass
{
public:
    QWidget *centralWidget;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_3;
    QGroupBox *groupBox;
    QLabel *label;
    QComboBox *comboBox;
    QLabel *label_2;
    QComboBox *comboBox_2;
    QLabel *label_3;
    QComboBox *comboBox_3;
    QLabel *label_4;
    QComboBox *comboBox_4;
    QLabel *label_5;
    QComboBox *comboBox_5;
    QPushButton *pushButton;
    QGroupBox *groupBox_3;
    QLabel *label_6;
    QTextBrowser *textBrowser;
    QGroupBox *groupBox_2;
    QTextEdit *textEdit;
    QVBoxLayout *verticalLayout_2;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MonitorTestToolClass)
    {
        if (MonitorTestToolClass->objectName().isEmpty())
            MonitorTestToolClass->setObjectName(QStringLiteral("MonitorTestToolClass"));
        MonitorTestToolClass->resize(942, 567);
        centralWidget = new QWidget(MonitorTestToolClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayoutWidget = new QWidget(centralWidget);
        horizontalLayoutWidget->setObjectName(QStringLiteral("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(10, 10, 871, 481));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        groupBox = new QGroupBox(horizontalLayoutWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 30, 61, 16));
        comboBox = new QComboBox(groupBox);
        comboBox->setObjectName(QStringLiteral("comboBox"));
        comboBox->setGeometry(QRect(80, 30, 69, 22));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(170, 30, 54, 12));
        comboBox_2 = new QComboBox(groupBox);
        comboBox_2->setObjectName(QStringLiteral("comboBox_2"));
        comboBox_2->setGeometry(QRect(220, 30, 69, 22));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(310, 40, 54, 12));
        comboBox_3 = new QComboBox(groupBox);
        comboBox_3->setObjectName(QStringLiteral("comboBox_3"));
        comboBox_3->setGeometry(QRect(360, 30, 69, 22));
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(20, 70, 54, 12));
        comboBox_4 = new QComboBox(groupBox);
        comboBox_4->setObjectName(QStringLiteral("comboBox_4"));
        comboBox_4->setGeometry(QRect(80, 60, 69, 22));
        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(170, 70, 54, 12));
        comboBox_5 = new QComboBox(groupBox);
        comboBox_5->setObjectName(QStringLiteral("comboBox_5"));
        comboBox_5->setGeometry(QRect(220, 60, 69, 22));
        pushButton = new QPushButton(groupBox);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(330, 60, 75, 23));

        verticalLayout_3->addWidget(groupBox);

        groupBox_3 = new QGroupBox(horizontalLayoutWidget);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        label_6 = new QLabel(groupBox_3);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(30, 30, 54, 12));
        textBrowser = new QTextBrowser(groupBox_3);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));
        textBrowser->setGeometry(QRect(20, 60, 256, 71));

        verticalLayout_3->addWidget(groupBox_3);

        groupBox_2 = new QGroupBox(horizontalLayoutWidget);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        textEdit = new QTextEdit(groupBox_2);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setGeometry(QRect(20, 30, 291, 181));

        verticalLayout_3->addWidget(groupBox_2);


        horizontalLayout->addLayout(verticalLayout_3);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(26);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setSizeConstraint(QLayout::SetNoConstraint);
        verticalLayout_2->setContentsMargins(4, 4, 4, 4);

        horizontalLayout->addLayout(verticalLayout_2);

        MonitorTestToolClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MonitorTestToolClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 942, 23));
        MonitorTestToolClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MonitorTestToolClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MonitorTestToolClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MonitorTestToolClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MonitorTestToolClass->setStatusBar(statusBar);

        retranslateUi(MonitorTestToolClass);

        QMetaObject::connectSlotsByName(MonitorTestToolClass);
    } // setupUi

    void retranslateUi(QMainWindow *MonitorTestToolClass)
    {
        MonitorTestToolClass->setWindowTitle(QApplication::translate("MonitorTestToolClass", "MonitorTestTool", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("MonitorTestToolClass", "\350\256\276\347\275\256\344\270\262\345\217\243", Q_NULLPTR));
        label->setText(QApplication::translate("MonitorTestToolClass", "\347\253\257\345\217\243\345\217\267\357\274\232", Q_NULLPTR));
        label_2->setText(QApplication::translate("MonitorTestToolClass", "\346\263\242\347\211\271\347\216\207\357\274\232", Q_NULLPTR));
        label_3->setText(QApplication::translate("MonitorTestToolClass", "\346\225\260\346\215\256\344\275\215\357\274\232", Q_NULLPTR));
        label_4->setText(QApplication::translate("MonitorTestToolClass", "\345\201\234\346\255\242\344\275\215\357\274\232", Q_NULLPTR));
        label_5->setText(QApplication::translate("MonitorTestToolClass", "\346\240\241\351\252\214\344\275\215\357\274\232", Q_NULLPTR));
        pushButton->setText(QApplication::translate("MonitorTestToolClass", "\346\211\223\345\274\200\344\270\262\345\217\243", Q_NULLPTR));
        groupBox_3->setTitle(QApplication::translate("MonitorTestToolClass", "\345\257\274\345\205\245\346\225\260\346\215\256", Q_NULLPTR));
        label_6->setText(QApplication::translate("MonitorTestToolClass", "TextLabel", Q_NULLPTR));
        groupBox_2->setTitle(QApplication::translate("MonitorTestToolClass", "\346\216\245\346\224\266\346\225\260\346\215\256", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MonitorTestToolClass: public Ui_MonitorTestToolClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MONITORTESTTOOL_H

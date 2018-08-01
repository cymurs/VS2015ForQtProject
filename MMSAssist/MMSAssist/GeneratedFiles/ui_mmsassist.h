/********************************************************************************
** Form generated from reading UI file 'mmsassist.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MMSASSIST_H
#define UI_MMSASSIST_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_MMSAssistClass
{
public:
    QLabel *label;
    QLabel *label_2;
    QLineEdit *lineEdit;
    QLineEdit *lineEdit_2;
    QGroupBox *groupBox;
    QPlainTextEdit *plainTextEdit;
    QLineEdit *lineEdit_3;
    QPushButton *pushButton;
    QLabel *label_3;
    QLabel *label_4;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QLabel *label_5;
    QLabel *label_6;

    void setupUi(QDialog *MMSAssistClass)
    {
        if (MMSAssistClass->objectName().isEmpty())
            MMSAssistClass->setObjectName(QStringLiteral("MMSAssistClass"));
        MMSAssistClass->resize(359, 522);
        label = new QLabel(MMSAssistClass);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(23, 10, 101, 16));
        label_2 = new QLabel(MMSAssistClass);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(23, 40, 81, 16));
        lineEdit = new QLineEdit(MMSAssistClass);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setGeometry(QRect(193, 10, 133, 20));
        lineEdit_2 = new QLineEdit(MMSAssistClass);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(193, 40, 133, 20));
        groupBox = new QGroupBox(MMSAssistClass);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(20, 100, 331, 311));
        plainTextEdit = new QPlainTextEdit(groupBox);
        plainTextEdit->setObjectName(QStringLiteral("plainTextEdit"));
        plainTextEdit->setGeometry(QRect(10, 20, 311, 281));
        lineEdit_3 = new QLineEdit(MMSAssistClass);
        lineEdit_3->setObjectName(QStringLiteral("lineEdit_3"));
        lineEdit_3->setGeometry(QRect(20, 420, 261, 20));
        pushButton = new QPushButton(MMSAssistClass);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(290, 420, 61, 23));
        label_3 = new QLabel(MMSAssistClass);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(20, 490, 331, 21));
        QFont font;
        font.setFamily(QStringLiteral("Times New Roman"));
        font.setPointSize(9);
        font.setBold(true);
        font.setWeight(75);
        label_3->setFont(font);
        label_4 = new QLabel(MMSAssistClass);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(190, 450, 81, 21));
        QFont font1;
        font1.setFamily(QStringLiteral("Times New Roman"));
        font1.setBold(false);
        font1.setWeight(50);
        label_4->setFont(font1);
        pushButton_2 = new QPushButton(MMSAssistClass);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(253, 67, 75, 23));
        pushButton_3 = new QPushButton(MMSAssistClass);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));
        pushButton_3->setGeometry(QRect(290, 450, 61, 23));
        label_5 = new QLabel(MMSAssistClass);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(20, 450, 54, 20));
        label_6 = new QLabel(MMSAssistClass);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(23, 70, 211, 16));

        retranslateUi(MMSAssistClass);

        QMetaObject::connectSlotsByName(MMSAssistClass);
    } // setupUi

    void retranslateUi(QDialog *MMSAssistClass)
    {
        MMSAssistClass->setWindowTitle(QApplication::translate("MMSAssistClass", "MMSAssist", Q_NULLPTR));
        label->setText(QApplication::translate("MMSAssistClass", "Remote Address:", Q_NULLPTR));
        label_2->setText(QApplication::translate("MMSAssistClass", "Remote Port:", Q_NULLPTR));
        lineEdit->setText(QApplication::translate("MMSAssistClass", "192.168.0.25", Q_NULLPTR));
        lineEdit_2->setText(QApplication::translate("MMSAssistClass", "8090", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("MMSAssistClass", "Data Receive", Q_NULLPTR));
        plainTextEdit->setPlainText(QApplication::translate("MMSAssistClass", "1|1079619201|2018-07-31 17:05:49|496903.663579119\n"
"2|1079619201|2018-07-31 17:05:49|496903.662188217\n"
"3|1079619201|2018-07-31 17:05:49|496903.662229220\n"
"4|1079619201|2018-07-31 17:05:49|496903.667599177", Q_NULLPTR));
        pushButton->setText(QApplication::translate("MMSAssistClass", "Send", Q_NULLPTR));
        label_3->setText(QApplication::translate("MMSAssistClass", "Warning & Status", Q_NULLPTR));
        label_4->setText(QApplication::translate("MMSAssistClass", "Receive\357\274\23223", Q_NULLPTR));
        pushButton_2->setText(QApplication::translate("MMSAssistClass", "Connect", Q_NULLPTR));
        pushButton_3->setText(QApplication::translate("MMSAssistClass", "Reset", Q_NULLPTR));
        label_5->setText(QApplication::translate("MMSAssistClass", "Send\357\274\2325", Q_NULLPTR));
        label_6->setText(QApplication::translate("MMSAssistClass", "Local Address: 192.168.0.96:8888", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MMSAssistClass: public Ui_MMSAssistClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MMSASSIST_H

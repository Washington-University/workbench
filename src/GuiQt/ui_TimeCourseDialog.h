/********************************************************************************
** Form generated from reading UI file 'TimeCourseDialog.ui'
**
** Created: Tue Jan 10 11:34:03 2012
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TIMECOURSEDIALOG_H
#define UI_TIMECOURSEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "TimeCourseDialog.h"

QT_BEGIN_NAMESPACE
using namespace caret;
class Ui_TimeCourseDialog
{
public:
    QHBoxLayout *horizontalLayout_6;
    QVBoxLayout *verticalLayout_3;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QDoubleSpinBox *TDMinTime;
    QDoubleSpinBox *TDMaxTime;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QDoubleSpinBox *TDMinActivity;
    QDoubleSpinBox *TDMaxActivity;
    QCheckBox *TDShowAverage;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *TDClearChart;
    QSpacerItem *horizontalSpacer_3;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QDoubleSpinBox *TDKeepLast;
    QSpacerItem *horizontalSpacer;
    QTextEdit *TDCoordsText;
    QVBoxLayout *verticalLayout_4;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *TDClose;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(TimeCourseDialog *TimeCourseDialog)
    {
        if (TimeCourseDialog->objectName().isEmpty())
            TimeCourseDialog->setObjectName(QString::fromUtf8("TimeCourseDialog"));
        TimeCourseDialog->resize(880, 550);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(TimeCourseDialog->sizePolicy().hasHeightForWidth());
        TimeCourseDialog->setSizePolicy(sizePolicy);
        horizontalLayout_6 = new QHBoxLayout(TimeCourseDialog);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        groupBox = new QGroupBox(TimeCourseDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy1);
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_2->addWidget(label_2);

        TDMinTime = new QDoubleSpinBox(groupBox);
        TDMinTime->setObjectName(QString::fromUtf8("TDMinTime"));

        horizontalLayout_2->addWidget(TDMinTime);

        TDMaxTime = new QDoubleSpinBox(groupBox);
        TDMaxTime->setObjectName(QString::fromUtf8("TDMaxTime"));

        horizontalLayout_2->addWidget(TDMaxTime);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy2);

        horizontalLayout->addWidget(label);

        TDMinActivity = new QDoubleSpinBox(groupBox);
        TDMinActivity->setObjectName(QString::fromUtf8("TDMinActivity"));

        horizontalLayout->addWidget(TDMinActivity);

        TDMaxActivity = new QDoubleSpinBox(groupBox);
        TDMaxActivity->setObjectName(QString::fromUtf8("TDMaxActivity"));

        horizontalLayout->addWidget(TDMaxActivity);


        verticalLayout->addLayout(horizontalLayout);

        TDShowAverage = new QCheckBox(groupBox);
        TDShowAverage->setObjectName(QString::fromUtf8("TDShowAverage"));

        verticalLayout->addWidget(TDShowAverage);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        TDClearChart = new QPushButton(groupBox);
        TDClearChart->setObjectName(QString::fromUtf8("TDClearChart"));

        horizontalLayout_5->addWidget(TDClearChart);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_3);


        verticalLayout->addLayout(horizontalLayout_5);


        gridLayout_2->addLayout(verticalLayout, 0, 0, 1, 1);


        verticalLayout_3->addWidget(groupBox);

        groupBox_2 = new QGroupBox(TimeCourseDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        sizePolicy1.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy1);
        gridLayout = new QGridLayout(groupBox_2);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_3->addWidget(label_3);

        TDKeepLast = new QDoubleSpinBox(groupBox_2);
        TDKeepLast->setObjectName(QString::fromUtf8("TDKeepLast"));

        horizontalLayout_3->addWidget(TDKeepLast);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);


        verticalLayout_2->addLayout(horizontalLayout_3);

        TDCoordsText = new QTextEdit(groupBox_2);
        TDCoordsText->setObjectName(QString::fromUtf8("TDCoordsText"));
        QSizePolicy sizePolicy3(QSizePolicy::Maximum, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(TDCoordsText->sizePolicy().hasHeightForWidth());
        TDCoordsText->setSizePolicy(sizePolicy3);

        verticalLayout_2->addWidget(TDCoordsText);


        gridLayout->addLayout(verticalLayout_2, 0, 0, 1, 1);


        verticalLayout_3->addWidget(groupBox_2);


        horizontalLayout_6->addLayout(verticalLayout_3);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setSizeConstraint(QLayout::SetMaximumSize);
        widget = new QWidget(TimeCourseDialog);
        widget->setObjectName(QString::fromUtf8("widget"));
        sizePolicy.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy);

        verticalLayout_4->addWidget(widget);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        TDClose = new QPushButton(TimeCourseDialog);
        TDClose->setObjectName(QString::fromUtf8("TDClose"));

        horizontalLayout_4->addWidget(TDClose);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);


        verticalLayout_4->addLayout(horizontalLayout_4);


        horizontalLayout_6->addLayout(verticalLayout_4);


        retranslateUi(TimeCourseDialog);

        QMetaObject::connectSlotsByName(TimeCourseDialog);
    } // setupUi

    void retranslateUi(TimeCourseDialog *TimeCourseDialog)
    {
        TimeCourseDialog->setWindowTitle(QApplication::translate("TimeCourseDialog", "Time Course", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("TimeCourseDialog", "Data Controls", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("TimeCourseDialog", "Time (seconds)", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("TimeCourseDialog", "Activity", 0, QApplication::UnicodeUTF8));
        TDShowAverage->setText(QApplication::translate("TimeCourseDialog", "Show Average", 0, QApplication::UnicodeUTF8));
        TDClearChart->setText(QApplication::translate("TimeCourseDialog", "Clear Chart", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("TimeCourseDialog", "History", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("TimeCourseDialog", "Keep Last", 0, QApplication::UnicodeUTF8));
        TDClose->setText(QApplication::translate("TimeCourseDialog", "Close", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class TimeCourseDialog: public Ui_TimeCourseDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TIMECOURSEDIALOG_H

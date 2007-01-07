/********************************************************************************
** Form generated from reading ui file 'waypointdialog.ui'
**
** Created: Sun Jan 7 13:47:49 2007
**      by: Qt User Interface Compiler version 4.2.1
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef WAYPOINTDIALOG_H
#define WAYPOINTDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>

class Ui_WaypointDialog
{
public:
    QWidget *widget;
    QGridLayout *gridLayout;
    QLabel *waitingTimeLabel;
    QLabel *maxSpeedLabel;
    QSpinBox *headingSpin;
    QPushButton *cancelButton;
    QSpacerItem *spacerItem;
    QDoubleSpinBox *waitingTimeSpin;
    QSpacerItem *spacerItem1;
    QSpinBox *maxTurnrateSpin;
    QLabel *yLabel;
    QLabel *xLabel;
    QPushButton *okButton;
    QDoubleSpinBox *xSpin;
    QSpacerItem *spacerItem2;
    QSpinBox *headingTolSpin;
    QSpacerItem *spacerItem3;
    QLabel *timeLabel;
    QLabel *label;
    QLabel *maxTurnrateLabel;
    QDoubleSpinBox *timeSpin;
    QLabel *distanceTolLabel;
    QSpacerItem *spacerItem4;
    QLabel *headingLabel;
    QSpacerItem *spacerItem5;
    QLabel *headingTolLabel;
    QSpacerItem *spacerItem6;
    QDoubleSpinBox *ySpin;
    QSpacerItem *spacerItem7;
    QDoubleSpinBox *distanceTolSpin;
    QDoubleSpinBox *maxSpeedSpin;

    void setupUi(QDialog *WaypointDialog)
    {
    WaypointDialog->setObjectName(QString::fromUtf8("WaypointDialog"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(WaypointDialog->sizePolicy().hasHeightForWidth());
    WaypointDialog->setSizePolicy(sizePolicy);
    widget = new QWidget(WaypointDialog);
    widget->setObjectName(QString::fromUtf8("widget"));
    widget->setGeometry(QRect(1, 20, 299, 384));
    gridLayout = new QGridLayout(widget);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(0);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    waitingTimeLabel = new QLabel(widget);
    waitingTimeLabel->setObjectName(QString::fromUtf8("waitingTimeLabel"));

    gridLayout->addWidget(waitingTimeLabel, 4, 5, 2, 1);

    maxSpeedLabel = new QLabel(widget);
    maxSpeedLabel->setObjectName(QString::fromUtf8("maxSpeedLabel"));

    gridLayout->addWidget(maxSpeedLabel, 11, 0, 1, 3);

    headingSpin = new QSpinBox(widget);
    headingSpin->setObjectName(QString::fromUtf8("headingSpin"));
    headingSpin->setMaximum(359);
    headingSpin->setSingleStep(1);
    headingSpin->setValue(90);

    gridLayout->addWidget(headingSpin, 2, 3, 1, 3);

    cancelButton = new QPushButton(widget);
    cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

    gridLayout->addWidget(cancelButton, 14, 5, 1, 1);

    spacerItem = new QSpacerItem(101, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(spacerItem, 9, 1, 1, 4);

    waitingTimeSpin = new QDoubleSpinBox(widget);
    waitingTimeSpin->setObjectName(QString::fromUtf8("waitingTimeSpin"));
    waitingTimeSpin->setDecimals(1);
    waitingTimeSpin->setMaximum(10000);
    waitingTimeSpin->setMinimum(-10000);
    waitingTimeSpin->setSingleStep(0.5);
    waitingTimeSpin->setValue(5);

    gridLayout->addWidget(waitingTimeSpin, 6, 4, 1, 2);

    spacerItem1 = new QSpacerItem(231, 21, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem1, 10, 0, 1, 6);

    maxTurnrateSpin = new QSpinBox(widget);
    maxTurnrateSpin->setObjectName(QString::fromUtf8("maxTurnrateSpin"));
    maxTurnrateSpin->setMaximum(2000000);
    maxTurnrateSpin->setSingleStep(1);
    maxTurnrateSpin->setValue(5);

    gridLayout->addWidget(maxTurnrateSpin, 12, 5, 1, 1);

    yLabel = new QLabel(widget);
    yLabel->setObjectName(QString::fromUtf8("yLabel"));

    gridLayout->addWidget(yLabel, 1, 1, 1, 2);

    xLabel = new QLabel(widget);
    xLabel->setObjectName(QString::fromUtf8("xLabel"));

    gridLayout->addWidget(xLabel, 1, 0, 1, 1);

    okButton = new QPushButton(widget);
    okButton->setObjectName(QString::fromUtf8("okButton"));

    gridLayout->addWidget(okButton, 14, 2, 1, 3);

    xSpin = new QDoubleSpinBox(widget);
    xSpin->setObjectName(QString::fromUtf8("xSpin"));
    xSpin->setMaximum(10000);
    xSpin->setMinimum(-10000);
    xSpin->setSingleStep(0.2);
    xSpin->setValue(2);

    gridLayout->addWidget(xSpin, 2, 0, 1, 1);

    spacerItem2 = new QSpacerItem(81, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(spacerItem2, 14, 0, 1, 2);

    headingTolSpin = new QSpinBox(widget);
    headingTolSpin->setObjectName(QString::fromUtf8("headingTolSpin"));
    headingTolSpin->setMaximum(180);
    headingTolSpin->setSingleStep(5);
    headingTolSpin->setValue(90);

    gridLayout->addWidget(headingTolSpin, 9, 5, 1, 1);

    spacerItem3 = new QSpacerItem(91, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(spacerItem3, 12, 1, 1, 3);

    timeLabel = new QLabel(widget);
    timeLabel->setObjectName(QString::fromUtf8("timeLabel"));

    gridLayout->addWidget(timeLabel, 4, 0, 2, 3);

    label = new QLabel(widget);
    label->setObjectName(QString::fromUtf8("label"));

    gridLayout->addWidget(label, 0, 0, 1, 6);

    maxTurnrateLabel = new QLabel(widget);
    maxTurnrateLabel->setObjectName(QString::fromUtf8("maxTurnrateLabel"));

    gridLayout->addWidget(maxTurnrateLabel, 11, 3, 1, 3);

    timeSpin = new QDoubleSpinBox(widget);
    timeSpin->setObjectName(QString::fromUtf8("timeSpin"));
    timeSpin->setDecimals(1);
    timeSpin->setMaximum(10000);
    timeSpin->setMinimum(-10000);
    timeSpin->setSingleStep(0.5);
    timeSpin->setValue(5);

    gridLayout->addWidget(timeSpin, 6, 0, 1, 1);

    distanceTolLabel = new QLabel(widget);
    distanceTolLabel->setObjectName(QString::fromUtf8("distanceTolLabel"));

    gridLayout->addWidget(distanceTolLabel, 8, 0, 1, 3);

    spacerItem4 = new QSpacerItem(231, 21, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem4, 13, 0, 1, 6);

    headingLabel = new QLabel(widget);
    headingLabel->setObjectName(QString::fromUtf8("headingLabel"));

    gridLayout->addWidget(headingLabel, 1, 3, 1, 3);

    spacerItem5 = new QSpacerItem(241, 21, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem5, 7, 0, 1, 6);

    headingTolLabel = new QLabel(widget);
    headingTolLabel->setObjectName(QString::fromUtf8("headingTolLabel"));

    gridLayout->addWidget(headingTolLabel, 8, 3, 1, 3);

    spacerItem6 = new QSpacerItem(241, 21, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem6, 3, 0, 1, 6);

    ySpin = new QDoubleSpinBox(widget);
    ySpin->setObjectName(QString::fromUtf8("ySpin"));
    ySpin->setMaximum(10000);
    ySpin->setMinimum(-10000);
    ySpin->setSingleStep(0.2);
    ySpin->setValue(0);

    gridLayout->addWidget(ySpin, 2, 1, 1, 2);

    spacerItem7 = new QSpacerItem(91, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(spacerItem7, 5, 1, 2, 3);

    distanceTolSpin = new QDoubleSpinBox(widget);
    distanceTolSpin->setObjectName(QString::fromUtf8("distanceTolSpin"));
    distanceTolSpin->setMaximum(10);
    distanceTolSpin->setSingleStep(0.2);
    distanceTolSpin->setValue(2);

    gridLayout->addWidget(distanceTolSpin, 9, 0, 1, 1);

    maxSpeedSpin = new QDoubleSpinBox(widget);
    maxSpeedSpin->setObjectName(QString::fromUtf8("maxSpeedSpin"));
    maxSpeedSpin->setMaximum(2e+06);
    maxSpeedSpin->setSingleStep(0.2);
    maxSpeedSpin->setValue(0);

    gridLayout->addWidget(maxSpeedSpin, 12, 0, 1, 1);


    retranslateUi(WaypointDialog);

    QSize size(348, 471);
    size = size.expandedTo(WaypointDialog->minimumSizeHint());
    WaypointDialog->resize(size);

    QObject::connect(okButton, SIGNAL(clicked()), WaypointDialog, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), WaypointDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(WaypointDialog);
    } // setupUi

    void retranslateUi(QDialog *WaypointDialog)
    {
    WaypointDialog->setWindowTitle(QApplication::translate("WaypointDialog", "Waypoint Paramters", 0, QApplication::UnicodeUTF8));
    waitingTimeLabel->setText(QApplication::translate("WaypointDialog", "waiting time [s]:", 0, QApplication::UnicodeUTF8));
    maxSpeedLabel->setText(QApplication::translate("WaypointDialog", "Maximum speed [m/s]:", 0, QApplication::UnicodeUTF8));
    cancelButton->setText(QApplication::translate("WaypointDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    yLabel->setText(QApplication::translate("WaypointDialog", "y [m]:", 0, QApplication::UnicodeUTF8));
    xLabel->setText(QApplication::translate("WaypointDialog", "x [m]:", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("WaypointDialog", "OK", 0, QApplication::UnicodeUTF8));
    timeLabel->setText(QApplication::translate("WaypointDialog", "time to reach [s]:", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("WaypointDialog", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:Sans Serif; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-weight:600;\">Waypoint Parameters</span></p></body></html>", 0, QApplication::UnicodeUTF8));
    maxTurnrateLabel->setText(QApplication::translate("WaypointDialog", "Maximum turnrate [deg/s]:", 0, QApplication::UnicodeUTF8));
    distanceTolLabel->setText(QApplication::translate("WaypointDialog", "Distance tolerance [m]:", 0, QApplication::UnicodeUTF8));
    headingLabel->setText(QApplication::translate("WaypointDialog", "heading [deg]:", 0, QApplication::UnicodeUTF8));
    headingTolLabel->setText(QApplication::translate("WaypointDialog", "Heading tol. (+/-)  [deg]:", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(WaypointDialog);
    } // retranslateUi

};

namespace Ui {
    class WaypointDialog: public Ui_WaypointDialog {};
} // namespace Ui

#endif // WAYPOINTDIALOG_H

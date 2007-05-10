/********************************************************************************
** Form generated from reading ui file 'waypointdialog.ui'
**
** Created: Thu May 10 22:50:44 2007
**      by: Qt User Interface Compiler version 4.2.2
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
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_WaypointDialog
{
public:
    QWidget *layoutWidget;
    QVBoxLayout *vboxLayout;
    QLabel *label;
    QGridLayout *gridLayout;
    QLabel *headingLabel;
    QDoubleSpinBox *xSpin;
    QLabel *xLabel;
    QDoubleSpinBox *ySpin;
    QSpinBox *headingSpin;
    QLabel *yLabel;
    QSpacerItem *spacerItem;
    QGridLayout *gridLayout1;
    QLabel *timeLabel;
    QDoubleSpinBox *waitingTimeSpin;
    QLabel *waitingTimeLabel;
    QDoubleSpinBox *timeSpin;
    QSpacerItem *spacerItem1;
    QGridLayout *gridLayout2;
    QSpinBox *headingTolSpin;
    QLabel *headingTolLabel;
    QLabel *distanceTolLabel;
    QDoubleSpinBox *distanceTolSpin;
    QSpacerItem *spacerItem2;
    QGridLayout *gridLayout3;
    QLabel *maxTurnrateLabel;
    QDoubleSpinBox *maxSpeedSpin;
    QSpinBox *maxTurnrateSpin;
    QLabel *maxSpeedLabel;
    QSpacerItem *spacerItem3;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem4;
    QPushButton *okButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *WaypointDialog)
    {
    WaypointDialog->setObjectName(QString::fromUtf8("WaypointDialog"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(WaypointDialog->sizePolicy().hasHeightForWidth());
    WaypointDialog->setSizePolicy(sizePolicy);
    layoutWidget = new QWidget(WaypointDialog);
    layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
    layoutWidget->setGeometry(QRect(0, 21, 248, 362));
    vboxLayout = new QVBoxLayout(layoutWidget);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(0);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    label = new QLabel(layoutWidget);
    label->setObjectName(QString::fromUtf8("label"));

    vboxLayout->addWidget(label);

    gridLayout = new QGridLayout();
    gridLayout->setSpacing(6);
    gridLayout->setMargin(0);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    headingLabel = new QLabel(layoutWidget);
    headingLabel->setObjectName(QString::fromUtf8("headingLabel"));

    gridLayout->addWidget(headingLabel, 0, 2, 1, 1);

    xSpin = new QDoubleSpinBox(layoutWidget);
    xSpin->setObjectName(QString::fromUtf8("xSpin"));
    xSpin->setMaximum(10000);
    xSpin->setMinimum(-10000);
    xSpin->setSingleStep(0.2);
    xSpin->setValue(2);

    gridLayout->addWidget(xSpin, 1, 0, 1, 1);

    xLabel = new QLabel(layoutWidget);
    xLabel->setObjectName(QString::fromUtf8("xLabel"));

    gridLayout->addWidget(xLabel, 0, 0, 1, 1);

    ySpin = new QDoubleSpinBox(layoutWidget);
    ySpin->setObjectName(QString::fromUtf8("ySpin"));
    ySpin->setMaximum(10000);
    ySpin->setMinimum(-10000);
    ySpin->setSingleStep(0.2);
    ySpin->setValue(0);

    gridLayout->addWidget(ySpin, 1, 1, 1, 1);

    headingSpin = new QSpinBox(layoutWidget);
    headingSpin->setObjectName(QString::fromUtf8("headingSpin"));
    headingSpin->setMaximum(359);
    headingSpin->setSingleStep(1);
    headingSpin->setValue(90);

    gridLayout->addWidget(headingSpin, 1, 2, 1, 1);

    yLabel = new QLabel(layoutWidget);
    yLabel->setObjectName(QString::fromUtf8("yLabel"));

    gridLayout->addWidget(yLabel, 0, 1, 1, 1);


    vboxLayout->addLayout(gridLayout);

    spacerItem = new QSpacerItem(221, 21, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout->addItem(spacerItem);

    gridLayout1 = new QGridLayout();
    gridLayout1->setSpacing(6);
    gridLayout1->setMargin(0);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    timeLabel = new QLabel(layoutWidget);
    timeLabel->setObjectName(QString::fromUtf8("timeLabel"));

    gridLayout1->addWidget(timeLabel, 0, 0, 1, 1);

    waitingTimeSpin = new QDoubleSpinBox(layoutWidget);
    waitingTimeSpin->setObjectName(QString::fromUtf8("waitingTimeSpin"));
    waitingTimeSpin->setDecimals(1);
    waitingTimeSpin->setMaximum(10000);
    waitingTimeSpin->setMinimum(-10000);
    waitingTimeSpin->setSingleStep(0.5);
    waitingTimeSpin->setValue(5);

    gridLayout1->addWidget(waitingTimeSpin, 1, 1, 1, 1);

    waitingTimeLabel = new QLabel(layoutWidget);
    waitingTimeLabel->setObjectName(QString::fromUtf8("waitingTimeLabel"));

    gridLayout1->addWidget(waitingTimeLabel, 0, 1, 1, 1);

    timeSpin = new QDoubleSpinBox(layoutWidget);
    timeSpin->setObjectName(QString::fromUtf8("timeSpin"));
    timeSpin->setDecimals(1);
    timeSpin->setMaximum(10000);
    timeSpin->setMinimum(-10000);
    timeSpin->setSingleStep(0.5);
    timeSpin->setValue(5);

    gridLayout1->addWidget(timeSpin, 1, 0, 1, 1);


    vboxLayout->addLayout(gridLayout1);

    spacerItem1 = new QSpacerItem(171, 21, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout->addItem(spacerItem1);

    gridLayout2 = new QGridLayout();
    gridLayout2->setSpacing(6);
    gridLayout2->setMargin(0);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    headingTolSpin = new QSpinBox(layoutWidget);
    headingTolSpin->setObjectName(QString::fromUtf8("headingTolSpin"));
    headingTolSpin->setMaximum(360);
    headingTolSpin->setSingleStep(5);
    headingTolSpin->setValue(180);

    gridLayout2->addWidget(headingTolSpin, 1, 1, 1, 1);

    headingTolLabel = new QLabel(layoutWidget);
    headingTolLabel->setObjectName(QString::fromUtf8("headingTolLabel"));

    gridLayout2->addWidget(headingTolLabel, 0, 1, 1, 1);

    distanceTolLabel = new QLabel(layoutWidget);
    distanceTolLabel->setObjectName(QString::fromUtf8("distanceTolLabel"));

    gridLayout2->addWidget(distanceTolLabel, 0, 0, 1, 1);

    distanceTolSpin = new QDoubleSpinBox(layoutWidget);
    distanceTolSpin->setObjectName(QString::fromUtf8("distanceTolSpin"));
    distanceTolSpin->setMaximum(10);
    distanceTolSpin->setSingleStep(0.2);
    distanceTolSpin->setValue(2);

    gridLayout2->addWidget(distanceTolSpin, 1, 0, 1, 1);


    vboxLayout->addLayout(gridLayout2);

    spacerItem2 = new QSpacerItem(211, 21, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout->addItem(spacerItem2);

    gridLayout3 = new QGridLayout();
    gridLayout3->setSpacing(6);
    gridLayout3->setMargin(0);
    gridLayout3->setObjectName(QString::fromUtf8("gridLayout3"));
    maxTurnrateLabel = new QLabel(layoutWidget);
    maxTurnrateLabel->setObjectName(QString::fromUtf8("maxTurnrateLabel"));

    gridLayout3->addWidget(maxTurnrateLabel, 0, 1, 1, 1);

    maxSpeedSpin = new QDoubleSpinBox(layoutWidget);
    maxSpeedSpin->setObjectName(QString::fromUtf8("maxSpeedSpin"));
    maxSpeedSpin->setMaximum(2e+06);
    maxSpeedSpin->setSingleStep(0.2);
    maxSpeedSpin->setValue(0);

    gridLayout3->addWidget(maxSpeedSpin, 1, 0, 1, 1);

    maxTurnrateSpin = new QSpinBox(layoutWidget);
    maxTurnrateSpin->setObjectName(QString::fromUtf8("maxTurnrateSpin"));
    maxTurnrateSpin->setMaximum(2000000);
    maxTurnrateSpin->setSingleStep(1);
    maxTurnrateSpin->setValue(5);

    gridLayout3->addWidget(maxTurnrateSpin, 1, 1, 1, 1);

    maxSpeedLabel = new QLabel(layoutWidget);
    maxSpeedLabel->setObjectName(QString::fromUtf8("maxSpeedLabel"));

    gridLayout3->addWidget(maxSpeedLabel, 0, 0, 1, 1);


    vboxLayout->addLayout(gridLayout3);

    spacerItem3 = new QSpacerItem(171, 21, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout->addItem(spacerItem3);

    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    spacerItem4 = new QSpacerItem(41, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem4);

    okButton = new QPushButton(layoutWidget);
    okButton->setObjectName(QString::fromUtf8("okButton"));

    hboxLayout->addWidget(okButton);

    cancelButton = new QPushButton(layoutWidget);
    cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

    hboxLayout->addWidget(cancelButton);


    vboxLayout->addLayout(hboxLayout);


    retranslateUi(WaypointDialog);

    QSize size(260, 405);
    size = size.expandedTo(WaypointDialog->minimumSizeHint());
    WaypointDialog->resize(size);

    QObject::connect(okButton, SIGNAL(clicked()), WaypointDialog, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), WaypointDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(WaypointDialog);
    } // setupUi

    void retranslateUi(QDialog *WaypointDialog)
    {
    WaypointDialog->setWindowTitle(QApplication::translate("WaypointDialog", "Waypoint Paramters", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("WaypointDialog", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:Sans Serif; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-weight:600;\">Waypoint Parameters</span></p></body></html>", 0, QApplication::UnicodeUTF8));
    headingLabel->setText(QApplication::translate("WaypointDialog", "heading [deg]:", 0, QApplication::UnicodeUTF8));
    xLabel->setText(QApplication::translate("WaypointDialog", "x [m]:", 0, QApplication::UnicodeUTF8));
    yLabel->setText(QApplication::translate("WaypointDialog", "y [m]:", 0, QApplication::UnicodeUTF8));
    timeLabel->setText(QApplication::translate("WaypointDialog", "time to reach [s]:", 0, QApplication::UnicodeUTF8));
    waitingTimeLabel->setText(QApplication::translate("WaypointDialog", "waiting time [s]:", 0, QApplication::UnicodeUTF8));
    headingTolLabel->setText(QApplication::translate("WaypointDialog", "Heading tol. (+/-)  [deg]:", 0, QApplication::UnicodeUTF8));
    distanceTolLabel->setText(QApplication::translate("WaypointDialog", "Distance tolerance [m]:", 0, QApplication::UnicodeUTF8));
    maxTurnrateLabel->setText(QApplication::translate("WaypointDialog", "Maximum turnrate [deg/s]:", 0, QApplication::UnicodeUTF8));
    maxSpeedLabel->setText(QApplication::translate("WaypointDialog", "Maximum speed [m/s]:", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("WaypointDialog", "OK", 0, QApplication::UnicodeUTF8));
    cancelButton->setText(QApplication::translate("WaypointDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(WaypointDialog);
    } // retranslateUi

};

namespace Ui {
    class WaypointDialog: public Ui_WaypointDialog {};
} // namespace Ui

#endif // WAYPOINTDIALOG_H

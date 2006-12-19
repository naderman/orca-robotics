/********************************************************************************
** Form generated from reading ui file 'waypointdialog.ui'
**
** Created: Tue Dec 19 13:43:46 2006
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
    QWidget *widget;
    QVBoxLayout *vboxLayout;
    QGridLayout *gridLayout;
    QDoubleSpinBox *timeSpin;
    QSpacerItem *spacerItem;
    QSpacerItem *spacerItem1;
    QLabel *maxTurnrateLabel;
    QSpacerItem *spacerItem2;
    QSpinBox *headingTolSpin;
    QDoubleSpinBox *ySpin;
    QSpinBox *headingSpin;
    QLabel *timeLabel;
    QLabel *yLabel;
    QLabel *xLabel;
    QLabel *maxSpeedLabel;
    QLabel *distanceTolLabel;
    QDoubleSpinBox *distanceTolSpin;
    QLabel *headingLabel;
    QLabel *headingTolLabel;
    QSpacerItem *spacerItem3;
    QSpacerItem *spacerItem4;
    QDoubleSpinBox *xSpin;
    QLabel *label;
    QDoubleSpinBox *maxSpeedSpin;
    QSpinBox *maxTurnrateSpin;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem5;
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
    widget = new QWidget(WaypointDialog);
    widget->setObjectName(QString::fromUtf8("widget"));
    widget->setGeometry(QRect(10, 13, 326, 330));
    vboxLayout = new QVBoxLayout(widget);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(0);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    gridLayout = new QGridLayout();
    gridLayout->setSpacing(6);
    gridLayout->setMargin(0);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    timeSpin = new QDoubleSpinBox(widget);
    timeSpin->setObjectName(QString::fromUtf8("timeSpin"));
    timeSpin->setDecimals(1);
    timeSpin->setMaximum(10000);
    timeSpin->setMinimum(-10000);
    timeSpin->setSingleStep(0.5);
    timeSpin->setValue(5);

    gridLayout->addWidget(timeSpin, 2, 3, 1, 1);

    spacerItem = new QSpacerItem(162, 37, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem, 6, 0, 1, 2);

    spacerItem1 = new QSpacerItem(162, 37, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem1, 3, 0, 1, 2);

    maxTurnrateLabel = new QLabel(widget);
    maxTurnrateLabel->setObjectName(QString::fromUtf8("maxTurnrateLabel"));

    gridLayout->addWidget(maxTurnrateLabel, 7, 2, 1, 2);

    spacerItem2 = new QSpacerItem(154, 37, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem2, 6, 2, 1, 2);

    headingTolSpin = new QSpinBox(widget);
    headingTolSpin->setObjectName(QString::fromUtf8("headingTolSpin"));
    headingTolSpin->setMaximum(180);
    headingTolSpin->setSingleStep(5);
    headingTolSpin->setValue(90);

    gridLayout->addWidget(headingTolSpin, 5, 2, 1, 2);

    ySpin = new QDoubleSpinBox(widget);
    ySpin->setObjectName(QString::fromUtf8("ySpin"));
    ySpin->setMaximum(10000);
    ySpin->setMinimum(-10000);
    ySpin->setSingleStep(0.2);
    ySpin->setValue(0);

    gridLayout->addWidget(ySpin, 2, 1, 1, 1);

    headingSpin = new QSpinBox(widget);
    headingSpin->setObjectName(QString::fromUtf8("headingSpin"));
    headingSpin->setMaximum(359);
    headingSpin->setSingleStep(1);
    headingSpin->setValue(90);

    gridLayout->addWidget(headingSpin, 2, 2, 1, 1);

    timeLabel = new QLabel(widget);
    timeLabel->setObjectName(QString::fromUtf8("timeLabel"));

    gridLayout->addWidget(timeLabel, 1, 3, 1, 1);

    yLabel = new QLabel(widget);
    yLabel->setObjectName(QString::fromUtf8("yLabel"));

    gridLayout->addWidget(yLabel, 1, 1, 1, 1);

    xLabel = new QLabel(widget);
    xLabel->setObjectName(QString::fromUtf8("xLabel"));

    gridLayout->addWidget(xLabel, 1, 0, 1, 1);

    maxSpeedLabel = new QLabel(widget);
    maxSpeedLabel->setObjectName(QString::fromUtf8("maxSpeedLabel"));

    gridLayout->addWidget(maxSpeedLabel, 7, 0, 1, 2);

    distanceTolLabel = new QLabel(widget);
    distanceTolLabel->setObjectName(QString::fromUtf8("distanceTolLabel"));

    gridLayout->addWidget(distanceTolLabel, 4, 0, 1, 2);

    distanceTolSpin = new QDoubleSpinBox(widget);
    distanceTolSpin->setObjectName(QString::fromUtf8("distanceTolSpin"));
    distanceTolSpin->setMaximum(10);
    distanceTolSpin->setSingleStep(0.2);
    distanceTolSpin->setValue(2);

    gridLayout->addWidget(distanceTolSpin, 5, 0, 1, 2);

    headingLabel = new QLabel(widget);
    headingLabel->setObjectName(QString::fromUtf8("headingLabel"));

    gridLayout->addWidget(headingLabel, 1, 2, 1, 1);

    headingTolLabel = new QLabel(widget);
    headingTolLabel->setObjectName(QString::fromUtf8("headingTolLabel"));

    gridLayout->addWidget(headingTolLabel, 4, 2, 1, 2);

    spacerItem3 = new QSpacerItem(154, 37, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem3, 3, 2, 1, 2);

    spacerItem4 = new QSpacerItem(154, 29, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem4, 9, 2, 1, 2);

    xSpin = new QDoubleSpinBox(widget);
    xSpin->setObjectName(QString::fromUtf8("xSpin"));
    xSpin->setMaximum(10000);
    xSpin->setMinimum(-10000);
    xSpin->setSingleStep(0.2);
    xSpin->setValue(2);

    gridLayout->addWidget(xSpin, 2, 0, 1, 1);

    label = new QLabel(widget);
    label->setObjectName(QString::fromUtf8("label"));

    gridLayout->addWidget(label, 0, 0, 1, 4);

    maxSpeedSpin = new QDoubleSpinBox(widget);
    maxSpeedSpin->setObjectName(QString::fromUtf8("maxSpeedSpin"));
    maxSpeedSpin->setMaximum(10);
    maxSpeedSpin->setSingleStep(0.2);
    maxSpeedSpin->setValue(1);

    gridLayout->addWidget(maxSpeedSpin, 8, 0, 1, 2);

    maxTurnrateSpin = new QSpinBox(widget);
    maxTurnrateSpin->setObjectName(QString::fromUtf8("maxTurnrateSpin"));
    maxTurnrateSpin->setMaximum(100);
    maxTurnrateSpin->setSingleStep(1);
    maxTurnrateSpin->setValue(5);

    gridLayout->addWidget(maxTurnrateSpin, 8, 2, 1, 2);


    vboxLayout->addLayout(gridLayout);

    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    spacerItem5 = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem5);

    okButton = new QPushButton(widget);
    okButton->setObjectName(QString::fromUtf8("okButton"));

    hboxLayout->addWidget(okButton);

    cancelButton = new QPushButton(widget);
    cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

    hboxLayout->addWidget(cancelButton);


    vboxLayout->addLayout(hboxLayout);


    retranslateUi(WaypointDialog);

    QSize size(344, 360);
    size = size.expandedTo(WaypointDialog->minimumSizeHint());
    WaypointDialog->resize(size);

    QObject::connect(okButton, SIGNAL(clicked()), WaypointDialog, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), WaypointDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(WaypointDialog);
    } // setupUi

    void retranslateUi(QDialog *WaypointDialog)
    {
    WaypointDialog->setWindowTitle(QApplication::translate("WaypointDialog", "Waypoint Paramters", 0, QApplication::UnicodeUTF8));
    maxTurnrateLabel->setText(QApplication::translate("WaypointDialog", "Maximum turnrate [deg/s]:", 0, QApplication::UnicodeUTF8));
    timeLabel->setText(QApplication::translate("WaypointDialog", "tims [s]:", 0, QApplication::UnicodeUTF8));
    yLabel->setText(QApplication::translate("WaypointDialog", "y [m]:", 0, QApplication::UnicodeUTF8));
    xLabel->setText(QApplication::translate("WaypointDialog", "x [m]:", 0, QApplication::UnicodeUTF8));
    maxSpeedLabel->setText(QApplication::translate("WaypointDialog", "Maximum speed [m/s]:", 0, QApplication::UnicodeUTF8));
    distanceTolLabel->setText(QApplication::translate("WaypointDialog", "Distance tolerance [m]:", 0, QApplication::UnicodeUTF8));
    headingLabel->setText(QApplication::translate("WaypointDialog", "heading [deg]:", 0, QApplication::UnicodeUTF8));
    headingTolLabel->setText(QApplication::translate("WaypointDialog", "Heading tol. (+/-)  [deg]:", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("WaypointDialog", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:Sans Serif; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-weight:600;\">Waypoint Parameters</span></p></body></html>", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("WaypointDialog", "OK", 0, QApplication::UnicodeUTF8));
    cancelButton->setText(QApplication::translate("WaypointDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(WaypointDialog);
    } // retranslateUi

};

namespace Ui {
    class WaypointDialog: public Ui_WaypointDialog {};
} // namespace Ui

#endif // WAYPOINTDIALOG_H

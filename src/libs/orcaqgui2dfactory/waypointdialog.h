/********************************************************************************
** Form generated from reading ui file 'waypointdialog.ui'
**
** Created: Sun Jul 27 21:59:31 2008
**      by: Qt User Interface Compiler version 4.4.0
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

QT_BEGIN_NAMESPACE

class Ui_WaypointDialog
{
public:
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QGridLayout *gridLayout;
    QLabel *headingLabel;
    QDoubleSpinBox *xSpin;
    QLabel *xLabel;
    QDoubleSpinBox *ySpin;
    QSpinBox *headingSpin;
    QLabel *yLabel;
    QSpacerItem *spacerItem;
    QGridLayout *globalGridLayout;
    QLabel *timeLabel;
    QDoubleSpinBox *timeSpin;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *spacerItem1;
    QGridLayout *gridLayout1;
    QSpinBox *headingTolSpin;
    QLabel *headingTolLabel;
    QLabel *distanceTolLabel;
    QDoubleSpinBox *distanceTolSpin;
    QSpacerItem *spacerItem2;
    QGridLayout *gridLayout2;
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
    if (WaypointDialog->objectName().isEmpty())
        WaypointDialog->setObjectName(QString::fromUtf8("WaypointDialog"));
    WaypointDialog->resize(288, 386);
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(WaypointDialog->sizePolicy().hasHeightForWidth());
    WaypointDialog->setSizePolicy(sizePolicy);
    widget = new QWidget(WaypointDialog);
    widget->setObjectName(QString::fromUtf8("widget"));
    widget->setGeometry(QRect(1, 22, 274, 348));
    verticalLayout = new QVBoxLayout(widget);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    verticalLayout->setContentsMargins(0, 0, 0, 0);
    label = new QLabel(widget);
    label->setObjectName(QString::fromUtf8("label"));

    verticalLayout->addWidget(label);

    gridLayout = new QGridLayout();
#ifndef Q_OS_MAC
    gridLayout->setSpacing(6);
#endif
    gridLayout->setMargin(0);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    headingLabel = new QLabel(widget);
    headingLabel->setObjectName(QString::fromUtf8("headingLabel"));

    gridLayout->addWidget(headingLabel, 0, 2, 1, 1);

    xSpin = new QDoubleSpinBox(widget);
    xSpin->setObjectName(QString::fromUtf8("xSpin"));
    xSpin->setMinimum(-10000);
    xSpin->setMaximum(10000);
    xSpin->setSingleStep(0.2);
    xSpin->setValue(2);

    gridLayout->addWidget(xSpin, 1, 0, 1, 1);

    xLabel = new QLabel(widget);
    xLabel->setObjectName(QString::fromUtf8("xLabel"));

    gridLayout->addWidget(xLabel, 0, 0, 1, 1);

    ySpin = new QDoubleSpinBox(widget);
    ySpin->setObjectName(QString::fromUtf8("ySpin"));
    ySpin->setMinimum(-10000);
    ySpin->setMaximum(10000);
    ySpin->setSingleStep(0.2);
    ySpin->setValue(0);

    gridLayout->addWidget(ySpin, 1, 1, 1, 1);

    headingSpin = new QSpinBox(widget);
    headingSpin->setObjectName(QString::fromUtf8("headingSpin"));
    headingSpin->setMaximum(359);
    headingSpin->setSingleStep(1);
    headingSpin->setValue(90);

    gridLayout->addWidget(headingSpin, 1, 2, 1, 1);

    yLabel = new QLabel(widget);
    yLabel->setObjectName(QString::fromUtf8("yLabel"));

    gridLayout->addWidget(yLabel, 0, 1, 1, 1);


    verticalLayout->addLayout(gridLayout);

    spacerItem = new QSpacerItem(269, 15, QSizePolicy::Minimum, QSizePolicy::Expanding);

    verticalLayout->addItem(spacerItem);

    globalGridLayout = new QGridLayout();
    globalGridLayout->setObjectName(QString::fromUtf8("globalGridLayout"));
    timeLabel = new QLabel(widget);
    timeLabel->setObjectName(QString::fromUtf8("timeLabel"));

    globalGridLayout->addWidget(timeLabel, 0, 0, 1, 2);

    timeSpin = new QDoubleSpinBox(widget);
    timeSpin->setObjectName(QString::fromUtf8("timeSpin"));
    timeSpin->setDecimals(1);
    timeSpin->setMinimum(-10000);
    timeSpin->setMaximum(10000);
    timeSpin->setSingleStep(0.5);
    timeSpin->setValue(5);

    globalGridLayout->addWidget(timeSpin, 1, 0, 1, 1);

    horizontalSpacer = new QSpacerItem(138, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    globalGridLayout->addItem(horizontalSpacer, 1, 1, 1, 1);


    verticalLayout->addLayout(globalGridLayout);

    spacerItem1 = new QSpacerItem(269, 15, QSizePolicy::Minimum, QSizePolicy::Expanding);

    verticalLayout->addItem(spacerItem1);

    gridLayout1 = new QGridLayout();
#ifndef Q_OS_MAC
    gridLayout1->setSpacing(6);
#endif
    gridLayout1->setMargin(0);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    headingTolSpin = new QSpinBox(widget);
    headingTolSpin->setObjectName(QString::fromUtf8("headingTolSpin"));
    headingTolSpin->setMaximum(360);
    headingTolSpin->setSingleStep(5);
    headingTolSpin->setValue(180);

    gridLayout1->addWidget(headingTolSpin, 1, 1, 1, 1);

    headingTolLabel = new QLabel(widget);
    headingTolLabel->setObjectName(QString::fromUtf8("headingTolLabel"));

    gridLayout1->addWidget(headingTolLabel, 0, 1, 1, 1);

    distanceTolLabel = new QLabel(widget);
    distanceTolLabel->setObjectName(QString::fromUtf8("distanceTolLabel"));

    gridLayout1->addWidget(distanceTolLabel, 0, 0, 1, 1);

    distanceTolSpin = new QDoubleSpinBox(widget);
    distanceTolSpin->setObjectName(QString::fromUtf8("distanceTolSpin"));
    distanceTolSpin->setMaximum(10);
    distanceTolSpin->setSingleStep(0.2);
    distanceTolSpin->setValue(2);

    gridLayout1->addWidget(distanceTolSpin, 1, 0, 1, 1);


    verticalLayout->addLayout(gridLayout1);

    spacerItem2 = new QSpacerItem(269, 16, QSizePolicy::Minimum, QSizePolicy::Expanding);

    verticalLayout->addItem(spacerItem2);

    gridLayout2 = new QGridLayout();
#ifndef Q_OS_MAC
    gridLayout2->setSpacing(6);
#endif
    gridLayout2->setMargin(0);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    maxTurnrateLabel = new QLabel(widget);
    maxTurnrateLabel->setObjectName(QString::fromUtf8("maxTurnrateLabel"));

    gridLayout2->addWidget(maxTurnrateLabel, 0, 1, 1, 1);

    maxSpeedSpin = new QDoubleSpinBox(widget);
    maxSpeedSpin->setObjectName(QString::fromUtf8("maxSpeedSpin"));
    maxSpeedSpin->setMaximum(2e+06);
    maxSpeedSpin->setSingleStep(0.2);
    maxSpeedSpin->setValue(0);

    gridLayout2->addWidget(maxSpeedSpin, 1, 0, 1, 1);

    maxTurnrateSpin = new QSpinBox(widget);
    maxTurnrateSpin->setObjectName(QString::fromUtf8("maxTurnrateSpin"));
    maxTurnrateSpin->setMaximum(2000000);
    maxTurnrateSpin->setSingleStep(1);
    maxTurnrateSpin->setValue(5);

    gridLayout2->addWidget(maxTurnrateSpin, 1, 1, 1, 1);

    maxSpeedLabel = new QLabel(widget);
    maxSpeedLabel->setObjectName(QString::fromUtf8("maxSpeedLabel"));

    gridLayout2->addWidget(maxSpeedLabel, 0, 0, 1, 1);


    verticalLayout->addLayout(gridLayout2);

    spacerItem3 = new QSpacerItem(269, 15, QSizePolicy::Minimum, QSizePolicy::Expanding);

    verticalLayout->addItem(spacerItem3);

    hboxLayout = new QHBoxLayout();
#ifndef Q_OS_MAC
    hboxLayout->setSpacing(6);
#endif
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    spacerItem4 = new QSpacerItem(41, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem4);

    okButton = new QPushButton(widget);
    okButton->setObjectName(QString::fromUtf8("okButton"));

    hboxLayout->addWidget(okButton);

    cancelButton = new QPushButton(widget);
    cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

    hboxLayout->addWidget(cancelButton);


    verticalLayout->addLayout(hboxLayout);


    retranslateUi(WaypointDialog);
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
    timeLabel->setText(QApplication::translate("WaypointDialog", "Time to reach [s]:", 0, QApplication::UnicodeUTF8));
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

QT_END_NAMESPACE

#endif // WAYPOINTDIALOG_H

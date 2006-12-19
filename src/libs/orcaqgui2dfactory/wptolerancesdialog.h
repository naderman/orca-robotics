/********************************************************************************
** Form generated from reading ui file 'wptolerancesdialog.ui'
**
** Created: Tue Dec 19 13:46:25 2006
**      by: Qt User Interface Compiler version 4.2.1
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef WPTOLERANCESDIALOG_H
#define WPTOLERANCESDIALOG_H

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
#include <QtGui/QWidget>

class Ui_WpTolerancesDialog
{
public:
    QWidget *widget;
    QGridLayout *gridLayout;
    QLabel *maxSpeedLabel;
    QSpacerItem *spacerItem;
    QDoubleSpinBox *distanceSpin;
    QLabel *headingLabel;
    QSpinBox *headingSpin;
    QLabel *timeLabel;
    QLabel *label;
    QLabel *distanceLabel;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem1;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QDoubleSpinBox *maxSpeedSpin;
    QLabel *loopsLabel;
    QSpacerItem *spacerItem2;
    QDoubleSpinBox *timePeriodSpin;
    QLabel *maxTurnrateLabel;
    QSpinBox *maxTurnrateSpin;
    QSpacerItem *spacerItem3;
    QSpinBox *numLoopsSpin;

    void setupUi(QDialog *WpTolerancesDialog)
    {
    WpTolerancesDialog->setObjectName(QString::fromUtf8("WpTolerancesDialog"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(WpTolerancesDialog->sizePolicy().hasHeightForWidth());
    WpTolerancesDialog->setSizePolicy(sizePolicy);
    widget = new QWidget(WpTolerancesDialog);
    widget->setObjectName(QString::fromUtf8("widget"));
    widget->setGeometry(QRect(13, 13, 313, 273));
    gridLayout = new QGridLayout(widget);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(0);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    maxSpeedLabel = new QLabel(widget);
    maxSpeedLabel->setObjectName(QString::fromUtf8("maxSpeedLabel"));

    gridLayout->addWidget(maxSpeedLabel, 7, 0, 1, 1);

    spacerItem = new QSpacerItem(301, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem, 9, 0, 1, 2);

    distanceSpin = new QDoubleSpinBox(widget);
    distanceSpin->setObjectName(QString::fromUtf8("distanceSpin"));
    distanceSpin->setMaximum(10);
    distanceSpin->setSingleStep(0.2);
    distanceSpin->setValue(2);

    gridLayout->addWidget(distanceSpin, 5, 0, 1, 1);

    headingLabel = new QLabel(widget);
    headingLabel->setObjectName(QString::fromUtf8("headingLabel"));

    gridLayout->addWidget(headingLabel, 4, 1, 1, 1);

    headingSpin = new QSpinBox(widget);
    headingSpin->setObjectName(QString::fromUtf8("headingSpin"));
    headingSpin->setMaximum(180);
    headingSpin->setSingleStep(5);
    headingSpin->setValue(90);

    gridLayout->addWidget(headingSpin, 5, 1, 1, 1);

    timeLabel = new QLabel(widget);
    timeLabel->setObjectName(QString::fromUtf8("timeLabel"));

    gridLayout->addWidget(timeLabel, 1, 0, 1, 1);

    label = new QLabel(widget);
    label->setObjectName(QString::fromUtf8("label"));

    gridLayout->addWidget(label, 0, 0, 1, 2);

    distanceLabel = new QLabel(widget);
    distanceLabel->setObjectName(QString::fromUtf8("distanceLabel"));

    gridLayout->addWidget(distanceLabel, 4, 0, 1, 1);

    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    spacerItem1 = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem1);

    okButton = new QPushButton(widget);
    okButton->setObjectName(QString::fromUtf8("okButton"));

    hboxLayout->addWidget(okButton);

    cancelButton = new QPushButton(widget);
    cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

    hboxLayout->addWidget(cancelButton);


    gridLayout->addLayout(hboxLayout, 10, 0, 1, 2);

    maxSpeedSpin = new QDoubleSpinBox(widget);
    maxSpeedSpin->setObjectName(QString::fromUtf8("maxSpeedSpin"));
    maxSpeedSpin->setMaximum(10);
    maxSpeedSpin->setSingleStep(0.2);
    maxSpeedSpin->setValue(1);

    gridLayout->addWidget(maxSpeedSpin, 8, 0, 1, 1);

    loopsLabel = new QLabel(widget);
    loopsLabel->setObjectName(QString::fromUtf8("loopsLabel"));

    gridLayout->addWidget(loopsLabel, 1, 1, 1, 1);

    spacerItem2 = new QSpacerItem(301, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem2, 3, 0, 1, 2);

    timePeriodSpin = new QDoubleSpinBox(widget);
    timePeriodSpin->setObjectName(QString::fromUtf8("timePeriodSpin"));
    timePeriodSpin->setDecimals(1);
    timePeriodSpin->setMaximum(10000);
    timePeriodSpin->setSingleStep(0.5);
    timePeriodSpin->setValue(5);

    gridLayout->addWidget(timePeriodSpin, 2, 0, 1, 1);

    maxTurnrateLabel = new QLabel(widget);
    maxTurnrateLabel->setObjectName(QString::fromUtf8("maxTurnrateLabel"));

    gridLayout->addWidget(maxTurnrateLabel, 7, 1, 1, 1);

    maxTurnrateSpin = new QSpinBox(widget);
    maxTurnrateSpin->setObjectName(QString::fromUtf8("maxTurnrateSpin"));
    maxTurnrateSpin->setMaximum(100);
    maxTurnrateSpin->setSingleStep(1);
    maxTurnrateSpin->setValue(5);

    gridLayout->addWidget(maxTurnrateSpin, 8, 1, 1, 1);

    spacerItem3 = new QSpacerItem(311, 16, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem3, 6, 0, 1, 2);

    numLoopsSpin = new QSpinBox(widget);
    numLoopsSpin->setObjectName(QString::fromUtf8("numLoopsSpin"));
    numLoopsSpin->setMaximum(1000);
    numLoopsSpin->setMinimum(1);
    numLoopsSpin->setSingleStep(1);
    numLoopsSpin->setValue(1);

    gridLayout->addWidget(numLoopsSpin, 2, 1, 1, 1);


    retranslateUi(WpTolerancesDialog);

    QSize size(350, 298);
    size = size.expandedTo(WpTolerancesDialog->minimumSizeHint());
    WpTolerancesDialog->resize(size);

    QObject::connect(okButton, SIGNAL(clicked()), WpTolerancesDialog, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), WpTolerancesDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(WpTolerancesDialog);
    } // setupUi

    void retranslateUi(QDialog *WpTolerancesDialog)
    {
    WpTolerancesDialog->setWindowTitle(QApplication::translate("WpTolerancesDialog", "Waypoints Settings", 0, QApplication::UnicodeUTF8));
    maxSpeedLabel->setText(QApplication::translate("WpTolerancesDialog", "Maximum speed [m/s]:", 0, QApplication::UnicodeUTF8));
    headingLabel->setText(QApplication::translate("WpTolerancesDialog", "Heading tol. (+/-)  [deg]:", 0, QApplication::UnicodeUTF8));
    timeLabel->setText(QApplication::translate("WpTolerancesDialog", "Time period [s]:", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("WpTolerancesDialog", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:Sans Serif; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-weight:600;\">Waypoints Settings</span></p></body></html>", 0, QApplication::UnicodeUTF8));
    distanceLabel->setText(QApplication::translate("WpTolerancesDialog", "Distance tolerance [m]:", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("WpTolerancesDialog", "OK", 0, QApplication::UnicodeUTF8));
    cancelButton->setText(QApplication::translate("WpTolerancesDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    loopsLabel->setText(QApplication::translate("WpTolerancesDialog", "Number of loops:", 0, QApplication::UnicodeUTF8));
    maxTurnrateLabel->setText(QApplication::translate("WpTolerancesDialog", "Maximum turnrate [deg/s]:", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(WpTolerancesDialog);
    } // retranslateUi

};

namespace Ui {
    class WpTolerancesDialog: public Ui_WpTolerancesDialog {};
} // namespace Ui

#endif // WPTOLERANCESDIALOG_H

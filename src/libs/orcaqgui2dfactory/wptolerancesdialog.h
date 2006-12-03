/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
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
    QLabel *headingLabel;
    QLabel *maxSpeedLabel;
    QLabel *distanceLabel;
    QLabel *timeLabel;
    QSpacerItem *spacerItem;
    QLabel *maxTurnrateLabel;
    QDoubleSpinBox *timePeriodSpin;
    QSpacerItem *spacerItem1;
    QDoubleSpinBox *distanceSpin;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem2;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QDoubleSpinBox *maxSpeedSpin;
    QLabel *label;
    QSpacerItem *spacerItem3;
    QSpacerItem *spacerItem4;
    QSpinBox *headingSpin;
    QSpacerItem *spacerItem5;
    QSpinBox *maxTurnrateSpin;

    void setupUi(QDialog *WpTolerancesDialog)
    {
    WpTolerancesDialog->setObjectName(QString::fromUtf8("WpTolerancesDialog"));
    WpTolerancesDialog->resize(QSize(350, 298).expandedTo(WpTolerancesDialog->minimumSizeHint()));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(WpTolerancesDialog->sizePolicy().hasHeightForWidth());
    WpTolerancesDialog->setSizePolicy(sizePolicy);
    widget = new QWidget(WpTolerancesDialog);
    widget->setObjectName(QString::fromUtf8("widget"));
    widget->setGeometry(QRect(10, 10, 321, 277));
    gridLayout = new QGridLayout(widget);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(0);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    headingLabel = new QLabel(widget);
    headingLabel->setObjectName(QString::fromUtf8("headingLabel"));

    gridLayout->addWidget(headingLabel, 4, 2, 1, 1);

    maxSpeedLabel = new QLabel(widget);
    maxSpeedLabel->setObjectName(QString::fromUtf8("maxSpeedLabel"));

    gridLayout->addWidget(maxSpeedLabel, 7, 0, 1, 2);

    distanceLabel = new QLabel(widget);
    distanceLabel->setObjectName(QString::fromUtf8("distanceLabel"));

    gridLayout->addWidget(distanceLabel, 4, 0, 1, 2);

    timeLabel = new QLabel(widget);
    timeLabel->setObjectName(QString::fromUtf8("timeLabel"));

    gridLayout->addWidget(timeLabel, 1, 0, 1, 3);

    spacerItem = new QSpacerItem(301, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem, 9, 0, 1, 3);

    maxTurnrateLabel = new QLabel(widget);
    maxTurnrateLabel->setObjectName(QString::fromUtf8("maxTurnrateLabel"));

    gridLayout->addWidget(maxTurnrateLabel, 7, 2, 1, 1);

    timePeriodSpin = new QDoubleSpinBox(widget);
    timePeriodSpin->setObjectName(QString::fromUtf8("timePeriodSpin"));
    timePeriodSpin->setDecimals(1);
    timePeriodSpin->setMaximum(10000);
    timePeriodSpin->setSingleStep(0.5);
    timePeriodSpin->setValue(5);

    gridLayout->addWidget(timePeriodSpin, 2, 0, 1, 1);

    spacerItem1 = new QSpacerItem(155, 16, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem1, 3, 2, 1, 1);

    distanceSpin = new QDoubleSpinBox(widget);
    distanceSpin->setObjectName(QString::fromUtf8("distanceSpin"));
    distanceSpin->setMaximum(10);
    distanceSpin->setSingleStep(0.2);
    distanceSpin->setValue(2);

    gridLayout->addWidget(distanceSpin, 5, 0, 1, 2);

    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    spacerItem2 = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem2);

    okButton = new QPushButton(widget);
    okButton->setObjectName(QString::fromUtf8("okButton"));

    hboxLayout->addWidget(okButton);

    cancelButton = new QPushButton(widget);
    cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

    hboxLayout->addWidget(cancelButton);


    gridLayout->addLayout(hboxLayout, 10, 0, 1, 3);

    maxSpeedSpin = new QDoubleSpinBox(widget);
    maxSpeedSpin->setObjectName(QString::fromUtf8("maxSpeedSpin"));
    maxSpeedSpin->setMaximum(10);
    maxSpeedSpin->setSingleStep(0.2);
    maxSpeedSpin->setValue(1);

    gridLayout->addWidget(maxSpeedSpin, 8, 0, 1, 2);

    label = new QLabel(widget);
    label->setObjectName(QString::fromUtf8("label"));

    gridLayout->addWidget(label, 0, 0, 1, 3);

    spacerItem3 = new QSpacerItem(221, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(spacerItem3, 2, 1, 1, 2);

    spacerItem4 = new QSpacerItem(155, 16, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem4, 3, 0, 1, 2);

    headingSpin = new QSpinBox(widget);
    headingSpin->setObjectName(QString::fromUtf8("headingSpin"));
    headingSpin->setMaximum(180);
    headingSpin->setSingleStep(5);
    headingSpin->setValue(90);

    gridLayout->addWidget(headingSpin, 5, 2, 1, 1);

    spacerItem5 = new QSpacerItem(311, 16, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem5, 6, 0, 1, 3);

    maxTurnrateSpin = new QSpinBox(widget);
    maxTurnrateSpin->setObjectName(QString::fromUtf8("maxTurnrateSpin"));
    maxTurnrateSpin->setMaximum(100);
    maxTurnrateSpin->setSingleStep(1);
    maxTurnrateSpin->setValue(5);

    gridLayout->addWidget(maxTurnrateSpin, 8, 2, 1, 1);

    retranslateUi(WpTolerancesDialog);
    QObject::connect(okButton, SIGNAL(clicked()), WpTolerancesDialog, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), WpTolerancesDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(WpTolerancesDialog);
    } // setupUi

    void retranslateUi(QDialog *WpTolerancesDialog)
    {
    WpTolerancesDialog->setWindowTitle(QApplication::translate("WpTolerancesDialog", "Waypoints Settings", 0, QApplication::UnicodeUTF8));
    headingLabel->setText(QApplication::translate("WpTolerancesDialog", "Heading tol. (+/-)  [deg]:", 0, QApplication::UnicodeUTF8));
    maxSpeedLabel->setText(QApplication::translate("WpTolerancesDialog", "Maximum speed [m/s]:", 0, QApplication::UnicodeUTF8));
    distanceLabel->setText(QApplication::translate("WpTolerancesDialog", "Distance tolerance [m]:", 0, QApplication::UnicodeUTF8));
    timeLabel->setText(QApplication::translate("WpTolerancesDialog", "Time period [s]:", 0, QApplication::UnicodeUTF8));
    maxTurnrateLabel->setText(QApplication::translate("WpTolerancesDialog", "Maximum turnrate [deg/s]:", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("WpTolerancesDialog", "OK", 0, QApplication::UnicodeUTF8));
    cancelButton->setText(QApplication::translate("WpTolerancesDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("WpTolerancesDialog", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:Sans Serif; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-weight:600;\">Waypoints Settings</span></p></body></html>", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(WpTolerancesDialog);
    } // retranslateUi

};

namespace Ui {
    class WpTolerancesDialog: public Ui_WpTolerancesDialog {};
} // namespace Ui

#endif // WPTOLERANCESDIALOG_H

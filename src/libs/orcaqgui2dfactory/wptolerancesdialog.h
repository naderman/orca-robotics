/********************************************************************************
** Form generated from reading ui file 'wptolerancesdialog.ui'
**
** Created: Wed Feb 14 11:00:29 2007
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
#include <QtGui/QComboBox>
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
    QLabel *spacingLabel_2;
    QLabel *label;
    QLabel *headingLabel;
    QSpacerItem *spacerItem;
    QLabel *spacingLabel;
    QSpinBox *headingSpin;
    QSpacerItem *spacerItem1;
    QSpinBox *maxTurnrateSpin;
    QLabel *distanceLabel;
    QComboBox *spacingPropertyCombo;
    QDoubleSpinBox *maxSpeedSpin;
    QDoubleSpinBox *spacingValueSpin;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem2;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QLabel *maxTurnrateLabel;
    QDoubleSpinBox *distanceSpin;
    QSpacerItem *spacerItem3;
    QLabel *maxSpeedLabel;

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
    widget->setGeometry(QRect(11, 11, 277, 273));
    gridLayout = new QGridLayout(widget);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(0);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    spacingLabel_2 = new QLabel(widget);
    spacingLabel_2->setObjectName(QString::fromUtf8("spacingLabel_2"));

    gridLayout->addWidget(spacingLabel_2, 1, 1, 1, 1);

    label = new QLabel(widget);
    label->setObjectName(QString::fromUtf8("label"));

    gridLayout->addWidget(label, 0, 0, 1, 2);

    headingLabel = new QLabel(widget);
    headingLabel->setObjectName(QString::fromUtf8("headingLabel"));

    gridLayout->addWidget(headingLabel, 4, 1, 1, 1);

    spacerItem = new QSpacerItem(271, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem, 3, 0, 1, 2);

    spacingLabel = new QLabel(widget);
    spacingLabel->setObjectName(QString::fromUtf8("spacingLabel"));

    gridLayout->addWidget(spacingLabel, 1, 0, 1, 1);

    headingSpin = new QSpinBox(widget);
    headingSpin->setObjectName(QString::fromUtf8("headingSpin"));
    headingSpin->setMaximum(180);
    headingSpin->setSingleStep(5);
    headingSpin->setValue(90);

    gridLayout->addWidget(headingSpin, 5, 1, 1, 1);

    spacerItem1 = new QSpacerItem(271, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem1, 9, 0, 1, 2);

    maxTurnrateSpin = new QSpinBox(widget);
    maxTurnrateSpin->setObjectName(QString::fromUtf8("maxTurnrateSpin"));
    maxTurnrateSpin->setMaximum(2000000);
    maxTurnrateSpin->setSingleStep(1);
    maxTurnrateSpin->setValue(5);

    gridLayout->addWidget(maxTurnrateSpin, 8, 1, 1, 1);

    distanceLabel = new QLabel(widget);
    distanceLabel->setObjectName(QString::fromUtf8("distanceLabel"));

    gridLayout->addWidget(distanceLabel, 4, 0, 1, 1);

    spacingPropertyCombo = new QComboBox(widget);
    spacingPropertyCombo->setObjectName(QString::fromUtf8("spacingPropertyCombo"));

    gridLayout->addWidget(spacingPropertyCombo, 2, 0, 1, 1);

    maxSpeedSpin = new QDoubleSpinBox(widget);
    maxSpeedSpin->setObjectName(QString::fromUtf8("maxSpeedSpin"));
    maxSpeedSpin->setMaximum(2e+06);
    maxSpeedSpin->setSingleStep(0.2);
    maxSpeedSpin->setValue(0);

    gridLayout->addWidget(maxSpeedSpin, 8, 0, 1, 1);

    spacingValueSpin = new QDoubleSpinBox(widget);
    spacingValueSpin->setObjectName(QString::fromUtf8("spacingValueSpin"));
    spacingValueSpin->setDecimals(1);
    spacingValueSpin->setMaximum(10000);
    spacingValueSpin->setSingleStep(0.1);
    spacingValueSpin->setValue(1);

    gridLayout->addWidget(spacingValueSpin, 2, 1, 1, 1);

    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    spacerItem2 = new QSpacerItem(111, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem2);

    okButton = new QPushButton(widget);
    okButton->setObjectName(QString::fromUtf8("okButton"));

    hboxLayout->addWidget(okButton);

    cancelButton = new QPushButton(widget);
    cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

    hboxLayout->addWidget(cancelButton);


    gridLayout->addLayout(hboxLayout, 10, 0, 1, 2);

    maxTurnrateLabel = new QLabel(widget);
    maxTurnrateLabel->setObjectName(QString::fromUtf8("maxTurnrateLabel"));

    gridLayout->addWidget(maxTurnrateLabel, 7, 1, 1, 1);

    distanceSpin = new QDoubleSpinBox(widget);
    distanceSpin->setObjectName(QString::fromUtf8("distanceSpin"));
    distanceSpin->setMaximum(10);
    distanceSpin->setSingleStep(0.2);
    distanceSpin->setValue(1);

    gridLayout->addWidget(distanceSpin, 5, 0, 1, 1);

    spacerItem3 = new QSpacerItem(271, 16, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem3, 6, 0, 1, 2);

    maxSpeedLabel = new QLabel(widget);
    maxSpeedLabel->setObjectName(QString::fromUtf8("maxSpeedLabel"));

    gridLayout->addWidget(maxSpeedLabel, 7, 0, 1, 1);


    retranslateUi(WpTolerancesDialog);

    QSize size(308, 296);
    size = size.expandedTo(WpTolerancesDialog->minimumSizeHint());
    WpTolerancesDialog->resize(size);

    QObject::connect(okButton, SIGNAL(clicked()), WpTolerancesDialog, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), WpTolerancesDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(WpTolerancesDialog);
    } // setupUi

    void retranslateUi(QDialog *WpTolerancesDialog)
    {
    WpTolerancesDialog->setWindowTitle(QApplication::translate("WpTolerancesDialog", "Waypoints Settings", 0, QApplication::UnicodeUTF8));
    spacingLabel_2->setText(QApplication::translate("WpTolerancesDialog", "Spacing value [s] or [m/s]:", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("WpTolerancesDialog", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:Sans Serif; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-weight:600;\">Waypoints Settings</span></p></body></html>", 0, QApplication::UnicodeUTF8));
    headingLabel->setText(QApplication::translate("WpTolerancesDialog", "Heading tol. (+/-)  [deg]:", 0, QApplication::UnicodeUTF8));
    spacingLabel->setText(QApplication::translate("WpTolerancesDialog", "Spacing property:", 0, QApplication::UnicodeUTF8));
    distanceLabel->setText(QApplication::translate("WpTolerancesDialog", "Distance tolerance [m]:", 0, QApplication::UnicodeUTF8));
    spacingPropertyCombo->clear();
    spacingPropertyCombo->addItem(QApplication::translate("WpTolerancesDialog", "Velocity", 0, QApplication::UnicodeUTF8));
    spacingPropertyCombo->addItem(QApplication::translate("WpTolerancesDialog", "Time", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("WpTolerancesDialog", "OK", 0, QApplication::UnicodeUTF8));
    cancelButton->setText(QApplication::translate("WpTolerancesDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    maxTurnrateLabel->setText(QApplication::translate("WpTolerancesDialog", "Maximum turnrate [deg/s]:", 0, QApplication::UnicodeUTF8));
    maxSpeedLabel->setText(QApplication::translate("WpTolerancesDialog", "Maximum speed [m/s]:", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(WpTolerancesDialog);
    } // retranslateUi

};

namespace Ui {
    class WpTolerancesDialog: public Ui_WpTolerancesDialog {};
} // namespace Ui

#endif // WPTOLERANCESDIALOG_H

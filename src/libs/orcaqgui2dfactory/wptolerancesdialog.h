/********************************************************************************
** Form generated from reading ui file 'wptolerancesdialog.ui'
**
** Created: Mon Jan 8 15:59:12 2007
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
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QSpacerItem *spacerItem;
    QDoubleSpinBox *spacingValueSpin;
    QLabel *distanceLabel;
    QLabel *spacingLabel_2;
    QDoubleSpinBox *distanceSpin;
    QComboBox *spacingPropertyCombo;
    QSpinBox *numLoopsSpin;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem1;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QLabel *loopsLabel;
    QSpinBox *headingSpin;
    QLabel *maxSpeedLabel;
    QDoubleSpinBox *maxSpeedSpin;
    QSpacerItem *spacerItem2;
    QSpacerItem *spacerItem3;
    QLabel *maxTurnrateLabel;
    QSpacerItem *spacerItem4;
    QLabel *label;
    QSpacerItem *spacerItem5;
    QLabel *spacingLabel;
    QSpinBox *maxTurnrateSpin;
    QLabel *headingLabel;

    void setupUi(QDialog *WpTolerancesDialog)
    {
    WpTolerancesDialog->setObjectName(QString::fromUtf8("WpTolerancesDialog"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(WpTolerancesDialog->sizePolicy().hasHeightForWidth());
    WpTolerancesDialog->setSizePolicy(sizePolicy);
    layoutWidget = new QWidget(WpTolerancesDialog);
    layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
    layoutWidget->setGeometry(QRect(10, 10, 277, 346));
    gridLayout = new QGridLayout(layoutWidget);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(0);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    spacerItem = new QSpacerItem(271, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem, 6, 0, 1, 3);

    spacingValueSpin = new QDoubleSpinBox(layoutWidget);
    spacingValueSpin->setObjectName(QString::fromUtf8("spacingValueSpin"));
    spacingValueSpin->setDecimals(1);
    spacingValueSpin->setMaximum(10000);
    spacingValueSpin->setSingleStep(0.1);
    spacingValueSpin->setValue(1);

    gridLayout->addWidget(spacingValueSpin, 5, 2, 1, 1);

    distanceLabel = new QLabel(layoutWidget);
    distanceLabel->setObjectName(QString::fromUtf8("distanceLabel"));

    gridLayout->addWidget(distanceLabel, 7, 0, 1, 2);

    spacingLabel_2 = new QLabel(layoutWidget);
    spacingLabel_2->setObjectName(QString::fromUtf8("spacingLabel_2"));

    gridLayout->addWidget(spacingLabel_2, 4, 2, 1, 1);

    distanceSpin = new QDoubleSpinBox(layoutWidget);
    distanceSpin->setObjectName(QString::fromUtf8("distanceSpin"));
    distanceSpin->setMaximum(10);
    distanceSpin->setSingleStep(0.2);
    distanceSpin->setValue(1);

    gridLayout->addWidget(distanceSpin, 8, 0, 1, 2);

    spacingPropertyCombo = new QComboBox(layoutWidget);
    spacingPropertyCombo->setObjectName(QString::fromUtf8("spacingPropertyCombo"));

    gridLayout->addWidget(spacingPropertyCombo, 5, 0, 1, 2);

    numLoopsSpin = new QSpinBox(layoutWidget);
    numLoopsSpin->setObjectName(QString::fromUtf8("numLoopsSpin"));
    numLoopsSpin->setMaximum(1000);
    numLoopsSpin->setMinimum(1);
    numLoopsSpin->setSingleStep(1);
    numLoopsSpin->setValue(1);

    gridLayout->addWidget(numLoopsSpin, 2, 0, 1, 1);

    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    spacerItem1 = new QSpacerItem(111, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem1);

    okButton = new QPushButton(layoutWidget);
    okButton->setObjectName(QString::fromUtf8("okButton"));

    hboxLayout->addWidget(okButton);

    cancelButton = new QPushButton(layoutWidget);
    cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

    hboxLayout->addWidget(cancelButton);


    gridLayout->addLayout(hboxLayout, 13, 0, 1, 3);

    loopsLabel = new QLabel(layoutWidget);
    loopsLabel->setObjectName(QString::fromUtf8("loopsLabel"));

    gridLayout->addWidget(loopsLabel, 1, 0, 1, 1);

    headingSpin = new QSpinBox(layoutWidget);
    headingSpin->setObjectName(QString::fromUtf8("headingSpin"));
    headingSpin->setMaximum(180);
    headingSpin->setSingleStep(5);
    headingSpin->setValue(90);

    gridLayout->addWidget(headingSpin, 8, 2, 1, 1);

    maxSpeedLabel = new QLabel(layoutWidget);
    maxSpeedLabel->setObjectName(QString::fromUtf8("maxSpeedLabel"));

    gridLayout->addWidget(maxSpeedLabel, 10, 0, 1, 2);

    maxSpeedSpin = new QDoubleSpinBox(layoutWidget);
    maxSpeedSpin->setObjectName(QString::fromUtf8("maxSpeedSpin"));
    maxSpeedSpin->setMaximum(2e+06);
    maxSpeedSpin->setSingleStep(0.2);
    maxSpeedSpin->setValue(0);

    gridLayout->addWidget(maxSpeedSpin, 11, 0, 1, 2);

    spacerItem2 = new QSpacerItem(271, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem2, 3, 0, 1, 3);

    spacerItem3 = new QSpacerItem(271, 16, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem3, 9, 0, 1, 3);

    maxTurnrateLabel = new QLabel(layoutWidget);
    maxTurnrateLabel->setObjectName(QString::fromUtf8("maxTurnrateLabel"));

    gridLayout->addWidget(maxTurnrateLabel, 10, 2, 1, 1);

    spacerItem4 = new QSpacerItem(141, 41, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(spacerItem4, 1, 1, 2, 2);

    label = new QLabel(layoutWidget);
    label->setObjectName(QString::fromUtf8("label"));

    gridLayout->addWidget(label, 0, 0, 1, 3);

    spacerItem5 = new QSpacerItem(271, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem5, 12, 0, 1, 3);

    spacingLabel = new QLabel(layoutWidget);
    spacingLabel->setObjectName(QString::fromUtf8("spacingLabel"));

    gridLayout->addWidget(spacingLabel, 4, 0, 1, 2);

    maxTurnrateSpin = new QSpinBox(layoutWidget);
    maxTurnrateSpin->setObjectName(QString::fromUtf8("maxTurnrateSpin"));
    maxTurnrateSpin->setMaximum(2000000);
    maxTurnrateSpin->setSingleStep(1);
    maxTurnrateSpin->setValue(5);

    gridLayout->addWidget(maxTurnrateSpin, 11, 2, 1, 1);

    headingLabel = new QLabel(layoutWidget);
    headingLabel->setObjectName(QString::fromUtf8("headingLabel"));

    gridLayout->addWidget(headingLabel, 7, 2, 1, 1);


    retranslateUi(WpTolerancesDialog);

    QSize size(308, 374);
    size = size.expandedTo(WpTolerancesDialog->minimumSizeHint());
    WpTolerancesDialog->resize(size);

    QObject::connect(okButton, SIGNAL(clicked()), WpTolerancesDialog, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), WpTolerancesDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(WpTolerancesDialog);
    } // setupUi

    void retranslateUi(QDialog *WpTolerancesDialog)
    {
    WpTolerancesDialog->setWindowTitle(QApplication::translate("WpTolerancesDialog", "Waypoints Settings", 0, QApplication::UnicodeUTF8));
    distanceLabel->setText(QApplication::translate("WpTolerancesDialog", "Distance tolerance [m]:", 0, QApplication::UnicodeUTF8));
    spacingLabel_2->setText(QApplication::translate("WpTolerancesDialog", "Spacing value [s] or [m/s]:", 0, QApplication::UnicodeUTF8));
    spacingPropertyCombo->clear();
    spacingPropertyCombo->addItem(QApplication::translate("WpTolerancesDialog", "Velocity", 0, QApplication::UnicodeUTF8));
    spacingPropertyCombo->addItem(QApplication::translate("WpTolerancesDialog", "Time", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("WpTolerancesDialog", "OK", 0, QApplication::UnicodeUTF8));
    cancelButton->setText(QApplication::translate("WpTolerancesDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    loopsLabel->setText(QApplication::translate("WpTolerancesDialog", "Number of loops:", 0, QApplication::UnicodeUTF8));
    maxSpeedLabel->setText(QApplication::translate("WpTolerancesDialog", "Maximum speed [m/s]:", 0, QApplication::UnicodeUTF8));
    maxTurnrateLabel->setText(QApplication::translate("WpTolerancesDialog", "Maximum turnrate [deg/s]:", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("WpTolerancesDialog", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:Sans Serif; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-weight:600;\">Waypoints Settings</span></p></body></html>", 0, QApplication::UnicodeUTF8));
    spacingLabel->setText(QApplication::translate("WpTolerancesDialog", "Spacing property:", 0, QApplication::UnicodeUTF8));
    headingLabel->setText(QApplication::translate("WpTolerancesDialog", "Heading tol. (+/-)  [deg]:", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(WpTolerancesDialog);
    } // retranslateUi

};

namespace Ui {
    class WpTolerancesDialog: public Ui_WpTolerancesDialog {};
} // namespace Ui

#endif // WPTOLERANCESDIALOG_H

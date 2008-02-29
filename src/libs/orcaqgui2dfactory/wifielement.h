/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_WIFI_ELEMENT_H
#define ORCAGUI_WIFI_ELEMENT_H

#include <QVector>
#include <orca/wifi.h>
#include <orcaqguielementutil/icestormelement.h>
#include <orcaqgui2dfactory/wifipainter.h>

class QProgressBar;
class QLCDNumber;
class QLabel;

namespace orcaqgui2d {

// Widget to display wifi data, number of interfaces is fixed
// after the first data is received
// Author: Tobias Kaupp
class WifiWidget : public QWidget
{
    public:
        WifiWidget( unsigned int numInterfaces, std::string proxyString );
        ~WifiWidget() {};
        void refresh( orca::WifiData &data );
        
    private:
        QVector<QProgressBar*> progressBars_;
        QVector<QLCDNumber*> lcdsSignal_;
        QVector<QLCDNumber*> lcdsMaxSignal_;
        QVector<QLCDNumber*> lcdsNoise_;
        QVector<QLCDNumber*> lcdsMaxNoise_;
        QVector<QLCDNumber*> lcdsLink_;
        QVector<QLCDNumber*> lcdsMaxLink_;
        QVector<QLCDNumber*> lcdsBitrate_;
        QVector<QLabel*> interfaceLabels_;
        QVector<QLabel*> accessPointLabels_;
        QVector<QLabel*> overallSigLabels_;
        
        void setupDisplay();
        unsigned int numInterfaces_;
        QVector<int> signalThreshholds_;
        QString getSignalLabel( int snr );

};

class WifiElement
    : public orcaqguielementutil::IceStormElement<WifiPainter,
                             orca::WifiData,
                             orca::WifiPrx,
                             orca::WifiConsumer,
                             orca::WifiConsumerPrx>
{
public:
    WifiElement( const orcaice::Context  &context,
                 const std::string       &proxyString,
                 int                      timeoutMs=30000 );
    ~WifiElement();

    virtual bool isInGlobalCS() { return true; };
    virtual void update();
    virtual void actionOnConnection() {};

private:
    WifiPainter painter_;
    WifiWidget *wifiWidget_;
    std::string proxyString_;
};

}

#endif

/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <QVector>
#include <QLabel>
#include <QProgressBar> 
#include <QLCDNumber>
#include <QVBoxLayout>

#include "wifielement.h"

using namespace std;
using namespace orca;
using namespace orcaqgui2d;

WifiWidget::WifiWidget( unsigned int numInterfaces, std::string proxyString )
    : numInterfaces_(numInterfaces)
{
    setupDisplay();
    setMinimumWidth(200);
    setMinimumHeight(150);
    setWindowTitle( QString(proxyString.c_str()) );
}

void
WifiWidget::refresh( WifiData &data )
{
    for (int i=0; i<interfaceLabels_.size(); i++) 
    {
        interfaceLabels_[i]->setText("<b>" + QString(data.interfaces[i].interfaceName.c_str()) + "</b>");
    }
    
    for (unsigned int i=0; i<numInterfaces_; i++)
    {
        WifiInterface &wifiInt = data.interfaces[i];
//         if (wifiInt.linkType!=LinkQualityTypeDbm) {
//             lcds_[i]->display("Err");
//         } else {
            lcds_[i]->display(wifiInt.signalLevel);
//         }
        
        progressBars_[i]->setMinimum(0);
        progressBars_[i]->setMaximum(wifiInt.maxLinkQuality);
        progressBars_[i]->setValue(wifiInt.linkQuality);
    }
    
}

void WifiWidget::setupDisplay()
{
    QGridLayout *globalLayout = new QGridLayout;
    
    for (unsigned int i=0; i<numInterfaces_; i++)
    {
        QLabel *interfaceLabel = new QLabel;
        globalLayout->addWidget(interfaceLabel,3*i,0);
        interfaceLabels_.push_back( interfaceLabel );
        
        QLCDNumber *lcdSignalLevel = new QLCDNumber;
        lcdSignalLevel->setSegmentStyle(QLCDNumber::Filled);
        lcds_.push_back(lcdSignalLevel);
        QLabel *signalLabel = new QLabel("Signal level: ");
        globalLayout->addWidget( signalLabel,3*i+1,0);
        globalLayout->addWidget( lcdSignalLevel,3*i+1,1);
    
        QProgressBar *barLinkQuality = new QProgressBar;
        progressBars_.push_back(barLinkQuality);
        QLabel *linkLabel = new QLabel("Link quality level: ");
        globalLayout->addWidget( linkLabel,3*i+2,0);
        globalLayout->addWidget( barLinkQuality,3*i+2,1);
    }
    setLayout(globalLayout);
}

WifiElement::WifiElement( const orcaice::Context  &context,
                          const std::string       &proxyString,
                          int                      timeoutMs)
    : IceStormElement<WifiPainter,
                      orca::WifiData,
                      orca::WifiPrx,
                      orca::WifiConsumer,
                      orca::WifiConsumerPrx>(context, proxyString, painter_, timeoutMs ),
        wifiWidget_(0),
        proxyString_(proxyString)
{
}

WifiElement::~WifiElement()
{
    if (wifiWidget_!=0) delete wifiWidget_;
}


void
WifiElement::update()
{
    if ( !needToUpdate() ) {
        return;
    }
    
    assert( !listener_.buffer().isEmpty() );

    WifiData data;
    
    // get data from the buffer
    listener_.buffer().getAndPop( data );
    
    //cout << orcaice::toString(data) << endl;
    
    if (wifiWidget_==0) {
        wifiWidget_ = new WifiWidget( data.interfaces.size(), proxyString_ );
    }
    wifiWidget_->refresh( data );
    if (wifiWidget_->isHidden()) {
        wifiWidget_->show();
    }
}





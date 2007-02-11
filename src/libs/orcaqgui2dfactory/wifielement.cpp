/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <QVector>
#include <QLabel>
#include <QProgressBar> 
#include <QVBoxLayout>

#include "wifielement.h"

using namespace orca;
using namespace orcaqgui;

void
WifiWidget::refresh( WifiData &data )
{
    unsigned int numInterfaces = data.interfaces.size();
    setupDisplay( numInterfaces );
    
    //update the progress bar data
    for (unsigned int i=0; i<2*numInterfaces; i+=2)
    {
        WifiInterface &wifiInt = data.interfaces[i];
        if (wifiInt.linkType!=LinkQualityTypeDbm) {
            progressBars_[i]->setEnabled(false);
        } else {
            progressBars_[i]->setMinimum(0);
            progressBars_[i]->setMaximum(wifiInt.maxSignalLevel);
            progressBars_[i]->setValue(wifiInt.signalLevel);
        }
        
        progressBars_[i+1]->setMinimum(0);
        progressBars_[i+1]->setMaximum(wifiInt.maxLinkQuality);
        progressBars_[i+1]->setValue(wifiInt.linkQuality);
    }
    
}

void WifiWidget::setupDisplay( unsigned int numInterfaces)
{
    delete this->layout();
    
    QVBoxLayout *globalLayout = new QVBoxLayout;
    
    for (unsigned int i=0; i<numInterfaces; i++)
    {
        QHBoxLayout *hLayout1 = new QHBoxLayout;
        QProgressBar *barSignalLevel = new QProgressBar;
        progressBars_.push_back(barSignalLevel);
        QLabel *signalLabel = new QLabel("signal level: ");
        hLayout1->addWidget( signalLabel );
        hLayout1->addWidget( barSignalLevel );
        globalLayout->addLayout(hLayout1);
        
        QHBoxLayout *hLayout2 = new QHBoxLayout;
        QProgressBar *barLinkQuality = new QProgressBar;
        progressBars_.push_back(barLinkQuality);
        QLabel *linkLabel = new QLabel("link quality level: ");
        hLayout2->addWidget( linkLabel );
        hLayout2->addWidget( barLinkQuality );
        globalLayout->addLayout(hLayout2);
    }
    setLayout(globalLayout);
    this->show();
}

WifiElement::WifiElement( const orcaice::Context  &context,
                          const std::string       &proxyString,
                          int                      timeoutMs)
    : IceStormElement<WifiPainter,
                            orca::WifiData,
                            orca::WifiPrx,
                            orca::WifiConsumer,
                            orca::WifiConsumerPrx>(context, proxyString, painter_, timeoutMs )
{
    wifiWidget_ = new WifiWidget();
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

    // transfer data into painter
    painter_.setData( data );
    wifiWidget_->refresh( data );
}





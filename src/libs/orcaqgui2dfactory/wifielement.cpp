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

// mimics Windows-style signal level
// see http://www.osuweb.net/wireless/faqs.html#whydoesmysignalstrengthsaylow
int overallSignalLevel( int signal, int noise, QString &label )
{
    int snr = signal-noise;
    if (snr<10) {
        label="very low";
        return 0;
    }
    else if (snr<15)
    {
        label="low";
        return 1;
    }
    else if (snr<20) 
    {
        label="good";   
        return 2;
    }
    else if (snr<25)
    {
        label="very good";
        return 3;
    }
    else 
    {
        label="excellent";
        return 4;    
    }
}

WifiWidget::WifiWidget( unsigned int numInterfaces, std::string proxyString )
    : numInterfaces_(numInterfaces)
{
    setupDisplay();
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
        lcdsSignal_[i]->display(wifiInt.signalLevel);
        lcdsNoise_[i]->display(wifiInt.noiseLevel);
        lcdsLink_[i]->display(wifiInt.linkQuality);
        
        if (wifiInt.linkType==LinkQualityTypeDbm) {
            lcdsMaxSignal_[i]->display("DB");
            lcdsMaxNoise_[i]->display("DB");
            progressBars_[i]->setFormat("%p%");
            QString label;
            int level = overallSignalLevel( wifiInt.signalLevel,
                                            wifiInt.noiseLevel,
                                            label );
            progressBars_[i]->setValue(level);
            overallSigLabels_[i]->setText( label );
        } else {
            lcdsMaxSignal_[i]->display(wifiInt.maxSignalLevel);
            lcdsMaxNoise_[i]->display(wifiInt.maxNoiseLevel);
            progressBars_[i]->setFormat("NA");
            overallSigLabels_[i]->setText( "Unknown" );
        }
        lcdsMaxLink_[i]->display(wifiInt.maxLinkQuality);
        lcdsBitrate_[i]->display(wifiInt.bitrate/1e6);
    }
    
}

void WifiWidget::setupDisplay()
{
    const int numRowsPerInterface = 6; 
    const int numDigits = 3;
    
    QGridLayout *globalLayout = new QGridLayout(this);
    globalLayout->setColumnStretch(0,0);
    globalLayout->setColumnStretch(1,1);
    globalLayout->setColumnStretch(2,1);
    
    for (unsigned int i=0; i<numInterfaces_; i++)
    {
        QLabel *interfaceLabel = new QLabel;
        globalLayout->addWidget(interfaceLabel,numRowsPerInterface*i,0);
        interfaceLabels_.push_back( interfaceLabel );
        
        QLCDNumber *lcdSignalLevel = new QLCDNumber(numDigits, this);
        lcdSignalLevel->setSegmentStyle(QLCDNumber::Filled);
        lcdsSignal_.push_back(lcdSignalLevel);
        QLCDNumber *lcdMaxSignalLevel = new QLCDNumber(numDigits, this);
        lcdMaxSignalLevel->setSegmentStyle(QLCDNumber::Filled);
        lcdsMaxSignal_.push_back(lcdMaxSignalLevel);
        QLabel *signalLabel = new QLabel("Signal: ");
        globalLayout->addWidget( signalLabel,numRowsPerInterface*i+1,0);
        globalLayout->addWidget( lcdSignalLevel,numRowsPerInterface*i+1,1);
        globalLayout->addWidget( lcdMaxSignalLevel,numRowsPerInterface*i+1,2);
        
        QLCDNumber *lcdNoiseLevel = new QLCDNumber(numDigits, this);
        lcdNoiseLevel->setSegmentStyle(QLCDNumber::Filled);
        lcdsNoise_.push_back(lcdNoiseLevel);
        QLCDNumber *lcdMaxNoiseLevel = new QLCDNumber(numDigits, this);
        lcdMaxNoiseLevel->setSegmentStyle(QLCDNumber::Filled);
        lcdsMaxNoise_.push_back(lcdMaxNoiseLevel);
        QLabel *noiseLabel = new QLabel("Noise: ");
        globalLayout->addWidget( noiseLabel,numRowsPerInterface*i+2,0);
        globalLayout->addWidget( lcdNoiseLevel,numRowsPerInterface*i+2,1);
        globalLayout->addWidget( lcdMaxNoiseLevel,numRowsPerInterface*i+2,2);
        
        QLCDNumber *lcdLinkLevel = new QLCDNumber(numDigits, this);
        lcdLinkLevel->setSegmentStyle(QLCDNumber::Filled);
        lcdsLink_.push_back(lcdLinkLevel);
        QLCDNumber *lcdMaxLinkLevel = new QLCDNumber(numDigits, this);
        lcdMaxLinkLevel->setSegmentStyle(QLCDNumber::Filled);
        lcdsMaxLink_.push_back(lcdMaxLinkLevel);
        QLabel *linkLabel = new QLabel("Link quality: ");
        globalLayout->addWidget( linkLabel,numRowsPerInterface*i+3,0);
        globalLayout->addWidget( lcdLinkLevel,numRowsPerInterface*i+3,1);
        globalLayout->addWidget( lcdMaxLinkLevel,numRowsPerInterface*i+3,2);
                
        QLCDNumber *lcdBitrate = new QLCDNumber(numDigits, this);
        lcdBitrate->setSegmentStyle(QLCDNumber::Filled);
        lcdsBitrate_.push_back(lcdBitrate);
        QLabel *bitRateLabel = new QLabel("Bit rate (Mb/s): ");
        globalLayout->addWidget( bitRateLabel,numRowsPerInterface*i+4,0);
        globalLayout->addWidget( lcdBitrate,numRowsPerInterface*i+4,1);
        
        QProgressBar *overall = new QProgressBar;
        overall->setMinimum(0);
        overall->setMaximum(4);
        progressBars_.push_back(overall);
        QLabel *progressLabel = new QLabel("Overall signal level: ");
        QLabel *overallSigLabel = new QLabel;
        overallSigLabel->setFont(QFont("Helvetica", 12, QFont::Bold));
        overallSigLabels_.push_back( overallSigLabel );
        globalLayout->addWidget( progressLabel,numRowsPerInterface*i+5,0);
        globalLayout->addWidget( overall,numRowsPerInterface*i+5,1);
        globalLayout->addWidget( overallSigLabel,numRowsPerInterface*i+5,2);
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





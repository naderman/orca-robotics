/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <QVector>
#include <QLabel>
#include <QSplitter>
#include <QProgressBar> 
#include <QLCDNumber>
#include <QVBoxLayout>

#include <gbxutilacfr/mathdefs.h>
#include <orca/wifi.h>
#include "wifielement.h"

using namespace std;
using namespace orcaqgui2d;

// maximum signal-to-noise-ratio
// everything above 25 is considered "excellent"
// see http://www.osuweb.net/wireless/faqs.html#whydoesmysignalstrengthsaylow
const int MAXIMUM_SNR = 28;

QString
WifiWidget::getSignalLabel( orca::DiscreteSignalLevel level )
{
    switch (level)
    {
        case orca::SignalLevelUnknown: return "UNKNOWN";
        case orca::SignalLevelVeryLow: return "VERY LOW";
        case orca::SignalLevelLow: return "LOW";
        case orca::SignalLevelGood: return "GOOD";
        case orca::SignalLevelVeryGood: return "VERY GOOD";
        case orca::SignalLevelExcellent: return "EXCELLENT";
        default: assert( false && "Unknown signal level");
    }
}

WifiWidget::WifiWidget( unsigned int numInterfaces, std::string proxyString )
    : numInterfaces_(numInterfaces)
{    
    setupDisplay();
    setWindowTitle( QString(proxyString.c_str()) );
}

void
WifiWidget::refresh( orca::WifiData &data )
{
    //cout << orcaobj::toString(data);
    
    for (unsigned int i=0; i<numInterfaces_; i++)
    {
        interfaceLabels_[i]->setText(QString(data.interfaces[i].interfaceName.c_str()));
        accessPointLabels_[i]->setText(QString(data.interfaces[i].accessPoint.c_str()));
        
        orca::WifiInterface &wifiInt = data.interfaces[i];
        lcdsSignal_[i]->display(wifiInt.signalLevel);
        lcdsNoise_[i]->display(wifiInt.noiseLevel);
        lcdsLink_[i]->display(wifiInt.linkQuality);
        
        if (wifiInt.linkType==orca::LinkQualityTypeDbm) {
            lcdsMaxSignal_[i]->display("DB");
            lcdsMaxNoise_[i]->display("DB");
            progressBars_[i]->setFormat("%p%");
            int snr = wifiInt.signalLevel-wifiInt.noiseLevel;
            progressBars_[i]->setValue( MIN(snr,MAXIMUM_SNR) );
            overallSigLabels_[i]->setText( getSignalLabel( wifiInt.discreteLevel ) );
        } else {
            lcdsMaxSignal_[i]->display(wifiInt.maxSignalLevel);
            lcdsMaxNoise_[i]->display(wifiInt.maxNoiseLevel);
            progressBars_[i]->setFormat("NA");
            progressBars_[i]->setValue(0);
            overallSigLabels_[i]->setText( "Unknown" );
        }
        lcdsMaxLink_[i]->display(wifiInt.maxLinkQuality);
        lcdsBitrate_[i]->display(wifiInt.bitrate/1e6);
    }
    
}

void WifiWidget::setupDisplay()
{
    const int numRowsPerInterface = 6; 
    const int numDigits = 4;
    
    QGridLayout *globalLayout = new QGridLayout(this);
    globalLayout->setColumnStretch(0,0);
    globalLayout->setColumnStretch(1,1);
    globalLayout->setColumnStretch(2,1);
    
    for (unsigned int i=0; i<numInterfaces_; i++)
    {
        QLabel *interfaceLabel = new QLabel;
        interfaceLabel->setFont(QFont("Helvetica", 12, QFont::Bold));
        globalLayout->addWidget(interfaceLabel,numRowsPerInterface*i,0);
        interfaceLabels_.push_back( interfaceLabel );
        
        QLabel *accessPointLabel = new QLabel;
        accessPointLabel->setFont(QFont("Helvetica", 12, QFont::Bold));
        globalLayout->addWidget(accessPointLabel,numRowsPerInterface*i,1);
        accessPointLabels_.push_back( accessPointLabel );
        
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
        overall->setMaximum(MAXIMUM_SNR);
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

/////////////////////////////////////////////////////////

WifiElement::WifiElement( const hydroqguielementutil::GuiElementInfo &guiElementInfo,
                          const orcaice::Context  &context,
                          const std::string       &proxyString,
                          QSplitter               *spaceBottomRight,
                          int                      timeoutMs )
    : orcaqguielementutil::IceStormGuiElement<orca::WifiData,
                                              orca::WifiPrx,
                                              orca::WifiConsumer,
                                              orca::WifiConsumerPrx>( guiElementInfo, context, proxyString, timeoutMs ),
        wifiWidget_(0),
        proxyString_(proxyString),
        spaceBottomRight_(spaceBottomRight)
                
{
}

WifiElement::~WifiElement()
{
    if (wifiWidget_!=0) delete wifiWidget_;
}


void
WifiElement::update()
{
    if ( updateFromStore() ) 
    {
        if (wifiWidget_==0) 
        {
            wifiWidget_ = new WifiWidget( data_.interfaces.size(), proxyString_ );
            if (spaceBottomRight_!=0)
                spaceBottomRight_->addWidget( wifiWidget_ );
        }
        wifiWidget_->refresh( data_ );
        if (wifiWidget_->isHidden()) {
            wifiWidget_->show();
        }
    }
}





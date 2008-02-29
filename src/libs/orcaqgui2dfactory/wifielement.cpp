/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <QVector>
#include <QLabel>
#include <QProgressBar> 
#include <QLCDNumber>
#include <QVBoxLayout>

#include <hydroutil/mathdefs.h>

#include "wifielement.h"

using namespace std;
using namespace orcaqgui2d;

// mimics Windows-style signal level
// see http://www.osuweb.net/wireless/faqs.html#whydoesmysignalstrengthsaylow
QString 
WifiWidget::getSignalLabel( int snr )
{
    assert(signalThreshholds_.size()==4);
    
    if (snr<signalThreshholds_[0]) return "very low";
    else if (snr<signalThreshholds_[1]) return "low";
    else if (snr<signalThreshholds_[2]) return "good";   
    else if (snr<signalThreshholds_[3]) return "very good";
    else return "excellent";
}

WifiWidget::WifiWidget( unsigned int numInterfaces, std::string proxyString )
    : numInterfaces_(numInterfaces)
{
    // threshholds to determine signal levels
    signalThreshholds_.push_back(10);
    signalThreshholds_.push_back(15);
    signalThreshholds_.push_back(20);
    signalThreshholds_.push_back(25);
    
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
            QString label = getSignalLabel( snr );
            progressBars_[i]->setValue( MIN(snr,signalThreshholds_.last()) );
            overallSigLabels_[i]->setText( label );
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
        overall->setMaximum(signalThreshholds_.last());
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
    : orcaqguielementutil::IceStormElement<WifiPainter,
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
    //cout << "================ DESTRUCTOR == " << endl;
    if (wifiWidget_!=0) delete wifiWidget_;
}


void
WifiElement::update()
{
    if ( !needToUpdate() ) {
        return;
    }
    
    assert( !listener_.buffer().isEmpty() );

    orca::WifiData data;
    
    // get data from the buffer
    listener_.buffer().getAndPop( data );
    
    //cout << orcaobj::toString(data) << endl;
    
    if (wifiWidget_==0) {
        wifiWidget_ = new WifiWidget( data.interfaces.size(), proxyString_ );
    }
    wifiWidget_->refresh( data );
    if (wifiWidget_->isHidden()) {
        wifiWidget_->show();
    }
}





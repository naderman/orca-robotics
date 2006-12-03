#ifndef FEATUREMAP2DELEMENT_H
#define FEATUREMAP2DELEMENT_H

#include <QObject>
#include <orcaqgui2d/icestormelement.h>
#include <orcaqgui/ihumanmanager.h>
#include <orcaqgui2dfactory/featuremap2dpainter.h>
#include <orcaqgui2dfactory/connectutils.h>

namespace orcaqgui
{

//!
//! @author Alex Brooks
//!
class FeatureMap2dElement
    : public QObject, public IceStormElement<FeatureMap2dPainter,
                                             orca::FeatureMap2dData,
                                             orca::FeatureMap2dDataPtr,
                                             orca::FeatureMap2dPrx,
                                             orca::FeatureMap2dConsumer,
                                             orca::FeatureMap2dConsumerPrx>
{
    Q_OBJECT

public: 

    FeatureMap2dElement( const orcaice::Context  &context,
                         const std::string       &proxyString,
                         IHumanManager*           humanManager );

    virtual bool isInGlobalCS() { return true; }
    virtual void actionOnConnection() 
    {
        paintInitialData<orca::FeatureMap2dPrx, FeatureMap2dPainter>
            ( context_, listener_.interfaceName(), painter_ );
    }
    virtual QStringList contextMenu();
    virtual void execute( int action );

 public slots:
    void saveFeatureMapAs();
    void saveFeatureMap();

private:
    FeatureMap2dPainter painter_;
    IHumanManager *humanManager_;
    QString featureMapFileName_;
    bool featureMapFileNameSet_;
};

}

#endif

#ifndef FEATUREMAP2DELEMENT_H
#define FEATUREMAP2DELEMENT_H

#include <orcaqgui2d/ptricestormelement.h>
#include <orcaqgui/ihumanmanager.h>
#include <orcaqgui2dfactory/featuremap2dpainter.h>
#include <orcaqgui2dfactory/connectutils.h>

namespace orcaqgui2d
{

//!
//! @author Alex Brooks
//!
class FeatureMap2dElement
    : public PtrIceStormElement<FeatureMap2dPainter,
                                             orca::FeatureMap2dData,
                                             orca::FeatureMap2dDataPtr,
                                             orca::FeatureMap2dPrx,
                                             orca::FeatureMap2dConsumer,
                                             orca::FeatureMap2dConsumerPrx>
{

public: 

    FeatureMap2dElement( const orcaice::Context  &context,
                         const std::string       &proxyString,
                         orcaqgui::IHumanManager *humanManager );

    virtual bool isInGlobalCS() { return true; }
    virtual void actionOnConnection() 
    {
        paintInitialData<orca::FeatureMap2dPrx, FeatureMap2dPainter>
            ( context_, listener_.interfaceName(), painter_ );
    }
    virtual QStringList contextMenu();
    virtual void execute( int action );
    virtual void setTransparency( bool useTransparency ) { painter_.setTransparency( useTransparency ); };

 public:
    void saveFeatureMapAs();
    void saveFeatureMap();

private:
    FeatureMap2dPainter painter_;
    orcaqgui::IHumanManager *humanManager_;
    QString featureMapFileName_;
    bool featureMapFileNameSet_;
};

}

#endif

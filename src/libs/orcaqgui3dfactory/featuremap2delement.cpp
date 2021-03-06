#include "featuremap2delement.h"
#include <iostream>
#include <QFileDialog>
#include <hydroqgui/exceptions.h>

using namespace std;

namespace orcaqgui3d {

FeatureMap2dElement::FeatureMap2dElement( const hydroqguielementutil::GuiElementInfo &guiElementInfo,
                                          const orcaice::Context                     &context )
    : orcaqgui3d::IceStormElement3d<FeatureMap2dPainter,
                      orca::FeatureMap2dData,
                      orca::FeatureMap2dPrx,
                      orca::FeatureMap2dConsumer,
                      orca::FeatureMap2dConsumerPrx>(guiElementInfo, context, painter_, -1 ),
      featureMapFileName_("/home"),
      featureMapFileNameSet_(false)
{
}

QStringList
FeatureMap2dElement::contextMenu()
{
    QStringList s;
    s<<"Save Feature Map"<<"Save Feature Map As";
    return s;
}

void
FeatureMap2dElement::execute( int action )
{
    switch ( action )
    {
    case 0 :
    {
        saveFeatureMap();
        break;
    }
    case 1 :
    {
        saveFeatureMapAs();
        break;
    }
    default:
    {
        throw hydroqgui::Exception( ERROR_INFO, "FeatureMap2dElement::execute(): What the hell? bad action." );
        break;
    }
    }
}

void FeatureMap2dElement::saveFeatureMapAs()
{
    featureMapFileName_ = QFileDialog::getSaveFileName(
        0,
        //this,
        "Choose a filename to save under",
        featureMapFileName_,
        "*.txt");

    if ( featureMapFileName_ != "" )
    {
        painter_.saveMap( featureMapFileName_, _stuff.humanManager );
        featureMapFileNameSet_ = true;
    }
}

void FeatureMap2dElement::saveFeatureMap()
{
    if (!featureMapFileNameSet_)
    {   
        saveFeatureMapAs();
    }
    else
    {
        painter_.saveMap( featureMapFileName_, _stuff.humanManager );
    }
}

}

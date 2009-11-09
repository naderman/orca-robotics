/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <QString>
#include "pathconversionutil.h"
#include <orcaice/orcaice.h>

namespace orcaqgui2d {
        
void guiPathToOrcaPath( const hydroqguipath::GuiPath &in, 
                        orca::Path2d                 &out,  
                        int            numLoops,
                        float          timeOffset )
{
    assert( numLoops >= 0 && "Number of loops cannot be negative" );    
    
    out.resize( in.size()*numLoops );

    int counter = 0;

    for (int k=0; k<numLoops; k++)
    {
        for (unsigned int i=0; i<in.size(); i++)
        {
            double heading = DEG2RAD((double)in[i].heading/hydroqguipath::QT_ANGLE_MULTIPLIER);
            NORMALISE_ANGLE( heading );

            out[counter].target.p.x = in[i].position.x();
            out[counter].target.p.y = in[i].position.y();
            out[counter].target.o = heading;
            out[counter].distanceTolerance = in[i].distanceTolerance;
            out[counter].headingTolerance = DEG2RAD((float)in[i].headingTolerance/hydroqguipath::QT_ANGLE_MULTIPLIER);      
            out[counter].timeTarget = in[i].timeTarget + k*timeOffset;
            out[counter].maxApproachSpeed = in[i].maxSpeed;
            out[counter].maxApproachTurnrate = DEG2RAD((float)in[i].maxTurnrate);
        
            counter++;
        }
    }
}

void orcaPathToGuiPath( const orca::Path2d     &in, 
                        hydroqguipath::GuiPath &out )
{
    out.resize( in.size() );

    for (unsigned int i=0; i<in.size(); i++)
    {
        out[i].position.setX( in[i].target.p.x );
        out[i].position.setY( in[i].target.p.y );
        out[i].heading = (int)floor(RAD2DEG(in[i].target.o))*hydroqguipath::QT_ANGLE_MULTIPLIER;
        out[i].distanceTolerance = in[i].distanceTolerance;
        out[i].headingTolerance = (int)floor(RAD2DEG(in[i].headingTolerance))*hydroqguipath::QT_ANGLE_MULTIPLIER;   
        out[i].timeTarget = in[i].timeTarget;
        out[i].maxSpeed = in[i].maxApproachSpeed;
        out[i].maxTurnrate = (int)floor(RAD2DEG(in[i].maxApproachTurnrate));
    }
}


hydroqguipath::WaypointSettings readWaypointSettings( const Ice::PropertiesPtr &props, 
                                                      const std::string        &tag )
{
    std::string prefix = tag + ".Config.Waypoints.";

    Ice::StringSeq strIn; strIn.push_back("Velocity"); Ice::StringSeq strOut;
    strOut = orcaice::getPropertyAsStringVectorWithDefault( props, prefix+"SpacingProperty", strIn );
    std::string spacingProperty = strOut[0];
    float spacingValue = orcaice::getPropertyAsDoubleWithDefault( props, prefix+"SpacingValue", 1.0 );
    float distanceTolerance = orcaice::getPropertyAsDoubleWithDefault( props, prefix+"DistanceTolerance", 1.0 );
    int headingTolerance = orcaice::getPropertyAsIntWithDefault( props, prefix+"HeadingTolerance", 90 );
    float maxApproachSpeed = orcaice::getPropertyAsDoubleWithDefault( props, prefix+"MaxApproachSpeed", 2e6 );
    int maxApproachTurnrate = orcaice::getPropertyAsIntWithDefault( props, prefix+"MaxApproachTurnRate", 6000000 );
    
    hydroqguipath::WaypointSettings wpSettings( spacingProperty, 
                                                spacingValue, 
                                                distanceTolerance, 
                                                headingTolerance, 
                                                maxApproachSpeed, 
                                                maxApproachTurnrate );
    
    return wpSettings;
}

bool readActivateImmediately( const Ice::PropertiesPtr &props, 
                              const std::string        &tag )
{
    std::string prefix = tag + ".Config.PathFollower2d.";
    return orcaice::getPropertyAsIntWithDefault( props, prefix+"ActivatePathImmediately", 1 );
}

QString readDumpPath( const Ice::PropertiesPtr &props, 
                      const std::string        &tag )
{
    Ice::StringSeq strIn; strIn.push_back("/tmp"); Ice::StringSeq strOut;
    strOut = orcaice::getPropertyAsStringVectorWithDefault( props, tag+".Config.General.DumpPath", strIn );
    return QString(strOut[0].c_str());
}

}

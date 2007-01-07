/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>
#include <IceUtil/Time.h>

#include <orcaobj/mathdefs.h>
#include <orcaobj/stringutils.h>

#include <orcaice/exceptions.h>
#include "miscutils.h"
#include <fstream>
#include <config.h>

// Random doubles and integers
#define RDOUBLE (rand()*M_PI)
#define RINT    (rand())
#define RCHAR   ((char)rand())

using namespace std;

namespace orcaice
{

void 
normalise( orca::Localise2dData& obj )
{
    float weightSum = 0.0;
    for ( unsigned int i=0; i < obj.hypotheses.size(); i++ )
    {
        weightSum += obj.hypotheses[i].weight;
    }
    assert( weightSum > 0.0 );

    for ( unsigned int i=0; i < obj.hypotheses.size(); i++ )
    {
        obj.hypotheses[i].weight /= weightSum;
    }
}

const orca::Pose2dHypothesis &
mlHypothesis( const orca::Localise2dData& obj )
{
    float maxWeight = -1;
    int   mlI       = -1;
    for ( unsigned int i=0; i < obj.hypotheses.size(); i++ )
    {
        if ( obj.hypotheses[i].weight > maxWeight )
        {
            maxWeight = obj.hypotheses[i].weight;
            mlI = i;
        }
    }
#ifndef NDEBUG
    if ( mlI < 0 )
    {
        std::stringstream ss;
        ss << "Dodgy Localise2dDataPtr: " << orcaice::toString(obj);
        cout << "ERROR(miscutils.cpp): " << ss.str() << endl;
        assert( false && "dodgy Localise2dDataPtr" );
        exit(1);
        // TODO: throw an exception here...
        //throw orcaice::Exception( ERROR_INFO, ss.str() );
    }
#endif
    return obj.hypotheses[mlI];
}

void 
saveToFile( const orca::FeatureMap2dDataPtr& fmap, FILE *f )
{
    for ( unsigned int i=0; i < fmap->features.size(); i++ )
    {
        assert( fmap->features[i] != 0 );

        const orca::Feature2dPtr &feature = fmap->features[i];
        fprintf( f, "%d %f ", feature->type, feature->pExists );

        //
        // I'm not convinced that this is the best way of doing things...
        //
        if ( feature->ice_isA( "::orca::CartesianPointFeature2d" ) )
        {
            const orca::CartesianPointFeature2d& r = dynamic_cast<const orca::CartesianPointFeature2d&>(*feature);
            fprintf(f, "%f %f %f %f %f\n",
                    r.p.x,
                    r.p.y,
                    r.c.xx,
                    r.c.xy,
                    r.c.yy );
        }
        else if ( feature->ice_isA( "::orca::CartesianLineFeature2d" ) )
        {
            const orca::CartesianLineFeature2d& r = dynamic_cast<const orca::CartesianLineFeature2d&>(*feature);
            fprintf(f, "%f %f %f %f %f %f %f %f %f %d %d\n",
                    r.rho,
                    r.alpha,
                    r.c.xx,
                    r.c.xy,
                    r.c.yy,
                    r.start.x,
                    r.start.y,
                    r.end.x,
                    r.end.y,
                    r.startSighted,
                    r.endSighted );
        }
        else
        {
            // Don't really know how to display info about this feature.
            cout<<"TRACE(objutils.cpp): TODO: saveToFile: Deal with other feature types" << endl;
            exit(1);
        }
    }
}

void 
loadFromFile( const std::string &filename, orca::FeatureMap2dDataPtr &fmap )
{
    std::ifstream f;
    f.open( filename.c_str(), ifstream::in );
    if ( f.good() == false )
    {
        std::stringstream ss;
        ss << "Failed to open file: "<<filename;
        throw ss.str();
    }

    const int bufSize=10000;
    char buf[bufSize];

    int line=1;
    while ( true )
    {
        if ( f.eof() ) 
            break;

        f.getline( buf, bufSize );
        
        // ignore comments
        if ( buf[0] == '#' )
            continue;

        char typeAsString[bufSize];
        int ret = sscanf( buf, "%s ", typeAsString );
        if ( ret != 1 )
        {
            // empty line
            continue;
        }
        int type = atoi(typeAsString);
        char *featureInfoBuf = &(buf[strlen(typeAsString)]);

        switch (type)
        {
        case orca::feature::LASERREFLECTOR:
        case orca::feature::FOREGROUNDPOINT:
        case orca::feature::DOOR:
        case orca::feature::CORNER:
        {
            orca::CartesianPointFeature2dPtr feature = new orca::CartesianPointFeature2d;
            feature->type = type;
            // include conversion char 'l' before 'f' to indicate that the pointers are to doubles
            // (rather than floats)
            const int numElements = 6;
            int num = sscanf(featureInfoBuf,"%lf %lf %lf %lf %lf %lf\n",
                             &(feature->pExists),
                             &(feature->p.x),
                             &(feature->p.y),
                             &(feature->c.xx),
                             &(feature->c.xy),
                             &(feature->c.yy) );
            if ( num != numElements )
            {
                std::stringstream ss;
                ss << "Malformed featuremap file!  Couldn't understand line " << line <<":"<<endl<<buf;
                f.close();
                throw( ss.str() );
            }

            fmap->features.push_back( feature );
            line++;
            break;
        }
        case orca::feature::LINE:
        {
            orca::CartesianLineFeature2dPtr feature = new orca::CartesianLineFeature2d;
            feature->type = type;
            // include conversion char 'l' before 'f' to indicate that the pointers are to doubles
            // (rather than floats)
            const int numElements = 12;
            int ss, es;
            int num = sscanf(featureInfoBuf,"%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %d %d\n",
                             &(feature->pExists),
                             &(feature->rho),
                             &(feature->alpha),
                             &(feature->c.xx),
                             &(feature->c.xy),
                             &(feature->c.yy),
                             &(feature->start.x),
                             &(feature->start.y),
                             &(feature->end.x),
                             &(feature->end.y),
                             &ss,
                             &es );
            if ( num != numElements )
            {
                std::stringstream ss;
                ss << "Malformed featuremap file!  Couldn't understand line " << line <<":"<<endl<<buf;
                f.close();
                throw( ss.str() );
            }
            feature->startSighted = ss;
            feature->endSighted = es;

            fmap->features.push_back( feature );
            line++;
            break;
        }
        default:
        {
            cout<<"TRACE(objutils.cpp): TODO: loadFromFile: deal with feature type " << type << endl;
            exit(1);
            break;
        }
        }
    }
    f.close();
}

bool isSane( const orca::PathFollower2dData& pathData, std::string& reason )
{
    std::stringstream ss;
    bool sane=true;
    for ( unsigned int i=0; i < pathData.path.size(); i++ )
    {
        const orca::Waypoint2d &wp = pathData.path[i];

        if ( wp.distanceTolerance < 0.0 )
        {
            ss << "Waypoint " << i << ": bad distance tolerance: " 
               << wp.distanceTolerance << "m" << endl;
            sane = false;
        }
        if ( wp.headingTolerance < 0.0 )
        {
            ss << "Waypoint " << i << ": bad heading tolerance: " 
               << wp.headingTolerance*180.0/M_PI << "deg" << endl;
            sane = false;
        }
        if ( wp.maxApproachSpeed < 0.0 )
        {
            ss << "Waypoint " << i << ": bad maxApproachSpeed: " 
               << wp.maxApproachSpeed << "m/s" << endl;
            sane = false;
        }
        if ( wp.maxApproachTurnrate < 0.0 )
        {
            ss << "Waypoint " << i << ": bad maxApproachTurnrate: " 
               << wp.maxApproachTurnrate*180.0/M_PI << "deg/s" << endl;
            sane = false;
        }
    }
    reason = ss.str();
    return sane;
}

////////////////////////////////////////////////////////////

// Utility function for toVerboseString:
// display a map cell:
char 
displayOgmapCell(unsigned char cell)
{
    if( (int)cell < 96 )
    {
        return ' ';
    }
    else if( (int)cell > 160 )
    {
        return '#';
    }
    //else
    return '-';
}




} // namespace

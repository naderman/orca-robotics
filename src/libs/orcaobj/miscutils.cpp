/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>
#include <IceUtil/Time.h>

#include <orcaobj/stringutils.h>

#include <gbxutilacfr/exceptions.h>
#include <gbxutilacfr/mathdefs.h>
#include "miscutils.h"
#include <fstream>

// alexm: not sure why this was ever here!
// if we have to put it back in, add this line to CMakeLists.txt
// # INCLUDE_DIRECTORIES( ${PROJECT_BINARY_DIR} # for config.h )
// #include <config.h>      

// Random doubles and integers
#define RDOUBLE (rand()*M_PI)
#define RINT    (rand())
#define RCHAR   ((char)rand())

using namespace std;

namespace orcaobj
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
        ss << "Dodgy Localise2dDataPtr: " << orcaobj::toString(obj);
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
#endif
    return obj.hypotheses[mlI];
}

const orca::Pose3dHypothesis &
mlHypothesis( const orca::Localise3dData& obj )
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
        ss << "Dodgy Localise3dDataPtr: " << orcaobj::toString(obj);
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
#endif
    return obj.hypotheses[mlI];
}

bool localisationIsUncertain( const orca::Localise2dData &localiseData,
                              double linearThreshold )
{
    // Some dodgy heuristics
    if ( localiseData.hypotheses.size() > 2 )
        return true;

    const orca::Pose2dHypothesis h = orcaobj::mlHypothesis( localiseData );
    if ( h.cov.xx > linearThreshold ||
         h.cov.yy > linearThreshold )
        return true;

    return false;
}

namespace {

    enum GeomType {
        GeomTypePoint=0,
        GeomTypePose=1,
        GeomTypeLine=2,
    };

    bool isSane( const orca::CartesianPointFeature2d &f )
    {
        return ( f.pExists >= 0 && f.pExists <= 1 &&
                 f.c.xx >= 0 &&
                 f.c.yy >= 0 );
    }
    bool isSane( const orca::CartesianPoseFeature2d &f )
    {
        return ( f.pExists >= 0 && f.pExists <= 1 &&
                 f.c.xx >= 0 &&
                 f.c.yy >= 0 &&
                 f.c.tt >= 0 );
    }
    bool isSane( const orca::CartesianLineFeature2d &f )
    {
        return ( f.pExists >= 0 && f.pExists <= 1 &&
                 f.c.xx >= 0 &&
                 f.c.yy >= 0 );
// Could check for this, but it's not strictly-speaking an error...
//                 && f.alpha <= M_PI && f.alpha >= -M_PI );
    }

}

void 
saveToFile( const orca::FeatureMap2dData& fmap, FILE *f )
{
    for ( unsigned int i=0; i < fmap.features.size(); i++ )
    {
        assert( fmap.features[i] != 0 );
        const orca::Feature2dPtr &feature = fmap.features[i];

        //
        // I'm not convinced that this is the best way of doing things...
        //
        if ( feature->ice_isA( "::orca::CartesianPointFeature2d" ) )
        {
            const orca::CartesianPointFeature2d& r = dynamic_cast<const orca::CartesianPointFeature2d&>(*feature);
            fprintf( f, "%d ", GeomTypePoint );
            fprintf( f, "%d %f ", feature->type, feature->pExists );
            fprintf(f, "%f %f %f %f %f\n",
                    r.p.x,
                    r.p.y,
                    r.c.xx,
                    r.c.xy,
                    r.c.yy );
        }
        else if ( feature->ice_isA( "::orca::CartesianPoseFeature2d" ) )
        {
            const orca::CartesianPoseFeature2d& r = dynamic_cast<const orca::CartesianPoseFeature2d&>(*feature);
            fprintf( f, "%d ", GeomTypePose );
            fprintf( f, "%d %f ", feature->type, feature->pExists );
            fprintf(f, "%f %f %f %f %f %f %f %f %f\n",
                    r.p.p.x,
                    r.p.p.y,
                    r.p.o,
                    r.c.xx,
                    r.c.xy,
                    r.c.xt,
                    r.c.yy,
                    r.c.yt,
                    r.c.tt );
        }
        else if ( feature->ice_isA( "::orca::CartesianLineFeature2d" ) )
        {
            const orca::CartesianLineFeature2d& r = dynamic_cast<const orca::CartesianLineFeature2d&>(*feature);
            fprintf( f, "%d ", GeomTypeLine );
            fprintf( f, "%d %f ", feature->type, feature->pExists );
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
            stringstream ss;
            ss << "Don't know how to save feature to file: " << orcaobj::toString( *feature );
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }
    }
}

void 
loadFromFile( const std::string &filename, orca::FeatureMap2dData &fmap )
{
    std::ifstream f;
    f.open( filename.c_str(), ifstream::in );
    if ( f.good() == false )
    {
        std::stringstream ss;
        ss << "Failed to open file: "<<filename;
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }

    const int bufSize=10000;
    char buf[bufSize];

    int line=0;
    while ( true )
    {
        line++;
        if ( f.eof() ) 
            break;

        f.getline( buf, bufSize );
        
        // ignore comments
        if ( buf[0] == '#' )
            continue;

        char geomTypeAsString[bufSize];
        int ret = sscanf( buf, "%s ", geomTypeAsString );
        if ( ret != 1 )
        {
            // empty line
            continue;
        }
        GeomType geomType = (GeomType)(atoi(geomTypeAsString));
        char *featureInfoBuf = &(buf[strlen(geomTypeAsString)]);

        switch (geomType)
        {
        case GeomTypePoint:
        {
            orca::CartesianPointFeature2dPtr feature = new orca::CartesianPointFeature2d;
            // include conversion char 'l' before 'f' to indicate that the pointers are to doubles
            // (rather than floats)
            const int numElements = 7;
            int num = sscanf(featureInfoBuf,"%d %lf %lf %lf %lf %lf %lf\n",
                             &(feature->type),
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
                throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
            }
            if ( !isSane( *feature ) )
            {
                stringstream ss;
                ss << "Badly-formed feature: " << orcaobj::toString(*feature);
                f.close();
                throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );                
            }

            fmap.features.push_back( feature );
            break;
        }
        case GeomTypePose:
        {
            orca::CartesianPoseFeature2dPtr feature = new orca::CartesianPoseFeature2d;
            // include conversion char 'l' before 'f' to indicate that the pointers are to doubles
            // (rather than floats)
            const int numElements = 11;
            int num = sscanf(featureInfoBuf,"%d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
                             &(feature->type),
                             &(feature->pExists),
                             &(feature->p.p.x),
                             &(feature->p.p.y),
                             &(feature->p.o),
                             &(feature->c.xx),
                             &(feature->c.xy),
                             &(feature->c.xt),
                             &(feature->c.yy),
                             &(feature->c.yt),
                             &(feature->c.tt) );
            if ( num != numElements )
            {
                std::stringstream ss;
                ss << "Malformed featuremap file!  Couldn't understand line " << line <<":"<<endl<<buf;
                f.close();
                throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
            }
            if ( !isSane( *feature ) )
            {
                stringstream ss;
                ss << "Badly-formed feature: " << orcaobj::toString(*feature);
                f.close();
                throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );                
            }

            fmap.features.push_back( feature );
            break;
        }
        case GeomTypeLine:
        {
            orca::CartesianLineFeature2dPtr feature = new orca::CartesianLineFeature2d;
            // include conversion char 'l' before 'f' to indicate that the pointers are to doubles
            // (rather than floats)
            const int numElements = 13;
            int ss, es;
            int num = sscanf(featureInfoBuf,"%d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %d %d\n",
                             &(feature->type),
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
                throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
            }
            feature->startSighted = ss;
            feature->endSighted = es;

            if ( !isSane( *feature ) )
            {
                stringstream ss;
                ss << "Badly-formed feature: " << orcaobj::toString(*feature);
                f.close();
                throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );                
            }

            fmap.features.push_back( feature );
            break;
        }
        default:
        {
            stringstream ss;
            ss<<"loadFromFile: don't know how to load with geom type " << geomType << endl;
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }
        }
        line++;
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

bool
isPathSketchy( const orca::Path2d& path, std::string &sketchyReason )
{
    std::stringstream ss;
    bool normal=true;
    const float epsLinear     = 1e-3;
    const float epsRotational = 1.0*M_PI/180.0;
    for ( unsigned int i=0; i < path.size(); i++ )
    {
        const orca::Waypoint2d &wp = path[i];

        if ( wp.distanceTolerance < epsLinear )
        {
            ss << "Waypoint " << i << ": possibly sketchy distance tolerance: " 
               << wp.distanceTolerance << "m" << endl;
            normal = false;
        }
        if ( wp.headingTolerance < epsRotational )
        {
            ss << "Waypoint " << i << ": possibly sketchy heading tolerance: " 
               << wp.headingTolerance*180.0/M_PI << "deg" << endl;
            normal = false;
        }
        if ( wp.maxApproachSpeed < epsLinear )
        {
            ss << "Waypoint " << i << ": possibly sketchy maxApproachSpeed: " 
               << wp.maxApproachSpeed << "m/s" << endl;
            normal = false;
        }
        if ( wp.maxApproachTurnrate < epsRotational )
        {
            ss << "Waypoint " << i << ": possibly sketchy maxApproachTurnrate: " 
               << wp.maxApproachTurnrate*180.0/M_PI << "deg/s" << endl;
            normal = false;
        }
        if ( wp.timeTarget.seconds < 0 || wp.timeTarget.useconds < 0 )
        {
            ss << "Waypoint " << i << ": funky timeTarget: "
               << wp.timeTarget.seconds << ":" << wp.timeTarget.useconds << endl;
            normal = false;
        }
    }
    if ( !normal )
    {
        sketchyReason = ss.str();
        return true;
    }
    else
        return false;
}

} // namespace

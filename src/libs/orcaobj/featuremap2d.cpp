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
#include <sstream>
#include <fstream>
#include <cmath>
#include <gbxutilacfr/exceptions.h>
#include "featuremap2d.h"

using namespace std;

namespace orcaobj
{

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

////////////////////////////////

std::string toString( const orca::FeatureMap2dData &obj )
{
    std::ostringstream s;

    s << " FeatureMap2d ["<<obj.features.size()<<" elements]: "<<endl;

    for (unsigned int i=0; i < obj.features.size(); i++)
    {
        assert( obj.features[i] != 0 );

        const orca::Feature2dPtr &f = obj.features[i];
        s << "  " << i << ": " << toString(*f) << endl;
    }    
    s << endl;
    
    return s.str();
}

std::string toString( const orca::Feature2d &f )
{
    std::ostringstream s;

    //
    // I'm not convinced that this is the best way of doing things...
    //
    s << "[t"<<f.type<<",p"<<f.pExists<<"]";

    if ( f.ice_isA( "::orca::CartesianPointFeature2d" ) )
    {
        const orca::CartesianPointFeature2d& r = dynamic_cast<const orca::CartesianPointFeature2d&>(f);
        s << "("<<r.p.x<<","<<r.p.y<<") ("<<r.c.xx<<","<<r.c.xy<<","<<r.c.yy<<")";
    }
    else if ( f.ice_isA( "::orca::CartesianPoseFeature2d" ) )
    {
        const orca::CartesianPoseFeature2d& r = dynamic_cast<const orca::CartesianPoseFeature2d&>(f);
        s << "("<<r.p.p.x<<","<<r.p.p.y<<","<<r.p.o*180.0/M_PI<<") ("<<r.c.xx<<","<<r.c.xy<<","<<r.c.yy<<")";
    }
    else if ( f.ice_isA( "::orca::CartesianLineFeature2d" ) )
    {
        const orca::CartesianLineFeature2d& r = dynamic_cast<const orca::CartesianLineFeature2d&>(f);
        s << "(r="<<r.rho<<",a="<<r.alpha*180.0/M_PI<<"deg) ("<<r.c.xx<<","<<r.c.xy<<","<<r.c.yy<<")" << " ("<<r.start.x<<","<<r.start.y<<")->("<<r.end.x<<","<<r.end.y<<")";
    }
    else
    {
        // Don't really know how to display info about this feature.
        assert( false );
    }
    
    return s.str();
}

void saveToFile( const orca::FeatureMap2dData& fmap, FILE *f )
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

void loadFromFile( const std::string &filename, orca::FeatureMap2dData &fmap )
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

} // namespace

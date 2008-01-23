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

#include <orcaice/proputils.h>
#include <orcaobj/proputils.h>
#include <hydroutil/mathdefs.h>
#include <orcaobj/stringutils.h>

using namespace std;

//
// Build the component into a stand-alone application
//
int main(int argc, char * argv[])
{
    // Instantiate the global property set
    Ice::PropertiesPtr props = Ice::createProperties();
    props->setProperty( "String", "test" );
    props->setProperty( "Double.Good", "99.9" );
    props->setProperty( "Double.Alpha", "nine" );
    props->setProperty( "Int.Good", "256" );
    props->setProperty( "Int.Alpha", "two" );
    props->setProperty( "StringSequence", "test:exam:check" );
    props->setProperty( "Frame2d.Good", "1.0 2.0 180.0" );
    props->setProperty( "Frame2d.Short", "1.0 2.0" );
    props->setProperty( "Frame2d.Alpha", "1.0 2.0 three" );
    props->setProperty( "Frame3d.Good", "1.0 2.0 3.0  4.0 5.0 6.0" );
    props->setProperty( "Frame3d.Short", "1.0 2.0 3.0  4.0 5.0" );
    props->setProperty( "Frame3d.Alpha", "1.0 2.0 three  4.0 5.0" );
    props->setProperty( "Size3d.Good", "1.0 2.0 3.0" );
    props->setProperty( "Size3d.Short", "1.0 2.0" );
    props->setProperty( "Size3d.Alpha", "1.0 2.0 three" );
    props->setProperty( "IntVector.Good", "1 2 3" );
    props->setProperty( "IntVector.Double", "1 2.0 3" );
    props->setProperty( "IntVector.Alpha", "1 two 3" );
    props->setProperty( "DoubleVector.Good", "1.0 2.0 3.0" );
    props->setProperty( "DoubleVector.Alpha", "1.0 two 3.0" );
    int ret;


    cout<<"testing getPropertyAsStringSeq() ... ";
    Ice::StringSeq retStringSeq;
    ret = orcaobj::getPropertyAsStringSeq( props, "StringSequence", retStringSeq );
    if ( ret!=0 || retStringSeq.size()!=3 || retStringSeq[0]!="test" ) {
        cout<<"failed"<<endl<<"\texisting key with string seq: ret="<<ret<<"; size="<<retStringSeq.size()<<endl;
        return EXIT_FAILURE;
    }
    ret = orcaobj::getPropertyAsStringSeq( props, "NotExist", retStringSeq );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\tnon-existing key with string seq: ret="<<ret<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing getPropertyAsStringSeqWithDefault() ... ";
    Ice::StringSeq defaultSeq;
    defaultSeq.push_back("standard");
    defaultSeq.push_back("normal");
    retStringSeq = orcaobj::getPropertyAsStringSeqWithDefault( props, "StringSequence", defaultSeq );
    if ( retStringSeq.size()!=3 || retStringSeq[2]!="check" ) {
        cout<<"failed"<<endl<<"\texisting key with default string seq: ret="<<retStringSeq.size()<<endl;
        return EXIT_FAILURE;
    }
    retStringSeq = orcaobj::getPropertyAsStringSeqWithDefault( props, "NotExist", defaultSeq );
    if ( retStringSeq.size()!=defaultSeq.size() || retStringSeq[1]!="normal" ) {
        cout<<"failed"<<endl<<"\tnon-existing key with default string seq: ret="<<retStringSeq.size()<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing getPropertyAsFrame2d() ... ";
    orca::Frame2d frame2d;
    ret = orcaobj::getPropertyAsFrame2d( props, "Frame2d.Good", frame2d );
    if ( ret!=0 || (frame2d.p.x+frame2d.p.y)!=3.0
                // degrees are converted to radians and then they are hard to compare
                || !NEAR(frame2d.o,M_PI,0.001) ) {
        cout<<"failed"<<endl<<"\texisting key with good Frame2d: ret="<<ret
            <<" angle :"<<frame2d.o<<endl;
        return EXIT_FAILURE;
    }
    ret = orcaobj::getPropertyAsFrame2d( props, "Frame2d.Short", frame2d );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\texisting key with short Frame2d: ret="<<ret<<endl;
        return EXIT_FAILURE;
    }
    ret = orcaobj::getPropertyAsFrame2d( props, "Frame2d.Alpha", frame2d );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\texisting key with alpha Frame2d: ret="<<ret<<endl;
        return EXIT_FAILURE;
    }
    ret = orcaobj::getPropertyAsFrame2d( props, "NotExist", frame2d );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\tnon-existing key with Frame2d: ret="<<ret<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;
    
    cout<<"testing getPropertyAsFrame3d() ... ";
    orca::Frame3d frame3d;
    ret = orcaobj::getPropertyAsFrame3d( props, "Frame3d.Good", frame3d );
    if ( ret!=0 || frame3d.p.x!=1.0 ) {
        cout<<"failed"<<endl<<"\texisting key with good Frame3d: ret="<<ret<<endl;
        cout<<"looking for x=1.0 value="<<orcaobj::toString(frame3d)<<endl;
        return EXIT_FAILURE;
    }
    if ( ret!=0 || frame3d.p.y!=2.0 ) {
        cout<<"failed"<<endl<<"\texisting key with good Frame3d: ret="<<ret<<endl;
        cout<<"looking for y=2.0 value="<<orcaobj::toString(frame3d)<<endl;
        return EXIT_FAILURE;
    }
    if ( ret!=0 || frame3d.p.z!=3.0 ) {
        cout<<"failed"<<endl<<"\texisting key with good Frame3d: ret="<<ret<<endl;
        cout<<"looking for z=3.0 value="<<orcaobj::toString(frame3d)<<endl;
        return EXIT_FAILURE;
    }
    // degrees are converted to radians and then they are hard to compare
    if ( ret!=0 || (int)floor(frame3d.o.r/DEG2RAD_RATIO+1.0e-3)!=4 ) {
        cout<<"failed"<<endl<<"\texisting key with good Frame3d: ret="<<ret<<endl;
        cout<<"looking for r=4.0 value="<<orcaobj::toString(frame3d)<<endl;
        return EXIT_FAILURE;
    }
    if ( ret!=0 || (int)floor(frame3d.o.p/DEG2RAD_RATIO+1.0e-3)!=5 ) {
        cout<<"failed"<<endl<<"\texisting key with good Frame3d: ret="<<ret<<endl;
        cout<<"looking for p=5.0 value="<<orcaobj::toString(frame3d)<<endl;
        return EXIT_FAILURE;
    }
    if ( ret!=0 || (int)floor(frame3d.o.y/DEG2RAD_RATIO+1.0e-3)!=6 ) {
        cout<<"failed"<<endl<<"\texisting key with good Frame3d: ret="<<ret<<endl;
        cout<<"looking for y=6.0 value="<<orcaobj::toString(frame3d)<<endl;
        return EXIT_FAILURE;
    }
//     if ( ret!=0 || (frame3d.p.x+frame3d.p.y+frame3d.p.z)!=6.0 
//     // degrees are converted to radians and then they are hard to compare
//                 || (int)floor((frame3d.o.p+frame3d.o.r+frame3d.o.y+1.0e-3)/DEG2RAD_RATIO)!=15 ) {
//         cout<<"failed"<<endl<<"\texisting key with good Frame3d: ret="<<ret
//             <<" sum of angles :"<<(int)floor((frame3d.o.p+frame3d.o.r+frame3d.o.y+1.0e-3)/DEG2RAD_RATIO)<<endl;
//         return EXIT_FAILURE;
//     }
    ret = orcaobj::getPropertyAsFrame3d( props, "Frame3d.Short", frame3d );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\texisting key with short Frame3d: ret="<<ret<<" value="<<orcaobj::toString(frame3d)<<endl;
        return EXIT_FAILURE;
    }
    ret = orcaobj::getPropertyAsFrame3d( props, "Frame3d.Alpha", frame3d );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\texisting key with alpha Frame3d: ret="<<ret<<" value="<<orcaobj::toString(frame3d)<<endl;
        return EXIT_FAILURE;
    }
    ret = orcaobj::getPropertyAsFrame3d( props, "NotExist", frame3d );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\tnon-existing key with Frame3d: ret="<<ret<<" value="<<orcaobj::toString(frame3d)<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    cout<<"testing getPropertyAsSize3d() ... ";
    orca::Size3d size3d;
    ret = orcaobj::getPropertyAsSize3d( props, "Size3d.Good", size3d );
    if ( ret!=0 || (size3d.l+size3d.w+size3d.h)!=6.0 ) {
        cout<<"failed"<<endl<<"\texisting key with Size3d: ret="<<ret<<endl;
        return EXIT_FAILURE;
    }
    ret = orcaobj::getPropertyAsSize3d( props, "Size3d.Short", size3d );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\texisting key with short Size3d: ret="<<ret<<endl;
        return EXIT_FAILURE;
    }
    ret = orcaobj::getPropertyAsSize3d( props, "Size3d.Alpha", size3d );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\texisting key with alpha Size3d: ret="<<ret<<endl;
        return EXIT_FAILURE;
    }
    ret = orcaobj::getPropertyAsSize3d( props, "NotExist", size3d );
    if ( ret==0 ) {
        cout<<"failed"<<endl<<"\tnon-existing key with Size3d: ret="<<ret<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    return EXIT_SUCCESS;
}
